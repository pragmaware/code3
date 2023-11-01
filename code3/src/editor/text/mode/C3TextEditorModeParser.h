#ifndef _C3TextEditorModeParser_h_
#define _C3TextEditorModeParser_h_
//=============================================================================
//
//   File : C3TextEditorModeParser.h
//   Creation Date : sab 31 ott 2015 22:48:38
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

#include "C3TextEditorLine.h"
#include "C3TextEditorCoreData.h"

class C3TextEditorModeWithInterLineState;


class C3TextEditorModeParserState
{
public:
	// This stuff is always valid
	C3TextEditorModeWithInterLineState * pMode;
	unsigned int uInterLineFlagMask;

	// This stuff is valid only after attach()
	C3TextEditorCoreData * pCoreData;

	// This stuff is valid only in computeMetadata()
	int iLineRow;
	C3TextEditorLine * pLine;
	unsigned int uInterLineFlags;
	qreal fX; // x position
	const QChar * p; // current char pointer
	const QChar * e; // line end char pointer
};


///
/// \class C3TextEditorModeParser
/// \brief The C3TextEditorModeParser class
///
/// This class...
///
class C3TextEditorModeParser
{
public:

	// WARNING: BlockFlags of PHP,CSS,Javascript and HTML must not collide
	// WARNING: Block flag 1 can't be used since it's already used by the line block flags
	// WARNING: Bit with shift 1 must always be assigned to single line comments (if present in the language)
	enum BlockFlags : unsigned int
	{
		// Must not share bit with XML/Javascript/PHP
		// do NOT use bit 1 - that must be assigned to single line comments
		InCSSMultiLineComment = (1 << 2),
		
		// Must not share bit with CSS/Javascript/PHP
		// do NOT use bit 1 - that must be assigned to single line comments
		InXMLTag = (1 << 3),
		InXMLMultiLineComment = (1 << 4),
		InXMLMultiLineDoubleQuoteString = (1 << 5),
		InXMLMultiLineSingleQuoteString = (1 << 6),
		InXMLJavascript = (1 << 7),
		InXMLCSS = (1 << 8),
		InXMLPHP = (1 << 9),
		InXMLCData = (1 << 10),
		
		// Must not share bit with CSS/XML/PHP
		InJavascriptMultiLineComment = (1 << 11),

		// Must not share bit with CSS/Javascript/XML
		InPHPMultiLineComment = (1 << 12),

		// Dart mode is independent so it shares bits with other modes
		// do NOT use bit 1 - that must be assigned to single line comments
		InDartMultiLineComment = (1 << 2),
		
		// CMake mode is independent so it shares bits with other modes
		// do NOT use bit 1 - that must be assigned to single line comments
		InCMakeMultiLineComment = (1 << 2),
		InCMakeMultiLineString = (1 << 3),

		// Chatscript mode is independent so it shares bits with other modes
		// do NOT use bit 1 - that must be assigned to single line comments
		ChatScriptSeenFirstToken = (1 << 2), // already seen a token, following tokens can't handle the ":" character (a: s: etc.)
		
		// Bash mode is independent so it shares bits with other modes.
		// do NOT use bit 1 - that must be assigned to single line comments
		InBashMultiLineWeakQuoteString = (1 << 2),
		InBashMultiLineStrongQuoteString = (1 << 3),
		InBashExpandedHeredoc = (1 << 4),
		InBashUnexpandedHeredoc = (1 << 5),

		// CPP mode is independent so it shares bits with other modes.
		InCPPSingleLineComment = (1 << 1), // not inter-line
		InCPPPreprocessor = (1 << 2), // this flag is used only in the "line continuation" block of a preprocessor line. Other blocks are unaffected
		InCPPIncludeFile = (1 << 3), // this is not inter-line
		InCPPPreprocessorToken = (1 << 4), // again not inter-line
		InCPPMultiLineComment = (1 << 5),
		InCPPString = (1 << 6), // not inter-line
		
		// ASM mode is independent so it shares bits with other modes.
		InASMSingleLineComment = (1 << 1),
		InASMIncludeFile = (1 << 2),
		InASMPreprocessorToken = (1 << 3),
		InASMPreprocessor = (1 << 4),
		InASMString = (1 << 5),
		InASMMultiLineComment = (1 << 6),

		// Java mode is independent so it shares bits with other modes.
		InJavaSingleLineComment = (1 << 1), // not inter-line
		InJavaMultiLineComment = (1 << 2),
		InJavaString = (1 << 3), // not inter-line
		
		// SQL mode is independent so it shares bits with other modes.
		InSQLSingleLineComment = (1 << 1),
		InSQLMultiLineComment = (1 << 2),
		InSQLSingleQuoteString = (1 << 4),
		InSQLDoubleQuoteString = (1 << 8),
		InSQLBackTickString = (1 << 16),
		
