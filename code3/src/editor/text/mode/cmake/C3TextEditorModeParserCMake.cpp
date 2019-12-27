//=============================================================================
//
//   File : C3TextEditorModeParserCMake.cpp
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

#include "C3TextEditorModeParserCMake.h"

#include "C3TextEditorOptions.h"

C3TextEditorModeParserCMake::C3TextEditorModeParserCMake(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserCMake::~C3TextEditorModeParserCMake()
{
}

void C3TextEditorModeParserCMake::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)

	KEYWORD("if");
	KEYWORD("endif");
	KEYWORD("macro");
	KEYWORD("endmacro");
	KEYWORD("function");
	KEYWORD("endfunction");
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserCMake::computeBlocksParseMultiLineComment()
{
	Q_ASSERT(m_p->uInterLineFlags & InCMakeMultiLineComment);

	const QChar * b = m_p->p;
	
	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case ']':
				m_p->p++;
				
				if(m_p->p >= m_p->e)
					goto end_of_line_in_comment;

				if(m_p->p->unicode() != ']')
				{
					BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
					b = m_p->p;
					continue;
				}

				m_p->p++;
				BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
				m_p->uInterLineFlags &= ~InCMakeMultiLineComment;
				if(m_p->p >= m_p->e)
				{
					// end of line just after a multi line comment end. store an additional empty block to signal state change
					BLOCK(0,0,oTextColor,0);
				}
				return;
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
	
				BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			case 'T':
				// TODO
				if(pointingAtString("TODO",4))
				{
					if(m_p->p > b)
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
					b = m_p->p;
					m_p->p += 4;
					BLOCK(b,m_p->p - b,oFixmeTodoTextColor,0);
					b = m_p->p;
				} else {
					m_p->p++;
				}
			break;
			case 'F':
				// FIXME
				if(pointingAtString("FIXME",5))
				{
					if(m_p->p > b)
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
					b = m_p->p;
					m_p->p += 5;
					BLOCK(b,m_p->p - b,oFixmeTodoTextColor,0);
					b = m_p->p;
				} else {
					m_p->p++;
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
					if(
							(u == (ushort)'\t') ||
							(u == (ushort)'-') ||
							(u == (ushort)'F') ||
							(u == (ushort)'T')
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

end_of_line_in_comment:
	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
}


void C3TextEditorModeParserCMake::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(m_hIdentifierColorHash.isEmpty())
		buildIdentifierColorHash();

	if(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case '(':
				pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
				if(!pBlock->pColor)
					pBlock->pColor = &(m_p->pCoreData->pOptions->oUserKeywordTextColor);
			break;
			case ' ':
				// tolerate spaces too
				b = m_p->p;
				m_p->p++;
				while((m_p->p < m_p->e) && (m_p->p->unicode() == ' '))
					m_p->p++;
				if((m_p->p < m_p->e) && (m_p->p->unicode() == '('))
				{
					pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
					if(!pBlock->pColor)
						pBlock->pColor = &(m_p->pCoreData->pOptions->oUserKeywordTextColor);
				}
				m_p->p = b;
			break;
			default:
				// nuthin
			break;
		}
	}

	if(!pBlock->pColor)
		pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
	
}

