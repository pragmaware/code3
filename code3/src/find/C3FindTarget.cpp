//=============================================================================
//
//   File : C3FindTarget.cpp
//   Creation Date : dom 01 nov 2015 19:55:29
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

#include "C3FindTarget.h"

C3FindTarget::C3FindTarget(Type eType)
	: m_eType(eType)
{
}

C3FindTarget::~C3FindTarget()
{
}

void C3FindTarget::findOperationReplaceAllFinished(C3FindOperation * pOperation)
{
}
