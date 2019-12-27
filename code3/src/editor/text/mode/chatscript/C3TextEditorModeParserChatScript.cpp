//=============================================================================
//
//   File : C3TextEditorModeParserChatScript.cpp
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

#include "C3TextEditorModeParserChatScript.h"

#include "C3TextEditorOptions.h"

C3TextEditorModeParserChatScript::C3TextEditorModeParserChatScript(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserChatScript::~C3TextEditorModeParserChatScript()
{
}

void C3TextEditorModeParserChatScript::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)

	KEYWORD("if");
	KEYWORD("else");
	KEYWORD("and");
	KEYWORD("or");
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserChatScript::computeBlocksParseMacro()
{
	// ^something() or ^something

	Q_ASSERT(m_p->p->unicode() == (ushort)'^');

	const QChar * b = m_p->p;
	
	m_p->p++;
	
	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(m_p->p >= m_p->e)
	{
		pBlock->pColor = &(m_p->pCoreData->pOptions->oUserKeywordTextColor);
		return;
	}

	if((m_p->p->unicode() == (ushort)' ') || (m_p->p->unicode() == (ushort)'\t'))
		computeBlocksSkipWhiteSpace();

	if(m_p->p >= m_p->e)
	{
		pBlock->pColor = &(m_p->pCoreData->pOptions->oUserKeywordTextColor);
		return;
	}

	pBlock->pColor = (m_p->p->unicode() == (ushort)'(') ?
			&(m_p->pCoreData->pOptions->oUserKeywordTextColor) :
			&(m_p->pCoreData->pOptions->oMUnderscoreIdentifierTextColor);
}


void C3TextEditorModeParserChatScript::computeBlocksParseIdentifier(QColor * pColor)
{
	//Q_ASSERT(m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_') || (m_p->p->unicode() == (ushort)'#'));

	const QChar * b = m_p->p;
	
	m_p->p++;
	
	if(m_p->p < m_p->e)
	{
		if(m_p->p->unicode() == '$')
			m_p->p++;
		while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
			m_p->p++;
	}

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(!(m_p->uInterLineFlags & ChatScriptSeenFirstToken))
	{
		if((m_p->p < m_p->e) && (m_p->p->unicode() == ':'))
		{
			pBlock->pColor = &(m_p->pCoreData->pOptions->oAnnotationTextColor);
			m_p->uInterLineFlags |= ChatScriptSeenFirstToken;
			return;
		}
	}

	if(m_hIdentifierColorHash.isEmpty())
		buildIdentifierColorHash();

	pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
	if(!pBlock->pColor)
		pBlock->pColor = pColor;

	m_p->uInterLineFlags |= ChatScriptSeenFirstToken;
}


void C3TextEditorModeParserChatScript::computeMetadata()
{
	m_p->uInterLineFlags &= ~ChatScriptSeenFirstToken;

	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	while(m_p->p < m_p->e)
	{
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

				if(m_p->p->isLetter())
				{
					// part of identifier
					m_p->p--;
					computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oBuiltinObjectTextColor));
				} else {
					m_p->p--;
					computeBlocksParseSingleLineComment();
					m_p->uInterLineFlags |= ChatScriptSeenFirstToken;
				}
			}
			break;
			case '{':
			case '}':
				m_p->p++;
				BLOCK(b,m_p->p - b,oCodeBlockDelimiterTextColor,0);
			break;
			case '%':
				computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oBuiltinObjectTextColor));
			break;
			case '$':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oGUnderscoreIdentifierTextColor,0);
					return;
				}

				if(m_p->p->unicode() == '$')
				{
					// single volley variable
					m_p->p--;
					computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oVariableTextColor));
				} else {
					// global variable
					m_p->p--;
					computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oGUnderscoreIdentifierTextColor));
				}
			break;
			case '_':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oBuiltinTypeTextColor,0);
					return;
				}

				if(m_p->p->isNumber())
				{
					// _0 _1 ...
					m_p->p--;
					computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oBuiltinTypeTextColor));
				} else {
					BLOCK(b,m_p->p - b,oBuiltinTypeTextColor,0);
				}
			break;
			case '~':
				computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oUserTypeTextColor));
			break;
			case '"':
				computeBlocksParseString('"',&(m_p->pCoreData->pOptions->oStringTextColor));
			break;
			case '^':
				computeBlocksParseMacro();
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
			case ':':
			case '=':
			case '*':
			case '+':
			case '?':
			case '!':
			case '>':
			case '<':
			case '|':
			case '&':
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
						(u != (ushort)'+') &&
						(u != (ushort)':') &&
						(u != (ushort)'-') &&
						(u != (ushort)'?') &&
						(u != (ushort)'/') &&
						(u != (ushort)'!') &&
						(u != (ushort)'>') &&
						(u != (ushort)'<') &&
						(u != (ushort)'|') &&
						(u != (ushort)'&') &&
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
				// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
				const QChar * tmpEnd = m_p->p + 256;
				if(tmpEnd > m_p->e)
					tmpEnd = m_p->e;
				
				if(m_p->p->isLetter())
				{
					computeBlocksParseIdentifier(&(m_p->pCoreData->pOptions->oTextColor));
				} else {
					m_p->p++;
					
					while(m_p->p < tmpEnd)
					{
						ushort u = m_p->p->unicode();
						if(
								m_p->p->isLetter() ||
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
								(u == (ushort)'"') ||
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
								(u == (ushort)'$') ||
								(u == (ushort)' ') ||
								(u == (ushort)'~') ||
								(u == (ushort)'\\') ||
								(u == (ushort)'-')
							)
							break;
						m_p->p++;
					}
					BLOCK(b,m_p->p - b,oTextColor,0);
				}
				
			}
			break;
		}
	}
}
