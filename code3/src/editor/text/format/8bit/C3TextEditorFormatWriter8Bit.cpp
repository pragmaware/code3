//=============================================================================
//
//   File : C3TextEditorFormatWriter8Bit.cpp
//   Creation Date : gio 22 ott 2015 23:14:06
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

#include "C3TextEditorFormatWriter8Bit.h"

#include <QFile>
#include <QTextCodec>

class C3TextEditorFormatWriter8BitPrivate
{
public:
	bool bByteOrderMark;
	QTextCodec * pTextCodec;
};

#define _p m_pC3TEFWB

C3TextEditorFormatWriter8Bit::C3TextEditorFormatWriter8Bit(QTextCodec * pCodec,bool bByteOrderMark)
	: C3TextEditorFormatWriter()
{
	Q_ASSERT(pCodec);

	_p = new C3TextEditorFormatWriter8BitPrivate();
	_p->pTextCodec = pCodec;
	_p->bByteOrderMark = bByteOrderMark;
}

C3TextEditorFormatWriter8Bit::~C3TextEditorFormatWriter8Bit()
{
	delete _p;
}


bool C3TextEditorFormatWriter8Bit::write(const QString &szPath,QList<C3TextEditorLine *> &lLines,QString &szError)
{
	QFile f(szPath);
	
	if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		szError = f.errorString();
		return false;
	}

	QTextCodec::ConverterState oState(QTextCodec::IgnoreHeader);

	int c = lLines.count();
	
	// FIXME: Write a temporary file and rename!!!
	int i=0;
	
	QByteArray ba;
	
	if(_p->bByteOrderMark)
	{
		unsigned char buffer[3];
		buffer[0] = 0xef;
		buffer[1] = 0xbb;
		buffer[2] = 0xbf;
		
		if(f.write((const char *)buffer,3) != 3)
		{
			QString szDetail = f.errorString();
			szError = szDetail.isEmpty() ? __tr("File I/O error") : szDetail;
			return false;
		}
	}

	C3TextEditorLine * pLine;

	while(i < c-1)
	{
		pLine = lLines.at(i);
	
		ba = _p->pTextCodec->fromUnicode(pLine->szText.unicode(),pLine->szText.length(),&oState);
		ba.append('\n');
		if(f.write(ba) != ba.size())
		{
			QString szDetail = f.errorString();
			szError = szDetail.isEmpty() ? __tr("File I/O error") : szDetail;
			return false;
		}
		i++;
	}

	pLine = lLines.at(i);

	// last line without newline
	if(!pLine->szText.isEmpty())
	{
		ba = _p->pTextCodec->fromUnicode(pLine->szText.unicode(),pLine->szText.length(),&oState);
		if(f.write(ba) != ba.size())
		{
			QString szDetail = f.errorString();
			szError = szDetail.isEmpty() ? __tr("File I/O error") : szDetail;
			return false;
		}
	}

	return true;
}
