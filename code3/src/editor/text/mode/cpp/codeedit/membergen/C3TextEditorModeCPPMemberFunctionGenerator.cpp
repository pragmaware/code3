//=============================================================================
//
//   File : C3TextEditorModeCPPMemberFunctionGenerator.cpp
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

#include "C3TextEditorModeCPPMemberFunctionGenerator.h"

#include "C3TextEditor.h"
#include "C3SymbolManagerClassDescription.h"
#include "C3Workspace.h"
#include "C3StringUtils.h"
#include "C3TextEditorModeCPPCodeGenerationUtils.h"

class C3TextEditorModeCPPMemberFunctionGeneratorPrivate
{
public:
	C3TextEditor * pEditor;
	C3TextEditorModeCPPMemberFunctionGenerator::Settings * pSettings;
};

#define _p m_pC3TEMCPPMG

C3TextEditorModeCPPMemberFunctionGenerator::C3TextEditorModeCPPMemberFunctionGenerator(C3TextEditor * pEditor,Settings * pSettings)
	: C3TextEditorModeCPPMemberGenerator()
{
	_p = new C3TextEditorModeCPPMemberFunctionGeneratorPrivate();
	_p->pEditor = pEditor;

	_p->pSettings = pSettings;
}

C3TextEditorModeCPPMemberFunctionGenerator::~C3TextEditorModeCPPMemberFunctionGenerator()
{
	delete _p;
}

bool C3TextEditorModeCPPMemberFunctionGenerator::insertMemberFunction(
		const QString &szComment,
		const QString &szDeclarationPrefix,
		const QString &szReturn,
		const QString &szSignature,
		const QString &szBody,
		C3Symbol::AccessLevel eAccessLevel
	)
{
	QString szText;

	switch(_p->pSettings->eLocation)
	{
		case AtCursorPosition:
			szText = C3StringUtils::indentLines(
						__ascii("\n") +
						szComment +
						(szDeclarationPrefix.isEmpty() ? QString() : (szDeclarationPrefix + __ascii(" "))) +
						szReturn + __ascii(" ") +
						szSignature + __ascii("\n") +
						szBody + __ascii("\n")
				);
			_p->pEditor->insertText(szText);
		break;
		case InContextContainerDeclaration:
			szText = C3StringUtils::indentLines(
						__ascii("\n") +
						szComment +
						(szDeclarationPrefix.isEmpty() ? QString() : (szDeclarationPrefix + __ascii(" "))) +
						szReturn + __ascii(" ") +
						szSignature + __ascii("\n") +
						szBody + __ascii("\n")
					);
			if(_p->pSettings->pClassDescription)
			{
				if(!C3TextEditorModeCPPCodeGenerationUtils::insertCodeInClass(
							_p->pSettings->pClassDescription,
							C3TextEditorModeCPPCodeGenerationUtils::BestLocationForMemberFunctionPrototypes,
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
		case InContextContainerDeclarationAndDefinition:

			if(_p->pSettings->pClassDescription)
			{
				szText = C3StringUtils::indentLines(
						__ascii("\n") +
						szComment +
						(szDeclarationPrefix.isEmpty() ? QString() : (szDeclarationPrefix + __ascii(" "))) +
						szReturn + __ascii(" ") +
						szSignature + __ascii(";\n")
					);

				if(!C3TextEditorModeCPPCodeGenerationUtils::insertCodeInClass(
							_p->pSettings->pClassDescription,
							C3TextEditorModeCPPCodeGenerationUtils::BestLocationForMemberFunctionPrototypes,
							_p->pSettings->eAccessLevel,
							szText
					))
				{
					_p->pSettings->eLocation = AtCursorPosition;
					_p->pEditor->insertText(szText);
				}

				szText = __ascii("\n") +
						szReturn + __ascii(" ") + _p->pSettings->pClassDescription->szClassName + __ascii("::") + szSignature + __ascii("\n") +
						szBody + __ascii("\n");

				if(!C3TextEditorModeCPPCodeGenerationUtils::insertCodeInClass(
							_p->pSettings->pClassDescription,
							C3TextEditorModeCPPCodeGenerationUtils::BestLocationForMemberFunctionDefinitions,
							_p->pSettings->eAccessLevel,
							szText
					))
				{
					_p->pSettings->eLocation = AtCursorPosition;
					_p->pEditor->insertText(szText);
				}

			} else {
				szText = C3StringUtils::indentLines(
						__ascii("\n") +
						szComment +
						(szDeclarationPrefix.isEmpty() ? QString() : (szDeclarationPrefix + __ascii(" "))) +
						szReturn + __ascii(" ") +
						szSignature + __ascii("\n") +
						szBody + __ascii("\n")
					);
				_p->pSettings->eLocation = AtCursorPosition;
				_p->pEditor->insertText(szText);
			}
		break;
		default:
			Q_ASSERT(false);
			return false;
		break;
	}
	
	return true;
}

bool C3TextEditorModeCPPMemberFunctionGenerator::run()
{
	QString szSig = _p->pSettings->szFunctionName;
	szSig += __ascii("(");
	szSig += _p->pSettings->szParameters;
	szSig += __ascii(")");

	if(_p->pSettings->bIsConst)
		szSig += __ascii(" const");

	QString szReturn = _p->pSettings->szReturnType;
	
	if(szReturn.isEmpty())
		szReturn = __ascii("void");

	QString szBody = _p->pSettings->szBody;
	
	if(szBody.isEmpty())
	{
		szBody = __ascii("{\n");
		
		szBody += __ascii("	// TODO\n");
		
		if(_p->pSettings->szReturnType == __ascii("void"))
		{
			// nothing
		} else if(_p->pSettings->szReturnType.indexOf(QChar('*')) >= 0)
		{
			szBody += __ascii("	return NULL;\n");
		} else if(_p->pSettings->szReturnType.indexOf(__ascii("int")) >= 0)
		{
			szBody += __ascii("	return 0;\n");
		} else if(_p->pSettings->szReturnType.indexOf(__ascii("double")) >= 0)
		{
			szBody += __ascii("	return 0.0;\n");
		} else if(_p->pSettings->szReturnType.indexOf(__ascii("bool")) >= 0)
		{
			szBody += __ascii("	return false;\n");
		} else if(_p->pSettings->szReturnType.indexOf(__ascii("float")) >= 0)
		{
			szBody += __ascii("	return 0.0f;\n");
		}
	
		szBody += __ascii("}");
	}

	QString szPrefix;
	if(_p->pSettings->bIsVirtual)
		szPrefix = __ascii("virtual");
	else if(_p->pSettings->bIsStatic)
		szPrefix = __ascii("static");
	
	QString szComment;
	
	if(!_p->pSettings->szComment.isEmpty())
		szComment = __ascii("///\n/// %1\n///\n").arg(_p->pSettings->szComment);

	if(!insertMemberFunction(szComment,szPrefix,szReturn,szSig,szBody,_p->pSettings->eAccessLevel))
		return false;
	
	return true;
}


