//=============================================================================
//
//   File : C3TextEditorModeCPPCodeGenerationUtils.cpp
//   Creation Date : 2016/09/22 04:08:09
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

#include "C3TextEditorModeCPPCodeGenerationUtils.h"
#include "C3TextEditor.h"
#include "C3TextEditorRowColumn.h"
#include "C3SymbolManagerClassDescription.h"
#include "C3FindOperation.h"
#include "C3Workspace.h"

#include <QStringList>
#include <QRegularExpression>

namespace C3TextEditorModeCPPCodeGenerationUtils
{
	int findLineWithEndingText(C3TextEditor * pEditor,int iStartLine,int iMaxLines,const QString &szText)
	{
		if(!pEditor)
			return -1;

		int i = 0;
		while(i < iMaxLines)
		{
			QString szLocalText = pEditor->getLineText(iStartLine).trimmed();

			if(i > 0)
			{
				if(szLocalText.indexOf(QChar('{')) != -1)
					return -1; // would cross a scope boundary
				if(szLocalText.endsWith(QChar(':')))
					return -1; // would cross a public: protected: private: block
			}

			if(szLocalText.endsWith(szText))
				return iStartLine;
			// may end with a a comment too
			int idx = szLocalText.indexOf(__ascii("//"));
			if(idx >= 0)
			{
				szLocalText.truncate(idx);
				szLocalText = szLocalText.trimmed();
				if(szLocalText.endsWith(szText))
					return iStartLine;
			} else if(szLocalText.indexOf(__ascii("/*")))
			{
				szLocalText.truncate(idx);
				szLocalText = szLocalText.trimmed();
				if(szLocalText.endsWith(szText))
					return iStartLine;
			}
			iStartLine++;
			i++;
		}
		
		return -1;
	}

	int findLineWithStartingText(C3TextEditor * pEditor,int iStartLine,int iMaxLines,const QString &szText)
	{
		if(!pEditor)
			return -1;

		int i = 0;

		while(i < iMaxLines)
		{
			QString szLocalText = pEditor->getLineText(iStartLine).trimmed();

			if(i > 0)
			{
				if(szLocalText.indexOf(QChar('{')) != -1)
					return -1; // would cross a scope boundary
			}

			if(szLocalText.startsWith(szText))
				return iStartLine;
			iStartLine++;
			i++;
		}
		
		return -1;
	}

	void insertCodeAtLine(
			int iLine,
			bool bAfter,
			C3SymbolManagerClassDescription * pClass,
			C3TextEditor * pEditor,
			const QString &szCode
		)
	{
		QString szText;
		
		if(bAfter)
		{
			if(szCode.endsWith(QChar('\n')))
				szText = __ascii("\n") + szCode.mid(0,szCode.length() - 1);
			else
				szText = __ascii("\n") + szCode;
		
			pEditor->cursorMoveTo(C3TextEditorRowColumn(iLine,0),false);
			pEditor->cursorEnd(false);
			pEditor->insertText(szText);

			pClass->codeInsertedInClassDeclaration(iLine,szText);
			return;
		}

		if(szCode.endsWith(QChar('\n')))
			szText = szCode;
		else
			szText = szCode + __ascii("\n");
	
		pEditor->cursorMoveTo(C3TextEditorRowColumn(iLine,0),false);
		pEditor->insertText(szText);

		pClass->codeInsertedInClassDeclaration(iLine,szText);
	}

