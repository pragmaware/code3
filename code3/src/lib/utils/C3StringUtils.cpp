//=============================================================================
//
//   File : C3StringUtils.cpp
//   Creation Date : ven 13 nov 2015 00:48:30
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

#include "C3StringUtils.h"

#include <QStringList>

namespace C3StringUtils
{
	QString fileNameFromPath(const QString &szPath)
	{
		int idx = szPath.lastIndexOf(QChar('/'));
		int idx2 = szPath.lastIndexOf(QChar('\\'));
		
		if(idx2 > idx)
			idx = idx2;
		
		if(idx < 0)
			return szPath;

		return szPath.mid(idx+1);
	}

	QString fileDirectoryFromPath(const QString &szPath)
	{
		int idx = szPath.lastIndexOf(QChar('/'));
		int idx2 = szPath.lastIndexOf(QChar('\\'));
		
		if(idx2 > idx)
			idx = idx2;
		
		if(idx < 0)
			return szPath;

		return szPath.left(idx);
	}

	QString fileExtensionFromPath(const QString &szPath)
	{
		int idx = szPath.lastIndexOf(QChar('/'));
		int idx2 = szPath.lastIndexOf(QChar('\\'));
		
		if(idx2 > idx)
			idx = idx2;

		int pnt = szPath.lastIndexOf(QChar('.'));
		if(pnt <= idx)
			return QString();
		return szPath.mid(pnt+1);
	}

	int commonCharacterCount(const QString &sz1,const QString &sz2)
	{
		int l1 = sz1.length();
		int l2 = sz2.length();
		int l = (l1 > l2) ? l2 : l1;
		if(l < 1)
			return 0;
		const QChar * c1 = sz1.unicode();
		const QChar * c2 = sz2.unicode();
		Q_ASSERT(c1 && c2);
		const QChar * e = c1 + l;
		const QChar * b = c1;
		while(c1 < e)
		{
			if(c1->unicode() != c2->unicode())
				break;
			c1++;
			c2++;
		}
		return c1 - b;
	}

	QString prefixLines(const QString &szPrefix,const QString &szText)
	{
		bool bTrailingLF = false;
	
		if(szText.endsWith(QChar('\n')))
			bTrailingLF = true;
	
		QStringList sl = szText.split(QChar('\n'));
		
		if(bTrailingLF)
			sl.removeLast();

		if(sl.count() < 1)
			return QString();
			
		
		QStringList out;
		
		Q_FOREACH(QString s,sl)
		{
			QString x = szPrefix;
			x += s;
			out.append(x);
		}
		
		QString szRet = out.join(QChar('\n'));
		if(bTrailingLF)
			szRet.append(QChar('\n'));
		return szRet;
	}


	QString indentLines(const QString &szText)
	{
		return prefixLines(__ascii("	"),szText);
	}

} // namespace C3StringUtils