void C3TextEditorModeParserCMake::computeBlocksParseVariable(QColor * pColor)
{
	// In CMake variables can nest!
	Q_ASSERT(m_p->p->unicode() == (ushort)'$');

	const QChar * b = m_p->p;
	
	m_p->p++;

	if(m_p->p >= m_p->e)
	{
		// FIXME: could be unterminated var in the process of being written
		BLOCK(b,m_p->p - b,oErrorTextColor,0);
		return;
	}

	bool bRequireBracket;
	
	if(m_p->p->unicode() == '{')
	{
		m_p->p++;

		if(m_p->p >= m_p->e)
		{
			// FIXME: could be unterminated var in the process of being written
			BLOCK(b,m_p->p - b,oErrorTextColor,0);
			return;
		}
	
		bRequireBracket = true;
	} else {
		bRequireBracket = false;
	}


	bool bError = false;

	while(m_p->p < m_p->e)
	{
		ushort u = m_p->p->unicode();
		switch(u)
		{
			case '}':
				if(!bRequireBracket)
					goto end_of_variable;
				m_p->p++;
				goto end_of_variable;
			break;
			case '$':
				createBlock(b,m_p->p - b,pColor,0);
				computeBlocksParseVariable(pColor);
				b = m_p->p;
			break;
			default:
				if(!(m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
				{
					if(bRequireBracket)
						bError = true;
					goto end_of_variable;
				}
				m_p->p++;
			break;
		}
	}

end_of_variable:
	if(m_p->p > b)
	{
		if(bError)
			BLOCK(b,m_p->p - b,oErrorTextColor,0);
		else
			createBlock(b,m_p->p - b,pColor,0);
	}
}


void C3TextEditorModeParserCMake::computeBlocksParseMultiLineString()
{
	// This is called when m_p->p is point AFTER the initial delimiter!

	Q_ASSERT(m_p->uInterLineFlags & InCMakeMultiLineString); // you must set the flag before calling this function


	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();

		switch(uChar)
		{
			case '"':
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				m_p->uInterLineFlags &= ~InCMakeMultiLineString;
				if(m_p->p >= m_p->e)
				{
					// end of line just after a multi line comment end. store an additional empty block to signal state change
					BLOCK(0,0,oTextColor,0);
				}
				return;
			break;
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					// line continuation
					if((m_p->p - b) > 1)
						BLOCK(b,m_p->p - b - 1,oStringTextColor,0);
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				if(m_p->p->unicode() == (ushort)'\t')
				{
					// special case of \<tab>
					BLOCK(b,m_p->p - b,oStringTextColor,0);
					b = m_p->p;
					m_p->p++;
					BLOCK(b,m_p->p - b,oStringTextColor,C3TextEditorLine::Block::IsTabBlock);
					b = m_p->p;
				} else {
					m_p->p++; // skip escape
				}
			break;
			case '\t':
			{
				if(m_p->p > b)
				{
					BLOCK(b,m_p->p - b,oStringTextColor,0);
					b = m_p->p;
				}
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oStringTextColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			case '$':
				if(m_p->p > b)
					BLOCK(b,m_p->p - b,oStringTextColor,0);
				computeBlocksParseVariable(&(m_p->pCoreData->pOptions->oVariableInStringTextColor));
				b = m_p->p;
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
					if(
							(u == (ushort)'\t') ||
							(u == (ushort)'\\') ||
							(u == (ushort)'$') ||
							(u == (ushort)'"')
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oStringTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oStringTextColor,0);
}



void C3TextEditorModeParserCMake::computeMetadata()
{
	bool bInCMakeMultiLineComment = m_p->uInterLineFlags & InCMakeMultiLineComment;

	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InCMakeMultiLineComment)
		{
			computeBlocksParseMultiLineComment();
			continue;
		}

		if(m_p->uInterLineFlags & InCMakeMultiLineString)
		{
			computeBlocksParseMultiLineString();
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '#':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
					return;
				}
				
				if(m_p->p->unicode() != '[')
				{
					m_p->p = b;
					computeBlocksParseSingleLineComment();
				} else {
					m_p->p++;

					if(m_p->p >= m_p->e)
					{
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
						return;
					}

					if(m_p->p->unicode() != '[')
					{
						m_p->p = b;
						computeBlocksParseSingleLineComment();
					} else {
						m_p->uInterLineFlags |= InCMakeMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseMultiLineComment();
					}
				}
			}
			break;
			case '"':
				m_p->uInterLineFlags |= InCMakeMultiLineString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseMultiLineString();
			break;
			case '{':
			case '}':
				m_p->p++;
				BLOCK(b,m_p->p - b,oCodeBlockDelimiterTextColor,0);
			break;
			case '$':
				computeBlocksParseVariable(&(m_p->pCoreData->pOptions->oVariableTextColor));
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
			case '-':
			case '/':
			case ';':
			case ',':
			case '.':
			case '\'':
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
						(u != (ushort)'/') &&
						(u != (ushort)'!') &&
						(u != (ushort)'>') &&
						(u != (ushort)'<') &&
						(u != (ushort)'|') &&
						(u != (ushort)'&') &&
						(u != (ushort)'%') &&
						(u != (ushort)';') &&
						(u != (ushort)',') &&
						(u != (ushort)'.') &&
						(u != (ushort)'\'') &&
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
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					// line continuation
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				if(m_p->p->unicode() == (ushort)'\t')
				{
					// special case of \<tab>
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					b = m_p->p;
					m_p->p++;
					BLOCK(b,m_p->p - b,oOperatorTextColor,C3TextEditorLine::Block::IsTabBlock);
					b = m_p->p;
				} else {
					m_p->p++; // skip escape
					BLOCK(b,m_p->p - b,oTextColor,0);
				}
			break;
			default:
			{
				if(m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_'))
				{
					computeBlocksParseIdentifier();
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
								(u == (ushort)'#') ||
								(u == (ushort)';') ||
								(u == (ushort)'.') ||
								(u == (ushort)',') ||
								(u == (ushort)'\'') ||
								(u == (ushort)'"') ||
								(u == (ushort)'$') ||
								(u == (ushort)' ') ||
								(u == (ushort)'\\') ||
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
