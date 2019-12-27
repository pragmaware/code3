#ifndef _C3TextEditorModeParserVHDL_h_
#define _C3TextEditorModeParserVHDL_h_
//=============================================================================
//
//   File : C3TextEditorModeParserVHDL.h
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

class C3TextEditorModeVHDLSharedData;

///
/// \class C3TextEditorModeParserVHDL
/// \brief The C3TextEditorModeParserVHDL class
///
/// This class...
///
class C3TextEditorModeParserVHDL : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserVHDL
	///
	C3TextEditorModeParserVHDL(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserVHDL
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserVHDL();

private:

	C3TextEditorModeVHDLSharedData * m_pSharedData;
	const QHash<QString,QColor *> & m_lIdentifierColorHash;

public:

	void computeMetadata();

private:

	void computeBlocksParseIdentifier();
	void computeBlocksParseVHDLMultiLineString(ushort uDelimiter,unsigned int uInterLineFlag);
	void computeBlocksParseVHDLNumber();
	
}; // class C3TextEditorModeParserVHDL

#endif //!_C3TextEditorModeParserVHDL_h_
