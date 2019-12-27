#ifndef _C3SymbolManagerBase_h_
#define _C3SymbolManagerBase_h_
//=============================================================================
//
//   File : C3SymbolManagerBase.h
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

#include "c3_config.h"

#include "C3SymbolManagerRequest.h"

#include <QThread>

class C3SymbolManagerBasePrivate;

// Manages the gory details of the request queues
class C3SymbolManagerBase : public QThread
{
	friend class C3SymbolManagerRequest;
	Q_OBJECT
public:
	C3SymbolManagerBase();
	virtual ~C3SymbolManagerBase();

private:
	C3SymbolManagerBasePrivate * m_pC3SM;

public:

	void requestTermination();

protected:

	// Called by C3SymbolManagerRequest
	// The owneship of the request is transferred.
	void addRequest(C3SymbolManagerRequest * pRequest);

	// Called by C3SymbolManagerRequest
	// If the terminated() signal hasn't been emitted yet
	// then it will never be emitted.
	// The ownership of the request remains of the manager.
	void abortRequest(C3SymbolManagerRequest * pRequest);

	virtual void run();

	void wakeUpSymbolThread();
	
	bool terminationRequested();

	virtual void processRequest(C3SymbolManagerRequest * pRequest) = 0;
	virtual void idleStep() = 0;

private slots:
	// This is an inter-thread self-connection 
	void interThreadSlotRequestTerminated();

signals:
	void interThreadSignalRequestTerminated();

}; // class C3SymbolManagerBase

#endif //!_C3SymbolManagerBase_h_
