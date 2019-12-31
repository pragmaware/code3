//=============================================================================
//
//   File : C3TextEditorModeCPPSharedData.cpp
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

#include "C3TextEditorModeCPPSharedData.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3SettingsUserHighlightingSet.h"

C3TextEditorModeCPPSharedData::C3TextEditorModeCPPSharedData(C3TextEditorFactory * pFactory)
	: m_pFactory(pFactory),
	m_pOptions(pFactory->options())
{
	QObject::connect(C3MainWindow::instance(),SIGNAL(workspacePropertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
}

C3TextEditorModeCPPSharedData::~C3TextEditorModeCPPSharedData()
{
}

void C3TextEditorModeCPPSharedData::slotWorkspacePropertiesChanged()
{
	rebuildColorHashes();
}

void C3TextEditorModeCPPSharedData::rebuildColorHashes()
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

#define KEYWORD(_szKeyword) \
		m_hLanguageKeywords.insert(_szKeyword,1); \
		IDENTIFIER(_szKeyword,oKeywordTextColor)

#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)
#define BUILTIN_TYPE(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinTypeTextColor)

	KEYWORD("alignas");
	KEYWORD("alignof");
	KEYWORD("and");
	KEYWORD("and_eq");
	KEYWORD("asm");
	KEYWORD("auto");
	KEYWORD("bitand");
	KEYWORD("bitor");
	KEYWORD("break");
	KEYWORD("case");
	KEYWORD("catch");
	KEYWORD("compl");
	KEYWORD("const");
	KEYWORD("constexpr");
	KEYWORD("const_cast");
	KEYWORD("continue");
	KEYWORD("decltype");
	KEYWORD("default");
	KEYWORD("delete");
	KEYWORD("do");
	KEYWORD("dynamic_cast");
	KEYWORD("else");
	KEYWORD("explicit");
	KEYWORD("export");
	KEYWORD("extern");
	KEYWORD("false");
	KEYWORD("final"); // not properly keyword
	KEYWORD("for");
	KEYWORD("friend");
	KEYWORD("goto");
	KEYWORD("if");
	KEYWORD("inline");
	KEYWORD("mutable");
	KEYWORD("namespace");
	KEYWORD("new");
	KEYWORD("noexcept");
	KEYWORD("not");
	KEYWORD("not_eq");
	KEYWORD("nullptr");
	KEYWORD("operator");
	KEYWORD("or");
	KEYWORD("or_eq");
	KEYWORD("override"); // not properly keyword
	KEYWORD("private");
	KEYWORD("protected");
	KEYWORD("public");
	KEYWORD("register");
	KEYWORD("reinterpret_cast");
	KEYWORD("return");
	KEYWORD("sizeof");
	KEYWORD("static");
	KEYWORD("static_assert");
	KEYWORD("static_cast");
	KEYWORD("switch");
	KEYWORD("template");
	KEYWORD("this");
	KEYWORD("thread_local");
	KEYWORD("throw");
	KEYWORD("true");
	KEYWORD("try");
	KEYWORD("typedef");
	KEYWORD("typeid");
	KEYWORD("typename");
	KEYWORD("union");
	KEYWORD("using");
	KEYWORD("virtual");
	KEYWORD("volatile");
	KEYWORD("while");
	KEYWORD("xor");
	KEYWORD("xor_eq");	

	BUILTIN_TYPE("bool");
	BUILTIN_TYPE("class");
	BUILTIN_TYPE("char");
	BUILTIN_TYPE("char16_t");
	BUILTIN_TYPE("char32_t");
	BUILTIN_TYPE("double");
	BUILTIN_TYPE("enum");
	BUILTIN_TYPE("float");
	BUILTIN_TYPE("int");
	BUILTIN_TYPE("long");
	BUILTIN_TYPE("short");
	BUILTIN_TYPE("signed");
	BUILTIN_TYPE("struct");
	BUILTIN_TYPE("unsigned");
	BUILTIN_TYPE("wchar_t");
	BUILTIN_TYPE("void");

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

	DO_ADDITIONAL(pSet->subsetIfPresent("cppUserKeywords"),oUserKeywordTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("cppUserTypes"),oUserTypeTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("cppUserObjects"),oUserObjectTextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("cppUserCustom1"),oUserCustom1TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("cppUserCustom2"),oUserCustom2TextColor);
	DO_ADDITIONAL(pSet->subsetIfPresent("cppUserCustom3"),oUserCustom3TextColor);

}

void C3TextEditorModeCPPSharedData::setDefaultUserHighlightingSet(C3SettingsUserHighlightingSet * pSet)
{
	// Fixme: maybe grep around files to figure out if we have Qt, Windows etc...

	QStringList * sl = pSet->subset("cppUserKeywords");

	sl->append("# Keyword extensions that are almost always available");
	sl->append("FALSE");
	sl->append("TRUE");
	sl->append("NULL");

	sl->append("# Qt keyword extensions");
	sl->append("emit");
	sl->append("signals");
	sl->append("slots");
	sl->append("foreach");
	sl->append("# CC keyword extensions");
	sl->append("cc_foreach");

	sl = pSet->subset("cppUserTypes");

	sl->append("# Types defined by C standard library");
	sl->append("*_t");

	sl->append("# Non standard types that are almost always available");
	sl->append("uchar");
	sl->append("uint");
	sl->append("ushort");
	sl->append("ulong");
	sl->append("ulonglong");

	sl->append("# Qt type extensions");
	sl->append("quint8");
	sl->append("quint16");
	sl->append("quint32");
	sl->append("quint64");
	sl->append("qint8");
	sl->append("qint16");
	sl->append("qint32");
	sl->append("qint64");
	sl->append("qlong");
	sl->append("qlonglong");
	sl->append("qulong");
	sl->append("qulonglong");
	sl->append("qreal");

	sl->append("# Windows types");
	sl->append("APIENTRY");
	sl->append("ATOM");
	sl->append("BOOL");
	sl->append("BOOLEAN");
	sl->append("BYTE");
	sl->append("CALLBACK");
	sl->append("CCHAR");
	sl->append("CHAR");
	sl->append("COLORREF");
	sl->append("CONST");
	sl->append("DWORD");
	sl->append("DWORDLONG");
	sl->append("DWORD_PTR");
	sl->append("DWORD32");
	sl->append("DWORD64");
	sl->append("FLOAT");
	sl->append("HACCEL");
	sl->append("HALF_PTR");
	sl->append("HANDLE");
	sl->append("HBITMAP");
	sl->append("HBRUSH");
	sl->append("HCOLORSPACE");
	sl->append("HCONV");
	sl->append("HCONVLIST");
	sl->append("HCURSOR");
	sl->append("HDC");
	sl->append("HDDEDATA");
	sl->append("HDESK");
	sl->append("HDROP");
	sl->append("HDWP");
	sl->append("HDENHMETAFILE");
	sl->append("HFILE");
	sl->append("HFONT");
	sl->append("HGDIOBJ");
	sl->append("HGLOBAL");
	sl->append("HHOOK");
	sl->append("HICON");
	sl->append("HINSTANCE");
	sl->append("HKEY");
	sl->append("HKL");
	sl->append("HLOCAL");
	sl->append("HMENU");
	sl->append("HMETAFILE");
	sl->append("HMODULE");
	sl->append("HMONITOR");
	sl->append("HPALETTE");
	sl->append("HPEN");
	sl->append("HRESULT");
	sl->append("HRGN");
	sl->append("HRSRC");
	sl->append("HSZ");
	sl->append("HWINSTA");
	sl->append("HWND");
	sl->append("INT");
	sl->append("INT_PTR");
	sl->append("INT8");
	sl->append("INT16");
	sl->append("INT32");
	sl->append("INT64");
	sl->append("LANGID");
	sl->append("LCID");
	sl->append("LCTYPE");
	sl->append("LGRPID");
	sl->append("LONG");
	sl->append("LONGLONG");
	sl->append("LONG_PTR");
	sl->append("LONG32");
	sl->append("LONG64");
	sl->append("LPARAM");
	sl->append("LPBOOL");
	sl->append("LPBYTE");
	sl->append("LPCOLORREF");
	sl->append("LPCSTR");
	sl->append("LPCTSTR");
	sl->append("LPCVOID");
	sl->append("LPCWSTR");
	sl->append("LPDWORD");
	sl->append("LPHANDLE");
	sl->append("LPINT");
	sl->append("LPLONG");
	sl->append("LPSTR");
	sl->append("LPTSTR");
	sl->append("LPVOID");
	sl->append("LPWORD");
	sl->append("LPWSTR");
	sl->append("LRESULT");
	sl->append("PBOOL");
	sl->append("PBYTE");
	sl->append("PCHAR");
	sl->append("PCSTR");
	sl->append("PCTSTR");
	sl->append("PCWSTR");
	sl->append("PDWORD");
	sl->append("PDWORDLONG");
	sl->append("PDWORD_PTR");
	sl->append("PDWORD32");
	sl->append("PDWORD64");
	sl->append("PFLOAT");
	sl->append("PHALF_PTR");
	sl->append("PHANDLE");
	sl->append("PHKEY");
	sl->append("PINT");
	sl->append("PINT_PTR");
	sl->append("PINT8");
	sl->append("PINT16");
	sl->append("PINT32");
	sl->append("PINT64");
	sl->append("PLCID");
	sl->append("PLONG");
	sl->append("PLONGLONG");
	sl->append("PLONG_PTR");
	sl->append("PLONG32");
	sl->append("PLONG64");
	sl->append("POINTER_32");
	sl->append("POINTER_64");
	sl->append("POINTER_SIGNED");
	sl->append("POINTER_UNSIGNED");
	sl->append("PSHORT");
	sl->append("PSIZE_T");
	sl->append("PSSIZE_T");
	sl->append("PSTR");
	sl->append("PTBYTE");
	sl->append("PTCHAR");
	sl->append("PTSTR");
	sl->append("PUCHAR");
	sl->append("PUHALF_PTR");
	sl->append("PUINT");
	sl->append("PUINT_PTR");
	sl->append("PUINT8");
	sl->append("PUINT16");
	sl->append("PUINT32");
	sl->append("PUINT64");
	sl->append("PULONG");
	sl->append("PULONGLONG");
	sl->append("PULONG_PTR");
	sl->append("PULONG32");
	sl->append("PULONG64");
	sl->append("PUSHORT");
	sl->append("PVOID");
	sl->append("PWCHAR");
	sl->append("PWORD");
	sl->append("PWSTR");
	sl->append("QWORD");
	sl->append("SC_HANDLE");
	sl->append("SC_LOCK");
	sl->append("SERVICE_STATUS_HANDLE");
	sl->append("SHORT");
	sl->append("SIZE_T");
	sl->append("SSIZE_T");
	sl->append("TBYTE");
	sl->append("TCHAR");
	sl->append("UCHAR");
	sl->append("UHALD_PTR");
	sl->append("UINT");
	sl->append("UINT_PTR");
	sl->append("UINT8");
	sl->append("UINT16");
	sl->append("UINT32");
	sl->append("UINT64");
	sl->append("ULONG");
	sl->append("ULONGLONG");
	sl->append("ULONG_PTR");
	sl->append("ULONG32");
	sl->append("ULONG64");
	sl->append("ULONG32");
	sl->append("UNICODE_STRING");
	sl->append("USHORT");
	sl->append("USN");
	sl->append("VOID");
	sl->append("WCHAR");
	sl->append("WINAPI");
	sl->append("WORD");
	sl->append("WPARAM");

	sl = pSet->subset("cppUserObjects");

	sl->append("# Qt classes");
	sl->append("Q^");
	sl->append("Qt");
	sl->append("Qt^");

	sl->append("# KDE classes");
	sl->append("K^");

	sl->append("# MFC classes");
	sl->append("C^");

	sl = pSet->subset("cppUserCustom1");

	sl->append("# SS^ classes");
	sl->append("SS^");

	sl = pSet->subset("cppUserCustom2");

	sl->append("# Cc^ classes");
	sl->append("Cc");
	sl->append("Cc^");

	sl = pSet->subset("cppUserCustom3");
	sl->append("# Qt macros");
	sl->append("SIGNAL");
	sl->append("SLOT");
	sl->append("Q_^");
	sl->append("QT_^");
	sl->append("# CC macros");
	sl->append("CC_^");
	sl->append("CCUI_^");
	sl->append("CCMATH_^");
	sl->append("CCDB_^");
	sl->append("CCNET_^");

}