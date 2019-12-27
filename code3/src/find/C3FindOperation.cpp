//=============================================================================
//
//   File : C3FindOperation.cpp
//   Creation Date : dom 01 nov 2015 19:55:55
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

#include "C3FindOperation.h"

C3FindOperation::C3FindOperation()
	:
	m_eDirection(Forward),
	m_eScope(Document),
	m_bCaptureReplacementsEnabled(false)
{
}

C3FindOperation::~C3FindOperation()
{
}

int C3FindOperation::nextOperationId()
{
	static int iNextOperationId = 0;
	iNextOperationId++;
	return iNextOperationId;
}
