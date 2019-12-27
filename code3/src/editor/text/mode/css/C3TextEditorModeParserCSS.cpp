//=============================================================================
//
//   File : C3TextEditorModeParserCSS.cpp
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

#include "C3TextEditorModeParserCSS.h"

#include "C3TextEditorOptions.h"


C3TextEditorModeParserCSS::C3TextEditorModeParserCSS(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserCSS::~C3TextEditorModeParserCSS()
{
}

void C3TextEditorModeParserCSS::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)

	KEYWORD("yield");

	BUILTIN_OBJECT("Function");
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)


void C3TextEditorModeParserCSS::computeBlocksParseIdentifier()
{
	Q_ASSERT(
			m_p->p->isLetter() ||
			(m_p->p->unicode() == (ushort)'_') ||
			(m_p->p->unicode() == (ushort)'-') ||
			(m_p->p->unicode() == (ushort)'#') ||
			(m_p->p->unicode() == (ushort)'.')
		);
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while(
			(m_p->p < m_p->e) &&
			(
				m_p->p->isLetterOrNumber() ||
				(m_p->p->unicode() == (ushort)'_') ||
				(m_p->p->unicode() == (ushort)'-')
			)
		)
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(m_hIdentifierColorHash.isEmpty())
		buildIdentifierColorHash();

	pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
	
	if(!pBlock->pColor)
		pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
}

void C3TextEditorModeParserCSS::computeMetadata()
{
	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InCSSMultiLineComment)
		{
			computeBlocksParseCLikeMultiLineComment(InCSSMultiLineComment);
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
						m_p->uInterLineFlags |= InCSSMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InCSSMultiLineComment);
					break;
					/*
					case '/':
						m_p->p++;
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseSingleLineComment();
					break;
					case '=':
						m_p->p++;
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
					*/
					case '\t':
						// stay here!
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
					default: // anything else
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
				}
			}
			break;
			case '\'':
				computeBlocksParseString('\'',&(m_p->pCoreData->pOptions->oStringTextColor));
			break;
			case '"':
				computeBlocksParseString('"',&(m_p->pCoreData->pOptions->oStringTextColor));
			break;
			case '{':
			case '}':
				m_p->p++;
				BLOCK(b,m_p->p - b,oCodeBlockDelimiterTextColor,0);
			break;
			case '$':
				m_p->p++;
				BLOCK(b,m_p->p - b,oBuiltinObjectTextColor,0);
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
			case ';':
			case ',':
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
						(u != (ushort)'.') &&
						(u != (ushort)'*')
					)
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
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
				if(
					m_p->p->isLetter() ||
					(m_p->p->unicode() == (ushort)'_') ||
					(m_p->p->unicode() == (ushort)'#') ||
					(m_p->p->unicode() == (ushort)'-') ||
					(m_p->p->unicode() == (ushort)'.')
				)
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
			}
			break;
		}
	}
}