#ifndef _C3TextEditorModeParserXML_h_
#define _C3TextEditorModeParserXML_h_
//=============================================================================
//
//   File : C3TextEditorModeParserXML.h
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

class C3TextEditorModeParserXMLPrivate;

///
/// \class C3TextEditorModeParserXML
/// \brief The C3TextEditorModeParserXML class
///
/// This class...
///
class C3TextEditorModeParserXML : public C3TextEditorModeParser
{
public:
	enum Flags
	{
		EnablePHPHandling = 1,
		EnableScriptTagHandling = 2,
		EnableStyleTagHandling = 4
	};

	///
	/// Creates an instance of C3TextEditorModeParserXML
	///
	C3TextEditorModeParserXML(
			C3TextEditorModeParserState * pParserState,
			unsigned int uFlags
		);

	///
	/// Destroys the instance of C3TextEditorModeParserXML
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParserXML();


private:

	C3TextEditorModeParserXMLPrivate * m_pTEMPHTMLP;

public:

	void computeMetadata();

private:

	// Returns the char before the delimiter (or the last char if no delimiter was found)
	const QChar * computeBlocksParseTagContentsString(unsigned short uDelimiter);
	void computeBlocksParseMultiLineComment();
	void computeBlocksParseCData();
	void computeBlocksParseJavascript();
	void computeBlocksParseCSS();
	void computeBlocksParseTagContents();
	void computeBlocksParseTag();
	void computeBlocksParseHTML();

}; // class C3TextEditorModeParserXML

#endif //!_C3TextEditorModeParserXML_h_
