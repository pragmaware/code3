#ifndef _C3TextEditorOptionsColors_h_
#define _C3TextEditorOptionsColors_h_
//=============================================================================
//
//   File : C3TextEditorOptionsColors.h
//   Creation Date : sab 24 ott 2015 04:54:22
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

class QSettings;

#include <QFont>
#include <QColor>

class C3TextEditorOptionsColors
{
public:

	QColor oMarginTextColor; // Margin Text
	QColor oMarginBackgroundColor; // Margin Background
	QColor oMarginActiveTextColor; // Active Margin Text
	QColor oMarginActiveBackgroundColor; // Active Margin Background
	QColor oCursorColor; // Cursor
	QColor oTextBackgroundColor; // Text Background
	QColor oEditorBackgroundColor; // Editor Background
	QColor oMarkBackgroundColor; // Mark Background
	QColor oCurrentLineBackgroundColor; // Current Line Background
	QColor oTextColor; // Default Text
	QColor oTabColor; // Tab Marker
	QColor oErrorTextColor; // Errors
	QColor oStringTextColor; // Strings
	QColor oUnterminatedStringTextColor; // Unterminated Strings
	QColor oNumberTextColor; // Numbers
	QColor oSingleLineCommentTextColor; // Single Line Comments
	QColor oMultiLineCommentTextColor; // Multi Line Comments
	QColor oOperatorTextColor; // Operators
	QColor oMarkupTagTextColor; // Markup Tags
	QColor oMarkupTagNameTextColor; // Markup Tag Names
	QColor oVariableTextColor; // Variables
	QColor oVariableInStringTextColor; // Variable in String
	QColor oAnnotationTextColor; // Annotations
	QColor oKeywordTextColor; // Keywords
	QColor oUserKeywordTextColor; // User Keywords
	QColor oBuiltinTypeTextColor; // Types
	QColor oUserTypeTextColor; // User Types
	QColor oBuiltinObjectTextColor; // Objects
	QColor oUserObjectTextColor; // User Objects
	QColor oCodeBlockDelimiterTextColor; // Code Block Delimiters
	QColor oParenthesisTextColor; // Parenthesis
	QColor oArrayDelimiterTextColor; // Array Delimiters
	QColor oPreprocessorTextColor; // Preprocessor Statements
	QColor oIncludeFileTextColor; // Include Files
	QColor oUnderscoreIdentifierTextColor; // Underscore Identifiers
	QColor oMUnderscoreIdentifierTextColor; // M-Underscore Identifiers
	QColor oGUnderscoreIdentifierTextColor; // G-Underscore Identifiers
	QColor oUserCustom1TextColor; // User Custom 1
	QColor oUserCustom2TextColor; // User Custom 2
	QColor oUserCustom3TextColor; // User Custom 3
	QColor oRegExpTextColor; // Regular Expressions
	QColor oFixmeTodoTextColor; // FIXME/TODO Markers
	QColor oHeredocTextColor; // Heredocs
	QColor oVariableInHeredocTextColor; // Variable in Heredoc

public:
	void loadColors(QSettings &s);
	void saveColors(QSettings &s);
	
	void copyColors(const C3TextEditorOptionsColors &src);

};

#endif //!_C3TextEditorOptionsColors_h_
