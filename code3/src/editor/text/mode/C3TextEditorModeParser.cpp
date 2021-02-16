//=============================================================================
//
//   File : C3TextEditorModeParser.cpp
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

#include "C3TextEditorModeParser.h"

#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorOptions.h"

C3TextEditorModeParser::C3TextEditorModeParser(C3TextEditorModeParserState * pParserState)
	: m_p(pParserState)
{
}

C3TextEditorModeParser::~C3TextEditorModeParser()
{
}

C3TextEditorLine::Block * C3TextEditorModeParser::createBlock(const QChar * pBegin,int iLength,QColor * pColor,unsigned int uSpecialFlags)
{
	C3TextEditorLine::Block * pBlock = new C3TextEditorLine::Block();
	if(iLength > 0)
		pBlock->szText.setUnicode(pBegin,iLength);
	if(uSpecialFlags & C3TextEditorLine::Block::IsTabBlock)
		pBlock->fWidth = C3TextEditorFontMetricsUtils::widthOfTabBlock(pBlock->szText.length(),m_p->fX,m_p->pCoreData->fFontTabStopWidth);
	else
		pBlock->fWidth = C3TextEditorFontMetricsUtils::widthOfNonTabBlock(m_p->pCoreData->pFontMetrics,pBlock->szText);
	pBlock->uFlags = m_p->uInterLineFlags | uSpecialFlags;
	pBlock->pColor = pColor;
	m_p->pLine->lBlocks.append(pBlock);
	m_p->fX += pBlock->fWidth;
	return pBlock;
}

bool C3TextEditorModeParser::pointingAtString(const char * szString,int iLength)
{
	const QChar * p = m_p->p;
	const QChar * e = m_p->e;
	int iLen = e - p;
	if(iLen < iLength)
		return false;

	const char * s = szString;
	for(;;)
	{
		uchar u = (uchar)*s;
		if(u == 0)
			break;
		if(p->unicode() != u)
			return false;
		p++;
		s++;
	}

	return true;
}

bool C3TextEditorModeParser::pointingAtString(const QChar * szString,int iLength)
{
	const QChar * p = m_p->p;
	const QChar * e = m_p->e;
	int iLen = e - p;
	if(iLen < iLength)
		return false;

	const QChar * s = szString;
	const QChar * se = szString + iLength;
	for(;;)
	{
		if(s >= se)
			break;
		if(p->unicode() != s->unicode())
			return false;
		p++;
		s++;
	}

	return true;
}

const QChar * C3TextEditorModeParser::findString(const char * szString,int iLength)
{
	const QChar * p = m_p->p;
	const QChar * e = m_p->e - iLength + 1;
	int iLen = e - p;
	if(iLen < 0)
		return NULL;

	
	while(p < e)
	{
		if(p->unicode() != (uchar)*szString)
		{
			p++;
			continue;
		}

		const QChar * b = p;

		p++;

		const char * s = szString + 1;

		const QChar * x = p;

		for(;;)
		{
			uchar u = (uchar)*s;
			if(u == 0)
				return b;
			if(x->unicode() != u)
				break;
			x++;
			s++;
		}
	}

	return NULL;
}

