//=============================================================================
//
//   File : C3TextEditorModeParserCPP.cpp
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

#include "C3TextEditorModeParserCPP.h"

#include "C3TextEditorOptions.h"
#include "C3TextEditorModeCPPSharedData.h"
#include "C3TextEditorModeWithInterLineState.h"
#include "C3TextEditorFactory.h"


C3TextEditorModeParserCPP::C3TextEditorModeParserCPP(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState),
	m_pSharedData(pParserState->pMode->factory()->modeCPPSharedData()),
	m_lPreprocessorIdentifierColorHash(m_pSharedData->preprocessorIdentifierColorHash()),
	m_lIdentifierColorHash(m_pSharedData->identifierColorHash()),
	m_lIdentifierPrefixColorHash(m_pSharedData->identifierPrefixColorHash()),
	m_lIdentifierPostfixColorHash(m_pSharedData->identifierPostfixColorHash()),
	m_hIdentifierPrefixWithUppercaseColorHash(m_pSharedData->identifierPrefixWithUppercaseColorHash())
{

}

C3TextEditorModeParserCPP::~C3TextEditorModeParserCPP()
{
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserCPP::computeBlocksParseIncludeFile()
{
	// skip spaces, tabs and comments

	const QChar * b;

	while(m_p->p < m_p->e)
	{
		b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '/':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oErrorTextColor,0);
					return;
				}
				switch(m_p->p->unicode())
				{
					case '*':
						m_p->uInterLineFlags |= InCPPMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InCPPMultiLineComment);
					break;
					default: // anything else
						m_p->p--;
						goto process_include;
					break;
				}
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
				goto process_include;
			break;
		}
	}

process_include:
	
	if(m_p->p >= m_p->e)
		return;

	switch(m_p->p->unicode())
	{
		case '<':
			m_p->uInterLineFlags |= InCPPIncludeFile;
			computeBlocksParseString('>',&(m_p->pCoreData->pOptions->oIncludeFileTextColor));
			m_p->uInterLineFlags &= ~InCPPIncludeFile;
		break;
		case '"':
			m_p->uInterLineFlags |= InCPPIncludeFile;
			computeBlocksParseString('"',&(m_p->pCoreData->pOptions->oIncludeFileTextColor));
			m_p->uInterLineFlags &= ~InCPPIncludeFile;
		break;
		default:
			// do nothing
		break;
	}

	// Any non comment from here on is an error
	while(m_p->p < m_p->e)
	{
		b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '/':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oErrorTextColor,0);
					return;
				}
				switch(m_p->p->unicode())
				{
					case '*':
						m_p->uInterLineFlags |= InCPPMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InCPPMultiLineComment);
					break;
					case '/':
						m_p->p++;
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						m_p->uInterLineFlags |= InCPPSingleLineComment;
						computeBlocksParseSingleLineComment();
						m_p->uInterLineFlags &= ~InCPPSingleLineComment;
						Q_ASSERT(m_p->p >= m_p->e);
					break;
					default: // anything else is an error
						m_p->p++;
						BLOCK(b,m_p->p - b,oErrorTextColor,0);
						b = m_p->p;
					break;
				}
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
				m_p->p++;

				// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
				const QChar * tmpEnd = m_p->p + 256;
				if(tmpEnd > m_p->e)
					tmpEnd = m_p->e;
				
				while(m_p->p < tmpEnd)
				{
					ushort u = m_p->p->unicode();
					if(
							(u == (ushort)' ') ||
							(u == (ushort)'\t') ||
							(u == (ushort)'/')
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oErrorTextColor,0);
			}
			break;
		}
	}
}

void C3TextEditorModeParserCPP::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	// Cannot move this to the constructor as we don't have proper state there, yet.

	if(m_bInPreprocessorLine)
	{
		if(m_bFirstPreprocessorTokenSeen)
		{
			// we recognize only "defined()" here.
			static QString defined("defined");
			if(pBlock->szText == defined)
			{
				pBlock->pColor = &(m_p->pCoreData->pOptions->oPreprocessorTextColor);
				return;
			}
		} else {
			m_bFirstPreprocessorTokenSeen = true;
			pBlock->uFlags |= InCPPPreprocessorToken;
			pBlock->pColor = m_lPreprocessorIdentifierColorHash.value(pBlock->szText,NULL);
			if(pBlock->pColor)
			{
				static QString include("include");
				if(pBlock->szText == include)
				{
					// special processing for #include blocks
					computeBlocksParseIncludeFile();
				}
				return;
			}
		}
	}

	pBlock->pColor = m_lIdentifierColorHash.value(pBlock->szText,NULL);
	if(pBlock->pColor)
		return;

	QHash<QString,QColor *>::const_iterator it;
	
	for(it = m_lIdentifierPrefixColorHash.begin();it != m_lIdentifierPrefixColorHash.end();++it)
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

	for(it = m_lIdentifierPostfixColorHash.begin();it != m_lIdentifierPostfixColorHash.end();++it)
	{
		if(pBlock->szText.endsWith(it.key()))
		{
			pBlock->pColor = it.value();
			return;
		}
	}

	pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
}


