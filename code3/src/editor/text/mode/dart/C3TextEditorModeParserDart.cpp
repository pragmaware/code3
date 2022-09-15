//=============================================================================
//
//   File : C3TextEditorModeParserDart.cpp
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

#include "C3TextEditorModeParserDart.h"

#include "C3TextEditorOptions.h"

C3TextEditorModeParserDart::C3TextEditorModeParserDart(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserDart::~C3TextEditorModeParserDart()
{
}

void C3TextEditorModeParserDart::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)
#define BUILTIN_TYPE(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinTypeTextColor)

	BUILTIN_TYPE("int");
	BUILTIN_TYPE("bool");
	BUILTIN_TYPE("double");
	BUILTIN_TYPE("void");
	BUILTIN_TYPE("String");
	BUILTIN_TYPE("List");
	BUILTIN_TYPE("Set");
	BUILTIN_TYPE("Map");
	BUILTIN_TYPE("Symbol");
	BUILTIN_TYPE("Null");
	BUILTIN_TYPE("Object");
	BUILTIN_TYPE("Enum");
	BUILTIN_TYPE("Future");
	BUILTIN_TYPE("Stream");
	BUILTIN_TYPE("Iterable");
	BUILTIN_TYPE("Never");


	KEYWORD("abstract");
	KEYWORD("as");
	KEYWORD("assert");
	KEYWORD("async");
	KEYWORD("await");
	KEYWORD("break");
	KEYWORD("case");
	KEYWORD("catch");
	KEYWORD("class");
	KEYWORD("const");
	KEYWORD("continue");
	KEYWORD("covariant");
	KEYWORD("default");
	KEYWORD("deferred");
	KEYWORD("do");
	KEYWORD("dynamic");
	KEYWORD("else");
	KEYWORD("enum");
	KEYWORD("export");
	KEYWORD("extends");
	KEYWORD("extension");
	KEYWORD("external");
	KEYWORD("false");
	KEYWORD("final");
	KEYWORD("finally");
	KEYWORD("for");
	KEYWORD("function");
	KEYWORD("Function");
	KEYWORD("get");
	KEYWORD("hide");
	KEYWORD("if");
	KEYWORD("import");
	KEYWORD("in");
	KEYWORD("interface");
	KEYWORD("is");
	KEYWORD("late");
	KEYWORD("library");
	KEYWORD("mixin");
	KEYWORD("new");
	KEYWORD("null");
	KEYWORD("operator");
	KEYWORD("part");
	KEYWORD("required");
	KEYWORD("rethrow");
	KEYWORD("return");
	KEYWORD("show");
	KEYWORD("static");
	KEYWORD("super");
	KEYWORD("switch");
	KEYWORD("sync");
	KEYWORD("this");
	KEYWORD("throw");
	KEYWORD("true");
	KEYWORD("try");
	KEYWORD("typedef");
	KEYWORD("var");
	KEYWORD("while");
	KEYWORD("with");
	KEYWORD("yield");

	///BUILTIN_OBJECT("Function");


}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserDart::computeBlocksParseRegExp()
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

void C3TextEditorModeParserDart::computeBlocksParseIdentifier()
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

void C3TextEditorModeParserDart::computeMetadata()
{
	bool bInDartMultiLineComment = m_p->uInterLineFlags & InDartMultiLineComment;

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
		if(m_p->uInterLineFlags & InDartMultiLineComment)
		{
			computeBlocksParseCLikeMultiLineComment(InDartMultiLineComment);
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
						m_p->uInterLineFlags |= InDartMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InDartMultiLineComment);
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