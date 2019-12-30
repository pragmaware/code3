#ifndef _C3SymbolCTagsParser_h_
#define _C3SymbolCTagsParser_h_
//=============================================================================
//
//   File : C3SymbolCTagsParser.h
//   Creation Date : 2015/12/05 18:02:12
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

#include "C3SymbolFile.h"

#include <QString>
#include <QHash>

class C3SymbolCTagsParserPrivate;

class C3SymbolCTagsParser
{
public:
	C3SymbolCTagsParser();
	~C3SymbolCTagsParser();

private:
	C3SymbolCTagsParserPrivate * m_pC3SCTP;

public:

	// If the replacement is empty, no replacement is made
	void setReplaceInPath(const QString &sPath,const QString &sReplacement);

	// This is meant to run on a slave thread.
	// if(*pbAbortSignal) then the process should be aborted
	bool parseFile(const QString &szPath,QString &szError,bool * pbAbortSignal);
	
	bool parseLine(const char * pLine,int len,QString &szError);
	//bool parseLineOld(const QString &szLine,QString &szError);

	QHash<QString,C3SymbolFile *> * takeSymbolFiles();

private:
	QString filterTypeFromSearchPattern(const QString &szType);
	//void addLocalsFromLastFunctionDefinitionSignature(C3Symbol::Language eLanguage,unsigned int uLineNumber,C3SymbolFile * pFile);
	//void addLocalsFromParameterDeclaration(const QString &szParameterDeclaration,const QString &szScope,C3Symbol::Language eLanguage,unsigned int uLineNumber,C3SymbolFile * pFile);


}; // class C3SymbolCTagsParser

#endif //!_C3SymbolCTagsParser_h_
