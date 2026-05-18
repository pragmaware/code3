#ifndef _C3TextEditorModeParserRust_h_
#define _C3TextEditorModeParserRust_h_
//=============================================================================
//
//   File : C3TextEditorModeParserRust.h
//   Creation Date : 18/05/2026 21:49
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

class C3TextEditorModeRustSharedData;

#include <QHash>
#include <QString>
#include <QColor>

///
/// \class C3TextEditorModeParserRust
/// \brief The C3TextEditorModeParserRust class
///
/// This class...
///

class C3TextEditorModeParserRust : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserRust
	///
	C3TextEditorModeParserRust(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserRust
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserRust();

private:

	C3TextEditorModeRustSharedData * m_pSharedData;


	// References to stuff owned by RustSharedData
	const QHash<QString,QColor *> & m_lIdentifierColorHash;
	const QHash<QString,QColor *> & m_lIdentifierPrefixColorHash;
	const QHash<QString,QColor *> & m_lIdentifierPostfixColorHash;
	const QHash<QString,QColor *> & m_hIdentifierPrefixWithUppercaseColorHash;

public:

	void computeMetadata();


private:

	void computeBlocksParseIdentifier();

}; // class C3TextEditorModeParserRust

#endif //!_C3TextEditorModeParserRust_h_
