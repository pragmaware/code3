//=============================================================================
//
//   File : C3SymbolManagerCTagsWorker.cpp
//   Creation Date : dom 08 nov 2015 06:55:19
//   Project : Code 3
//   Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This file is part of the Code 3 Editor distribution
//   Copyright (C) 2015-2019 Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 3
//   of the License, or (at your option) any later version.
//
//=============================================================================

#include "C3SymbolManagerCTagsWorker.h"
#include "C3SymbolCTagsRunner.h"
#include "C3SymbolManager.h"
#include "C3SymbolFile.h"
#include "C3Workspace.h"
#include "C3FileUtils.h"
#include "C3Editor.h"
#include "C3TextEditor.h"
#include "C3StringUtils.h"

#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QThread>
#include <QMutex>
#include <QFileInfo>

#include <sys/types.h>
#include <unistd.h>

class C3SymbolManagerCTagsWorkerPrivate
{
public:
	int iId;
	QMutex oSymbolManagerMutex;
	C3SymbolManager * pSymbolManager;
	QString szOutputFile;
	QString szCTagsToolPath;
	QStringList lExclusionPatterns;
	C3SymbolCTagsRunner * pRunner;
	QHash<QString,C3SymbolFile *> * pFiles;
	QStringList lPaths;
	bool bAbortSignal;
	int iEditorId;
	unsigned int uFlags;
	QString szEditorPath;
	QString szTemporaryFile;
	QString szMagicSymbol; // added at the end of cpp files
};

#define _p m_pC3WCTW

C3SymbolManagerCTagsWorker::C3SymbolManagerCTagsWorker(
		int iId,
		C3SymbolManager * pSymbolManager,
		const QString &szCTagsToolPath,
		const QStringList &lPaths,
		const QStringList &lExclusionPatterns,
		int iEditorId,
		unsigned int uFlags
	)
	: QRunnable()
{
	_p = new C3SymbolManagerCTagsWorkerPrivate();
	_p->iId = iId;
	_p->pSymbolManager = pSymbolManager;
	_p->szCTagsToolPath = szCTagsToolPath;
	_p->lPaths = lPaths;
	_p->lExclusionPatterns = lExclusionPatterns;
	_p->pRunner = NULL;
	_p->pFiles = NULL;
	_p->bAbortSignal = false;
	_p->iEditorId = iEditorId;
	_p->uFlags = uFlags;
}

C3SymbolManagerCTagsWorker::~C3SymbolManagerCTagsWorker()
{
	Q_ASSERT(!_p->pRunner);
	if(_p->pFiles)
	{
		qDeleteAll(*(_p->pFiles));
		delete _p->pFiles;
	}
	delete _p;
}

int C3SymbolManagerCTagsWorker::id()
{
	return _p->iId;
}

bool C3SymbolManagerCTagsWorker::abort()
{
	if(!_p->oSymbolManagerMutex.tryLock(100))
		return false;
	_p->pSymbolManager = NULL;
	if(_p->pRunner)
		_p->pRunner->abort();
	_p->bAbortSignal = true;
	_p->oSymbolManagerMutex.unlock();
	return true;
}

QString C3SymbolManagerCTagsWorker::saveEditorContentsToTemporaryFile()
{
	C3Workspace * pWorkspace = C3Workspace::currentWorkspace();

	C3Editor * pEditor = pWorkspace->lockEditor(_p->iEditorId);
	if(!pEditor)
		return __tr("No editor"); // editor not present

	_p->szEditorPath = pEditor->path();
	if(_p->szEditorPath.isEmpty())
	{
		pWorkspace->unlockEditor(pEditor);
		return __tr("Unsaved file, name not known");
	}

	if(!pEditor->modified())
	{
		// editor not modified, no need to save
		pWorkspace->unlockEditor(pEditor);
		return QString();
	}

	C3TextEditor * pTextEditor = dynamic_cast<C3TextEditor *>(pEditor);
	if(!pTextEditor)
	{
		pWorkspace->unlockEditor(pEditor);
		return __tr("No text editor");
	}

	QString szPath = QDir::tempPath();
	if(szPath.isEmpty() || (!QFileInfo::exists(szPath)))
	{
		szPath = C3FileUtils::mergePathComponents(QDir::homePath(),__utf8(".code3"),__utf8("tmp"));
		C3FileUtils::createDirectory(szPath);
	}

	QString szExt = C3StringUtils::fileExtensionFromPath(_p->szEditorPath);
	if(szExt.isEmpty())
		szExt = __ascii("cpp"); // FIXME: Guess it?
	
	_p->szTemporaryFile = C3FileUtils::mergePathComponents(szPath,__utf8("code3.ctags.%1.%2").arg(_p->iEditorId).arg(szExt));

	QString szError;

	QString szTrailer;

	if((szExt == "cpp") || (szExt == "c") || (szExt == "cc") || (szExt == "h"))
	{
		_p->szMagicSymbol = __ascii("_w__xl_%1").arg((long long int)this);
		szTrailer = __ascii("\n;int %2;\n").arg(_p->szMagicSymbol);
	}

	bool bRet = pTextEditor->saveTemporaryFile(_p->szTemporaryFile,szError,&(_p->bAbortSignal),szTrailer);
	
	pWorkspace->unlockEditor(pEditor);

	if(!bRet)
		return szError;

	return QString();
}

