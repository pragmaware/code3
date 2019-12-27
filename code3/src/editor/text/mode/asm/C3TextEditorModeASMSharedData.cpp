//=============================================================================
//
//   File : C3TextEditorModeASMSharedData.cpp
//   Creation Date : 2015/11/18 00:48:16
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

#include "C3TextEditorModeASMSharedData.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3SettingsUserHighlightingSet.h"

C3TextEditorModeASMSharedData::C3TextEditorModeASMSharedData(C3TextEditorFactory * pFactory)
	: m_pFactory(pFactory),
	m_pOptions(pFactory->options())
{
	QObject::connect(C3MainWindow::instance(),SIGNAL(workspacePropertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
}

C3TextEditorModeASMSharedData::~C3TextEditorModeASMSharedData()
{
}

void C3TextEditorModeASMSharedData::slotWorkspacePropertiesChanged()
{
	rebuildColorHashes();
}

void C3TextEditorModeASMSharedData::rebuildColorHashes()
{
	m_hIdentifierColorHash.clear();
	m_hPreprocessorIdentifierColorHash.clear();
	m_hIdentifierPrefixColorHash.clear();
	m_hIdentifierPrefixWithUppercaseColorHash.clear();
	m_hIdentifierPostfixColorHash.clear();

#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_pOptions->_oColor))

#define PREPROCESSOR_IDENTIFIER(_szIdentifier) \
	m_hPreprocessorIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_pOptions->oPreprocessorTextColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)
#define BUILTIN_TYPE(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinTypeTextColor)

	//KEYWORD("section");

	//BUILTIN_TYPE("bool");

	// preprocessor: if elif else endif defined ifdef ifndef define undef include line error pragma
	
	PREPROCESSOR_IDENTIFIER("if");
	PREPROCESSOR_IDENTIFIER("elif");
	PREPROCESSOR_IDENTIFIER("else");
	PREPROCESSOR_IDENTIFIER("endif");
	PREPROCESSOR_IDENTIFIER("defined");
	PREPROCESSOR_IDENTIFIER("ifdef");
	PREPROCESSOR_IDENTIFIER("ifndef");
	PREPROCESSOR_IDENTIFIER("define");
	PREPROCESSOR_IDENTIFIER("undef");
	PREPROCESSOR_IDENTIFIER("include");
	PREPROCESSOR_IDENTIFIER("line");
	PREPROCESSOR_IDENTIFIER("error");
	PREPROCESSOR_IDENTIFIER("pragma");
	
	// also _Pragma ?
	
	// It seems that also anything that contains a double underscore __ in *any* position
	// and anything that starts with an underscore followed by an uppercase letter is reserved
	// to be used as name in global namespace (?)
	
	// also std?

	//m_hIdentifierPrefixColorHash.insert(__utf8("_"),&(m_pOptions->oUnderscoreIdentifierTextColor));

	//m_hIdentifierPrefixColorHash.insert(__utf8("m_"),&(m_pOptions->oMUnderscoreIdentifierTextColor));
	//m_hIdentifierPrefixColorHash.insert(__utf8("g_"),&(m_pOptions->oGUnderscoreIdentifierTextColor));
}
