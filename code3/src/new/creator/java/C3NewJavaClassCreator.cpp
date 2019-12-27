//=============================================================================
//
//   File : C3NewJavaClassCreator.cpp
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
//   of the License, or (at your option) any later version
//
//=============================================================================

#include "C3NewJavaClassCreator.h"

#include "C3NewJavaClassDialog.h"

C3NewJavaClassCreator::C3NewJavaClassCreator()
	: C3NewObjectCreator()
{
}

C3NewJavaClassCreator::~C3NewJavaClassCreator()
{
}

QString C3NewJavaClassCreator::name()
{
	return __tr("Java Class");
}

QString C3NewJavaClassCreator::icon()
{
	return __utf8("new-java.png");
}

void C3NewJavaClassCreator::run()
{
	C3NewJavaClassDialog oDlg;
	oDlg.exec();
}

