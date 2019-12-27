//=============================================================================
//
//   File : C3DebugContext.cpp
//   Creation Date : 2018/01/21 01:37:05
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

#include "C3DebugContext.h"

#include <QMutex>
#include <QString>

#include "c3_config.h"

class C3DebugContextGlobals
{
public:
	QMutex oMutex;
	QByteArray szIndentPrefix;
};

static C3DebugContextGlobals g_oDebugContextGlobals;

class C3DebugContextPrivate
{
public:
	QString m_szContext;
};

C3DebugContext::C3DebugContext(const char * szContext,...)
{
	QMutexLocker oLocker(&(g_oDebugContextGlobals.oMutex));

	m_p = new C3DebugContextPrivate();

	va_list list;
	va_start(list,szContext);

	m_p->m_szContext.vsprintf(szContext,list);

	va_end(list);
	
	static QString szEnterMark = __ascii(">> ");
	
	QString szOut = g_oDebugContextGlobals.szIndentPrefix;
	szOut.append(szEnterMark);
	szOut.append(m_p->m_szContext);

	qDebug("%s",szOut.toUtf8().data());
	
	static QString szSpaces = __ascii("  ");
	
	g_oDebugContextGlobals.szIndentPrefix.append(szSpaces);
}


C3DebugContext::~C3DebugContext()
{
	QMutexLocker oLocker(&(g_oDebugContextGlobals.oMutex));

	C3_ASSERT(g_oDebugContextGlobals.szIndentPrefix.length() >= 2);
	g_oDebugContextGlobals.szIndentPrefix.remove(g_oDebugContextGlobals.szIndentPrefix.length() - 2,2);

	static QString szExitMark = __ascii("<< ");

	QString szOut = g_oDebugContextGlobals.szIndentPrefix;
	szOut.append(szExitMark);
	szOut.append(m_p->m_szContext);

	qDebug("%s",szOut.toUtf8().data());
	
	delete m_p;
}

void C3DebugContext::trace(const char * szFmt,...)
{
	QMutexLocker oLocker(&(g_oDebugContextGlobals.oMutex));

	va_list list;
	va_start(list,szFmt);

	QString szTxt;
	szTxt.vsprintf(szFmt,list);

	va_end(list);
	
	static QString szEnterMark = __ascii("-- ");
	
	QString szOut = g_oDebugContextGlobals.szIndentPrefix;
	szOut.append(szEnterMark);
	szOut.append(szTxt);

	qDebug("%s",szOut.toUtf8().data());
}
