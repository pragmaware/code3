//=============================================================================
//
//   File : C3TextEditorModePHP.cpp
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

#include "C3TextEditorModePHP.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserXML.h"
#include "C3TextEditorModeParserPHP.h"

#include "C3TextEditorSymbolContext.h"
#include "C3TextEditor.h"


C3TextEditorModePHP::C3TextEditorModePHP(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithSymbolAnalysis(
			pFactory,
			__utf8("php"),
			__tr("PHP"),
			C3TextEditorModeParserPHP::PHPInterLineFlagMask,
			C3Symbol::Php
		)
{
	m_pParser = new C3TextEditorModeParserXML(
			parserState(),
			C3TextEditorModeParserXML::EnablePHPHandling | C3TextEditorModeParserXML::EnableStyleTagHandling | C3TextEditorModeParserXML::EnableScriptTagHandling
		);
}

C3TextEditorModePHP::~C3TextEditorModePHP()
{
	delete m_pParser;
}

void C3TextEditorModePHP::computeMetadata()
{
	m_pParser->computeMetadata();
}

