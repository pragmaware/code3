//=============================================================================
//
//   File : C3TextEditorModeJavascript.cpp
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

#include "C3TextEditorModeJavascript.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserJavascript.h"
#include "C3TextEditorSymbolContext.h"
#include "C3TextEditor.h"
#include "C3Symbol.h"

C3TextEditorModeJavascript::C3TextEditorModeJavascript(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithSymbolAnalysis(
			pFactory,
			__utf8("js"),
			__tr("Javascript"),
			C3TextEditorModeParserJavascript::JavascriptInterLineFlagMask,
			C3Symbol::Javascript
		)
{
	m_pParser = new C3TextEditorModeParserJavascript(parserState());
}

C3TextEditorModeJavascript::~C3TextEditorModeJavascript()
{
	delete m_pParser;
}

void C3TextEditorModeJavascript::computeMetadata()
{
	m_pParser->computeMetadata();
}

/*
void C3TextEditorModeJavascript::maybeCompletion(const C3TextEditorRowColumn &rc)
{
	C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

	if(!m_oParserState.pCoreData->pEditor->computeSymbolContext(rc,*pContext))
	{
		delete pContext;
		return;
	}

	//qDebug("Javascript symbol context is %s %s %s",pContext->szLeftText.toUtf8().data(),pContext->szLeftOperator.toUtf8().data(),pContext->szText.toUtf8().data());

	C3TextEditorModeWithSymbolAnalysis::symbolCompletion(pContext,C3Symbol::Javascript);
}
*/