//=============================================================================
//
//   File : C3TextEditorModeParserXML.cpp
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

#include "C3TextEditorModeParserXML.h"
#include "C3TextEditorModeParserJavascript.h"
#include "C3TextEditorModeParserCSS.h"
#include "C3TextEditorModeParserPHP.h"

#include "C3TextEditorOptions.h"

class C3TextEditorModeParserXMLPrivate
{
public:
	C3TextEditorModeParserJavascript * pJavascriptParser;
	C3TextEditorModeParserCSS * pCSSParser;
	C3TextEditorModeParserPHP * pPHPParser;
	unsigned int uFlags;
	
	// Tag parsing
	QString szTagName;

	enum NotableTag
	{
		NotableTagScript,
		NotableTagStyle,
		NotableTagNone
	};

	NotableTag eNotableTag;
	QStringList lNotableTagTokens;

	bool bIsClosingTag;
	
	int findNotableTagToken(const QString &szToken)
	{
		//qDebug("Look for '%s'",szToken.toUtf8().data());
		int idx = 0;
		Q_FOREACH(QString s,lNotableTagTokens)
		{
			//qDebug("- in '%s'",s.toUtf8().data());
			if(s.compare(szToken,Qt::CaseInsensitive) == 0)
				return idx;
			idx++;
		}
		//qDebug("Not found");
		return -1;
	}
	
};

#define _p m_pTEMPHTMLP

C3TextEditorModeParserXML::C3TextEditorModeParserXML(C3TextEditorModeParserState * pParserState,unsigned int uFlags)
	: C3TextEditorModeParser(pParserState)
{
	_p = new C3TextEditorModeParserXMLPrivate();
	_p->pJavascriptParser = NULL;
	_p->pCSSParser = NULL;
	_p->uFlags = uFlags;

	if(_p->uFlags & EnablePHPHandling)
		_p->pPHPParser = new C3TextEditorModeParserPHP(m_p);
	else
		_p->pPHPParser = NULL;
}

C3TextEditorModeParserXML::~C3TextEditorModeParserXML()
{
	if(_p->pJavascriptParser)
		delete _p->pJavascriptParser;
	if(_p->pCSSParser)
		delete _p->pCSSParser;
	if(_p->pPHPParser)
		delete _p->pPHPParser;
	delete _p;
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)

