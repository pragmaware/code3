//=============================================================================
//
//   File : C3SymbolManagerBase.cpp
//   Creation Date : 2015/12/06 04:56:57
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

#include "C3SymbolManagerBase.h"

#include <QMutex>
#include <QLinkedList>
#include <QWaitCondition>

class C3SymbolManagerBasePrivate
{
public:
	bool bTerminationRequested;
	QLinkedList<C3SymbolManagerRequest *> lRequestQueue;
	QMutex oRequestQueueMutex;
	QList<C3SymbolManagerRequest *> lTerminatedRequests;
	QMutex oTerminatedRequestListMutex;
	QMutex oWaitMutex;
	QWaitCondition oWaitCondition;
	QMutex oWakeUpMutex;
	bool bWakeUp;
};

#define _p m_pC3SM

C3SymbolManagerBase::C3SymbolManagerBase()
	: QThread()
{
	_p = new C3SymbolManagerBasePrivate();
	_p->bTerminationRequested = false;
	_p->bWakeUp = false;

	QObject::connect(
			this,
			SIGNAL(interThreadSignalRequestTerminated()),
			this,
			SLOT(interThreadSlotRequestTerminated()),
			Qt::QueuedConnection
		);
}

C3SymbolManagerBase::~C3SymbolManagerBase()
{
	// disconnect, just in case
	QObject::disconnect(
			this,
			SIGNAL(interThreadSignalRequestTerminated()),
			this,
			SLOT(interThreadSlotRequestTerminated())
		);

	_p->bTerminationRequested = true;
	wakeUpSymbolThread();

	wait();

	_p->oRequestQueueMutex.lock();
	while(!_p->lRequestQueue.isEmpty())
		delete _p->lRequestQueue.takeFirst();
	_p->oRequestQueueMutex.unlock();

	_p->oTerminatedRequestListMutex.lock();
	while(!_p->lTerminatedRequests.isEmpty())
		delete _p->lTerminatedRequests.takeFirst();
	_p->oTerminatedRequestListMutex.unlock();

	delete _p;
}


void C3SymbolManagerBase::addRequest(C3SymbolManagerRequest * pRequest)
{
	_p->oRequestQueueMutex.lock();
	_p->lRequestQueue.append(pRequest);
	_p->oRequestQueueMutex.unlock();
	wakeUpSymbolThread();
}

void C3SymbolManagerBase::abortRequest(C3SymbolManagerRequest * pRequest)
{
	// This is called from the UI thread
	Q_ASSERT(pRequest);
	_p->oRequestQueueMutex.lock();
	if(_p->lRequestQueue.contains(pRequest))
	{
		_p->lRequestQueue.removeOne(pRequest);
		_p->oRequestQueueMutex.unlock();
		delete pRequest;
		return;
	}
	_p->oRequestQueueMutex.unlock();
	// Not in the request queue. Already being executed.
	// The termination signal hasn't been emitted yet (unless the user screwed it up).
	pRequest->markAsAborted();
}

void C3SymbolManagerBase::interThreadSlotRequestTerminated()
{
	// this comes on the UI thread
	_p->oTerminatedRequestListMutex.lock();
	while(!_p->lTerminatedRequests.isEmpty())
	{
		C3SymbolManagerRequest * pRequest = _p->lTerminatedRequests.takeFirst();
		_p->oTerminatedRequestListMutex.unlock();
		if(!pRequest->aborted())
			pRequest->emitTerminationSignal();
		delete pRequest;
		_p->oTerminatedRequestListMutex.lock();
	}
	_p->oTerminatedRequestListMutex.unlock();
}

void C3SymbolManagerBase::requestTermination()
{
	_p->bTerminationRequested = true;
	wakeUpSymbolThread();
}

bool C3SymbolManagerBase::terminationRequested()
{
	return _p->bTerminationRequested;
}

void C3SymbolManagerBase::wakeUpSymbolThread()
{
	// First signal that we want to wake up
	_p->oWakeUpMutex.lock();
	// Here the symbol thread is either BEFORE checking bWakeUp (Point A below)
	// or about to call wait() (Point B below) or it has just exited wait()
	// (Point C Below).
	// If it's in Point A or Point B it will be certainly woken up by this variable.
	_p->bWakeUp = true;
	_p->oWakeUpMutex.unlock();
	// Now if the symbol thread was in Point C then it's either holding WaitMutex
	// or it has released it and it's sleeping inside wait(). In either case
	// wakeAll() will wake it up.
	_p->oWaitCondition.wakeAll();
}

void C3SymbolManagerBase::run()
{
	QString szStuff;

	C3SymbolManagerRequest * pRequest;

	while(!_p->bTerminationRequested)
	{
		_p->oRequestQueueMutex.lock();
		if(_p->lRequestQueue.isEmpty())
			pRequest = NULL;
		else
			pRequest = _p->lRequestQueue.takeFirst();
		_p->oRequestQueueMutex.unlock();

		if(pRequest)
		{
			processRequest(pRequest);
			_p->oTerminatedRequestListMutex.lock();
			_p->lTerminatedRequests.append(pRequest);
			_p->oTerminatedRequestListMutex.unlock();
			emit interThreadSignalRequestTerminated();
			continue;
		}

		idleStep();

		// Point A
		_p->oWakeUpMutex.lock();
		if(_p->bWakeUp)
		{
			_p->bWakeUp = false;
			_p->oWakeUpMutex.unlock();
			continue;
		}

		_p->oWaitMutex.lock();
		_p->oWakeUpMutex.unlock();
		// Point B
		_p->oWaitCondition.wait(&(_p->oWaitMutex),1000); // max 1 second
		_p->oWaitMutex.unlock();
		// Point C
	}
}
