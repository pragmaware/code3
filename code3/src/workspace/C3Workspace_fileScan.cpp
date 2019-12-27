//=============================================================================
//
//   File : C3Workspace.cpp
//   Creation Date : mar 20 ott 2015 01:01:10
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

#include "C3Workspace_private.h"

#include <QDir>
#include <QThreadPool>

void C3Workspace::abortFileScanWorker()
{
	if(!_p->pFileScanWorker)
		return; // was not running at all (only our thread can start it)

	_p->oFileCompletionMutex.lock();

	if(!_p->pFileScanWorker)
	{
		_p->oFileCompletionMutex.unlock();
		return;
	}
	
	// the file scan worker still exists here.
	// it's either still queued or it's already running.
	// it *might* be reentering inside our asyncFileScanWorkerTerminated call now
	// but if it is then it's blocked (or going to be) on _p->oFileCompletionMutex.
	
	if(!_p->pFileScanWorker->abort())
	{
		// The abort call failed: the worker is already inside our asyncFileScanWorkerTerminated() call.
		// It IS going to deadlock if we don't release the mutex now.
		_p->oFileCompletionMutex.unlock();

		// Wait for clean completion

		//qDebug("[C3Workspace::abortFileScanWorker] Waiting for the file scan worker to exit...");

		while(_p->pFileScanWorker)
			QThread::msleep(50);

	} else {
		// Abort succeeded.
		// From here on the worker will no longer call us back
#if QT_VERSION >= 0x50500
		QThreadPool::globalInstance()->cancel(_p->pFileScanWorker);
#endif
		_p->pFileScanWorker = NULL;
		_p->oFileCompletionMutex.unlock();
	}
}

void C3Workspace::asyncFileScanWorkerTerminated(C3WorkspaceFileScanWorker * pWorker,const QString &szError,C3CompletionStore * pFileCompletionStore)
{
	Q_ASSERT(_p->pFileScanWorker);
	Q_ASSERT(_p->pFileScanWorker == pWorker);

	_p->oFileCompletionMutex.lock();
	if(_p->pFileCompletionStore)
		delete _p->pFileCompletionStore;
	_p->pFileCompletionStore = pFileCompletionStore;
	_p->pFileScanWorker = NULL;
	_p->oFileCompletionMutex.unlock();
}


void C3Workspace::maybeStartFileScanWorker()
{
	_p->oFileCompletionMutex.lock();
	if(_p->pFileScanWorker)
	{
		// already running
		_p->oFileCompletionMutex.unlock();
		return;
	}

	QStringList lPaths;
	
	if(isDefault())
	{
		lPaths.append(QDir::currentPath());
	} else {
		lPaths.append(_p->szRootPath);
		lPaths.append(_p->lIncludePaths);
	}

	_p->pFileScanWorker = new C3WorkspaceFileScanWorker(this,lPaths,_p->lFileExclusionPatterns,isDefault() ? 1 : 10);
	_p->pFileScanWorker->setAutoDelete(true);

	QThreadPool::globalInstance()->start(_p->pFileScanWorker);

	_p->oFileCompletionMutex.unlock();
	_p->tFileScanTimer.restart();
	_p->iTimeToNextFileScan = 360000; // 6 mins
}

