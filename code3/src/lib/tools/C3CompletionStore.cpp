//=============================================================================
//
//   File : C3CompletionStore.cpp
//   Creation Date : 2015/11/14 02:45:31
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

#include "C3CompletionStore.h"

#include <QHash>

// FIXME: Phase this out?

class C3CompletionStorePrivate
{
public:
	// Hash(<first_char> -> Hash(<first_two_chars>,List(<string>)))
	QHash<quint16, QHash<quint32,QStringList * > * > hCompletions;
};

#define _p m_pC3CS

C3CompletionStore::C3CompletionStore()
{
	_p = new C3CompletionStorePrivate();
}

C3CompletionStore::~C3CompletionStore()
{
	clear();
	delete _p;
}

void C3CompletionStore::clear()
{
	for(QHash<quint16,QHash<quint32,QStringList *> * >::iterator it = _p->hCompletions.begin();it != _p->hCompletions.end();++it)
	{
		QHash<quint32,QStringList *> * pSubHash = it.value();
		qDeleteAll(*pSubHash);
		pSubHash->clear();
	}

	qDeleteAll(_p->hCompletions);
	_p->hCompletions.clear();
}

void C3CompletionStore::addCompletion(const QString &szText)
{
	if(szText.isEmpty())
		return;

	quint16 u = szText[0].unicode();

	QHash<quint32,QStringList * > * pSubHash = _p->hCompletions.value(u);
	if(!pSubHash)
	{
		pSubHash = new QHash<quint32,QStringList * >();
		_p->hCompletions.insert(u,pSubHash);
	}
	
	quint32 uu;
	
	if(szText.length() > 1)
		uu = *((quint32 *)szText.unicode());
	else
		uu = u;

	QStringList * sl = pSubHash->value(uu);
	if(!sl)
	{
		sl = new QStringList();
		pSubHash->insert(uu,sl);
		sl->append(szText);
	} else {
		QStringList::iterator i = qLowerBound(sl->begin(),sl->end(),szText);
		if((i != sl->end()) && (*i == szText))
			return;
		sl->insert(i,szText);
	}
	
	//static int iEntries = 0;
	//iEntries++;
	//qDebug("Got %d entries",iEntries);
}

void C3CompletionStore::getCompletions(const QString &szText,QStringList &lBuffer)
{
	if(szText.isEmpty())
		return;

	quint16 u = szText[0].unicode();

	QHash<quint32,QStringList * >  * pSubHash = _p->hCompletions.value(u);
	if(!pSubHash)
		return;
	
	
	if(szText.length() < 2)
	{
		// must scan the whole hash
		for(QHash<quint32,QStringList *>::iterator it = pSubHash->begin();it != pSubHash->end();++it)
		{
			QStringList * sl = it.value();
			foreach(QString s,*sl)
			{
				if(!s.startsWith(szText))
					continue;
		
				QStringList::iterator i = qLowerBound(lBuffer.begin(),lBuffer.end(),s);
				if((i != lBuffer.end()) && (*i == s))
					continue; // already there

				lBuffer.insert(i,s);
			}
		}
		
		return;
	}

	quint32 uu = *((quint32 *)szText.unicode());

	{
		QStringList * sl = pSubHash->value(uu);
		if(!sl)
			return;
	
		foreach(QString s,*sl)
		{
			if(!s.startsWith(szText))
				continue;
	
			QStringList::iterator i = qLowerBound(lBuffer.begin(),lBuffer.end(),s);
			if((i != lBuffer.end()) && (*i == s))
				continue; // already there
	
			lBuffer.insert(i,s);
		}
	}
}

bool C3CompletionStore::findExact(const QString &szText)
{
	if(szText.isEmpty())
		return false;

	quint16 u = szText[0].unicode();

	QHash<quint32,QStringList * >  * pSubHash = _p->hCompletions.value(u);
	if(!pSubHash)
		return false;
	
	
	if(szText.length() < 2)
	{
		// must scan the whole hash
		for(QHash<quint32,QStringList *>::iterator it = pSubHash->begin();it != pSubHash->end();++it)
		{
			QStringList * sl = it.value();
			foreach(QString s,*sl)
			{
				if(s == szText)
					return true;
			}
		}
		
		return false;
	}

	quint32 uu = *((quint32 *)szText.unicode());

	{
		QStringList * sl = pSubHash->value(uu);
		if(!sl)
			return false;
	
		foreach(QString s,*sl)
		{
			if(s == szText)
				return true;
		}
	}

	return false;
}
