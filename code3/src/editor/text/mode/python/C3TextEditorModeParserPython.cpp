//=============================================================================
//
//   File : C3TextEditorModeParserPython.cpp
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

#include "C3TextEditorModeParserPython.h"

#include "C3TextEditorOptions.h"

C3TextEditorModeParserPython::C3TextEditorModeParserPython(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserPython::~C3TextEditorModeParserPython()
{
}

void C3TextEditorModeParserPython::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)


	KEYWORD("and");
	KEYWORD("as");
	KEYWORD("assert");
	KEYWORD("break");
	KEYWORD("class");
	KEYWORD("continue");
	KEYWORD("def");
	KEYWORD("del");
	KEYWORD("elif");
	KEYWORD("else");
	KEYWORD("except");
	KEYWORD("exec");
	KEYWORD("finally");
	KEYWORD("for");
	KEYWORD("from");
	KEYWORD("global");
	KEYWORD("if");
	KEYWORD("import");
	KEYWORD("in");
	KEYWORD("is");
	KEYWORD("lambda");
	KEYWORD("not");
	KEYWORD("or");
	KEYWORD("pass");
	KEYWORD("print");
	KEYWORD("raise");
	KEYWORD("return");
	KEYWORD("self");
	KEYWORD("try");
	KEYWORD("while");
	KEYWORD("with");
	KEYWORD("yield");

	KEYWORD("True");
	KEYWORD("False");
	KEYWORD("None");

	/*
	QColor oKeywordTextColor; // Keywords
	QColor oUserKeywordTextColor; // User Keywords
	QColor oBuiltinTypeTextColor; // Types
	QColor oUserTypeTextColor; // User Types
	QColor oBuiltinObjectTextColor; // Objects
	QColor oUserObjectTextColor; // User Objects
	*/


	BUILTIN_OBJECT("__main__");
	BUILTIN_OBJECT("__module__");
	BUILTIN_OBJECT("__init__");
	BUILTIN_OBJECT("__dict__");
	
	//BUILTIN_TYPE("int");

	//BUILTIN_OBJECT("ValueError");
	//BUILTIN_OBJECT("IOError");

	/*
	BUILTIN_OBJECT("Data");
	BUILTIN_OBJECT("Float");
	BUILTIN_OBJECT("Int");
	BUILTIN_OBJECT("Numeric");
	BUILTIN_OBJECT("Oxphys");
	
	BUILTIN_OBJECT("array");
	BUILTIN_OBJECT("close");
	BUILTIN_OBJECT("float");
	BUILTIN_OBJECT("int");
	*/
}

// FIXME: RAW Strings?

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserPython::computeBlocksParseTripleQuoteMultiLineString(const char * szTripleDelimiter,unsigned int uInterLineFlag)
{
	Q_ASSERT(m_p->uInterLineFlags & uInterLineFlag);

	computeBlocksParseMultiLineBlockWithLongTerminator(
			szTripleDelimiter,
			3,
			uInterLineFlag,
			&(m_p->pCoreData->pOptions->oStringTextColor),
			ParseMultiLineBlockWithLongTerminatorHandleEscapeSequences
		);
}

void C3TextEditorModeParserPython::computeBlocksParseIdentifier()
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
		static QString szUnderscore = __utf8("_");
		static QString szG = __utf8("g_");
		static QString szM = __utf8("m_");

		if(pBlock->szText.startsWith(szG))
			pBlock->pColor = &(m_p->pCoreData->pOptions->oGUnderscoreIdentifierTextColor);
		else if(pBlock->szText.startsWith(szM))
			pBlock->pColor = &(m_p->pCoreData->pOptions->oMUnderscoreIdentifierTextColor);
		else if(pBlock->szText.startsWith(szUnderscore))
			pBlock->pColor = &(m_p->pCoreData->pOptions->oUnderscoreIdentifierTextColor);
		else
			pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
	}
}

void C3TextEditorModeParserPython::computeMetadata()
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
		if(m_p->uInterLineFlags & InPythonTripleSingleQuoteString)
		{
			computeBlocksParseTripleQuoteMultiLineString("\'\'\'",InPythonTripleSingleQuoteString);
			continue;
		}
		if(m_p->uInterLineFlags & InPythonTripleDoubleQuoteString)
		{
			computeBlocksParseTripleQuoteMultiLineString("\"\"\"",InPythonTripleDoubleQuoteString);
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '\'':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oUnterminatedStringTextColor,0);
					return;
				}

				switch(m_p->p->unicode())
				{
					case '\'':
						m_p->p++;
						if(m_p->p >= m_p->e)
						{
							BLOCK(b,m_p->p - b,oStringTextColor,InPythonSingleQuoteString);
							return;
						}
						switch(m_p->p->unicode())
						{
							case '\'':
								m_p->p++;

								m_p->uInterLineFlags |= InPythonTripleSingleQuoteString;
								BLOCK(b,m_p->p - b,oStringTextColor,0);

								if(m_p->p >= m_p->e)
									return;

								// multi line single quote string
								computeBlocksParseTripleQuoteMultiLineString("\'\'\'",InPythonTripleSingleQuoteString);
							break;
							default:
								BLOCK(b,m_p->p - b,oStringTextColor,0);
								// and stay here
							break;
						}
					break;
					default:
						m_p->p--;
						m_p->uInterLineFlags |= InPythonSingleQuoteString;
						computeBlocksParseString('\'',&(m_p->pCoreData->pOptions->oStringTextColor));
						m_p->uInterLineFlags &= ~InPythonSingleQuoteString;
					break;
				}
			break;

			case '\"':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oUnterminatedStringTextColor,0);
					return;
				}

				switch(m_p->p->unicode())
				{
					case '\"':
						m_p->p++;
						if(m_p->p >= m_p->e)
						{
							BLOCK(b,m_p->p - b,oStringTextColor,InPythonDoubleQuoteString);
							return;
						}
						switch(m_p->p->unicode())
						{
							case '\"':
								m_p->p++;

								m_p->uInterLineFlags |= InPythonTripleDoubleQuoteString;
								BLOCK(b,m_p->p - b,oStringTextColor,0);

								if(m_p->p >= m_p->e)
									return;

								// multi line single quote string
								computeBlocksParseTripleQuoteMultiLineString("\"\"\"",InPythonTripleDoubleQuoteString);
							break;
							default:
								BLOCK(b,m_p->p - b,oStringTextColor,0);
								// and stay here
							break;
						}
					break;
					default:
						m_p->p--;
						m_p->uInterLineFlags |= InPythonDoubleQuoteString;
						computeBlocksParseString('"',&(m_p->pCoreData->pOptions->oStringTextColor));
						m_p->uInterLineFlags &= ~InPythonDoubleQuoteString;
					break;
				}
			break;
			case '#':
				m_p->uInterLineFlags |= InPythonSingleLineComment;
				computeBlocksParseSingleLineComment();
				m_p->uInterLineFlags &= ~InPythonSingleLineComment;
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
						(u != (ushort)'-') &&
						(u != (ushort)'?') &&
						(u != (ushort)'!') &&
						(u != (ushort)'>') &&
						(u != (ushort)'<') &&
						(u != (ushort)'|') &&
						(u != (ushort)'&') &&
						(u != (ushort)'%') &&
						(u != (ushort)',') &&
						(u != (ushort)'/') &&
						(u != (ushort)'.') &&
						(u != (ushort)'*')
					)
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
			}
			break;
			case ';':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)']')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oErrorTextColor,0);
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
								(u == (ushort)'#') ||
								(u == (ushort)'\'') ||
								(u == (ushort)'"') ||
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