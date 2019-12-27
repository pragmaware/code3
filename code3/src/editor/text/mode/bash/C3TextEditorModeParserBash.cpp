//=============================================================================
//
//   File : C3TextEditorModeParserBash.cpp
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

#include "C3TextEditorModeParserBash.h"

#include "C3TextEditorOptions.h"

class C3TextEditorModeParserBashLinePayload : public C3TextEditorLinePayload
{
public:
	QString szHeredocIdentifier;
	
public:
	virtual ~C3TextEditorModeParserBashLinePayload()
	{
	}
};

C3TextEditorModeParserBash::C3TextEditorModeParserBash(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
	m_uNextHeredocId = 0;
}

C3TextEditorModeParserBash::~C3TextEditorModeParserBash()
{
}

void C3TextEditorModeParserBash::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)

	KEYWORD("case");
	KEYWORD("alias");
	KEYWORD("bg");
	KEYWORD("bind");
	KEYWORD("break");
	KEYWORD("builtin");
	KEYWORD("caller");
	KEYWORD("cd");
	KEYWORD("command");
	KEYWORD("compgen");
	KEYWORD("complete");
	KEYWORD("compopt");
	KEYWORD("continue");
	KEYWORD("declare");
	KEYWORD("typeset");
	KEYWORD("dirs");
	KEYWORD("disown");
	KEYWORD("do");
	KEYWORD("done");
	KEYWORD("echo");
	KEYWORD("enable");
	KEYWORD("eval");
	KEYWORD("exec");
	KEYWORD("exit");
	KEYWORD("export");
	KEYWORD("elif");
	KEYWORD("else");
	KEYWORD("esac");
	KEYWORD("fc");
	KEYWORD("fg");
	KEYWORD("fi");
	KEYWORD("for");
	KEYWORD("function");
	KEYWORD("getopts");
	KEYWORD("hash");
	KEYWORD("help");
	KEYWORD("history");
	KEYWORD("if");
	KEYWORD("in");
	KEYWORD("jobs");
	KEYWORD("kill");
	KEYWORD("let");
	KEYWORD("logout");
	KEYWORD("local");
	KEYWORD("mapfile");
	KEYWORD("readarray");
	KEYWORD("popd");
	KEYWORD("printf");
	KEYWORD("pushd");
	KEYWORD("pwd");
	KEYWORD("read");
	KEYWORD("readonly");
	KEYWORD("return");
	KEYWORD("set");
	KEYWORD("select");
	KEYWORD("shift");
	KEYWORD("shopt");
	KEYWORD("suspend");
	KEYWORD("source");
	KEYWORD("test");
	KEYWORD("then");
	KEYWORD("time");
	KEYWORD("times");
	KEYWORD("trap");
	KEYWORD("type");
	KEYWORD("ulimit");
	KEYWORD("umask");
	KEYWORD("unalias");
	KEYWORD("unset");
	KEYWORD("until");
	KEYWORD("wait");
	KEYWORD("while");
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)


void C3TextEditorModeParserBash::computeBlocksParseIdentifier(bool bCanBeKeyword)
{
	Q_ASSERT(m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(!bCanBeKeyword)
	{
		pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
		return;
	}

	if(m_hIdentifierColorHash.isEmpty())
		buildIdentifierColorHash();

	pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText,NULL);
	if(!pBlock->pColor)
		pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
	
}

