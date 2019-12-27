#ifndef _C3TextEditorModeParserPHP_h_
#define _C3TextEditorModeParserPHP_h_
//=============================================================================
//
//   File : C3TextEditorModeParserPHP.h
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

#include <QHash>
#include <QString>
#include <QColor>

#include "C3TextEditorLine.h"
#include "C3TextEditorModeParserJavascript.h"
#include "C3TextEditorModeParserCSS.h"

class C3TextEditorModeParserPHPPrivate;
class C3TextEditorModePHPSharedData;

///
/// \class C3TextEditorModeParserPHP
/// \brief The C3TextEditorModeParserPHP class
///
/// This class...
///
class C3TextEditorModeParserPHP : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserPHP
	///
	C3TextEditorModeParserPHP(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserPHP
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserPHP();

private:

	C3TextEditorModePHPSharedData * m_pSharedData;

	// References to stuff owned by PHPSharedData
	const QHash<QString,QColor *> & m_hIdentifierColorHash;
	const QHash<QString,QColor *> & m_hIdentifierPrefixColorHash;
	const QHash<QString,QColor *> & m_hIdentifierPostfixColorHash;
	const QHash<QString,QColor *> & m_hIdentifierPrefixWithUppercaseColorHash;

public:

	void computeMetadata();

private:

	void computeBlocksParseIdentifier();

}; // class C3TextEditorModeParserPHP

#endif //!_C3TextEditorModeParserPHP_h_
