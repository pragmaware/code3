﻿//=============================================================================
//
//   File : C3TextEditorModeCMake.cpp
//   Creation Date : gio 29 ott 2015 02:50:45
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

#include "C3TextEditorModeCMake.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserCMake.h"


C3TextEditorModeCMake::C3TextEditorModeCMake(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithInterLineState(pFactory,__utf8("cmake"),__tr("CMake"),C3TextEditorModeParserCMake::CMakeInterLineFlagMask)
{
	m_pParser = new C3TextEditorModeParserCMake(parserState());
}

C3TextEditorModeCMake::~C3TextEditorModeCMake()
{
	delete m_pParser;
}

void C3TextEditorModeCMake::computeMetadata()
{
	m_pParser->computeMetadata();
}
