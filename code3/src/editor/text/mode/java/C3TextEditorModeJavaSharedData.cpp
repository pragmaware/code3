//=============================================================================
//
//   File : C3TextEditorModeJavaSharedData.java
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

#include "C3TextEditorModeJavaSharedData.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3SettingsUserHighlightingSet.h"

C3TextEditorModeJavaSharedData::C3TextEditorModeJavaSharedData(C3TextEditorFactory * pFactory)
	: m_pFactory(pFactory),
	m_pOptions(pFactory->options())
{
	QObject::connect(C3MainWindow::instance(),SIGNAL(workspacePropertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
}

C3TextEditorModeJavaSharedData::~C3TextEditorModeJavaSharedData()
{
}

void C3TextEditorModeJavaSharedData::slotWorkspacePropertiesChanged()
{
	rebuildColorHashes();
}

void C3TextEditorModeJavaSharedData::rebuildColorHashes()
{
	m_hIdentifierColorHash.clear();
	m_hIdentifierPrefixColorHash.clear();
	m_hIdentifierPrefixWithUppercaseColorHash.clear();
	m_hIdentifierPostfixColorHash.clear();

#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)
#define BUILTIN_TYPE(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinTypeTextColor)

	KEYWORD("abstract");
	KEYWORD("assert");
	KEYWORD("break");
	KEYWORD("case");
	KEYWORD("catch");
	KEYWORD("const");
	KEYWORD("continue");
	KEYWORD("default");
	KEYWORD("do");
	KEYWORD("else");
	KEYWORD("extends");
	KEYWORD("final");
	KEYWORD("finally");
	KEYWORD("for");
	KEYWORD("goto");
	KEYWORD("if");
	KEYWORD("implements");
	KEYWORD("import");
	KEYWORD("instanceof");
	KEYWORD("interface");
	KEYWORD("native");
	KEYWORD("new");
	KEYWORD("package");
	KEYWORD("private");
	KEYWORD("protected");
	KEYWORD("public");
	KEYWORD("return");
	KEYWORD("static");
	KEYWORD("strictfp");
	KEYWORD("super");
	KEYWORD("switch");
	KEYWORD("synchronized");
	KEYWORD("this");
	KEYWORD("throw");
	KEYWORD("throws");
	KEYWORD("transient");
	KEYWORD("try");
	KEYWORD("volatile");
	KEYWORD("while");

	// reserved words for constants
	KEYWORD("false");
	KEYWORD("true");
	KEYWORD("null");


	BUILTIN_TYPE("boolean");
	BUILTIN_TYPE("byte");
	BUILTIN_TYPE("char");
	BUILTIN_TYPE("class");
	BUILTIN_TYPE("double");
	BUILTIN_TYPE("enum");
	BUILTIN_TYPE("float");
	BUILTIN_TYPE("int");
	BUILTIN_TYPE("long");
	BUILTIN_TYPE("short");
	BUILTIN_TYPE("void");


	m_hIdentifierPrefixColorHash.insert(__utf8("_"),&(m_pOptions->oUnderscoreIdentifierTextColor));
	m_hIdentifierPrefixColorHash.insert(__utf8("m_"),&(m_pOptions->oMUnderscoreIdentifierTextColor));
	m_hIdentifierPrefixColorHash.insert(__utf8("g_"),&(m_pOptions->oGUnderscoreIdentifierTextColor));

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

	DO_ADDITIONAL(pSet->subsetIfPresent("javaUserKeywords"),oUserKeywordTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("javaUserTypes"),oUserTypeTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("javaUserObjects"),oUserObjectTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("javaUserCustom1"),oUserCustom1TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("javaUserCustom2"),oUserCustom2TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("javaUserCustom3"),oUserCustom3TextColor);

}

void C3TextEditorModeJavaSharedData::setDefaultUserHighlightingSet(C3SettingsUserHighlightingSet * pSet)
{

}
