//=============================================================================
//
//   File : C3TextEditorModeRustSharedData.rust
//   Creation Date : 18/05/2026 21:49
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

#include "C3TextEditorModeRustSharedData.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3SettingsUserHighlightingSet.h"

C3TextEditorModeRustSharedData::C3TextEditorModeRustSharedData(C3TextEditorFactory * pFactory)
	: m_pFactory(pFactory),
	m_pOptions(pFactory->options())
{
	QObject::connect(C3MainWindow::instance(),SIGNAL(workspacePropertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
}

C3TextEditorModeRustSharedData::~C3TextEditorModeRustSharedData()
{
}

void C3TextEditorModeRustSharedData::slotWorkspacePropertiesChanged()
{
	rebuildColorHashes();
}

void C3TextEditorModeRustSharedData::rebuildColorHashes()
{
	m_hIdentifierColorHash.clear();
	m_hIdentifierPrefixColorHash.clear();
	m_hIdentifierPrefixWithUppercaseColorHash.clear();
	m_hIdentifierPostfixColorHash.clear();

#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define USER_KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oUserKeywordTextColor)

#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)
#define BUILTIN_TYPE(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinTypeTextColor)

	KEYWORD("_");
	KEYWORD("as");
	KEYWORD("async");
	KEYWORD("await");
	KEYWORD("break");
	KEYWORD("const");
	KEYWORD("continue");
	KEYWORD("crate");
	KEYWORD("dyn");
	KEYWORD("else");
	KEYWORD("enum");
	KEYWORD("extern");
	KEYWORD("false");
	KEYWORD("fn");
	KEYWORD("for");
	KEYWORD("if");
	KEYWORD("impl");
	KEYWORD("in");
	KEYWORD("let");
	KEYWORD("loop");
	KEYWORD("match");
	KEYWORD("mod");
	KEYWORD("move");
	KEYWORD("mut");
	KEYWORD("pub");
	KEYWORD("ref");
	KEYWORD("return");
	KEYWORD("self");
	KEYWORD("Self");
	KEYWORD("static");
	KEYWORD("struct");
	KEYWORD("super");
	KEYWORD("trait");
	KEYWORD("true");
	KEYWORD("type");
	KEYWORD("unsafe");
	KEYWORD("use");
	KEYWORD("where");
	KEYWORD("while");
	
	// reserved for future use
	USER_KEYWORD("abstract");
	USER_KEYWORD("become");
	USER_KEYWORD("box");
	USER_KEYWORD("do");
	USER_KEYWORD("final");
	USER_KEYWORD("gen");
	USER_KEYWORD("macro");
	USER_KEYWORD("override");
	USER_KEYWORD("priv");
	USER_KEYWORD("try");
	USER_KEYWORD("typeof");
	USER_KEYWORD("unsized");
	USER_KEYWORD("virtual");
	USER_KEYWORD("yield");
	
	// reserved in certain contexts
	USER_KEYWORD("macro_rules");
	USER_KEYWORD("raw");
	USER_KEYWORD("safe");
	USER_KEYWORD("union");

	BUILTIN_TYPE("i8");
	BUILTIN_TYPE("u8");
	BUILTIN_TYPE("i16");
	BUILTIN_TYPE("u16");
	BUILTIN_TYPE("i32");
	BUILTIN_TYPE("u32");
	BUILTIN_TYPE("i64");
	BUILTIN_TYPE("u64");
	BUILTIN_TYPE("i128");
	BUILTIN_TYPE("u128");
	BUILTIN_TYPE("isize");
	BUILTIN_TYPE("usize");
	BUILTIN_TYPE("f32");
	BUILTIN_TYPE("f64");
	BUILTIN_TYPE("bool");
	BUILTIN_TYPE("char");

	BUILTIN_OBJECT("true");
	BUILTIN_OBJECT("false");

	/*
	m_hIdentifierPrefixColorHash.insert(__utf8("_"),&(m_pOptions->oUnderscoreIdentifierTextColor));
	m_hIdentifierPrefixColorHash.insert(__utf8("m_"),&(m_pOptions->oMUnderscoreIdentifierTextColor));
	m_hIdentifierPrefixColorHash.insert(__utf8("g_"),&(m_pOptions->oGUnderscoreIdentifierTextColor));
	*/

#define DO_ADDITIONAL(_pIdentifiers,_oColor) \
	do { \
		QStringList * pIdentifiers = _pIdentifiers; \
		if(pIdentifiers) \
			foreach(QString szIdentifier,*_pIdentifiers) \
			{ \
				if(szIdentifier.isEmpty()) \
				{ \
					continue; \
				} \
				if(szIdentifier.startsWith('#')) \
				{ \
					continue; \
				} \
				if(szIdentifier.length() < 2) \
				{ \
					m_hIdentifierColorHash.insert(szIdentifier,&(m_pOptions->_oColor)); \
					continue; \
				} \
				\
				int idx = szIdentifier.indexOf(QChar('*')); \
				if(idx == 0) \
				{ \
					m_hIdentifierPostfixColorHash.insert(szIdentifier.mid(1),&(m_pOptions->_oColor)); \
					continue; \
				} \
				if(idx == (szIdentifier.length() - 1)) \
				{ \
					m_hIdentifierPrefixColorHash.insert(szIdentifier.left(szIdentifier.length() - 1),&(m_pOptions->_oColor)); \
					continue; \
				} \
				idx = szIdentifier.indexOf(QChar('^')); \
				if(idx == (szIdentifier.length() - 1)) \
				{ \
					m_hIdentifierPrefixWithUppercaseColorHash.insert(szIdentifier.left(szIdentifier.length() - 1),&(m_pOptions->_oColor)); \
					continue; \
				} \
				\
				m_hIdentifierColorHash.insert(szIdentifier,&(m_pOptions->_oColor)); \
			} \
	} while(0)

	C3SettingsUserHighlightingSet * pSet = C3Workspace::currentWorkspace()->userHighlightingSet();

	DO_ADDITIONAL(pSet->subsetIfPresent("rustUserKeywords"),oUserKeywordTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("rustUserTypes"),oUserTypeTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("rustUserObjects"),oUserObjectTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("rustUserCustom1"),oUserCustom1TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("rustUserCustom2"),oUserCustom2TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("rustUserCustom3"),oUserCustom3TextColor);

}

void C3TextEditorModeRustSharedData::setDefaultUserHighlightingSet(C3SettingsUserHighlightingSet * pSet)
{

}
