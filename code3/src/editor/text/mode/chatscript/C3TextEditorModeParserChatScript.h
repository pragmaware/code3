#ifndef _C3TextEditorModeParserChatScript_h_
#define _C3TextEditorModeParserChatScript_h_
//=============================================================================
//
//   File : C3TextEditorModeParserChatScript.h
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
/// \class C3TextEditorModeParserChatScript
/// \brief The C3TextEditorModeParserChatScript class
///
/// This class...
///
class C3TextEditorModeParserChatScript : public C3TextEditorModeParser
{
public:

	///
	/// Creates an instance of C3TextEditorModeParserChatScript
	///
	C3TextEditorModeParserChatScript(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParserChatScript
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserChatScript();

private:

	QHash<QString,QColor *> m_hIdentifierColorHash;

public:

	void computeMetadata();

private:

	void computeBlocksParseMacro();
	void computeBlocksParseIdentifier(QColor * pColor);
	void buildIdentifierColorHash();

}; // class C3TextEditorModeParserChatScript

#endif //!_C3TextEditorModeParserChatScript_h_