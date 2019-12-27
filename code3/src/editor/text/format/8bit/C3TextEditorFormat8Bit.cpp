//=============================================================================
//
//   File : C3TextEditorFormat8Bit.cpp
//   Creation Date : gio 22 ott 2015 23:06:22
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

#include "C3TextEditorFormat8Bit.h"

#include "C3TextEditorFormatReader8Bit.h"
#include "C3TextEditorFormatWriter8Bit.h"

#include <QTextCodec>

#define _p m_pTEF8P

class C3TextEditorFormat8BitPrivate
{
public:
	QString szTextCodecName;
	bool bByteOrderMark;
	QTextCodec * pTextCodec;
};


C3TextEditorFormat8Bit::C3TextEditorFormat8Bit(const QString &szId,const QString &szName,const QString &szTextCodecName,bool bByteOrderMark)
	: C3TextEditorFormat(szId,szName)
{
	_p = new C3TextEditorFormat8BitPrivate();
	
	_p->bByteOrderMark = bByteOrderMark;
	_p->szTextCodecName = szTextCodecName;
	_p->pTextCodec = QTextCodec::codecForName(szTextCodecName.toUtf8());
	if(!_p->pTextCodec)
		qDebug("ERROR: Could not find codec for name %s",szTextCodecName.toUtf8().data());
}

C3TextEditorFormat8Bit::~C3TextEditorFormat8Bit()
{
	delete _p;
}

C3TextEditorFormatReader * C3TextEditorFormat8Bit::createReader()
{
	return new C3TextEditorFormatReader8Bit(_p->pTextCodec,_p->bByteOrderMark);
}

C3TextEditorFormatWriter * C3TextEditorFormat8Bit::createWriter()
{
	return new C3TextEditorFormatWriter8Bit(_p->pTextCodec,_p->bByteOrderMark);
}