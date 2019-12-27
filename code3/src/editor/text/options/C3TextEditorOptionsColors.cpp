//=============================================================================
//
//   File : C3TextEditorOptionsColors.cpp
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

#include "C3TextEditorOptions.h"

#include <QSettings>

void C3TextEditorOptionsColors::loadColors(QSettings &s)
{

#define LOAD_COLOR(_oObject,_szOptionName,_iDefaultR,_iDefaultG,_iDefaultB) \
	do { \
		QVariant v = s.value(_szOptionName); \
		if((!v.isNull()) && v.canConvert<QColor>()) \
		{ \
			_oObject = v.value<QColor>(); \
		} else { \
			_oObject = QColor(_iDefaultR,_iDefaultG,_iDefaultB); \
		} \
	} while(0)

	LOAD_COLOR(oMarginTextColor,"marginTextColor",78,95,68);
	LOAD_COLOR(oMarginBackgroundColor,"marginBackgroundColor",30,41,30);
	LOAD_COLOR(oMarginActiveTextColor,"marginActiveTextColor",110,119,63);
	LOAD_COLOR(oMarginActiveBackgroundColor,"marginActiveBackgroundColor",49,66,49);
	LOAD_COLOR(oCursorColor,"cursorColor",255,0,0);
	LOAD_COLOR(oTextBackgroundColor,"textBackgroundColor",0,0,0);
	LOAD_COLOR(oEditorBackgroundColor,"editorBackgroundColor",10,10,10);
	LOAD_COLOR(oMarkBackgroundColor,"markBackgroundColor",20,0,30);
	LOAD_COLOR(oCurrentLineBackgroundColor,"currentLineBackgroundColor",2,2,2);
	LOAD_COLOR(oTextColor,"textColor",90,255,0);
	LOAD_COLOR(oTabColor,"tabColor",40,40,40);
	LOAD_COLOR(oErrorTextColor,"errorTextColor",255,0,0);
	LOAD_COLOR(oStringTextColor,"stringTextColor",99,172,255);
	LOAD_COLOR(oUnterminatedStringTextColor,"unterminatedStringTextColor",188,53,244);
	LOAD_COLOR(oNumberTextColor,"numberTextColor",190,200,255);
	LOAD_COLOR(oSingleLineCommentTextColor,"singleLineCommentTextColor",20,180,0);
	LOAD_COLOR(oMultiLineCommentTextColor,"multiLineCommentTextColor",40,150,0);
	LOAD_COLOR(oOperatorTextColor,"operatorTextColor",150,150,40);
	LOAD_COLOR(oMarkupTagTextColor,"markupTagTextColor",171,97,188);
	LOAD_COLOR(oMarkupTagNameTextColor,"markupTagNameTextColor",171,140,231);
	LOAD_COLOR(oVariableTextColor,"variableTextColor",230,210,25);
	LOAD_COLOR(oVariableInStringTextColor,"variableInStringTextColor",177,161,255);
	LOAD_COLOR(oAnnotationTextColor,"annotationTextColor",210,130,200);
	LOAD_COLOR(oKeywordTextColor,"keywordTextColor",130,120,130);
	LOAD_COLOR(oUserKeywordTextColor,"userKeywordTextColor",130,140,150);
	LOAD_COLOR(oBuiltinTypeTextColor,"builtinBuiltinTypeTextColor",140,140,110);
	LOAD_COLOR(oUserTypeTextColor,"userBuiltinTypeTextColor",150,140,100);
	LOAD_COLOR(oBuiltinObjectTextColor,"builtinObjectTextColor",200,200,100);
	LOAD_COLOR(oUserObjectTextColor,"userBuiltinObjectTextColor",255,255,50);
	LOAD_COLOR(oCodeBlockDelimiterTextColor,"codeBlockDelimiterTextColor",255,30,30);
	LOAD_COLOR(oParenthesisTextColor,"parenthesisTextColor",160,160,0);
	LOAD_COLOR(oArrayDelimiterTextColor,"arrayDelimiterTextColor",190,160,0);
	LOAD_COLOR(oPreprocessorTextColor,"preprocessorTextColor",255,255,255);
	LOAD_COLOR(oIncludeFileTextColor,"includeFileTextColor",200,200,200);
	LOAD_COLOR(oUnderscoreIdentifierTextColor,"underscoreIdentifierTextColor",255,80,255);
	LOAD_COLOR(oMUnderscoreIdentifierTextColor,"mUnderscoreIdentifierTextColor",190,170,80);
	LOAD_COLOR(oGUnderscoreIdentifierTextColor,"gUnderscoreIdentifierTextColor",210,170,80);
	LOAD_COLOR(oUserCustom1TextColor,"userCustom1TextColor",255,200,0);
	LOAD_COLOR(oUserCustom2TextColor,"userCustom2TextColor",200,200,80);
	LOAD_COLOR(oUserCustom3TextColor,"userCustom3TextColor",210,200,100);
	LOAD_COLOR(oRegExpTextColor,"regExpTextColor",140,142,245);
	LOAD_COLOR(oFixmeTodoTextColor,"fixmeToDoTextColor",255,150,12);
	LOAD_COLOR(oHeredocTextColor,"heredocTextColor",140,162,235);
	LOAD_COLOR(oVariableInHeredocTextColor,"variableInHeredocTextColor",207,151,235);

}

