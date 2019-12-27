//=============================================================================
//
//   File : C3TextEditorModePHPSharedData.cpp
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

#include "C3TextEditorModePHPSharedData.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3SettingsUserHighlightingSet.h"

C3TextEditorModePHPSharedData::C3TextEditorModePHPSharedData(C3TextEditorFactory * pFactory)
	: m_pFactory(pFactory),
	m_pOptions(pFactory->options())
{
	QObject::connect(C3MainWindow::instance(),SIGNAL(workspacePropertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
}

C3TextEditorModePHPSharedData::~C3TextEditorModePHPSharedData()
{
}

void C3TextEditorModePHPSharedData::slotWorkspacePropertiesChanged()
{
	rebuildColorHashes();
}

void C3TextEditorModePHPSharedData::rebuildColorHashes()
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

#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_pOptions->_oColor))

	KEYWORD("__halt_compiler");
	KEYWORD("abstract");
	KEYWORD("and");
	KEYWORD("array");
	KEYWORD("as");
	KEYWORD("break");
	KEYWORD("callable");
	KEYWORD("case");
	KEYWORD("catch");
	KEYWORD("class");
	KEYWORD("clone");
	KEYWORD("const");
	KEYWORD("continue");
	KEYWORD("declare");
	KEYWORD("default");
	KEYWORD("die");
	KEYWORD("do");
	KEYWORD("echo");
	KEYWORD("else");
	KEYWORD("elseif");
	KEYWORD("empty");
	KEYWORD("enddeclare");
	KEYWORD("endfor");
	KEYWORD("endforeach");
	KEYWORD("endif");
	KEYWORD("endswitch");
	KEYWORD("endwhile");
	KEYWORD("eval");
	KEYWORD("exit");
	KEYWORD("extends");
	KEYWORD("final");
	KEYWORD("finally");
	KEYWORD("for");
	KEYWORD("foreach");
	KEYWORD("function");
	KEYWORD("global");
	KEYWORD("goto");
	KEYWORD("if");
	KEYWORD("implements");
	KEYWORD("include");
	KEYWORD("include_once");
	KEYWORD("instanceof");
	KEYWORD("insteadof");
	KEYWORD("interface");
	KEYWORD("isset");
	KEYWORD("list");
	KEYWORD("namespace");
	KEYWORD("new");
	KEYWORD("or");
	KEYWORD("print");
	KEYWORD("private");
	KEYWORD("protected");
	KEYWORD("public");
	KEYWORD("require");
	KEYWORD("require_once");
	KEYWORD("return");
	KEYWORD("static");
	KEYWORD("switch");
	KEYWORD("throw");
	KEYWORD("trait");
	KEYWORD("try");
	KEYWORD("unset");
	KEYWORD("use");
	KEYWORD("var");
	KEYWORD("while");
	KEYWORD("xor");
	KEYWORD("yield");

	// Reserved words (but don't seem to be proper keywords?)
	// As of PHP7 they seem to be errors (?)
	BUILTIN_TYPE("int");
	BUILTIN_TYPE("float");
	BUILTIN_TYPE("double");
	BUILTIN_TYPE("bool");
	BUILTIN_TYPE("string");
	BUILTIN_TYPE("resource");
	BUILTIN_TYPE("object");
	BUILTIN_TYPE("mixed");
	BUILTIN_TYPE("numeric");

	// case insensitive!
	KEYWORD("true");
	KEYWORD("false");
	KEYWORD("null");
	KEYWORD("TRUE");
	KEYWORD("FALSE");
	KEYWORD("NULL");
	KEYWORD("True");
	KEYWORD("False");
	KEYWORD("Null");

	KEYWORD("self"); // not properly a keyword? only in class contex?
	KEYWORD("parent"); // not properly a keyword? only in class contex?

	KEYWORD("__CLASS__");
	KEYWORD("__DIR__");
	KEYWORD("__FILE__");
	KEYWORD("__FUNCTION__");
	KEYWORD("__LINE__");
	KEYWORD("__METHOD__");
	KEYWORD("__NAMESPACE__");
	KEYWORD("__TRAIT__");

	// FIXME: Predefined variables $_SERVER $_GET $_POST $_SESSION etc...

	BUILTIN_OBJECT("$_SERVER");
	BUILTIN_OBJECT("$_GET");
	BUILTIN_OBJECT("$_POST");
	BUILTIN_OBJECT("$_REQUEST");
	BUILTIN_OBJECT("$_SESSION");
	BUILTIN_OBJECT("$GLOBALS");

	KEYWORD("$this");

	//m_hIdentifierPrefixColorHash.insert(__utf8("_"),&(m_pOptions->oUnderscoreIdentifierTextColor));

	m_hIdentifierPrefixColorHash.insert(__utf8("m_"),&(m_pOptions->oMUnderscoreIdentifierTextColor));
	m_hIdentifierPrefixColorHash.insert(__utf8("$m_"),&(m_pOptions->oMUnderscoreIdentifierTextColor));
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

	DO_ADDITIONAL(pSet->subsetIfPresent("phpUserKeywords"),oUserKeywordTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("phpUserTypes"),oUserTypeTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("phpUserObjects"),oUserObjectTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("phpUserCustom1"),oUserCustom1TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("phpUserCustom2"),oUserCustom2TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("phpUserCustom3"),oUserCustom3TextColor);

}

void C3TextEditorModePHPSharedData::setDefaultUserHighlightingSet(C3SettingsUserHighlightingSet * pSet)
{
	// Fixme: maybe grep around files to figure out if we have Qt, Windows etc...

	QStringList * sl = pSet->subset("phpUserObjects");

	sl->append("# STS classes");
	sl->append("STS^");
	sl->append("SS^");

}