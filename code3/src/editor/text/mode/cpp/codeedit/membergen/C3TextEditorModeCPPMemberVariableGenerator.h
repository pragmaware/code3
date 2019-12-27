#ifndef _C3TextEditorModeCPPMemberVariableGenerator_h_
#define _C3TextEditorModeCPPMemberVariableGenerator_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPMemberVariableGenerator.h
//   Creation Date : 2016/09/13 00:57:33
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

#include "c3_config.h"

#include <QString>

#include "C3Symbol.h"
#include "C3TextEditorModeCPPMemberGenerator.h"

class C3TextEditorModeCPPMemberVariableGeneratorPrivate;
class C3TextEditor;
class C3SymbolManagerClassDescription;

class C3TextEditorModeCPPMemberVariableGenerator : public C3TextEditorModeCPPMemberGenerator
{
public:

	class Settings
	{
	public:
		QString szTypeName;
		QString szMemberName;
		C3Symbol::AccessLevel eAccessLevel;
		QString szComment;
		
		bool bGenerateGetter;
		QString szSetterName;
		bool bGetterReturnsReference;
		bool bGetterReturnsConst;
		bool bGetterIsConst;
		C3Symbol::AccessLevel eGetterAccessLevel;
		
		bool bGenerateSetter;
		QString szGetterName;
		bool bSetterAcceptsReference;
		bool bSetterAcceptsConst;
		C3Symbol::AccessLevel eSetterAccessLevel;
		
		Location eLocation;
		C3SymbolManagerClassDescription * pClassDescription;
	};

public:
	C3TextEditorModeCPPMemberVariableGenerator(C3TextEditor * pEditor,Settings * pSettings);
	~C3TextEditorModeCPPMemberVariableGenerator();

public:
	bool run();

	
private:
	C3TextEditorModeCPPMemberVariableGeneratorPrivate * m_pC3TEMCPPMG;

}; // class C3TextEditorModeCPPMemberVariableGenerator

#endif //!_C3TextEditorModeCPPMemberVariableGenerator_h_