QString C3SymbolManagerCTagsWorker::runInternal()
{
	//qDebug("CTags worker run internal");

	if(!_p->pSymbolManager)
	{
		//qDebug("CTags worker aborted!");
		return __tr("Worker aborted");
	}

	unsigned int uFlags = 0;
	if(_p->uFlags & JobLimitRecursionDepthTo1)
		uFlags |= C3SymbolCTagsRunner::JobLimitRecursionDepthTo1;

	if(_p->iEditorId >= 0)
	{
		//qDebug("CTags worker saving contents to temporary file");

		// save the editor contents to a temporary file
		QString szErr = saveEditorContentsToTemporaryFile();
		if(!szErr.isEmpty())
		{
			qDebug("Saving temporary file failed: %s",szErr.toUtf8().data());
			return szErr;
		}

		//qDebug("CTags worker contents saved");

		_p->oSymbolManagerMutex.lock();
		_p->pRunner = new C3SymbolCTagsRunner(_p->szCTagsToolPath);
		if(_p->szTemporaryFile.isEmpty())
			// file unmodified: scan the real file on disk
			_p->pRunner->addJob(_p->szEditorPath,QString(),uFlags);
		else
			// file modified: scan the temporary file and use replacement
			_p->pRunner->addJob(_p->szTemporaryFile,_p->szEditorPath,uFlags);
		_p->oSymbolManagerMutex.unlock();

	} else {
		_p->oSymbolManagerMutex.lock();
		_p->pRunner = new C3SymbolCTagsRunner(_p->szCTagsToolPath);
		foreach(QString s,_p->lPaths)
			_p->pRunner->addJob(s,QString(),_p->lExclusionPatterns,uFlags);
		_p->oSymbolManagerMutex.unlock();
	}

	QString szError;

	//qDebug("CTags worker starting runner");

	bool bRet = _p->pRunner->run();
	if(!bRet)
		szError = _p->pRunner->error();

	_p->oSymbolManagerMutex.lock();
	_p->pFiles = _p->pRunner->takeSymbolFiles(); // may be null
	delete _p->pRunner;
	_p->pRunner = NULL;
	_p->oSymbolManagerMutex.unlock();

	//qDebug("CTags worker runner finished");
	
	// FIXME: If the file is empty: ignore it.
	if((!_p->szMagicSymbol.isEmpty()) && _p->pFiles && (_p->pFiles->count() == 1))
	{
		QHash<QString,C3SymbolFile *>::Iterator it;

		C3SymbolFile * f = NULL;
		for(it = _p->pFiles->begin();it != _p->pFiles->end();++it)
		{
			f = *it;
			break;
		}

		Q_ASSERT(f);
	
		C3Symbol * pSym = f->symbols().last();
		if((!pSym) || (pSym->identifier() != _p->szMagicSymbol))
		{
			//qDebug("ctags invocation didn't find the magic symbol %s",_p->szMagicSymbol.toUtf8().data());
			qDeleteAll(*(_p->pFiles));
			szError = __ascii("Possible syntax error");
			_p->pFiles->clear();
		}
	}
	

	if(!_p->szTemporaryFile.isEmpty())
		QFile::remove(_p->szTemporaryFile);

	if(!_p->pSymbolManager)
		return __tr("Worker aborted");
	
	return szError;
}

void C3SymbolManagerCTagsWorker::run()
{
	//qDebug("CTags worker starting");
	QString szError = runInternal();
	//qDebug("CTags worker runInternal terminated");

	_p->oSymbolManagerMutex.lock();
	if(_p->pSymbolManager)
	{
		_p->pSymbolManager->CTagsWorkerTerminated(this,szError,_p->pFiles);
	} else {
		if(_p->pFiles)
		{
			qDeleteAll(*(_p->pFiles));
			delete _p->pFiles;
		}
	}
	_p->pFiles = NULL;
	_p->oSymbolManagerMutex.unlock();

	//qDebug("CTags worker terminating");
}
