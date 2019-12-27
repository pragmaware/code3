//=============================================================================
//
//   File : C3TextEditorModeParserVHDL.cpp
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

#include "C3TextEditorModeParserVHDL.h"
#include "C3TextEditorModeWithInterLineState.h"
#include "C3TextEditorModeVHDLSharedData.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorFactory.h"


C3TextEditorModeParserVHDL::C3TextEditorModeParserVHDL(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState),
	m_pSharedData(pParserState->pMode->factory()->modeVHDLSharedData()),
	m_lIdentifierColorHash(m_pSharedData->identifierColorHash())
{
}

C3TextEditorModeParserVHDL::~C3TextEditorModeParserVHDL()
{
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)


void C3TextEditorModeParserVHDL::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	int iLen = m_p->p - b;

	C3TextEditorLine::Block * pBlock = createBlock(b,iLen,NULL,0);
	
	if((iLen > 2) && (b[1].unicode() == (ushort)'_'))
	{
		if(b[0].unicode() == (ushort)'v')
			pBlock->pColor = &(m_p->pCoreData->pOptions->oGUnderscoreIdentifierTextColor);
		else if(b[0].unicode() == (ushort)'s')
			pBlock->pColor = &(m_p->pCoreData->pOptions->oMUnderscoreIdentifierTextColor);
		else {
			pBlock->pColor = m_lIdentifierColorHash.value(pBlock->szText.toLower(),NULL);
			if(!pBlock->pColor)
				pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
		}
	} else {
		pBlock->pColor = m_lIdentifierColorHash.value(pBlock->szText.toLower(),NULL);
		if(!pBlock->pColor)
			pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
	}
	
}

void C3TextEditorModeParserVHDL::computeBlocksParseVHDLMultiLineString(ushort uDelimiter,unsigned int uInterLineFlag)
{
	// This is called when m_p->p is pointed AFTER the initial delimiter!

	Q_ASSERT(m_p->uInterLineFlags & uInterLineFlag); // you must set the flag before calling this function

	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();

		if(uChar == uDelimiter)
		{
			m_p->p++;
			BLOCK(b,m_p->p - b,oStringTextColor,0);
			m_p->uInterLineFlags &= ~uInterLineFlag;
			if(m_p->p >= m_p->e)
			{
				// end of line just after a multi line comment end. store an additional empty block to signal state change
				BLOCK(0,0,oTextColor,0);
			}
			return;
		}

		switch(uChar)
		{
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
							(u == uDelimiter)
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

void C3TextEditorModeParserVHDL::computeBlocksParseVHDLNumber()
{
	Q_ASSERT(m_p->p->isNumber() || (m_p->p->unicode() == (ushort)'.'));
	
	const QChar * b = m_p->p;
	
	bool bSeenDot = (m_p->p->unicode() == (ushort)'.');
	bool bSeenExp = (m_p->p->unicode() == (ushort)'e') || (m_p->p->unicode() == (ushort)'E');
	const QChar * pFirstHash = NULL;
	
	bool bError = false;
	
	m_p->p++;
	
	while(m_p->p < m_p->e)
	{
		if(m_p->p->isNumber())
		{
			m_p->p++;
			continue;
		}
		
		uchar u = m_p->p->unicode();
		
		switch(m_p->p->unicode())
		{
			case '.':
				if(bSeenDot)
					bError = true;
				bSeenDot = true;
				m_p->p++;
			break;
			case '_':
				m_p->p++; // readability separator
			break;
			case 'A':
			case 'B':
			case 'C':
			case 'a':
			case 'b':
			case 'c':
				if(!pFirstHash)
					bError = true;
				m_p->p++;
			break;
			case 'e':
			case 'E':
				if(!pFirstHash)
				{
					if(bSeenExp)
						bError = true;
					bSeenExp = true;
				}
				m_p->p++;
			break;
			case 'D':
			case 'd':
			case 'F':
			case 'f':
				// FIXME: If bSeenDot then this should be the last letter?
				if(!(pFirstHash || bSeenDot))
					bError = true;
				m_p->p++;
			break;
			case '#':
				m_p->p++;
				if(pFirstHash)
				{
					// Special treatment!
					BLOCK(b,pFirstHash - b,oNumberTextColor,0);
					BLOCK(pFirstHash,1,oUnterminatedStringTextColor,0);
					const QChar * pAfterFirstHash = pFirstHash + 1;
					BLOCK(pAfterFirstHash,(m_p->p - 1) - pAfterFirstHash,oNumberTextColor,0);
					BLOCK(m_p->p - 1,1,oUnterminatedStringTextColor,0);
					return;
				}
				pFirstHash = m_p->p - 1;
			break;
			default:
				if(!m_p->p->isLetter())
				{
					if(pFirstHash)
						bError = true;
					goto end_of_number;
				}
				bError = true;
				m_p->p++;
			break;
		}
	}

end_of_number:

	if(bError)
		BLOCK(b,m_p->p - b,oErrorTextColor,0);
	else
		BLOCK(b,m_p->p - b,oNumberTextColor,0);
}


void C3TextEditorModeParserVHDL::computeMetadata()
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
		if(m_p->uInterLineFlags & InVHDLDoubleQuoteString)
		{
			computeBlocksParseVHDLMultiLineString('"',InVHDLDoubleQuoteString);
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '-':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				switch(m_p->p->unicode())
				{
					case '-':
						m_p->p--;
						computeBlocksParseSingleLineComment();
					break;
					default: // anything else
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
				}
			}
			break;
			case '"':
				m_p->uInterLineFlags |= InVHDLDoubleQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseVHDLMultiLineString('"',InVHDLDoubleQuoteString);
			break;
			case '\'':
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseString('\'',&(m_p->pCoreData->pOptions->oStringTextColor));
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
			case '.':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(
						(u != (ushort)'=') &&
						(u != (ushort)':') &&
						(u != (ushort)'+') &&
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
				if(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'))
				{
					computeBlocksParseIdentifier();
				} else if(m_p->p->isNumber())
				{
					computeBlocksParseVHDLNumber();
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
								(u == (ushort)'`') ||
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