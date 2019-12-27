//=============================================================================
//
//   File : C3TextEditorFormat.cpp
//   Creation Date : gio 22 ott 2015 22:44:21
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
//   This file is part of the Code 3 Editor distribution
//   Copyright (C) 2015-2019 Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 3
//   of the License, or (at your option) any later version.

//
//=============================================================================

#include "C3TextEditorFormat.h"

C3TextEditorFormat::C3TextEditorFormat(const QString &szId,const QString &szName)
	: C3EditorFormat(szId,szName,__utf8("*"))
{
}

C3TextEditorFormat::~C3TextEditorFormat()
{
}

