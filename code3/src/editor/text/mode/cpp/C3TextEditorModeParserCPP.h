#ifndef _C3TextEditorModeParserCPP_h_
#define _C3TextEditorModeParserCPP_h_
//=============================================================================
//
//   File : C3TextEditorModeParserCPP.h
//   Creation Date : sab 31 ott 2015 23:10:30
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

#include "C3TextEditorModeParser.h"
#include "C3TextEditorLine.h"

class C3TextEditorModeCPPSharedData;

#include <QHash>
#include <QString>
#include <QColor>

///
/// \class C3TextEditorModeParserCPP
/// \brief The C3TextEditorModeParserCPP class
///
/// This class...
///

class C3TextEditorModeParserCPP : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserCPP
	///
	C3TextEditorModeParserCPP(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserCPP
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserCPP();

private:

	bool m_bInPreprocessorLine;
	bool m_bFirstPreprocessorTokenSeen;

	C3TextEditorModeCPPSharedData * m_pSharedData;


	// References to stuff owned by CPPSharedData
	const QHash<QString,QColor *> & m_lPreprocessorIdentifierColorHash;
	const QHash<QString,QColor *> & m_lIdentifierColorHash;
	const QHash<QString,QColor *> & m_lIdentifierPrefixColorHash;
	const QHash<QString,QColor *> & m_lIdentifierPostfixColorHash;
	const QHash<QString,QColor *> & m_hIdentifierPrefixWithUppercaseColorHash;

public:

	void computeMetadata();


private:

	void computeBlocksParseIdentifier();
	void computeBlocksParseIncludeFile();

}; // class C3TextEditorModeParserCPP

#endif //!_C3TextEditorModeParserCPP_h_