const QChar * C3TextEditorModeParser::findString(const QChar * szString,int iLength)
{
	const QChar * p = m_p->p;
	const QChar * e = m_p->e - iLength + 1;
	int iLen = e - p;
	if(iLen < 0)
		return NULL;


	while(p < e)
	{
		if(p->unicode() != szString->unicode())
		{
			p++;
			continue;
		}

		const QChar * b = p;

		p++;

		const QChar * s = szString + 1;
		const QChar * se = szString + iLength;

		const QChar * x = p;

		for(;;)
		{
			if(s >= se)
				return b;
			if(x->unicode() != s->unicode())
				break;
			x++;
			s++;
		}
	}

	return NULL;
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParser::computeBlocksParseCLikeMultiLineComment(unsigned int uCommentFlag)
{
	Q_ASSERT(m_p->uInterLineFlags & uCommentFlag); // you must set the flag before calling this function

	const QChar * b = m_p->p;
	
	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case '*':
				m_p->p++;
				
				if(m_p->p >= m_p->e)
					goto end_of_line_in_comment;

				switch(m_p->p->unicode())
				{
					case '/':
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						m_p->uInterLineFlags &= ~uCommentFlag;
						if(m_p->p >= m_p->e)
						{
							// end of line just after a multi line comment end. store an additional empty block to signal state change
							BLOCK(0,0,oTextColor,0);
						}
						return;
					break;
					default:
						// stay here!
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						b = m_p->p;
					break;
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
							(u == (ushort)'*') ||
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

void C3TextEditorModeParser::computeBlocksParseSingleLineComment()
{
	const QChar * b = m_p->p;
	
	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case '\t':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			case 'T':
				// TODO
				if(pointingAtString("TODO",4))
				{
					if(m_p->p > b)
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
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
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
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
							(u == (ushort)'F') ||
							(u == (ushort)'T')
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

	BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
}

void C3TextEditorModeParser::computeBlocksParseString(ushort uDelimiter,QColor * pColor)
{
	// This is called with m_p->p pointing at the initial delimiter (which may be different from the final one)

	const QChar * b = m_p->p;
	
	if(m_p->p->unicode() == uDelimiter)
		m_p->p++; // initial delimiter
	// else we assume that we were pointing to something different which *MIGHT* also be a tab or an escape.

	int iBlockCount = 0;
	
	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();
		if(uChar == uDelimiter)
		{
			m_p->p++;
			createBlock(b,m_p->p - b,pColor,0);
			return;
		}
	
		switch(uChar)
		{
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
					goto mark_as_unterminated_string; // unterminated string
				if(m_p->p->unicode() == (ushort)'\t')
				{
					// special case of \<tab>
					iBlockCount++;
					createBlock(b,m_p->p - b,pColor,0);
					b = m_p->p;
					m_p->p++;
					iBlockCount++;
					createBlock(b,m_p->p - b,pColor,C3TextEditorLine::Block::IsTabBlock);
					b = m_p->p;
				} else {
					m_p->p++; // skip escape
				}
			break;
			case '\t':
			{
				if(m_p->p > b)
				{
					createBlock(b,m_p->p - b,pColor,0);
					b = m_p->p;
				}
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				iBlockCount++;
				createBlock(b,m_p->p - b,pColor,C3TextEditorLine::Block::IsTabBlock);
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
							(u == (ushort)'\\') ||
							(u == uDelimiter)
						)
						break;
					m_p->p++;
				}

				iBlockCount++;
				createBlock(b,m_p->p - b,pColor,0);
				b = m_p->p;
			}
			break;
		}
	}

mark_as_unterminated_string:

	// uDelimiter 0 is used explicitly to mark everything as error
	if(uDelimiter == 0)
		pColor = &(m_p->pCoreData->pOptions->oErrorTextColor);
	else
		pColor = &(m_p->pCoreData->pOptions->oUnterminatedStringTextColor);

	QLinkedList<C3TextEditorLine::Block *>::iterator it = m_p->pLine->lBlocks.end();
	while(iBlockCount > 0)
	{
		iBlockCount--;
		it--;
		(*it)->pColor = pColor;
	}

	if(m_p->p > b)
		createBlock(b,m_p->p - b,pColor,0);
	return;

}


void C3TextEditorModeParser::computeBlocksParseStringWithCFormat(ushort uDelimiter,QColor * pColor)
{
	// This is called with m_p->p pointing at the initial delimiter (which may be different from the final one)

	const QChar * b = m_p->p;
	
	if(m_p->p->unicode() == uDelimiter)
		m_p->p++; // initial delimiter
	// else we assume that we were pointing to something different which *MIGHT* also be a tab or an escape.

	int iBlockCount = 0;
	
	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();
		if(uChar == uDelimiter)
		{
			m_p->p++;
			createBlock(b,m_p->p - b,pColor,0);
			return;
		}
	
		switch(uChar)
		{
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
					goto mark_as_unterminated_string; // unterminated string
				if(m_p->p->unicode() == (ushort)'\t')
				{
					// special case of \<tab>
					iBlockCount++;
					createBlock(b,m_p->p - b,pColor,0);
					b = m_p->p;

					m_p->p++;
					iBlockCount++;
					createBlock(b,m_p->p - b,pColor,C3TextEditorLine::Block::IsTabBlock);
					b = m_p->p;
				} else {
					m_p->p++; // skip escape
				}
			break;
			case '\t':
			{
				if(m_p->p > b)
				{
					createBlock(b,m_p->p - b,pColor,0);
					b = m_p->p;
				}
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				iBlockCount++;
				createBlock(b,m_p->p - b,pColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			case '%':
			{
				const QChar * pSave = m_p->p;
			
				m_p->p++;
				if(m_p->p >= m_p->e)
					goto mark_as_unterminated_string; // unterminated string

				if(m_p->p->unicode() == '.')
				{
					m_p->p++;
					if(m_p->p >= m_p->e)
						goto mark_as_unterminated_string; // unterminated string
				}

				if(m_p->p->isLetterOrNumber())
				{
					if(pSave > b)
					{
						iBlockCount++;
						createBlock(b,pSave - b,pColor,0);
						b = pSave;
					}

					const QChar * tmpEnd = m_p->p + 3;
					if(tmpEnd > m_p->e)
						tmpEnd = m_p->e;
	
					while(m_p->p < tmpEnd)
					{
						if(m_p->p->isLetter())
						{
							m_p->p++;
							break;
						}
						if(!m_p->p->isNumber())
							break;
						m_p->p++;
					}
					
					createBlock(b,m_p->p - b,&(m_p->pCoreData->pOptions->oVariableInStringTextColor),0);
					b = m_p->p;
				}
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
							(u == (ushort)'\\') ||
							(u == (ushort)'%') ||
							(u == uDelimiter)
						)
						break;
					m_p->p++;
				}

				iBlockCount++;
				createBlock(b,m_p->p - b,pColor,0);
				b = m_p->p;
			}
			break;
		}
	}

mark_as_unterminated_string:

	// uDelimiter 0 is used explicitly to mark everything as error
	if(uDelimiter == 0)
		pColor = &(m_p->pCoreData->pOptions->oErrorTextColor);
	else
		pColor = &(m_p->pCoreData->pOptions->oUnterminatedStringTextColor);

	QLinkedList<C3TextEditorLine::Block *>::iterator it = m_p->pLine->lBlocks.end();
	while(iBlockCount > 0)
	{
		iBlockCount--;
		it--;
		(*it)->pColor = pColor;
	}

	if(m_p->p > b)
		createBlock(b,m_p->p - b,pColor,0);
	return;

}


