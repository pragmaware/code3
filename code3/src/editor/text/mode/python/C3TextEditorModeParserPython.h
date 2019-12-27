#ifndef _C3TextEditorModeParserPython_h_
#define _C3TextEditorModeParserPython_h_
//=============================================================================
//
//   File : C3TextEditorModeParserPython.h
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
/// \class C3TextEditorModeParserPython
/// \brief The C3TextEditorModeParserPython class
///
/// This class...
///
class C3TextEditorModeParserPython : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserPython
	///
	C3TextEditorModeParserPython(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserPython
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserPython();

private:

	QHash<QString,QColor *> m_hIdentifierColorHash;

public:

	void computeMetadata();

private:

	void computeBlocksParseIdentifier();

	void computeBlocksParseTripleQuoteMultiLineString(const char * szTripleDelimiter,unsigned int uInterLineFlag);
	
	void buildIdentifierColorHash();

}; // class C3TextEditorModeParserPython

#endif //!_C3TextEditorModeParserPython_h_
