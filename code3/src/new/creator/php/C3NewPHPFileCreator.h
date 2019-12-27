#ifndef _C3NewPHPFileCreator_h_
#define _C3NewPHPFileCreator_h_
//=============================================================================
//
//   File : C3NewPHPFileCreator.h
//   Creation Date : 2016/02/08 03:27:42
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

#include "C3NewObjectCreator.h"

class C3NewPHPFileCreator : public C3NewObjectCreator
{
public:
	C3NewPHPFileCreator();
	virtual ~C3NewPHPFileCreator();

public:

	virtual QString name();
	virtual QString icon();

	virtual void run();

}; // class C3NewPHPFileCreator

#endif //!_C3NewPHPFileCreator_h_