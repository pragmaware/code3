#ifndef _C3SymbolManagerClassDescription_h_
#define _C3SymbolManagerClassDescription_h_
//=============================================================================
//
//   File : C3SymbolManagerClassDescription.h
//   Creation Date : 2016/09/22 02:29:41
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
#include <QStringList>

#include "C3Symbol.h"

class C3SymbolManagerClassDescription
{
public:
	QString szClassName;
	QString szScopeName;
	
	QString szDeclaratonFile;
	quint32 uDeclarationStartLine;
	quint32 uDeclarationEndLine;

	// C3Symbol::AccessLevelUnknown includes all of them
	bool bFoundMemberVariableDeclarations[C3Symbol::AccessLevelCount];
	quint32 uFirstMemberVariableDeclarationLine[C3Symbol::AccessLevelCount];
	quint32 uLastMemberVariableDeclarationLine[C3Symbol::AccessLevelCount];

	bool bFoundMemberFunctionsInDeclaration[C3Symbol::AccessLevelCount];
	quint32 uFirstMemberFunctionInDeclarationLine[C3Symbol::AccessLevelCount];
	quint32 uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelCount];

	QString szDefinitionFile;
	
public:

	// adjust lines
	void codeInsertedInClassDeclaration(quint32 uAtLine,const QString &szText)
	{
		QStringList sl = szText.split(QChar('\n'));
		
		quint32 uCount = sl.count();
		if(uCount < 2)
			return;
		uCount--; // number of newlines
	
		//qDebug("Before insertion declaration ends at %d",uDeclarationEndLine);
	
		if(uAtLine <= uDeclarationEndLine)
			uDeclarationEndLine += uCount;

		for(int i=0;i<C3Symbol::AccessLevelCount;i++)
		{
			if(uAtLine <= uFirstMemberFunctionInDeclarationLine[i])
				uFirstMemberFunctionInDeclarationLine[i] += uCount;
			
			if(uAtLine <= uFirstMemberVariableDeclarationLine[i])
				uFirstMemberVariableDeclarationLine[i] += uCount;
	
			if(uAtLine <= uLastMemberFunctionInDeclarationLine[i])
				uLastMemberFunctionInDeclarationLine[i] += uCount;
			
			if(uAtLine <= uLastMemberVariableDeclarationLine[i])
				uLastMemberVariableDeclarationLine[i] += uCount;
		}

		//qDebug("Declaration now ends at %d",uDeclarationEndLine);
	}
	
}; // class C3SymbolManagerClassDescription

#endif //!_C3SymbolManagerClassDescription_h_