void C3TextEditorModeParserXML::computeBlocksParseMultiLineComment()
{
	Q_ASSERT(m_p->uInterLineFlags & InXMLMultiLineComment);

	const QChar * b = m_p->p;
	
	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case '-':
				m_p->p++;
				
				if(m_p->p >= m_p->e)
					goto end_of_line_in_comment;

				if(m_p->p->unicode() != '-')
				{
					BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
					b = m_p->p;
					continue;
				}

				m_p->p++;

				if(m_p->p >= m_p->e)
					goto end_of_line_in_comment;

				if(m_p->p->unicode() != '>')
				{
					BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
					b = m_p->p;
					continue;
				}

				m_p->p++;
				BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
				m_p->uInterLineFlags &= ~InXMLMultiLineComment;
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



void C3TextEditorModeParserXML::computeBlocksParseCData()
{
	Q_ASSERT(m_p->uInterLineFlags & InXMLCData);

	const QChar * b = m_p->p;
	
	while(m_p->p < m_p->e)
	{
		switch(m_p->p->unicode())
		{
			case ']':
				m_p->p++;
				
				if(m_p->p >= m_p->e)
					goto end_of_line_in_cdata;

				if(m_p->p->unicode() != ']')
				{
					BLOCK(b,m_p->p - b,oTextColor,0);
					b = m_p->p;
					continue;
				}

				m_p->p++;

				if(m_p->p >= m_p->e)
					goto end_of_line_in_cdata;

				if(m_p->p->unicode() != '>')
				{
					BLOCK(b,m_p->p - b,oTextColor,0);
					b = m_p->p;
					continue;
				}

				m_p->p++;
				BLOCK(b,m_p->p - b,oMarkupTagTextColor,0);
				m_p->uInterLineFlags &= ~InXMLCData;
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
	
				BLOCK(b,m_p->p - b,oTextColor,C3TextEditorLine::Block::IsTabBlock);
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
							(u == (ushort)']')
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

end_of_line_in_cdata:
	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oTextColor,0);
}


// Returns the char before the delimiter (or the last char if no delimiter was found)
const QChar * C3TextEditorModeParserXML::computeBlocksParseTagContentsString(unsigned short uDelimiter)
{
	Q_ASSERT(m_p->uInterLineFlags & InXMLTag);
	Q_ASSERT(m_p->uInterLineFlags & (InXMLMultiLineDoubleQuoteString | InXMLMultiLineSingleQuoteString)); // you must set the flag before calling this function

	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();

		switch(uChar)
		{
			case '\\':
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					// line continuation
					if((m_p->p - b) > 1)
						BLOCK(b,m_p->p - b - 1,oStringTextColor,0);
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return m_p->p;
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
				if(m_p->p->unicode() == uDelimiter)
				{
					const QChar * pDelim = m_p->p;
					m_p->p++;
					BLOCK(b,m_p->p - b,oStringTextColor,0);
					m_p->uInterLineFlags &= ~(InXMLMultiLineDoubleQuoteString | InXMLMultiLineSingleQuoteString);
					if(m_p->p >= m_p->e)
					{
						// end of line just after a multi line string end. store an additional empty block to signal state change
						BLOCK(0,0,oTextColor,0);
					}
					return pDelim;
				}

				// non tab, non delimiter
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

				BLOCK(b,m_p->p - b,oStringTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oStringTextColor,0);
	
	return m_p->p;
}

void C3TextEditorModeParserXML::computeBlocksParseTagContents()
{
	Q_ASSERT(m_p->uInterLineFlags & InXMLTag);

	const QChar * b;

	if(m_p->uInterLineFlags & InXMLMultiLineDoubleQuoteString)
	{
		if(_p->eNotableTag != C3TextEditorModeParserXMLPrivate::NotableTagNone)
		{
			b = m_p->p;
			const QChar * x = computeBlocksParseTagContentsString('"');
			if(_p->lNotableTagTokens.count() < 50)
				_p->lNotableTagTokens.append(QString(b,x - b));
		} else {
			computeBlocksParseTagContentsString('\"');
		}
	} else if(m_p->uInterLineFlags & InXMLMultiLineSingleQuoteString)
	{
		if(_p->eNotableTag != C3TextEditorModeParserXMLPrivate::NotableTagNone)
		{
			b = m_p->p;
			const QChar * x = computeBlocksParseTagContentsString('\'');
			if(_p->lNotableTagTokens.count() < 50)
				_p->lNotableTagTokens.append(QString(b,x - b));
		} else {
			computeBlocksParseTagContentsString('\'');
		}
	}

	const QChar * e = m_p->e;

	while(m_p->p < e)
	{
		b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '>':
				m_p->p++;
				BLOCK(b,m_p->p - b,oMarkupTagTextColor,0);
				m_p->uInterLineFlags &= ~InXMLTag;
				if(m_p->p >= e)
				{
					// end of line just after a multi line comment end. store an additional empty block to signal state change
					BLOCK(0,0,oTextColor,0);
				}
				return;
			break;
			case '/':
				m_p->p++;
				if(m_p->p >= e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}

				if(m_p->p->unicode() == '>')
				{
					_p->bIsClosingTag = true;
					m_p->p++;
					BLOCK(b,m_p->p - b,oMarkupTagTextColor,0);
					m_p->uInterLineFlags &= ~InXMLTag;
					if(m_p->p >= e)
					{
						// end of line just after a multi line comment end. store an additional empty block to signal state change
						BLOCK(0,0,oTextColor,0);
					}
					return;
				}

				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
			break;
			case '=':
				m_p->p++;
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
			break;
			case '"':
				m_p->uInterLineFlags |= InXMLMultiLineDoubleQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				if(_p->eNotableTag != C3TextEditorModeParserXMLPrivate::NotableTagNone)
				{
					b = m_p->p;
					const QChar * x = computeBlocksParseTagContentsString('"');
					if(_p->lNotableTagTokens.count() < 50)
						_p->lNotableTagTokens.append(QString(b,x - b));
				} else {
					computeBlocksParseTagContentsString('"');
				}
			break;
			case '\'':
				m_p->uInterLineFlags |= InXMLMultiLineSingleQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;

				if(_p->eNotableTag != C3TextEditorModeParserXMLPrivate::NotableTagNone)
				{
					b = m_p->p;
					const QChar * x = computeBlocksParseTagContentsString('\'');
					if(_p->lNotableTagTokens.count() < 50)
						_p->lNotableTagTokens.append(QString(b,x - b));
				} else {
					computeBlocksParseTagContentsString('\'');
				}
			break;
			case '\t':
			{
				m_p->p++;
				while(m_p->p < e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oMarkupTagTextColor,C3TextEditorLine::Block::IsTabBlock);
			}
			break;
			case ' ':
			{
				m_p->p++;
				while(m_p->p < e)
				{
					if(m_p->p->unicode() != (ushort)' ')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oMarkupTagTextColor,0);
			}
			break;
			default:
			{
				m_p->p++;
				
				// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
				const QChar * tmpEnd = m_p->p + 256;
				if(tmpEnd > e)
					tmpEnd = e;
				
				while(m_p->p < tmpEnd)
				{
					ushort u = m_p->p->unicode();
					if(
							(u == (ushort)'\t') ||
							(u == (ushort)'/') ||
							(u == (ushort)'"') ||
							(u == (ushort)'=') ||
							(u == (ushort)'\'') ||
							(u == (ushort)' ') ||
							(u == (ushort)'>')
						)
						break;
					m_p->p++;
				}

				if(_p->eNotableTag != C3TextEditorModeParserXMLPrivate::NotableTagNone)
				{
					if(_p->lNotableTagTokens.count() < 50)
						_p->lNotableTagTokens.append(QString(b,m_p->p - b));
				}

				BLOCK(b,m_p->p - b,oMarkupTagTextColor,0);
			}
			break;
		}
	}
}

void C3TextEditorModeParserXML::computeBlocksParseTag()
{
	Q_ASSERT(m_p->uInterLineFlags & InXMLTag); // caller sets this
	Q_ASSERT(m_p->p->unicode() == (ushort)'<');
	Q_ASSERT(m_p->p < m_p->e);

	const QChar * b = m_p->p;
	const QChar * e = m_p->e;

	m_p->p++;

	if(m_p->p >= e)
	{
		_p->szTagName = QString();
		_p->bIsClosingTag = false;
		_p->eNotableTag = C3TextEditorModeParserXMLPrivate::NotableTagNone;
		BLOCK(b,m_p->p - b,oErrorTextColor,0); // this is *invalid* per HTML spec.
		return;
	}

	switch(m_p->p->unicode())
	{
		case '/':
			_p->bIsClosingTag = true; // not at the beginning at least. (might be self closing)
			m_p->p++; // move ahead

			if(m_p->p >= e)
			{
				_p->szTagName = QString();
				_p->eNotableTag = C3TextEditorModeParserXMLPrivate::NotableTagNone;
				BLOCK(b,m_p->p - b,oErrorTextColor,0); // this is *invalid* per HTML spec.
				return;
			}
		break;
		default:
			_p->bIsClosingTag = false; // not at the beginning at least. (might be self closing)
		break;
	}

	BLOCK(b,m_p->p - b,oMarkupTagTextColor,0);

	b = m_p->p;
	while(m_p->p < e)
	{
		if(!(m_p->p->isLetter() || (m_p->p->unicode() == '_')))
			break;
		m_p->p++;
	}
	
	if(m_p->p > b)
	{
		BLOCK(b,m_p->p - b,oMarkupTagNameTextColor,0);
		_p->szTagName.setUnicode(b,m_p->p - b);

		static QString szScript = __ascii("script");
		static QString szStyle = __ascii("style");

		if((_p->uFlags & EnableScriptTagHandling) && (_p->szTagName.compare(szScript,Qt::CaseInsensitive) == 0))
		{
			_p->eNotableTag = C3TextEditorModeParserXMLPrivate::NotableTagScript;
			_p->lNotableTagTokens.clear();
		} else if((_p->uFlags & EnableStyleTagHandling) && (_p->szTagName.compare(szStyle,Qt::CaseInsensitive) == 0))
		{
			_p->eNotableTag = C3TextEditorModeParserXMLPrivate::NotableTagStyle;
			_p->lNotableTagTokens.clear();
		} else
			_p->eNotableTag = C3TextEditorModeParserXMLPrivate::NotableTagNone;
		
	} else {
		// empty tag name or or some other crap?
		_p->szTagName = QString();
		_p->eNotableTag = C3TextEditorModeParserXMLPrivate::NotableTagNone;
	}

	if(m_p->p >= e)
		return;
	
	// continue in generic tag contents
	computeBlocksParseTagContents();
}

void C3TextEditorModeParserXML::computeBlocksParseJavascript()
{
	if(!_p->pJavascriptParser)
		_p->pJavascriptParser = new C3TextEditorModeParserJavascript(m_p);

	const QChar * save;

	// This is *exactly* what google-chrome does. It terminates
	// the script at the first instance of the _exact_ sequence </script[ ]*>,
	// even in a javascript comment or string (and that's actually invalid javascript)

	const QChar * e = findString("</script",8);
	if(e)
	{
		// maybe script terminator?
		const QChar * ee = e + 8;
		while((ee < m_p->e) && ee->isSpace())
			ee++;
		if((ee < m_p->e) && (ee->unicode() == '>'))
		{
			save = m_p->e;
			m_p->e = e;
		} else {
			// not a script terminator
			e = NULL;
		}
	}

	_p->pJavascriptParser->computeMetadata();
	if(e)
	{
		// Javascript inter-line flags are NOT preserved across script tags.
		m_p->uInterLineFlags &= ~(InXMLJavascript | C3TextEditorModeParserJavascript::JavascriptInterLineFlagMask);
		m_p->e = save;
	}
}

void C3TextEditorModeParserXML::computeBlocksParseCSS()
{
	if(!_p->pCSSParser)
		_p->pCSSParser = new C3TextEditorModeParserCSS(m_p);

	const QChar * save;

	// This is *exactly* what google-chrome does. It terminates
	// the style at the first instance of the _exact_ sequence </style[ ]*>,
	// even in a CSS comment or string (and that's actually invalid CSS)

	const QChar * e = findString("</style",7);
	if(e)
	{
		// maybe style terminator?
		const QChar * ee = e + 7;
		while((ee < m_p->e) && ee->isSpace())
			ee++;
		if((ee < m_p->e) && (ee->unicode() == '>'))
		{
			save = m_p->e;
			m_p->e = e;
		} else {
			// not a style terminator
			e = NULL;
		}
	}

	_p->pCSSParser->computeMetadata();
	if(e)
	{
		// CSS inter-line flags are NOT preserved across script tags.
		m_p->uInterLineFlags &= ~(InXMLCSS | C3TextEditorModeParserCSS::CSSInterLineFlagMask);
		m_p->e = save;
	}
}

void C3TextEditorModeParserXML::computeBlocksParseHTML()
{
	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InXMLTag)
		{
			computeBlocksParseTagContents();
			continue;
		}

		if(m_p->uInterLineFlags & InXMLJavascript)
		{
			computeBlocksParseJavascript();
			continue;
		}

		if(m_p->uInterLineFlags & InXMLCSS)
		{
			computeBlocksParseCSS();
			continue;
		}

		if(m_p->uInterLineFlags & InXMLMultiLineComment)
		{
			computeBlocksParseMultiLineComment();
			continue;
		}

		if(m_p->uInterLineFlags & InXMLCData)
		{
			computeBlocksParseCData();
			continue;
		}

		// not in tag comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '<':
				// FIXME: Handle <?[xml] ?
				if(pointingAtString("<!--",4))
				{
					m_p->uInterLineFlags |= InXMLMultiLineComment;
					computeBlocksParseMultiLineComment();
				} else if(pointingAtString("<![CDATA[",9))
				{
					BLOCK(b,1,oMarkupTagTextColor,0);
					b++;
					m_p->p += 9;
					BLOCK(b,m_p->p - b,oMarkupTagNameTextColor,0);
					m_p->uInterLineFlags |= InXMLCData;
					computeBlocksParseCData();
				} else {
					m_p->uInterLineFlags |= InXMLTag;
					computeBlocksParseTag();
					if((!(m_p->uInterLineFlags & InXMLTag)) && (!_p->bIsClosingTag))
					{
						// just finished an opening tag
						
						// the notable tags are set only if enabled.
						
						const QString szTextCss = __ascii("text/css");
						const QString szJavascript = __ascii("javascript");
						const QString szType = __ascii("type");
						
						switch(_p->eNotableTag)
						{
							case C3TextEditorModeParserXMLPrivate::NotableTagScript:
							{
								int idx = _p->findNotableTagToken(szType);
								if(
									// either no type="" tag or type="javascript"
									(idx < 0) ||
									(_p->findNotableTagToken(szJavascript) == (idx+1))
								)
								{
									m_p->uInterLineFlags |= InXMLJavascript;
									if(m_p->p >= m_p->e)
									{
										// zero size block to signal we're in javascript
										b = m_p->p;
										BLOCK(b,m_p->p - b,oTextColor,0);
										return;
									}
									computeBlocksParseJavascript();
								}
							}
							break;
							case C3TextEditorModeParserXMLPrivate::NotableTagStyle:
							{
								int idx = _p->findNotableTagToken(szType);
								if(
									// either no type="" tag or type="text/css"
									(idx < 0) ||
									(_p->findNotableTagToken(szTextCss) == (idx+1))
								)
								{
									m_p->uInterLineFlags |= InXMLCSS;
									if(m_p->p >= m_p->e)
									{
										// zero size block to signal we're in css
										b = m_p->p;
										BLOCK(b,m_p->p - b,oTextColor,0);
										return;
									}
									computeBlocksParseCSS();
								}
							}
							break;
							default:
							break;
						}
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
							(u == (ushort)'\t') ||
							(u == (ushort)'<')
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oTextColor,0);
			}
			break;
		}
	}
}


void C3TextEditorModeParserXML::computeMetadata()
{
	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}
	
	if(!_p->pPHPParser)
	{
		computeBlocksParseHTML();
		return;
	}

	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InXMLPHP)
		{
			_p->pPHPParser->computeMetadata(); // manages also InXMLPhp flag!
			continue;
		}

		// look for a PHP tag.

		const QChar * save;
	
		const QChar * e = findString("<?",2);
		
		if(!e)
		{
			computeBlocksParseHTML();
			return;
		}

		if(e > m_p->p)
		{
			// a HTML part
			save = m_p->e;
			m_p->e = e;
	
			computeBlocksParseHTML();
	
			m_p->e = save;
		}

		m_p->uInterLineFlags |= InXMLPHP;
		_p->pPHPParser->computeMetadata(); // manages also InXMLPhp flag!
	}
}

