//=============================================================================
//
//   File : C3TextEditorModeChatScript.cpp
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

#include "C3TextEditorModeChatScript.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserChatScript.h"


C3TextEditorModeChatScript::C3TextEditorModeChatScript(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithInterLineState(pFactory,__utf8("chatscript"),__tr("ChatScript"),C3TextEditorModeParserChatScript::ChatScriptInterLineFlagMask)
{
	m_pParser = new C3TextEditorModeParserChatScript(parserState());
}

C3TextEditorModeChatScript::~C3TextEditorModeChatScript()
{
	delete m_pParser;
}

void C3TextEditorModeChatScript::computeMetadata()
{
	m_pParser->computeMetadata();
}