void C3TextEditorModeParserCPP::computeMetadata()
{
	if(m_p->uInterLineFlags & InCPPPreprocessor)
	{
		// The InCPPPreprocessor state is local. We set it only at an explicit line continuation.
		// There was an explicit line continuation at the end of the previous line that was in preprocessor state.
		m_bInPreprocessorLine = true;
		// add an empty block so inter-line state looks preserved.
		BLOCK(0,0,oTextColor,0); // color doesn't matter
		m_p->uInterLineFlags &= ~InCPPPreprocessor;
		// assume that we have already seen the initial preprocessor token
		m_bFirstPreprocessorTokenSeen = true;
	} else {
		m_bInPreprocessorLine = false;
	}


	if(m_p->p >= m_p->e)
	{
		// add a single empty block
		BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}

	if(m_p->uInterLineFlags & InCPPMultiLineComment)
	{
		computeBlocksParseCLikeMultiLineComment(InCPPMultiLineComment);
		if(m_p->p >= m_p->e)
			return;
		// no longer in multi line comment
		Q_ASSERT(!(m_p->uInterLineFlags & InCPPMultiLineComment));
	}

	Q_ASSERT(m_p->p < m_p->e);

	if(!m_bInPreprocessorLine)
	{
		// Initial loop to check if we enter preprocessor state.

		for(;;)
		{
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
							m_p->uInterLineFlags |= InCPPMultiLineComment;
							m_p->p++;
							BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
							if(m_p->p >= m_p->e)
								return;
							computeBlocksParseCLikeMultiLineComment(InCPPMultiLineComment);
							if(m_p->p >= m_p->e)
								return;
						break;
						case '/':
							m_p->p++;
							BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
							if(m_p->p >= m_p->e)
								return;
							m_p->uInterLineFlags |= InCPPSingleLineComment;
							computeBlocksParseSingleLineComment();
							m_p->uInterLineFlags &= ~InCPPSingleLineComment;
							Q_ASSERT(m_p->p >= m_p->e);
							return;
						break;
						case '=':
							m_p->p++;
							BLOCK(b,m_p->p - b,oOperatorTextColor,0);
							goto preprocessor_check_complete;
						break;
						default: // anything else: stay here
							BLOCK(b,m_p->p - b,oOperatorTextColor,0);
							goto preprocessor_check_complete;
						break;
					}
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
					if(m_p->p >= m_p->e)
						return;
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
					if(m_p->p >= m_p->e)
						return;
				}
				break;
				case '#':
					m_p->p++;
					BLOCK(b,m_p->p - b,oPreprocessorTextColor,0);
					m_bInPreprocessorLine = true;
					m_bFirstPreprocessorTokenSeen = false;
					goto preprocessor_check_complete;
				break;
				default:
					goto preprocessor_check_complete;
				break;
			}
		} // for(;;)
	}

preprocessor_check_complete:

	while(m_p->p < m_p->e)
	{
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
						m_p->uInterLineFlags |= InCPPMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InCPPMultiLineComment);
					break;
					case '/':
						m_p->p++;
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						m_p->uInterLineFlags |= InCPPSingleLineComment;
						computeBlocksParseSingleLineComment();
						m_p->uInterLineFlags &= ~InCPPSingleLineComment;
					break;
					case '=':
						m_p->p++;
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
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
				m_p->uInterLineFlags |= InCPPString;
				computeBlocksParseString('\'',&(m_p->pCoreData->pOptions->oStringTextColor));
				m_p->uInterLineFlags &= ~InCPPString;
			break;
			case '"':
				m_p->uInterLineFlags |= InCPPString;
				computeBlocksParseStringWithCFormat('"',&(m_p->pCoreData->pOptions->oStringTextColor));
				m_p->uInterLineFlags &= ~InCPPString;
			break;
			case '{':
			case '}':
				m_p->p++;
				BLOCK(b,m_p->p - b,oCodeBlockDelimiterTextColor,0);
			break;
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					// line continuation
					if(m_bInPreprocessorLine)
						m_p->uInterLineFlags |= InCPPPreprocessor; // so it will be set in the line continuation block
					BLOCK(b,m_p->p - b,oOperatorTextColor,0); // FIXME: do we want a different color?
					return;
				}
				// error
				BLOCK(b,m_p->p - b,oErrorTextColor,0);
			break;
			case '$':
			case '#':
				m_p->p++;
				BLOCK(b,m_p->p - b,oPreprocessorTextColor,0); // this is ALWAYS preprocessor (though sometimes it's error
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
			case '+':
			case '*':
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
			case '^':
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
						(u != (ushort)'*') &&
						(u != (ushort)'~') &&
						(u != (ushort)'^')
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
								(u == (ushort)'^') ||
								(u == (ushort)'$') ||
								(u == (ushort)'#') ||
								(u == (ushort)'~') ||
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