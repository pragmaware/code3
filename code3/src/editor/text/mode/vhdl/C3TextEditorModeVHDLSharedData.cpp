//=============================================================================
//
//   File : C3TextEditorModeVHDLSharedData.cpp
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

#include "C3TextEditorModeVHDLSharedData.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3SettingsUserHighlightingSet.h"

C3TextEditorModeVHDLSharedData::C3TextEditorModeVHDLSharedData(C3TextEditorFactory * pFactory)
	: m_pFactory(pFactory),
	m_pOptions(pFactory->options())
{
	QObject::connect(C3MainWindow::instance(),SIGNAL(workspacePropertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
}

C3TextEditorModeVHDLSharedData::~C3TextEditorModeVHDLSharedData()
{
}

void C3TextEditorModeVHDLSharedData::slotWorkspacePropertiesChanged()
{
	rebuildColorHashes();
}

void C3TextEditorModeVHDLSharedData::rebuildColorHashes()
{
	m_hIdentifierColorHash.clear();

#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)
#define BUILTIN_TYPE(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinTypeTextColor)

	KEYWORD("abs");
	KEYWORD("access");
	KEYWORD("after");
	KEYWORD("alias");
	KEYWORD("all");
	KEYWORD("and");
	KEYWORD("architecture");
	KEYWORD("array");
	KEYWORD("assert");
	KEYWORD("attribute");
	KEYWORD("begin");
	KEYWORD("block");
	KEYWORD("body");
	KEYWORD("buffer");
	KEYWORD("bus");
	KEYWORD("case");
	KEYWORD("component");
	KEYWORD("configuration");
	KEYWORD("constant");
	KEYWORD("disconnect");
	KEYWORD("downto");
	KEYWORD("else");
	KEYWORD("elsif");
	KEYWORD("end");
	KEYWORD("entity");
	KEYWORD("exit");
	KEYWORD("file");
	KEYWORD("for");
	KEYWORD("function");
	KEYWORD("generate");
	KEYWORD("generic");
	KEYWORD("group");
	KEYWORD("guarded");
	KEYWORD("if");
	KEYWORD("impure");
	KEYWORD("in");
	KEYWORD("inertial");
	KEYWORD("inout");
	KEYWORD("is");
	KEYWORD("label");
	KEYWORD("library");
	KEYWORD("linkage");
	KEYWORD("literal");
	KEYWORD("loop");
	KEYWORD("map");
	KEYWORD("mod");
	KEYWORD("nand");
	KEYWORD("new");
	KEYWORD("next");
	KEYWORD("nor");
	KEYWORD("not");
	KEYWORD("null");
	KEYWORD("of");
	KEYWORD("on");
	KEYWORD("open");
	KEYWORD("or");
	KEYWORD("others");
	KEYWORD("out");
	KEYWORD("package");
	KEYWORD("port");
	KEYWORD("postponed");
	KEYWORD("procedure");
	KEYWORD("process");
	KEYWORD("pure");
	KEYWORD("range");
	KEYWORD("record");
	KEYWORD("register");
	KEYWORD("reject");
	KEYWORD("rem");
	KEYWORD("report");
	KEYWORD("return");
	KEYWORD("rol");
	KEYWORD("ror");
	KEYWORD("select");
	KEYWORD("severity");
	KEYWORD("signal");
	KEYWORD("shared");
	KEYWORD("sla");
	KEYWORD("sll");
	KEYWORD("sra");
	KEYWORD("srl");
	KEYWORD("subtype");
	KEYWORD("then");
	KEYWORD("to");
	KEYWORD("transport");
	KEYWORD("type");
	KEYWORD("unaffected");
	KEYWORD("units");
	KEYWORD("until");
	KEYWORD("use");
	KEYWORD("variable");
	KEYWORD("wait");
	KEYWORD("when");
	KEYWORD("while");
	KEYWORD("with");
	KEYWORD("xnor");
	KEYWORD("xor");
	
	BUILTIN_TYPE("std_logic");
	BUILTIN_TYPE("std_logic_vector");
	BUILTIN_TYPE("integer");
	BUILTIN_TYPE("unsigned");
	BUILTIN_TYPE("positive");
	BUILTIN_TYPE("natural");
	BUILTIN_TYPE("bit");
	BUILTIN_TYPE("boolean");
	BUILTIN_TYPE("severity_level");
	BUILTIN_TYPE("character");
	BUILTIN_TYPE("real");
	BUILTIN_TYPE("string");
	BUILTIN_TYPE("bit_vector");
	
	BUILTIN_OBJECT("TRUE");
	BUILTIN_OBJECT("FALSE");
	BUILTIN_OBJECT("NOTE");
	BUILTIN_OBJECT("WARNING");
	BUILTIN_OBJECT("ERROR");
	BUILTIN_OBJECT("FAILURE");

	// ??????
	BUILTIN_OBJECT("NUL");
	BUILTIN_OBJECT("SOH");
	BUILTIN_OBJECT("DEL");
	// ??????
}
