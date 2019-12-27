//=============================================================================
//
//   File : C3TextEditorModeBash.cpp
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

#include "C3TextEditorModeBash.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserBash.h"


C3TextEditorModeBash::C3TextEditorModeBash(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithInterLineState(pFactory,__utf8("bash"),__tr("Bash"),C3TextEditorModeParserBash::BashInterLineFlagMask)
{
	m_pParser = new C3TextEditorModeParserBash(parserState());
}

C3TextEditorModeBash::~C3TextEditorModeBash()
{
	delete m_pParser;
}

void C3TextEditorModeBash::computeMetadata()
{
	// we don't clear blocks here as the parser needs to access the previous state to cleanup
	m_pParser->computeMetadata();
}
