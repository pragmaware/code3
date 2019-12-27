//=============================================================================
//
//   File : C3TextEditorModeParserJava.java
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

#include "C3TextEditorModeParserJava.h"

#include "C3TextEditorOptions.h"
#include "C3TextEditorModeJavaSharedData.h"
#include "C3TextEditorModeWithInterLineState.h"
#include "C3TextEditorFactory.h"


C3TextEditorModeParserJava::C3TextEditorModeParserJava(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState),
	m_pSharedData(pParserState->pMode->factory()->modeJavaSharedData()),
	m_lIdentifierColorHash(m_pSharedData->identifierColorHash()),
	m_lIdentifierPrefixColorHash(m_pSharedData->identifierPrefixColorHash()),
	m_lIdentifierPostfixColorHash(m_pSharedData->identifierPostfixColorHash()),
	m_hIdentifierPrefixWithUppercaseColorHash(m_pSharedData->identifierPrefixWithUppercaseColorHash())
{

}

C3TextEditorModeParserJava::~C3TextEditorModeParserJava()
{
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserJava::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	// Cannot move this to the constructor as we don't have proper state there, yet.

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
				(pBlock->szText.at(len).isUpper())
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

void C3TextEditorModeParserJava::computeBlocksParseAnnotation()
{
	Q_ASSERT(m_p->p->unicode() == (ushort)'@');
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_') || (m_p->p->unicode() == (ushort)'.')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	static QString szInterface("@interface");
	
	if(pBlock->szText == szInterface)
	{
		pBlock->pColor = &(m_p->pCoreData->pOptions->oKeywordTextColor);
		return;
	}

	pBlock->pColor = &(m_p->pCoreData->pOptions->oAnnotationTextColor);
}


void C3TextEditorModeParserJava::computeMetadata()
{
	if(m_p->p >= m_p->e)
	{
		// add a single empty block
		BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}

	if(m_p->uInterLineFlags & InJavaMultiLineComment)
	{
		computeBlocksParseCLikeMultiLineComment(InJavaMultiLineComment);
		if(m_p->p >= m_p->e)
			return;
		// no longer in multi line comment
		Q_ASSERT(!(m_p->uInterLineFlags & InJavaMultiLineComment));
	}

	Q_ASSERT(m_p->p < m_p->e);

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
						m_p->uInterLineFlags |= InJavaMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InJavaMultiLineComment);
					break;
					case '/':
						m_p->p++;
						BLOCK(b,m_p->p - b,oSingleLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						m_p->uInterLineFlags |= InJavaSingleLineComment;
						computeBlocksParseSingleLineComment();
						m_p->uInterLineFlags &= ~InJavaSingleLineComment;
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
				m_p->uInterLineFlags |= InJavaString;
				computeBlocksParseString('\'',&(m_p->pCoreData->pOptions->oStringTextColor));
				m_p->uInterLineFlags &= ~InJavaString;
			break;
			case '"':
				m_p->uInterLineFlags |= InJavaString;
				computeBlocksParseString('"',&(m_p->pCoreData->pOptions->oStringTextColor));
				m_p->uInterLineFlags &= ~InJavaString;
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
					BLOCK(b,m_p->p - b,oOperatorTextColor,0); // FIXME: do we want a different color?
					return;
				}
				// error
				BLOCK(b,m_p->p - b,oErrorTextColor,0);
			break;
			case '@':
				computeBlocksParseAnnotation();
			break;
			case '$':
			case '#':
				m_p->p++;
				BLOCK(b,m_p->p - b,oErrorTextColor,0); // this is always error
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
								(u == (ushort)'@') ||
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
