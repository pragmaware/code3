//=============================================================================
//
//   File : C3TextEditorModeCPPMemberVariableGenerator.cpp
//   Creation Date : 2016/09/13 00:57:33
//   Project : ssex
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

#include "C3TextEditorModeCPPMemberVariableGenerator.h"

#include "C3TextEditor.h"
#include "C3SymbolManagerClassDescription.h"
#include "C3Workspace.h"
#include "C3StringUtils.h"
#include "C3TextEditorModeCPPCodeGenerationUtils.h"
#include "C3TextEditorModeCPPMemberFunctionGenerator.h"

class C3TextEditorModeCPPMemberVariableGeneratorPrivate
{
public:
	C3TextEditor * pEditor;
	C3TextEditorModeCPPMemberVariableGenerator::Settings * pSettings;
};

#define _p m_pC3TEMCPPMG

C3TextEditorModeCPPMemberVariableGenerator::C3TextEditorModeCPPMemberVariableGenerator(C3TextEditor * pEditor,Settings * pSettings)
	: C3TextEditorModeCPPMemberGenerator()
{
	_p = new C3TextEditorModeCPPMemberVariableGeneratorPrivate();
	_p->pEditor = pEditor;

	_p->pSettings = pSettings;
}

C3TextEditorModeCPPMemberVariableGenerator::~C3TextEditorModeCPPMemberVariableGenerator()
{
	delete _p;
}

bool C3TextEditorModeCPPMemberVariableGenerator::run()
{
	QString szMember;
	szMember += _p->pSettings->szTypeName;
	szMember += __ascii(" ");
	szMember += _p->pSettings->szMemberName;
	szMember += __ascii(";");
	szMember += __ascii("\n");

	QString szComment;
	
	if(!_p->pSettings->szComment.isEmpty())
		szComment = __ascii("\n///\n/// %1\n///\n").arg(_p->pSettings->szComment);

	QString szText = C3StringUtils::indentLines(szComment + szMember);

	switch(_p->pSettings->eLocation)
	{
		case AtCursorPosition:
			_p->pEditor->insertText(szText);
		break;
		case InContextContainerDeclaration:
		case InContextContainerDeclarationAndDefinition:
			if(_p->pSettings->pClassDescription)
			{
				if(!C3TextEditorModeCPPCodeGenerationUtils::insertCodeInClass(
							_p->pSettings->pClassDescription,
							C3TextEditorModeCPPCodeGenerationUtils::BestLocationForMemberVariables,
							_p->pSettings->eAccessLevel,
							szText
					))
				{
					_p->pSettings->eLocation = AtCursorPosition;
					_p->pEditor->insertText(szText);
				}
			} else {
				_p->pSettings->eLocation = AtCursorPosition;
				_p->pEditor->insertText(szText);
			}
		break;
		default:
			Q_ASSERT(false);
			return false;
		break;
	}

	if(_p->pSettings->bGenerateGetter)
	{
		QString szGetterReturn;
		if(_p->pSettings->bGetterReturnsConst)
			szGetterReturn += __ascii("const ");
		szGetterReturn += _p->pSettings->szTypeName;
		if(_p->pSettings->bGetterReturnsReference)
			szGetterReturn += __ascii(" &");

		QString szGetterBody = __ascii("{\n");
		szGetterBody += __ascii("	return ");
		szGetterBody += _p->pSettings->szMemberName;
		szGetterBody += __ascii(";\n");
		szGetterBody += __ascii("}");

		C3TextEditorModeCPPMemberFunctionGenerator::Settings oSettings;
		oSettings.szReturnType = szGetterReturn;
		oSettings.szBody = szGetterBody;
		oSettings.szFunctionName = _p->pSettings->szGetterName;
		oSettings.bIsVirtual = false;
		oSettings.bIsConst = _p->pSettings->bGetterIsConst;
		oSettings.eLocation = _p->pSettings->eLocation;
		oSettings.pClassDescription = _p->pSettings->pClassDescription;
		oSettings.eAccessLevel = _p->pSettings->eGetterAccessLevel;
		oSettings.szComment = _p->pSettings->szComment;

		C3TextEditorModeCPPMemberFunctionGenerator oGen(_p->pEditor,&oSettings);
		
		if(!oGen.run())
			return false;
	}
	
	if(_p->pSettings->bGenerateSetter)
	{
		QString szSetterSig;

		if(_p->pSettings->bSetterAcceptsConst)
			szSetterSig += __ascii("const ");
		szSetterSig += _p->pSettings->szTypeName;
		szSetterSig += __ascii(" ");
		if(_p->pSettings->bSetterAcceptsReference)
			szSetterSig += __ascii("&");

		QString szVarName = _p->pSettings->szMemberName;
		if(szVarName.startsWith(__ascii("m_")))
			szVarName.remove(0,2);
		else if(szVarName.startsWith(__ascii("_")))
			szVarName.remove(0,1);
		else
			szVarName = __ascii("value");

		szSetterSig += szVarName;

		QString szSetterBody = __ascii("{\n");
		szSetterBody += __ascii("	");
		szSetterBody += _p->pSettings->szMemberName;
		szSetterBody += __ascii(" = ");
		szSetterBody += szVarName;
		szSetterBody += __ascii(";\n");
		szSetterBody += __ascii("}");

		C3TextEditorModeCPPMemberFunctionGenerator::Settings oSettings;
		oSettings.szReturnType = __ascii("void");
		oSettings.szBody = szSetterBody;
		oSettings.szParameters = szSetterSig;
		oSettings.szFunctionName = _p->pSettings->szSetterName;
		oSettings.bIsVirtual = false;
		oSettings.bIsConst = false;
		oSettings.eLocation = _p->pSettings->eLocation;
		oSettings.pClassDescription = _p->pSettings->pClassDescription;
		oSettings.eAccessLevel = _p->pSettings->eSetterAccessLevel;
		oSettings.szComment = _p->pSettings->szComment;

		C3TextEditorModeCPPMemberFunctionGenerator oGen(_p->pEditor,&oSettings);
		
		if(!oGen.run())
			return false;
	}
	
	return true;
}


