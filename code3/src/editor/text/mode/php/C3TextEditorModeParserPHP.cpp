//=============================================================================
//
//   File : C3TextEditorModeParserPHP.cpp
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

#include "C3TextEditorModeParserPHP.h"
#include "C3TextEditorModeWithInterLineState.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorModePHPSharedData.h"

#include "C3TextEditorOptions.h"

C3TextEditorModeParserPHP::C3TextEditorModeParserPHP(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState),
	m_pSharedData(pParserState->pMode->factory()->modePHPSharedData()),
	m_hIdentifierColorHash(m_pSharedData->identifierColorHash()),
	m_hIdentifierPrefixColorHash(m_pSharedData->identifierPrefixColorHash()),
	m_hIdentifierPostfixColorHash(m_pSharedData->identifierPostfixColorHash()),
	m_hIdentifierPrefixWithUppercaseColorHash(m_pSharedData->identifierPrefixWithUppercaseColorHash())
{
}

C3TextEditorModeParserPHP::~C3TextEditorModeParserPHP()
{
}


#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserPHP::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_') || (m_p->p->unicode() == (ushort)'$'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
	if(pBlock->pColor)
		return;

	QHash<QString,QColor *>::const_iterator it;
	
	for(it = m_hIdentifierPrefixColorHash.begin();it != m_hIdentifierPrefixColorHash.end();++it)
	{
		if(pBlock->szText.startsWith(it.key()))
		{
			pBlock->pColor = it.value();
			return;
		}
	}

	for(it = m_hIdentifierPrefixWithUppercaseColorHash.begin();it != m_hIdentifierPrefixWithUppercaseColorHash.end();++it)
	{
		QString szKey = it.key();
		int len = szKey.length();
		if(
				(pBlock->szText.length() > len) &&
				(pBlock->szText.startsWith(it.key())) &&
				(pBlock->szText.at(len).isUpper() || pBlock->szText.at(len).isNumber())
			)
		{
			pBlock->pColor = it.value();
			return;
		}
	}

	for(it = m_hIdentifierPostfixColorHash.begin();it != m_hIdentifierPostfixColorHash.end();++it)
	{
		if(pBlock->szText.endsWith(it.key()))
		{
			pBlock->pColor = it.value();
			return;
		}
	}

	pBlock->pColor = (b->unicode() == '$') ? &(m_p->pCoreData->pOptions->oVariableTextColor) : &(m_p->pCoreData->pOptions->oTextColor);
}


void C3TextEditorModeParserPHP::computeMetadata()
{
	Q_ASSERT(m_p->uInterLineFlags & InXMLPHP);

	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InPHPMultiLineComment)
		{
			computeBlocksParseCLikeMultiLineComment(InPHPMultiLineComment);
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '?':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				if(m_p->p->unicode() == '>')
				{
					m_p->p++;
					BLOCK(b,m_p->p - b,oOperatorTextColor,0); // FIXME: Terminator?
					m_p->uInterLineFlags &= ~InXMLPHP; // Clear html php state
					if(m_p->p >= m_p->e)
						BLOCK(0,0,oTextColor,0); // empty block to signal that php finished
					return;
				} else {
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
				}
			break;
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
						m_p->uInterLineFlags |= InPHPMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InPHPMultiLineComment);
					break;
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
					break;
					default: // anything else
						// stay here!
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
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
			case '<':
			case '=':
			case ':':
			case '*':
			case '+':
			case '!':
			case '>':
			case '|':
			case '&':
			case '%':
			case '-':
			case ';':
			case ',':
			case '.':
			case '\\':
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
						(u != (ushort)'\\') &&
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
				if(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_') || (m_p->p->unicode() == (ushort)'$'))
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
								(u == (ushort)'\\') ||
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