void C3TextEditorOptionsColors::saveColors(QSettings &s)
{

#define SAVE_COLOR(_oObject,_szOptionName) \
	do { \
		s.setValue(_szOptionName,QVariant(_oObject)); \
	} while(0)

	SAVE_COLOR(oMarginTextColor,"marginTextColor");
	SAVE_COLOR(oMarginBackgroundColor,"marginBackgroundColor");
	SAVE_COLOR(oMarginActiveTextColor,"marginActiveTextColor");
	SAVE_COLOR(oMarginActiveBackgroundColor,"marginActiveBackgroundColor");
	SAVE_COLOR(oCursorColor,"cursorColor");
	SAVE_COLOR(oTextBackgroundColor,"textBackgroundColor");
	SAVE_COLOR(oEditorBackgroundColor,"editorBackgroundColor");
	SAVE_COLOR(oMarkBackgroundColor,"markBackgroundColor");
	SAVE_COLOR(oCurrentLineBackgroundColor,"currentLineBackgroundColor");
	SAVE_COLOR(oTextColor,"textColor");
	SAVE_COLOR(oTabColor,"tabColor");
	SAVE_COLOR(oErrorTextColor,"errorTextColor");
	SAVE_COLOR(oStringTextColor,"stringTextColor");
	SAVE_COLOR(oUnterminatedStringTextColor,"unterminatedStringTextColor");
	SAVE_COLOR(oNumberTextColor,"numberTextColor");
	SAVE_COLOR(oSingleLineCommentTextColor,"singleLineCommentTextColor");
	SAVE_COLOR(oMultiLineCommentTextColor,"multiLineCommentTextColor");
	SAVE_COLOR(oOperatorTextColor,"operatorTextColor");
	SAVE_COLOR(oMarkupTagTextColor,"markupTagTextColor");
	SAVE_COLOR(oMarkupTagNameTextColor,"markupTagNameTextColor");
	SAVE_COLOR(oVariableTextColor,"variableTextColor");
	SAVE_COLOR(oVariableInStringTextColor,"variableInStringTextColor");
	SAVE_COLOR(oAnnotationTextColor,"annotationTextColor");
	SAVE_COLOR(oKeywordTextColor,"keywordTextColor");
	SAVE_COLOR(oUserKeywordTextColor,"userKeywordTextColor");
	SAVE_COLOR(oBuiltinTypeTextColor,"builtinBuiltinTypeTextColor");
	SAVE_COLOR(oUserTypeTextColor,"userBuiltinTypeTextColor");
	SAVE_COLOR(oBuiltinObjectTextColor,"builtinObjectTextColor");
	SAVE_COLOR(oUserObjectTextColor,"userBuiltinObjectTextColor");
	SAVE_COLOR(oCodeBlockDelimiterTextColor,"codeBlockDelimiterTextColor");
	SAVE_COLOR(oParenthesisTextColor,"parenthesisTextColor");
	SAVE_COLOR(oArrayDelimiterTextColor,"arrayDelimiterTextColor");
	SAVE_COLOR(oPreprocessorTextColor,"preprocessorTextColor");
	SAVE_COLOR(oIncludeFileTextColor,"includeFileTextColor");
	SAVE_COLOR(oUnderscoreIdentifierTextColor,"underscoreIdentifierTextColor");
	SAVE_COLOR(oMUnderscoreIdentifierTextColor,"mUnderscoreIdentifierTextColor");
	SAVE_COLOR(oGUnderscoreIdentifierTextColor,"gUnderscoreIdentifierTextColor");
	SAVE_COLOR(oUserCustom1TextColor,"userCustom1TextColor");
	SAVE_COLOR(oUserCustom2TextColor,"userCustom2TextColor");
	SAVE_COLOR(oUserCustom3TextColor,"userCustom3TextColor");
	SAVE_COLOR(oRegExpTextColor,"regExpTextColor");
	SAVE_COLOR(oFixmeTodoTextColor,"fixmeToDoTextColor");
	SAVE_COLOR(oHeredocTextColor,"heredocTextColor");
	SAVE_COLOR(oVariableInHeredocTextColor,"variableInHeredocTextColor");

}

