//=============================================================================
//
//   File : C3TextEditorModeParserJavascript.cpp
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

#include "C3TextEditorModeParserJavascript.h"

#include "C3TextEditorOptions.h"

C3TextEditorModeParserJavascript::C3TextEditorModeParserJavascript(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserJavascript::~C3TextEditorModeParserJavascript()
{
}

void C3TextEditorModeParserJavascript::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)

	KEYWORD("abstract");
	KEYWORD("arguments");
	KEYWORD("boolean");
	KEYWORD("break");
	KEYWORD("byte");
	KEYWORD("case");
	KEYWORD("catch");
	KEYWORD("char");
	KEYWORD("class");
	KEYWORD("const");
	KEYWORD("constructor");
	KEYWORD("continue");
	KEYWORD("debugger");
	KEYWORD("default");
	KEYWORD("delete");
	KEYWORD("do");
	KEYWORD("double");
	KEYWORD("else");
	KEYWORD("enum");
	KEYWORD("eval");
	KEYWORD("export");
	KEYWORD("extends");
	KEYWORD("false");
	KEYWORD("final");
	KEYWORD("finally");
	KEYWORD("float");
	KEYWORD("for");
	KEYWORD("function");
	KEYWORD("goto");
	KEYWORD("if");
	KEYWORD("implements");
	KEYWORD("import");
	KEYWORD("in");
	KEYWORD("Infinity"); // not properly a keyword: member of global object
	KEYWORD("instanceof");
	KEYWORD("int");
	KEYWORD("interface");
	KEYWORD("let");
	KEYWORD("long");
	KEYWORD("NaN"); // not properly a keyword: member of global object
	KEYWORD("native");
	KEYWORD("new");
	KEYWORD("null");
	KEYWORD("package");
	KEYWORD("private");
	KEYWORD("protected");
	KEYWORD("public");
	KEYWORD("return");
	KEYWORD("short");
	KEYWORD("static");
	KEYWORD("super");
	KEYWORD("switch");
	KEYWORD("synchronized");
	KEYWORD("this");
	KEYWORD("throw");
	KEYWORD("throws");
	KEYWORD("transient");
	KEYWORD("true");
	KEYWORD("try");
	KEYWORD("typeof");
	KEYWORD("undefined"); // not properly a keyword: member of global object
	KEYWORD("var");
	KEYWORD("void");
	KEYWORD("volatile");
	KEYWORD("while");
	KEYWORD("with");
	KEYWORD("yield");

	BUILTIN_OBJECT("Function");
	BUILTIN_OBJECT("Boolean");
	BUILTIN_OBJECT("Symbol");
	BUILTIN_OBJECT("Error");
	BUILTIN_OBJECT("EvalError");
	BUILTIN_OBJECT("RangeError");
	BUILTIN_OBJECT("ReferenceError");
	BUILTIN_OBJECT("SyntaxError");
	BUILTIN_OBJECT("TypeError");
	BUILTIN_OBJECT("URIError");
	BUILTIN_OBJECT("Date");
	BUILTIN_OBJECT("Array");
	BUILTIN_OBJECT("Int8Array");
	BUILTIN_OBJECT("Uint8Array");
	BUILTIN_OBJECT("Uint8ClampedArray");
	BUILTIN_OBJECT("Int16Array");
	BUILTIN_OBJECT("Uint16Array");
	BUILTIN_OBJECT("Int32Array");
	BUILTIN_OBJECT("Uint32Array");
	BUILTIN_OBJECT("Float32Array");
	BUILTIN_OBJECT("Float64Array");
	BUILTIN_OBJECT("Object");
	BUILTIN_OBJECT("Number");
	BUILTIN_OBJECT("String");
	BUILTIN_OBJECT("RegExp");
	BUILTIN_OBJECT("Math");
	BUILTIN_OBJECT("Map");
	BUILTIN_OBJECT("Set");
	BUILTIN_OBJECT("WeakMap");
	BUILTIN_OBJECT("WeakSet");
	BUILTIN_OBJECT("ArrayBuffer");
	BUILTIN_OBJECT("DataView");
	BUILTIN_OBJECT("JSON");
	BUILTIN_OBJECT("Promise");
	BUILTIN_OBJECT("Generator");
	BUILTIN_OBJECT("GeneratorFunction");
	BUILTIN_OBJECT("Reflect");
	BUILTIN_OBJECT("Proxy");
	BUILTIN_OBJECT("Intl");
	BUILTIN_OBJECT("Collator");
	BUILTIN_OBJECT("DateTimeFormat");
	BUILTIN_OBJECT("NumberFormat");
	BUILTIN_OBJECT("jQuery");

	// sure?
	BUILTIN_OBJECT("window");
	BUILTIN_OBJECT("document");
	BUILTIN_OBJECT("navigator");
	BUILTIN_OBJECT("location");
	BUILTIN_OBJECT("history");
	BUILTIN_OBJECT("screen");

}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserJavascript::computeBlocksParseRegExp()
{
	const QChar * b = m_p->p;
	
	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case '/':
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(!m_p->p->isLetter())
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oRegExpTextColor,0);
				return;
			break;
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
					goto unterminated_regexp;
				if(m_p->p->unicode() == (ushort)'\t')
				{
					// special case of \<tab>
					BLOCK(b,m_p->p - b,oRegExpTextColor,0);
					b = m_p->p;
					m_p->p++;
					BLOCK(b,m_p->p - b,oRegExpTextColor,C3TextEditorLine::Block::IsTabBlock);
					b = m_p->p;
				} else {
					m_p->p++; // skip escape
				}
			break;
			case '\t':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oRegExpTextColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			default:
			{
				// non tab
				m_p->p++;
				
				// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
				const QChar * tmpEnd = m_p->p + 256;
				if(tmpEnd > m_p->e)
					tmpEnd = m_p->e;
				
				while(m_p->p < tmpEnd)
				{
					ushort u = m_p->p->unicode();
					if((u == (ushort)'\t') || (u == (ushort)'/') || (u == (ushort)'\\'))
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oRegExpTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

unterminated_regexp:
	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oRegExpTextColor,0);
}

