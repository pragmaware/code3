//=============================================================================
//
//   File : C3NewCPPClassCreator.cpp
//   Creation Date : sab 07 nov 2015 06:46:09
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

#include "C3NewCPPClassCreator.h"

#include "C3NewCPPClassDialog.h"

C3NewCPPClassCreator::C3NewCPPClassCreator()
	: C3NewObjectCreator()
{
}

C3NewCPPClassCreator::~C3NewCPPClassCreator()
{
}

QString C3NewCPPClassCreator::name()
{
	return __tr("C++ Class");
}

QString C3NewCPPClassCreator::icon()
{
	return __utf8("new-cpp.png");
}

void C3NewCPPClassCreator::run()
{
	C3NewCPPClassDialog oDlg;
	oDlg.exec();
}