void C3TextEditorOptionsColors::copyColors(const C3TextEditorOptionsColors &src)
{

	oMarginTextColor = src.oMarginTextColor;
	oMarginBackgroundColor = src.oMarginBackgroundColor;
	oMarginActiveTextColor = src.oMarginActiveTextColor;
	oMarginActiveBackgroundColor = src.oMarginActiveBackgroundColor;
	oCursorColor = src.oCursorColor;
	oTextBackgroundColor = src.oTextBackgroundColor;
	oEditorBackgroundColor = src.oEditorBackgroundColor;
	oMarkBackgroundColor = src.oMarkBackgroundColor;
	oCurrentLineBackgroundColor = src.oCurrentLineBackgroundColor;
	oTextColor = src.oTextColor;
	oTabColor = src.oTabColor;
	oErrorTextColor = src.oErrorTextColor;
	oStringTextColor = src.oStringTextColor;
	oUnterminatedStringTextColor = src.oUnterminatedStringTextColor;
	oNumberTextColor = src.oNumberTextColor;
	oSingleLineCommentTextColor = src.oSingleLineCommentTextColor;
	oMultiLineCommentTextColor = src.oMultiLineCommentTextColor;
	oOperatorTextColor = src.oOperatorTextColor;
	oMarkupTagTextColor = src.oMarkupTagTextColor;
	oMarkupTagNameTextColor = src.oMarkupTagNameTextColor;
	oVariableTextColor = src.oVariableTextColor;
	oVariableInStringTextColor = src.oVariableInStringTextColor;
	oAnnotationTextColor = src.oAnnotationTextColor;
	oKeywordTextColor = src.oKeywordTextColor;
	oUserKeywordTextColor = src.oUserKeywordTextColor;
	oBuiltinTypeTextColor = src.oBuiltinTypeTextColor;
	oUserTypeTextColor = src.oUserTypeTextColor;
	oBuiltinObjectTextColor = src.oBuiltinObjectTextColor;
	oUserObjectTextColor = src.oUserObjectTextColor;
	oCodeBlockDelimiterTextColor = src.oCodeBlockDelimiterTextColor;
	oParenthesisTextColor = src.oParenthesisTextColor;
	oArrayDelimiterTextColor = src.oArrayDelimiterTextColor;
	oPreprocessorTextColor = src.oPreprocessorTextColor;
	oIncludeFileTextColor = src.oIncludeFileTextColor;
	oUnderscoreIdentifierTextColor = src.oUnderscoreIdentifierTextColor;
	oMUnderscoreIdentifierTextColor = src.oMUnderscoreIdentifierTextColor;
	oGUnderscoreIdentifierTextColor = src.oGUnderscoreIdentifierTextColor;
	oUserCustom1TextColor = src.oUserCustom1TextColor;
	oUserCustom2TextColor = src.oUserCustom2TextColor;
	oUserCustom3TextColor = src.oUserCustom3TextColor;
	oRegExpTextColor = src.oRegExpTextColor;
	oFixmeTodoTextColor = src.oFixmeTodoTextColor;
	oHeredocTextColor = src.oHeredocTextColor;
	oVariableInHeredocTextColor = src.oVariableInHeredocTextColor;

}