void C3TextEditorModeParserJavascript::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(m_hIdentifierColorHash.isEmpty())
		buildIdentifierColorHash();

	pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
	
	if(!pBlock->pColor)
	{
		static QString szM_("m_");
		static QString szG_("g_");
		if(pBlock->szText.startsWith(szM_))
			pBlock->pColor = &(m_p->pCoreData->pOptions->oMUnderscoreIdentifierTextColor);
		else if(pBlock->szText.startsWith(szG_))
			pBlock->pColor = &(m_p->pCoreData->pOptions->oGUnderscoreIdentifierTextColor);
		else
			pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
	}
}

void C3TextEditorModeParserJavascript::computeMetadata()
{
	bool bInJavascriptMultiLineComment = m_p->uInterLineFlags & InJavascriptMultiLineComment;

	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	bool bRegExpMayFollow = true;

	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InJavascriptMultiLineComment)
		{
			computeBlocksParseCLikeMultiLineComment(InJavascriptMultiLineComment);
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '/':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				switch(m_p->p->unicode())
				{
					case '*':
						m_p->uInterLineFlags |= InJavascriptMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InJavascriptMultiLineComment);
						bRegExpMayFollow = true; // not true in corner cases
					break;
					case '/':
						m_p->p++;
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseSingleLineComment();
						bRegExpMayFollow = false;
					break;
					case '=':
						m_p->p++;
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
						bRegExpMayFollow = true;
					break;
					case '\t':
						// stay here!
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
						bRegExpMayFollow = false;
					break;
					default: // anything else
						if(bRegExpMayFollow)
						{
							BLOCK(b,m_p->p - b,oRegExpTextColor,0);
							computeBlocksParseRegExp();
							bRegExpMayFollow = false;
						} else {
							BLOCK(b,m_p->p - b,oOperatorTextColor,0);
							bRegExpMayFollow = true; // not true in corner cases
						}
						b = m_p->p;
					break;
				}
			}
			break;
			case '\'':
				computeBlocksParseString('\'',&(m_p->pCoreData->pOptions->oStringTextColor));
				bRegExpMayFollow = false;
			break;
			case '"':
				computeBlocksParseString('"',&(m_p->pCoreData->pOptions->oStringTextColor));
				bRegExpMayFollow = false;
			break;
			case '{':
			case '}':
				m_p->p++;
				BLOCK(b,m_p->p - b,oCodeBlockDelimiterTextColor,0);
				bRegExpMayFollow = false; // not true for edge cases
			break;
			case '$':
				m_p->p++;
				BLOCK(b,m_p->p - b,oBuiltinObjectTextColor,0);
				bRegExpMayFollow = false; // not true for edge cases
			break;
			case '(':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)'(')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oParenthesisTextColor,0);
				bRegExpMayFollow = true;
			}
			break;
			case ')':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)')')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oParenthesisTextColor,0);
				bRegExpMayFollow = false;
			}
			break;
			case '[':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)'[')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oArrayDelimiterTextColor,0);
				bRegExpMayFollow = true;
			}
			break;
			case ']':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)']')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oArrayDelimiterTextColor,0);
				bRegExpMayFollow = false;
			}
			break;
			case '=':
			case ':':
			case '*':
			case '+':
			case '?':
			case '!':
			case '>':
			case '<':
			case '|':
			case '&':
			case '%':
			case '-':
			case ';':
			case ',':
			case '.':
			case '~':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(
						(u != (ushort)'=') &&
						(u != (ushort)':') &&
						(u != (ushort)'+') &&
						(u != (ushort)'-') &&
						(u != (ushort)'?') &&
						(u != (ushort)'!') &&
						(u != (ushort)'>') &&
						(u != (ushort)'<') &&
						(u != (ushort)'|') &&
						(u != (ushort)'&') &&
						(u != (ushort)'%') &&
						(u != (ushort)';') &&
						(u != (ushort)',') &&
						(u != (ushort)'~') &&
						(u != (ushort)'.') &&
						(u != (ushort)'*')
					)
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
				bRegExpMayFollow = true; // not true for some edge cases
			}
			break;
			case '\t':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oTextColor,C3TextEditorLine::Block::IsTabBlock);
				bRegExpMayFollow = true;
			}
			break;
			case ' ':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)' ')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oTextColor,0);
			}
			break;
			default:
			{
				if(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'))
				{
					computeBlocksParseIdentifier();
				} else if(m_p->p->isNumber())
				{
					computeBlocksParseNumber();
				} else {
					m_p->p++;

					// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
					const QChar * tmpEnd = m_p->p + 256;
					if(tmpEnd > m_p->e)
						tmpEnd = m_p->e;
					
					while(m_p->p < tmpEnd)
					{
						ushort u = m_p->p->unicode();
						if(
								m_p->p->isLetterOrNumber() ||
								(u == (ushort)'_') ||
								(u == (ushort)'\t') ||
								(u == (ushort)'/') ||
								(u == (ushort)'(') ||
								(u == (ushort)')') ||
								(u == (ushort)'[') ||
								(u == (ushort)']') ||
								(u == (ushort)'{') ||
								(u == (ushort)'}') ||
								(u == (ushort)'=') ||
								(u == (ushort)':') ||
								(u == (ushort)'*') ||
								(u == (ushort)'+') ||
								(u == (ushort)'?') ||
								(u == (ushort)'>') ||
								(u == (ushort)'<') ||
								(u == (ushort)'!') ||
								(u == (ushort)'|') ||
								(u == (ushort)'&') ||
								(u == (ushort)'%') ||
								(u == (ushort)';') ||
								(u == (ushort)'.') ||
								(u == (ushort)',') ||
								(u == (ushort)'\'') ||
								(u == (ushort)'"') ||
								(u == (ushort)'$') ||
								(u == (ushort)' ') ||
								(u == (ushort)'-')
							)
							break;
						m_p->p++;
					}

					BLOCK(b,m_p->p - b,oErrorTextColor,0);
				}
				bRegExpMayFollow = false;
			}
			break;
		}
	}
}