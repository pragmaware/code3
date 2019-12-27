//=============================================================================
//
//   File : C3NewCPPClassSettings.cpp
//   Creation Date : mer 11 nov 2015 03:41:24
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

#include "C3NewCPPClassSettings.h"


QList<QString> C3NewCPPClassSettings::splitClassNameList(const QString &nl)
{
	QList<QString> ret;
	
	const QChar * c = nl.data();
	const QChar * e = c + nl.length();

	while(c < e)
	{
		while((c < e) && (!(c->isLetter() || (c->unicode() == '_'))))
			c++;
		
		if(c >= e)
			break;
		
		const QChar * b = c;
		
		while((c < e) && (c->isLetterOrNumber() || c->unicode() == '_'))
			c++;
		
		Q_ASSERT(c > b);

		QString cl(b,c-b);

		while((c < e) && c->isSpace())
			c++;

		if(c >= e)
		{
			ret.append(cl);
			break;
		}
		
		if(c->unicode() != '<')
		{
			ret.append(cl);
			continue;
		}

		int iLevel = 1;

		b = c;

		c++;

		while(c < e)
		{
			uchar u = c->unicode();
			
			if(u =='<')
			{
				iLevel++;
				c++;
				continue;
			}
			
			if(u == '>')
			{
				iLevel--;
				c++;
				if(iLevel < 1)
					break;
			}
			
			c++;
		}
		
		cl.append(QString(b,c-b));
		ret.append(cl);
	}

	return ret;
}
