//=============================================================================
//
//   File : C3WorkspaceFileScanWorker.cpp
//   Creation Date : 2015/11/14 03:28:41
//   Project : 
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

#include "C3WorkspaceFileScanWorker.h"

#include "C3Workspace.h"
#include "C3CompletionStore.h"

#include <QMutex>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QRegExp>
#include <QList>

class C3WorkspaceFileScanWorkerPrivate
{
public:
	QMutex oWorkspaceMutex;
	C3Workspace * pWorkspace;
	QString szCurrentRootPath; // includes trailing slash
	QStringList lPaths;
	QList<QRegExp> lFileExclusionPatterns;
	QList<QRegExp> lDirectoryExclusionPatterns;
	C3CompletionStore * pCompletionStore;
	QHash<QString,int> hValidExtensions;
	int iDepth;
	int iMaxDepth;
};

#define _p m_pC3WFSW

C3WorkspaceFileScanWorker::C3WorkspaceFileScanWorker(
		C3Workspace * pWorkspace,
		const QStringList &lPaths,
		const QStringList &lExclusionPatterns,
		int iMaxDepth
	)
	: QRunnable()
{
	_p = new C3WorkspaceFileScanWorkerPrivate();
	_p->pWorkspace = pWorkspace;
	_p->lPaths = lPaths;
	_p->iMaxDepth = iMaxDepth;
	QString cSlash('/');
	QString cBSlash('\\');
	foreach(QString s,lExclusionPatterns)
	{
		if(s.contains(cSlash) || s.contains(cBSlash))
		{
			s.replace(cSlash,QString());
			s.replace(cBSlash,QString());
			_p->lDirectoryExclusionPatterns.append(QRegExp(s,Qt::CaseSensitive,QRegExp::Wildcard));
		} else {
			_p->lFileExclusionPatterns.append(QRegExp(s,Qt::CaseSensitive,QRegExp::Wildcard));
		}
	}
	_p->pCompletionStore = NULL;
	_p->iDepth = 1; // base depth
}

C3WorkspaceFileScanWorker::~C3WorkspaceFileScanWorker()
{
	_p->oWorkspaceMutex.lock();
	if(_p->pCompletionStore)
		delete _p->pCompletionStore;
	_p->oWorkspaceMutex.unlock();
	delete _p;
}


bool C3WorkspaceFileScanWorker::abort()
{
	if(!_p->oWorkspaceMutex.tryLock())
		return false;
	_p->pWorkspace = NULL;
	_p->oWorkspaceMutex.unlock();
	return true;
}

void C3WorkspaceFileScanWorker::scanPath(const QString &szPath)
{
	if(_p->iDepth >= _p->iMaxDepth)
		return;
	_p->iDepth++;
	scanPathInternal(szPath);
	_p->iDepth--;
}

void C3WorkspaceFileScanWorker::scanPathInternal(const QString &szPath)
{
	//qDebug("Scanning path %s",szPath.toUtf8().data());

	if(!_p->pWorkspace)
		return;

	QDir d(szPath);

	QFileInfoList fil = d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	int len = _p->szCurrentRootPath.length();

	QChar fs('/');
	QChar bs('\\');

	foreach(QFileInfo fi,fil)
	{
		QString szEntryPath = fi.absoluteFilePath();

		if(fi.isDir())
		{
			if(!_p->lDirectoryExclusionPatterns.isEmpty())
			{
				bool bSkip = false;
			
				foreach(QRegExp re,_p->lDirectoryExclusionPatterns)
				{
					if(re.exactMatch(fi.fileName()))
					{
						bSkip = true;
						break;
					}
				}
				
				if(bSkip)
					continue;
			}
		
			scanPath(szEntryPath);
			
			if(!_p->pWorkspace)
				return;
				
			continue;
		}

		// regular file
		QString szSuffix = fi.suffix().toLower();
			
		if((!szSuffix.isEmpty()) && (_p->hValidExtensions.value(szSuffix,0) == 0))
			continue;

		if(!_p->lFileExclusionPatterns.isEmpty())
		{
			bool bSkip = false;
		
			foreach(QRegExp re,_p->lFileExclusionPatterns)
			{
				if(re.exactMatch(fi.fileName()))
				{
					bSkip = true;
					break;
				}
			}
			
			if(bSkip)
				continue;
		}

		QString szRelative = szEntryPath.mid(len);
		
		_p->pCompletionStore->addCompletion(szRelative);
		
		for(;;)
		{
			int idx = szRelative.indexOf(fs);
			if(idx < 0)
			{
				idx = szRelative.indexOf(bs);
				if(idx < 0)
					break;
			}

			szRelative = szRelative.mid(idx+1);
			
			if(szRelative.isEmpty())
				break;

			_p->pCompletionStore->addCompletion(szRelative);

			if(!_p->pWorkspace)
				return;
		}
		
		if(!_p->pWorkspace)
			return;
	}
}

QString C3WorkspaceFileScanWorker::runInternal()
{
	if(!_p->pWorkspace)
		return __tr("Worker aborted");

	// Only files that can be actually completed.
	// C, ObjC and php support includes.
	_p->hValidExtensions.insert("cpp",1);
	_p->hValidExtensions.insert("cc",1);
	_p->hValidExtensions.insert("c",1);
	_p->hValidExtensions.insert("cxx",1);
	_p->hValidExtensions.insert("lxx",1); // opencascade
	_p->hValidExtensions.insert("h",1);
	_p->hValidExtensions.insert("hpp",1);
	_p->hValidExtensions.insert("hh",1);
	_p->hValidExtensions.insert("hxx",1);
	_p->hValidExtensions.insert("m",1);
	_p->hValidExtensions.insert("php",1);

	_p->pCompletionStore = new C3CompletionStore();

	foreach(QString s,_p->lPaths)
	{
		QFileInfo inf(s);
		
		QString szFixedPath = inf.canonicalFilePath();

		_p->szCurrentRootPath = szFixedPath;
	
		QChar sep = QDir::separator();

		if(!_p->szCurrentRootPath.endsWith(sep))
			_p->szCurrentRootPath.append(sep);

		scanPath(szFixedPath);

		if(!_p->pWorkspace)
			return __tr("Worker aborted");
	}

	if(!_p->pWorkspace)
		return __tr("Worker aborted");

	return QString(); // okie
}

void C3WorkspaceFileScanWorker::run()
{
	QString szError = runInternal();

	_p->oWorkspaceMutex.lock();
	if(_p->pWorkspace)
		_p->pWorkspace->asyncFileScanWorkerTerminated(this,szError,_p->pCompletionStore);
	else
		delete _p->pCompletionStore;
	_p->pCompletionStore = NULL;
	_p->oWorkspaceMutex.unlock();
}