void C3TextEditorModeParserBash::computeBlocksParseVariable(QColor * pColor)
{
	// In Bash variables can nest!
	Q_ASSERT(m_p->p->unicode() == (ushort)'$');

	const QChar * b = m_p->p;
	
	m_p->p++;

	if(m_p->p >= m_p->e)
	{
		// FIXME: could be unterminated var in the process of being written
		BLOCK(b,m_p->p - b,oErrorTextColor,0);
		return;
	}

	bool bGotBracket = false;
	bool bError = false;

	switch(m_p->p->unicode())
	{
		case '#':
			// number of commandline arguments or positional parameters
		case '*':
			// all of the positional parameters as a single word, with shell expansion applied
		case '@':
			// all of the positional parameters unexpanded (will be seen as multiple words)
		case '-':
			// flags passed to the bash script itself
		case '!':
			// PID of the last background job
		case '_':
			// final argument of previous command
		case '?':
			// exit status of command, function or script itself
		case '$':
			// process pid of the script itself
			m_p->p++;
			// change color unless in string
			if(pColor != &(m_p->pCoreData->pOptions->oVariableInStringTextColor))
				pColor = &(m_p->pCoreData->pOptions->oBuiltinObjectTextColor);
			goto end_of_variable;
		break;
		case '{':
			m_p->p++;
	
			if(m_p->p >= m_p->e)
			{
				// FIXME: could be unterminated var in the process of being written
				BLOCK(b,m_p->p - b,oErrorTextColor,0);
				return;
			}
		
			bGotBracket = true;
			
			// a normal variable, fall down
		break;
		default:
			if(m_p->p->isNumber())
			{
				m_p->p++;
		
				while((m_p->p < m_p->e) && (m_p->p->isNumber()))
					m_p->p++;

				goto end_of_variable;
			}
			
			// a normal variable, fall down
		break;
	}

	while(m_p->p < m_p->e)
	{
		ushort u = m_p->p->unicode();
		switch(u)
		{
			case '}':
				if(!bGotBracket)
					goto end_of_variable;
				m_p->p++;
				goto end_of_variable;
			break;
			case '!':
			case ':':
			case ',':
			case '[':
			case ']':
			case '^':
			case '@':
			case '*':
			case '?':
			case '+':
			case '=':
			case '-':
				if(!bGotBracket)
					goto end_of_variable;
				m_p->p++;
			break;
			default:
				if(!(m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
				{
					if(bGotBracket)
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


void C3TextEditorModeParserBash::computeBlocksParseMultiLineWeakQuoteString()
{
	Q_ASSERT(m_p->uInterLineFlags & InBashMultiLineWeakQuoteString); // you must set the flag before calling this function


	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();

		switch(uChar)
		{
			case '"':
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				m_p->uInterLineFlags &= ~InBashMultiLineWeakQuoteString;
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
			case '*':
			case '~':
				if(m_p->p > b)
				{
					BLOCK(b,m_p->p - b,oStringTextColor,0);
					b = m_p->p;
				}
				m_p->p++;
				BLOCK(b,m_p->p - b,oVariableInStringTextColor,0);
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
							(u == (ushort)'*') ||
							(u == (ushort)'~') ||
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

void C3TextEditorModeParserBash::computeBlocksParseMultiLineStrongQuoteString()
{
	// This is called when m_p->p is point AFTER the initial delimiter!

	Q_ASSERT(m_p->uInterLineFlags & InBashMultiLineStrongQuoteString); // you must set the flag before calling this function


	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();

		switch(uChar)
		{
			case '\'':
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				m_p->uInterLineFlags &= ~InBashMultiLineStrongQuoteString;
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
							(u == (ushort)'\'')
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

void C3TextEditorModeParserBash::computeBlocksParseExpandedHeredoc()
{
	Q_ASSERT(m_p->uInterLineFlags & InBashExpandedHeredoc);

	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
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

				BLOCK(b,m_p->p - b,oHeredocTextColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			case '$':
				if(m_p->p > b)
					BLOCK(b,m_p->p - b,oHeredocTextColor,0);
				computeBlocksParseVariable(&(m_p->pCoreData->pOptions->oVariableInHeredocTextColor));
				b = m_p->p;
			break;
			case '*':
			case '~':
				if(m_p->p > b)
				{
					BLOCK(b,m_p->p - b,oHeredocTextColor,0);
					b = m_p->p;
				}
				m_p->p++;
				BLOCK(b,m_p->p - b,oVariableInHeredocTextColor,0);
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
						(u == (ushort)'$') ||
						(u == (ushort)'*') ||
						(u == (ushort)'~')
					)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oHeredocTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oHeredocTextColor,0);

	Q_ASSERT(m_p->p >= m_p->e);
}

void C3TextEditorModeParserBash::computeBlocksParseUnexpandedHeredoc()
{
	Q_ASSERT(m_p->uInterLineFlags & InBashUnexpandedHeredoc);

	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
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

				BLOCK(b,m_p->p - b,oHeredocTextColor,C3TextEditorLine::Block::IsTabBlock);
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
					if(m_p->p->unicode() == (ushort)'\t')
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oHeredocTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oHeredocTextColor,0);

	Q_ASSERT(m_p->p >= m_p->e);
}

void C3TextEditorModeParserBash::computeMetadata()
{
	Q_ASSERT(m_p->pLine->lBlocks.count() == 0);
	Q_ASSERT(m_p->pLine->pPayload == NULL);

	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	bool bKeywordMayFollow = true;

	if(m_p->uInterLineFlags & (InBashExpandedHeredoc | InBashUnexpandedHeredoc))
	{
		// in heredoc.
		// find the start identifier.
		int iRow = m_p->iLineRow;
		QString szIdentifier;

		while(iRow > 0)
		{
			iRow--;
			C3TextEditorLine * pLine = m_p->pCoreData->lLines.at(iRow);
			if(pLine->pPayload)
			{
				C3TextEditorModeParserBashLinePayload * pPayload = dynamic_cast<C3TextEditorModeParserBashLinePayload *>(pLine->pPayload);
				Q_ASSERT(pPayload);
				szIdentifier = pPayload->szHeredocIdentifier;
				break;
			}
		}
		
		if(szIdentifier.isEmpty())
		{
			qDebug("WARNING: Could not find heredoc identifier for line %d (heredoc id %u)",m_p->iLineRow,(m_p->uInterLineFlags & 0xffff0000) >> 16);
			szIdentifier = __utf8("EOF");
		}

		// look for heredoc end
		while(m_p->p < m_p->e)
		{
			const QChar * b = m_p->p;

			switch(m_p->p->unicode())
			{
				case '\t':
				{
					if(m_p->p > b)
					{
						BLOCK(b,m_p->p - b,oHeredocTextColor,0);
						b = m_p->p;
					}
					m_p->p++;
					while(m_p->p < m_p->e)
					{
						if(m_p->p->unicode() != (ushort)'\t')
							break;
						m_p->p++;
					}
	
					BLOCK(b,m_p->p - b,oHeredocTextColor,C3TextEditorLine::Block::IsTabBlock);
					b = m_p->p;
				}
				break;
				default:
					if(pointingAtString(szIdentifier.unicode(),szIdentifier.length()))
					{
						m_p->p += szIdentifier.length();
				
						BLOCK(b,m_p->p - b,oHeredocTextColor,0);

						m_p->uInterLineFlags &= ~(InBashExpandedHeredoc | InBashUnexpandedHeredoc | 0xffff0000);
						
						if(m_p->p >= m_p->e)
						{
							// empty block to mark the exit from heredoc
							BLOCK(m_p->p,0,oHeredocTextColor,0);
							return;
						}
						
						goto end_of_heredoc;
					}

					goto not_end_of_heredoc;
				break;
			}
		}

not_end_of_heredoc:

		if(m_p->uInterLineFlags & InBashExpandedHeredoc)
			computeBlocksParseExpandedHeredoc();
		else
			computeBlocksParseUnexpandedHeredoc();

		Q_ASSERT(m_p->p >= m_p->e);

		if(m_p->p >= m_p->e)
			return;
	}

end_of_heredoc:

	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InBashMultiLineWeakQuoteString)
		{
			computeBlocksParseMultiLineWeakQuoteString();
			bKeywordMayFollow = false;
			continue;
		}

		if(m_p->uInterLineFlags & InBashMultiLineStrongQuoteString)
		{
			computeBlocksParseMultiLineStrongQuoteString();
			bKeywordMayFollow = false;
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '#':
				computeBlocksParseSingleLineComment();
			break;
			case '"':
				m_p->uInterLineFlags |= InBashMultiLineWeakQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseMultiLineWeakQuoteString();
				bKeywordMayFollow = false;
			break;
			case '\'':
				m_p->uInterLineFlags |= InBashMultiLineStrongQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseMultiLineStrongQuoteString();
				bKeywordMayFollow = false;
			break;
			case '{':
			case '}':
				m_p->p++;
				BLOCK(b,m_p->p - b,oCodeBlockDelimiterTextColor,0);
				bKeywordMayFollow = true;
			break;
			case '$':
				computeBlocksParseVariable(&(m_p->pCoreData->pOptions->oVariableTextColor));
				bKeywordMayFollow = false;
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
				bKeywordMayFollow = true;
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
				bKeywordMayFollow = false;
			}
			break;
			case '[': // FIXME
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
				bKeywordMayFollow = false;
			}
			break;
			case ']': // FIXME
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
				bKeywordMayFollow = false;
			}
			break;
			case '|':
			case '&':
			case ';':
			case '`': // FIXME: backticks maybe could be treated as strings?
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(
						(u != (ushort)'|') &&
						(u != (ushort)'&') &&
						(u != (ushort)'`') &&
						(u != (ushort)';')
					)
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
				bKeywordMayFollow = true;
			}
			break;
			case '<':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				if(m_p->p->unicode() == '<')
				{
					// <<HEREDOC
					m_p->p++;
					
					if(m_p->p >= m_p->e)
					{
						// unterminated heredoc
						BLOCK(b,m_p->p - b,oErrorTextColor,0);
						return;
					}

					if(m_p->p->unicode() == '-')
					{
						//<<-HEREDOC actually (skips initial line tabs)
						m_p->p++;
					}

					if(m_p->p >= m_p->e)
					{
						// unterminated heredoc
						BLOCK(b,m_p->p - b,oErrorTextColor,0);
						return;
					}

					if(
							m_p->p->isLetterOrNumber() ||
							(m_p->p->unicode() == '_') || 
							(m_p->p->unicode() == '"')
						)
					{
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
						
						m_p->p++;
						
						while(
								(m_p->p < m_p->e) &&
								m_p->p->isLetterOrNumber() ||
								(m_p->p->unicode() == '_') || 
								(m_p->p->unicode() == '"')
							)
							m_p->p++;

						QString szIdentifier(b,m_p->p - b);
						
						bool bGotQuote = szIdentifier.contains(QChar('"')) || szIdentifier.contains(QChar('\''));
						
						m_uNextHeredocId++;
						if(m_uNextHeredocId == 0)
							m_uNextHeredocId++;
						quint32 uHeredocId = m_uNextHeredocId;

						if(bGotQuote)
						{
							szIdentifier.replace(QString("\""),QString());
							szIdentifier.replace(QString("'"),QString());
							m_p->uInterLineFlags |= InBashUnexpandedHeredoc | (uHeredocId << 16);
						} else {
							m_p->uInterLineFlags |= InBashExpandedHeredoc | (uHeredocId << 16);
						}

						BLOCK(b,m_p->p - b,oHeredocTextColor,0);

						//qDebug("Will create heredoc payload at line %d: id is %u->%s (got ptr %x)",m_p->iLineRow,uHeredocId,szIdentifier.toUtf8().data(),m_p->pLine->pPayload);

						Q_ASSERT(!m_p->pLine->pPayload);
						C3TextEditorModeParserBashLinePayload * pPayload = new C3TextEditorModeParserBashLinePayload();
						pPayload->szHeredocIdentifier = szIdentifier;
						m_p->pLine->pPayload = pPayload;

						if(m_p->p >= m_p->e)
						{
							BLOCK(m_p->p,0,oHeredocTextColor,0); // empty block to mark heredoc
							return;
						}

						b = m_p->p;

						// hmmm... this is actually an error, but well...
					} else {
						// ERROR: stay here tho
						BLOCK(b,m_p->p - b,oErrorTextColor,0);
					}
				} else {
					// operator, stay here tho
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
				}
			break;
			case '=':
			case ':':
			case '*':
			case '+':
			case '?':
			case '!':
			case '>':
			case '%':
			case '-':
			case '~':
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
						(u != (ushort)'/') &&
						(u != (ushort)'!') &&
						(u != (ushort)'>') &&
						(u != (ushort)'%') &&
						(u != (ushort)'~') &&
						(u != (ushort)',') &&
						(u != (ushort)'.') &&
						(u != (ushort)'*')
					)
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
				bKeywordMayFollow = false;
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
					computeBlocksParseIdentifier(bKeywordMayFollow);
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
								(u == (ushort)'`') ||
								(u == (ushort)'.') ||
								(u == (ushort)',') ||
								(u == (ushort)'\'') ||
								(u == (ushort)'"') ||
								(u == (ushort)'$') ||
								(u == (ushort)'~') ||
								(u == (ushort)' ') ||
								(u == (ushort)'\\') ||
								(u == (ushort)'-')
							)
							break;
						m_p->p++;
					}

					BLOCK(b,m_p->p - b,oErrorTextColor,0);
					bKeywordMayFollow = false;
				}
			}
			break;
		}
	}
}