	bool insertCodeInClass(
			C3SymbolManagerClassDescription * pClass,
			InsertCodeInClassLocation eLocation,
			C3Symbol::AccessLevel eAccessLevel,
			const QString &szOrigCode
		)
	{
		C3TextEditor * pEditor;
	
		switch(eLocation)
		{
			case BestLocationForMemberVariables:
			case BestLocationForMemberFunctionPrototypes:
				pEditor = dynamic_cast<C3TextEditor *>(
						C3Workspace::currentWorkspace()->findEditor(__ascii("text"),pClass->szDeclaratonFile)
					);
			break;
			case BestLocationForMemberFunctionDefinitions:
				pEditor = dynamic_cast<C3TextEditor *>(
						C3Workspace::currentWorkspace()->findEditor(__ascii("text"),pClass->szDefinitionFile)
					);
			break;
		}

		if(!pEditor)
			return false;

		pEditor->clearSelection();

		int iLine;
		
		QString szCode = szOrigCode;

		switch(eLocation)
		{
			case BestLocationForMemberVariables:
				if(
						pClass->bFoundMemberVariableDeclarations[eAccessLevel] &&
						(pClass->uLastMemberVariableDeclarationLine[eAccessLevel] < pClass->uDeclarationEndLine)
					)
				{
					iLine = findLineWithEndingText(pEditor,pClass->uLastMemberVariableDeclarationLine[eAccessLevel],5,__ascii(";"));
					if(iLine < 0)
						iLine = findLineWithEndingText(pEditor,pClass->uLastMemberVariableDeclarationLine[eAccessLevel],5,__ascii("}"));

					if((iLine > -1) && ((quint32)iLine < pClass->uDeclarationEndLine))
					{
						insertCodeAtLine(iLine,true,pClass,pEditor,szCode);
						return true;
					}
				}

				if(eAccessLevel != C3Symbol::AccessLevelUnknown)
				{
					if(
							pClass->bFoundMemberVariableDeclarations[C3Symbol::AccessLevelUnknown] &&
							(pClass->uLastMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown] < pClass->uDeclarationEndLine)
						)
					{
						iLine = findLineWithEndingText(pEditor,pClass->uLastMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii(";"));
						if(iLine < 0)
							iLine = findLineWithEndingText(pEditor,pClass->uLastMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii("}"));

						if((iLine > -1) && ((quint32)iLine < pClass->uDeclarationEndLine))
						{
							szCode = __ascii("\n%1:\n").arg(C3Symbol::accessLevelKeyword(eAccessLevel)) + szCode;

							pClass->bFoundMemberVariableDeclarations[eAccessLevel] = true;
							pClass->uFirstMemberVariableDeclarationLine[eAccessLevel] = iLine + 2;
							pClass->uLastMemberVariableDeclarationLine[eAccessLevel] = iLine + 2;

							insertCodeAtLine(iLine,true,pClass,pEditor,szCode);
							return true;
						}
					}
				}

				iLine = findLineWithStartingText(pEditor,pClass->uDeclarationEndLine,5,__ascii("};"));
				if(iLine < 0)
				{
					iLine = findLineWithStartingText(pEditor,pClass->uDeclarationEndLine-1,5,__ascii("};"));
					if(iLine < 0)
					{
						iLine = findLineWithStartingText(pEditor,pClass->uDeclarationEndLine,5,__ascii("}"));
						if(iLine < 0)
							return false;
					}
				}

				if(eAccessLevel != C3Symbol::AccessLevelUnknown)
				{
					szCode = __ascii("\n%1:\n").arg(C3Symbol::accessLevelKeyword(eAccessLevel)) + szCode;

					pClass->bFoundMemberVariableDeclarations[eAccessLevel] = true;
					pClass->uFirstMemberVariableDeclarationLine[eAccessLevel] = iLine + 2;
					pClass->uLastMemberVariableDeclarationLine[eAccessLevel] = iLine + 2;
				}

				insertCodeAtLine(iLine,false,pClass,pEditor,szCode);
				return true;
			break;
			case BestLocationForMemberFunctionPrototypes:
				if(
						pClass->bFoundMemberFunctionsInDeclaration[eAccessLevel] &&
						(pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel] < pClass->uDeclarationEndLine)
					)
				{
					iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel],5,__ascii(");"));
					if(iLine < 0)
					{
						iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel],5,__ascii("0;"));
						if(iLine < 0)
						{
							iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel],5,__ascii("const;"));
							if(iLine < 0)
							{
								iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel],5,__ascii("}"));
								if(iLine < 0)
									iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel],5,__ascii(";"));
							}
						}
					}
					if((iLine > -1) && ((quint32)iLine < pClass->uDeclarationEndLine))
					{
						insertCodeAtLine(iLine,true,pClass,pEditor,szCode);
						return true;
					}
				}

				if(eAccessLevel != C3Symbol::AccessLevelUnknown)
				{
					if(
							pClass->bFoundMemberFunctionsInDeclaration[C3Symbol::AccessLevelUnknown] &&
							(pClass->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] < pClass->uDeclarationEndLine)
						)
					{
						iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii(");"));
						if(iLine < 0)
						{
							iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii("0;"));
							if(iLine < 0)
							{
								iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii("const;"));
								if(iLine < 0)
								{
									iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii("}"));
									if(iLine < 0)
										iLine = findLineWithEndingText(pEditor,pClass->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown],5,__ascii(";"));
								}
							}
						}
						if((iLine > -1) && ((quint32)iLine < pClass->uDeclarationEndLine))
						{
							szCode = __ascii("\n%1:\n").arg(C3Symbol::accessLevelKeyword(eAccessLevel)) + szCode;

							pClass->bFoundMemberFunctionsInDeclaration[eAccessLevel] = true;
							pClass->uFirstMemberFunctionInDeclarationLine[eAccessLevel] = iLine + 2;
							pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel] = iLine + 2;

							insertCodeAtLine(iLine,true,pClass,pEditor,szCode);
							return true;
						}
					}
				}

				// insert just before class declaration end
				iLine = findLineWithStartingText(pEditor,pClass->uDeclarationEndLine,5,__ascii("};"));
				if(iLine < 0)
				{
					iLine = findLineWithStartingText(pEditor,pClass->uDeclarationEndLine-1,5,__ascii("};"));
					if(iLine < 0)
					{
						iLine = findLineWithStartingText(pEditor,pClass->uDeclarationEndLine,5,__ascii("}"));
						if(iLine < 0)
							return false;
					}
				}
				
				if(eAccessLevel != C3Symbol::AccessLevelUnknown)
				{
					szCode = __ascii("\n%1:\n").arg(C3Symbol::accessLevelKeyword(eAccessLevel)) + szCode;

					pClass->bFoundMemberFunctionsInDeclaration[eAccessLevel] = true;
					pClass->uFirstMemberFunctionInDeclarationLine[eAccessLevel] = iLine + 1;
					pClass->uLastMemberFunctionInDeclarationLine[eAccessLevel] = iLine + 1;
				}

				insertCodeAtLine(iLine,false,pClass,pEditor,szCode);
				return true;
			break;
			case BestLocationForMemberFunctionDefinitions:
				// at the end of the file
				pEditor->cursorFileEnd(false);
				if(pEditor->getLineText(pEditor->cursorRow()).trimmed().isEmpty())
					pEditor->insertText(__ascii("\n") + szCode);
				else
					pEditor->insertText(__ascii("\n\n") + szCode);
				return true;
			break;
		}

		return false;
	}

	bool insertIncludeLine(
			C3TextEditor * pEditor,
			const QString &szLine
		)
	{
	
		// FIXME: There are problems with directives here. This code will insert
		// #include lines wherever it finds them, possibly within #ifdef blocks.
	
		C3TextEditor::CursorAndScrollBarState oSave;
	
		pEditor->saveCursorAndScrollBarState(oSave);

		C3FindOperation oOp;

		pEditor->cursorMoveTo(C3TextEditorRowColumn(200,0),false);

		bool bAngular = (szLine.indexOf(QChar('<')) >= 0);

		// First try to look at the beginning (no initial spaces)
		// This mitigates the #ifdef problem as often the #if blocks
		// are indented.

		oOp.setId(C3FindOperation::nextOperationId());
		oOp.setDirection(C3FindOperation::Backward);
		oOp.setRegularExpression(QRegularExpression(
				bAngular ?
					__ascii("^#include[ 	]+<.+") :
					__ascii("^#include[ 	]+\".+")
			));
	
		bool bRet = pEditor->findOperationRun(&oOp);

		if(!bRet)
		{
			// try again, broader search
			
			//qDebug("PROPER INCLUDE NOT FOUND");

			oOp.setId(C3FindOperation::nextOperationId());
			oOp.setRegularExpression(QRegularExpression(__ascii("#include[ 	]+.+")));
	
			pEditor->cursorMoveTo(C3TextEditorRowColumn(200,0),false);
			oOp.setDirection(C3FindOperation::Backward);
	
			bRet = pEditor->findOperationRun(&oOp);
			
			if(!bRet)
			{
				//qDebug("BROADER NOT FOUND");

				oOp.setId(C3FindOperation::nextOperationId());
				oOp.setRegularExpression(QRegularExpression(__ascii("#include[ 	]+.+")));
		
				pEditor->cursorMoveTo(C3TextEditorRowColumn(0,0),false);

				oOp.setDirection(C3FindOperation::Forward);

				bRet = pEditor->findOperationRun(&oOp);
				if(!bRet)
				{
					//qDebug("BROADER FORWARD NOT FOUND");
				}
			}
		}

		bool bAtEnd = false;

		C3TextEditorRowColumn oInsertPos;
	
		if(bRet)
		{
			//qDebug("GOTCHA");

			int iRow = pEditor->selectedRange().end.row;

			QString szText = pEditor->getLineText(iRow);
			
			QString szLookFor = bAngular ? __ascii("#include <") : __ascii("#include \"");
			
			if(!szText.startsWith(szLookFor))
				szLookFor = __ascii("#include");
			
			while(szText.startsWith(szLookFor))
			{
				iRow++;
				szText = pEditor->getLineText(iRow);
			}

			oInsertPos.set(iRow,0);
		} else {
			int iRow = 0;
			QString szText = pEditor->getLineText(iRow);
			while(szText.startsWith(__ascii("//")))
			{
				iRow++;
				szText = pEditor->getLineText(iRow);
			}
			oInsertPos.set(iRow,0);
		}

		pEditor->clearSelection();
		pEditor->cursorMoveTo(oInsertPos,false);
		

		if(szLine.endsWith(QChar('\n')))
			pEditor->insertText(szLine);
		else
			pEditor->insertText(szLine + __ascii("\n"));

		if(oSave.iYOffset >= oInsertPos.row)
			oSave.iYOffset++;
		if(oSave.oCursorPos.row >= oInsertPos.row)
			oSave.oCursorPos.row++;

		pEditor->restoreCursorAndScrollBarState(oSave);

		return true;
	}

} // namespace C3TextEditorModeCPPCodeGenerationUtils
