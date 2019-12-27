#ifndef _C3TextEditorModeParserCMake_h_
#define _C3TextEditorModeParserCMake_h_
//=============================================================================
//
//   File : C3TextEditorModeParserCMake.h
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

///
/// \class C3TextEditorModeParserCMake
/// \brief The C3TextEditorModeParserCMake class
///
/// This class...
///
class C3TextEditorModeParserCMake : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserCMake
	///
	C3TextEditorModeParserCMake(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserCMake
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserCMake();

private:

	QHash<QString,QColor *> m_hIdentifierColorHash;

public:

	void computeMetadata();

private:

	void computeBlocksParseVariable(QColor * pColor);
	void computeBlocksParseIdentifier();
	void computeBlocksParseMultiLineComment();
	void computeBlocksParseMultiLineString();
	void buildIdentifierColorHash();

}; // class C3TextEditorModeParserCMake

#endif //!_C3TextEditorModeParserCMake_h_