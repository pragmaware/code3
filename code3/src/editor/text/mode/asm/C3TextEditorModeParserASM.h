﻿#ifndef _C3TextEditorModeParserASM_h_
#define _C3TextEditorModeParserASM_h_
//=============================================================================
//
//   File : C3TextEditorModeParserASM.h
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

class C3TextEditorModeASMSharedData;

#include <QHash>
#include <QString>
#include <QColor>

///
/// \class C3TextEditorModeParserASM
/// \brief The C3TextEditorModeParserASM class
///
/// This class...
///

class C3TextEditorModeParserASM : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserASM
	///
	C3TextEditorModeParserASM(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserASM
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserASM();

private:

	bool m_bInPreprocessorLine;
	bool m_bFirstPreprocessorTokenSeen;

	C3TextEditorModeASMSharedData * m_pSharedData;

	// References to stuff owned by ASMSharedData
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

}; // class C3TextEditorModeParserASM

#endif //!_C3TextEditorModeParserASM_h_