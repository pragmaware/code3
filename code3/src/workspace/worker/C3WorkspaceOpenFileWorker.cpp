//=============================================================================
//
//   File : C3WorkspaceOpenFileWorker.cpp
//   Creation Date : ven 23 ott 2015 01:51:39
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

#include "C3WorkspaceOpenFileWorker.h"
#include "C3EditorFactory.h"
#include "C3Application.h"
#include "C3Workspace.h"

#include <QFileInfo>
#include <QFile>
#include <QMutex>

class C3WorkspaceOpenFileWorkerPrivate
{
public:
	C3Link oLink;
	QMutex oWorkspaceMutex;
	C3Workspace * pWorkspace;
	int iId;
};

#define _p m_pC3WOFW

static int g_iNextWorkerId = 0;

C3WorkspaceOpenFileWorker::C3WorkspaceOpenFileWorker(C3Workspace * pWorkspace,const C3Link &oLink)
	: QRunnable()
{
	_p = new C3WorkspaceOpenFileWorkerPrivate();
	g_iNextWorkerId++;
	_p->iId = g_iNextWorkerId;
	_p->pWorkspace = pWorkspace;
	_p->oLink = oLink;
}

C3WorkspaceOpenFileWorker::~C3WorkspaceOpenFileWorker()
{
	delete _p;
}

bool C3WorkspaceOpenFileWorker::abort()
{
	if(!_p->oWorkspaceMutex.tryLock())
		return false;
	_p->pWorkspace = NULL;
	_p->oWorkspaceMutex.unlock();
	return true;
}

int C3WorkspaceOpenFileWorker::id() const
{
	return _p->iId;
}

C3Link & C3WorkspaceOpenFileWorker::link()
{
	return _p->oLink;
}

QString C3WorkspaceOpenFileWorker::runInternal()
{
	if(!_p->pWorkspace)
		return __tr("Worker aborted");

	QString szPath = _p->oLink.path();

	if(szPath.isEmpty())
		return __tr("Invalid empty file specified");

	QFileInfo inf(szPath);

	if(!inf.exists())
		return __tr("File does not exist");

	if(inf.isDir())
		return __tr("Cannot open a directory");

	_p->oLink.setPath(inf.canonicalFilePath());

	_p->oLink.insert("size",QVariant((qlonglong)inf.size()));
	_p->oLink.insert("name",inf.fileName());

	if(!_p->pWorkspace)
		return __tr("Worker aborted");

	QByteArray data;

	if(inf.size() > 0)
	{
		// Try to read it!

		QFile f(szPath);
		
		if(!f.open(QFile::ReadOnly))
			return f.errorString();

		qint64 iRead = inf.size();
		if(iRead > 128)
			iRead = 128;

		data = f.read(iRead);

		f.close();
	}

	_p->oLink.insert("data",QVariant(data));

	QString szFactoryId = _p->oLink.editorFactoryId();

	if(!_p->pWorkspace)
		return __tr("Worker aborted");
	
	if(szFactoryId.isEmpty())
	{
		// guess it

		QList<C3EditorFactory *> &lFactories = C3Application::instance()->editorFactories();
	
		C3EditorFactory * pBest = NULL;
		int iBestScore = 0;
	
		Q_FOREACH(C3EditorFactory * f,lFactories)
		{
			int iScore = f->score(_p->oLink,inf);
			if(iScore > iBestScore)
			{
				iBestScore = iScore;
				pBest = f;
			}
		}
		
		if(!pBest)
			return __tr("Could not find an editor to open this kind of file");

		_p->oLink.setEditorFactoryId(pBest->id());
	}

	if(!_p->pWorkspace)
		return __tr("Worker aborted");

	return QString(); // okie
}

void C3WorkspaceOpenFileWorker::run()
{
	QString szError = runInternal();

	if(szError.isEmpty())
	{
		_p->oLink.insert("status","ok");
	} else {
		_p->oLink.insert("status","error");
		_p->oLink.insert("error",szError);
	}

	_p->oWorkspaceMutex.lock();
	if(_p->pWorkspace)
		_p->pWorkspace->openFileWorkerTerminated(this,_p->oLink);
	_p->oWorkspaceMutex.unlock();
}
