#ifndef _C3TextEditorModeParserJava_h_
#define _C3TextEditorModeParserJava_h_
//=============================================================================
//
//   File : C3TextEditorModeParserJava.h
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

class C3TextEditorModeJavaSharedData;

#include <QHash>
#include <QString>
#include <QColor>

///
/// \class C3TextEditorModeParserJava
/// \brief The C3TextEditorModeParserJava class
///
/// This class...
///

class C3TextEditorModeParserJava : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserJava
	///
	C3TextEditorModeParserJava(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserJava
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserJava();

private:

	C3TextEditorModeJavaSharedData * m_pSharedData;


	// References to stuff owned by JavaSharedData
	const QHash<QString,QColor *> & m_lIdentifierColorHash;
	const QHash<QString,QColor *> & m_lIdentifierPrefixColorHash;
	const QHash<QString,QColor *> & m_lIdentifierPostfixColorHash;
	const QHash<QString,QColor *> & m_hIdentifierPrefixWithUppercaseColorHash;

public:

	void computeMetadata();


private:

	void computeBlocksParseIdentifier();
	void computeBlocksParseAnnotation();

}; // class C3TextEditorModeParserJava

#endif //!_C3TextEditorModeParserJava_h_