		// Python mode is independent so it shares bits with other modes.
		InPythonSingleLineComment = (1 << 1),
		InPythonSingleQuoteString = (1 << 2),
		InPythonDoubleQuoteString = (1 << 4),
		InPythonTripleSingleQuoteString = (1 << 5),
		InPythonTripleDoubleQuoteString = (1 << 6),
		InPythonJustAfterColon = (1 << 7),
		InPythonTripleSingleQuoteComment = (1 << 8),
		InPythonTripleDoubleQuoteComment = (1 << 9),
		
		// VHDL mode is independent so it shared bits with other modes.
		InVHDLSingleLineComment = (1 << 1),
		InVHDLDoubleQuoteString = (1 << 2),
		InVHDLSingleQuoteString = (1 << 3),

		PythonInterLineFlagMask = InPythonTripleSingleQuoteString | InPythonTripleDoubleQuoteString | InPythonJustAfterColon | InPythonTripleSingleQuoteComment | InPythonTripleDoubleQuoteComment,
		SQLInterLineFlagMask = InSQLMultiLineComment | InSQLSingleQuoteString | InSQLDoubleQuoteString | InSQLBackTickString,
		CSSInterLineFlagMask = InCSSMultiLineComment,
		JavascriptInterLineFlagMask = InJavascriptMultiLineComment,
		DartInterLineFlagMask = InDartMultiLineComment,
		XMLInterLineFlagMask = InXMLCData | InXMLTag | InXMLMultiLineDoubleQuoteString | InXMLMultiLineSingleQuoteString | InXMLMultiLineComment | InXMLJavascript | InXMLCSS | InXMLPHP | CSSInterLineFlagMask | JavascriptInterLineFlagMask,
		PHPInterLineFlagMask = XMLInterLineFlagMask | InPHPMultiLineComment,
		CPPInterLineFlagMask = InCPPMultiLineComment | InCPPPreprocessor,
		JavaInterLineFlagMask = InJavaMultiLineComment,
		CMakeInterLineFlagMask = InCMakeMultiLineComment | InCMakeMultiLineString,
		ChatScriptInterLineFlagMask = 0,
		ASMInternLineFlagMask = InASMMultiLineComment | InASMPreprocessor,
		VHDLInterLineFlagMask = InVHDLDoubleQuoteString,
		// Bash uses the high 16 bits to store the HEREDOC id (we could use more bits eventually)
		BashInterLineFlagMask = InBashMultiLineWeakQuoteString | InBashMultiLineStrongQuoteString | InBashExpandedHeredoc | InBashUnexpandedHeredoc | 0xffff0000
	};

	///
	/// Creates an instance of C3TextEditorModeParser
	///
	C3TextEditorModeParser(C3TextEditorModeParserState * pParserState);

	///
	/// Destroys the instance of C3TextEditorModeParser
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeParser();

protected:

	C3TextEditorModeParserState * m_p;

protected:


	C3TextEditorLine::Block * createBlock(const QChar * pBegin,int iLength,QColor * pColor,unsigned int uSpecialFlags);
	bool pointingAtString(const char * szString,int iLength);
	bool pointingAtString(const QChar * szString,int iLength);
	const QChar * findString(const char * szString,int iLength);
	const QChar * findString(const QChar * szString,int iLength);

	// Standard parsing routines
	
	// Parse a C/C++-like multiline comment.
	// This must be called somewhere inside the coment itelf with the comment flag already set in m_p->uInterLineFlags.
	void computeBlocksParseCLikeMultiLineComment(unsigned int uCommentFlag);

	// Parse any kind of single line commend until the end of the line.
	// This function does not set any special flag on the blocks (so you eventually need to set it youself)
	void computeBlocksParseSingleLineComment();

	// Parse any kind of string expecting uDelimiter as terminator.
	// This function should be called when pointing at the initial delimiter (which may de different than uDelimiter)
	void computeBlocksParseString(ushort uDelimiter,QColor * pColor);

	// Parse any kind of string expecting uDelimiter as terminator and handling the % format strings
	// This function should be called when pointing at the initial delimiter (which may de different than uDelimiter)
	void computeBlocksParseStringWithCFormat(ushort uDelimiter,QColor * pColor);

	// Parse a number. This is something in the form DDDD or DDDD.DDDD or .DDDD or .eDDD
	void computeBlocksParseNumber();
	
	// Skip tabs or spaces
	void computeBlocksSkipWhiteSpace();

	enum ParseMultiLineBlockWithLongTerminatorFlags
	{
		ParseMultiLineBlockWithLongTerminatorHandleEscapeSequences = 1
	};

	// Parse a block with a long terminator (something like the triple quote python script, for example)
	// WARNING: If the terminator is equal to the initial delimiter then you *MUST* handle
	// at least the first character yourself, otherwise this function will exit after it.
	void computeBlocksParseMultiLineBlockWithLongTerminator(
			const char * szTerminator,
			unsigned int uTerminatorLength,
			unsigned int uInterLineFlag,
			QColor * pColor,
			unsigned int uFlags
		);
	

}; // class C3TextEditorModeParser

#endif //!_C3TextEditorModeParser_h_
