//=============================================================================
//
//   File : C3TextEditorFormatReader8Bit.cpp
//   Creation Date : gio 22 ott 2015 23:13:44
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

#include "C3TextEditorFormatReader8Bit.h"
#include "C3TextEditorLine.h"

#include <QTextCodec>
#include <QFile>
#include <QLinkedList>

#include <string.h> // memcpy

// We read in 1Meg chunks
#define BUFFER_SIZE (1024 * 1024)

class C3TextEditorFormatReader8BitPrivate
{
public:
	bool bByteOrderMark;
	QTextCodec * pTextCodec;
	QTextCodec::ConverterState oConverterState;
	
	qint64 iDataInBuffer;
	unsigned char buffer[BUFFER_SIZE];

	bool bLastLineHadNewline;
	qint64 iLineCost;
	QLinkedList<C3TextEditorLine *> lLines;
};

#define _p m_pC3TEFRB

C3TextEditorFormatReader8Bit::C3TextEditorFormatReader8Bit(QTextCodec * pCodec,bool bByteOrderMark)
	: C3TextEditorFormatReader()
{
	Q_ASSERT(pCodec);

	_p = new C3TextEditorFormatReader8BitPrivate();
	_p->pTextCodec = pCodec;
	_p->bByteOrderMark = bByteOrderMark;
}

C3TextEditorFormatReader8Bit::~C3TextEditorFormatReader8Bit()
{
	// kill anything not emitted
	qDeleteAll(_p->lLines);
	_p->lLines.clear();
	delete _p;
}


void C3TextEditorFormatReader8Bit::runInternal()
{
	if(aborted())
		return;
	
	QFile f(path());
	
	if(!f.open(QFile::ReadOnly))
	{
		error(__tr("Could not open file: %1").arg(f.errorString()));
		return;
	}

	if(aborted())
		return;

	if(_p->bByteOrderMark)
	{
		// this is set only for utf8 encoding
		// three bytes: EF BB BF
		
		if(f.read((char *)(_p->buffer),3) != 3)
		{
			error(__tr("Could not read the byte order mark"));
			return;
		}

		if(
				(((unsigned char)_p->buffer[0]) != 0xef) ||
				(((unsigned char)_p->buffer[1]) != 0xbb) ||
				(((unsigned char)_p->buffer[2]) != 0xbf)
			)
		{
			error(__tr("Invalid byte order mark"));
			return;
		}
	}

	_p->iDataInBuffer = 0;
	_p->iLineCost = 0;
	_p->bLastLineHadNewline = true; // set to true so in case of empty file a final empty line will be emitted

	
	while(!f.atEnd())
	{
		if(aborted())
			return;
	
		qint64 iToRead = BUFFER_SIZE - _p->iDataInBuffer;
		if(iToRead == 0)
		{
			// no space!
			processData(true,false);
			markContentAsAltered(__tr("File contains lines longer than %1 bytes").arg(BUFFER_SIZE));
			iToRead = BUFFER_SIZE - _p->iDataInBuffer;
			if(iToRead == 0)
			{
				error(__tr("Internal error: failed to process buffer"));
				return;
			}
		}

		qint64 iRead = f.read((char *)((_p->buffer) + _p->iDataInBuffer),iToRead);
		if(iRead < 0)
		{
			error(__tr("IO error: %1").arg(f.errorString()));
			return;
		}
		
		if(iRead > 0)
		{
			_p->iDataInBuffer += iRead;
			processData(false,false);
		}
	}

	if(aborted())
		return;

	processData(true,true);
}

void C3TextEditorFormatReader8Bit::processData(bool bForceEmitLastLine,bool bIsLastCall)
{
	unsigned char * p = _p->buffer;
	unsigned char * e = _p->buffer + _p->iDataInBuffer;
	unsigned char * b = p;

	while(p < e)
	{
		if(*p == '\n')
		{
			// newline!
			C3TextEditorLine * l = new C3TextEditorLine();
			l->szText = _p->pTextCodec->toUnicode((char *)b,p-b,&(_p->oConverterState));
			_p->lLines.append(l);
			_p->bLastLineHadNewline = true;
			p++;
			b = p;
		} else {
			p++;
		}
	}

	if(b < e)
	{
		if(bForceEmitLastLine)
		{
			C3TextEditorLine * l = new C3TextEditorLine();
			l->szText = _p->pTextCodec->toUnicode((char *)b,e-b,&(_p->oConverterState));
			_p->lLines.append(l);
			_p->bLastLineHadNewline = false;
			_p->iLineCost += _p->iDataInBuffer;
			_p->iDataInBuffer = 0;
		} else {
			_p->iLineCost += b - _p->buffer;
			::memcpy(_p->buffer,b,e-b);
			_p->iDataInBuffer = e-b;
		}
	} else {
		_p->iLineCost += _p->iDataInBuffer;
		_p->iDataInBuffer = 0;
	}

	if(bIsLastCall && _p->bLastLineHadNewline)
	{
		// emit an empty final line
		C3TextEditorLine * l = new C3TextEditorLine();
		_p->lLines.append(l);
	}

	if(
			bIsLastCall ||
			(_p->iLineCost > (1024 * 4096)) || // 4 megs
			(_p->lLines.count() > 8192) // too many lines
		)
	{
		_p->iLineCost = 0;
		if(_p->lLines.count() > 0)
		{
			emitLines(_p->lLines);
			Q_ASSERT(_p->lLines.count() == 0);
		}
	}
}
