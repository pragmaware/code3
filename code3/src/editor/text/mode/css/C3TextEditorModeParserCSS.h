#ifndef _C3TextEditorModeParserCSS_h_
#define _C3TextEditorModeParserCSS_h_
//=============================================================================
//
//   File : C3TextEditorModeParserCSS.h
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
/// \class C3TextEditorModeParserCSS
/// \brief The C3TextEditorModeParserCSS class
///
/// This class...
///
class C3TextEditorModeParserCSS : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserCSS
	///
	C3TextEditorModeParserCSS(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserCSS
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserCSS();

private:

	QHash<QString,QColor *> m_hIdentifierColorHash;

public:

	void computeMetadata();

private:

	void computeBlocksParseIdentifier();
	
	void buildIdentifierColorHash();

}; // class C3TextEditorModeParserCSS

#endif //!_C3TextEditorModeParserCSS_h_
