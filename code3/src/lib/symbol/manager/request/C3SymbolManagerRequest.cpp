//=============================================================================
//
//   File : C3SymbolManagerRequest.cpp
//   Creation Date : 2015/12/06 05:01:22
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

#include "C3SymbolManagerRequest.h"
#include "C3SymbolManager.h"

#include <QMutex>

class C3SymbolManagerRequestPrivate
{
public:
	bool bAborted;
	C3SymbolManagerRequest::Type eType;
};

#define _p m_pSMRP


C3SymbolManagerRequest::C3SymbolManagerRequest(Type eType)
	: QObject()
{
	_p = new C3SymbolManagerRequestPrivate();
	_p->bAborted = false;
	_p->eType = eType;
}

C3SymbolManagerRequest::~C3SymbolManagerRequest()
{
	delete _p;
}

C3SymbolManagerRequest::Type C3SymbolManagerRequest::type() const
{
	return _p->eType;
}

void C3SymbolManagerRequest::start()
{
	C3SymbolManager::instance()->addRequest(this);
}

void C3SymbolManagerRequest::abort()
{
	// disconnect is thread safe
	QObject::disconnect(this,SIGNAL(terminated(C3SymbolManagerRequest *)),NULL,NULL);
	C3SymbolManager::instance()->abortRequest(this);
}

void C3SymbolManagerRequest::markAsAborted()
{
	// disconnect is thread-safe
	QObject::disconnect(this,SIGNAL(terminated(C3SymbolManagerRequest *)),NULL,NULL);
	_p->bAborted = true;
}

bool C3SymbolManagerRequest::aborted()
{
	// makes no sense to lock here
	return _p->bAborted;
}

void C3SymbolManagerRequest::emitTerminationSignal()
{
	if(_p->bAborted)
		return;
	emit terminated(this);
}