void C3TextEditorModeParser::computeBlocksSkipWhiteSpace()
{
	if(m_p->p >= m_p->e)
		return;

	Q_ASSERT((m_p->p->unicode() == (ushort)' ') || (m_p->p->unicode() == (ushort)'\t'));
	
	const QChar * b = m_p->p;
	
	ushort val = m_p->p->unicode();

	while(m_p->p < m_p->e)
	{
		ushort here = m_p->p->unicode();
		if(here != val)
		{
			BLOCK(b,m_p->p - b,oTextColor,(val == (ushort)'\t') ? C3TextEditorLine::Block::IsTabBlock : 0);
			
			if((here != (ushort)' ') && (here != (ushort)'\t'))
				return;
			
			b = m_p->p;
		}

		m_p->p++;
	}
	
	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oTextColor,(val == (ushort)'\t') ? C3TextEditorLine::Block::IsTabBlock : 0);	
}


void C3TextEditorModeParser::computeBlocksParseNumber()
{
	Q_ASSERT(m_p->p->isNumber() || (m_p->p->unicode() == (ushort)'.'));
	
	const QChar * b = m_p->p;
	
	bool bSeenDot = (m_p->p->unicode() == (ushort)'.');
	bool bSeenExp = (m_p->p->unicode() == (ushort)'e') || (m_p->p->unicode() == (ushort)'E');
	bool bSeenX = false;
	
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
			case 'x':
				if((m_p->p - b) > 1)
					bError = true;
				else
					bSeenX = true;
				m_p->p++;
			break;
			case 'A':
			case 'B':
			case 'C':
			case 'a':
			case 'b':
			case 'c':
				if(!bSeenX)
					bError = true;
				m_p->p++;
			break;
			case 'e':
			case 'E':
				if(!bSeenX)
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
				if(!(bSeenX || bSeenDot))
					bError = true;
				m_p->p++;
			break;
			case 'L':
			case 'l':
			case 'U':
			case 'u':
				// FIXME: Should be at the end?
				m_p->p++;
			break;
			default:
				if(!m_p->p->isLetter())
					goto end_of_number;
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



void C3TextEditorModeParser::computeBlocksParseMultiLineBlockWithLongTerminator(
		const char * szTerminator,
		unsigned int uTerminatorLength,
		unsigned int uInterLineFlag,
		QColor * pColor,
		unsigned int uFlags
	)
{
	Q_ASSERT(m_p->uInterLineFlags & uInterLineFlag);
	Q_ASSERT(szTerminator);
	Q_ASSERT(uTerminatorLength > 0);

	const QChar * b = m_p->p;
	
	ushort uTerminator = *szTerminator;
	
	while(m_p->p < m_p->e)
	{
		if(m_p->p->unicode() == uTerminator)
		{
			if(pointingAtString(szTerminator,uTerminatorLength))
			{
				m_p->p += uTerminatorLength;
				createBlock(b,m_p->p - b,pColor,0);
				m_p->uInterLineFlags &= ~uInterLineFlag;
				
				if(m_p->p >= m_p->e)
					createBlock(b,0,pColor,0); // empty block without flags at the end of line
				return;
			}
		}
	
		switch(m_p->p->unicode())
		{
			case '\t':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				createBlock(b,m_p->p - b,pColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			case '\\':
				m_p->p++;
				
				if(m_p->p >= m_p->e)
					goto end_of_line_in_block;

				if(uFlags & ParseMultiLineBlockWithLongTerminatorHandleEscapeSequences)
				{
					if(m_p->p->unicode() == (ushort)'\t')
					{
						// special case of \<tab>
						createBlock(b,m_p->p - b,pColor,0);
						b = m_p->p;
						m_p->p++;
						createBlock(b,m_p->p - b,pColor,C3TextEditorLine::Block::IsTabBlock);
						b = m_p->p;
					} else {
						m_p->p++; // skip escape
					}
				} else {
					createBlock(b,m_p->p - b,pColor,0);
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
							(u == (ushort)'\\') ||
							(u == (ushort)uTerminator)
						)
						break;
					m_p->p++;
				}

				createBlock(b,m_p->p - b,pColor,0);
				b = m_p->p;
			}
			break;
		}
	}

end_of_line_in_block:
	if(m_p->p > b)
		createBlock(b,m_p->p - b,pColor,0);
}
