//=============================================================================
//
//   File : C3SettingsUserHighlightingSet.cpp
//   Creation Date : 2015/11/22 05:03:48
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

#include "C3SettingsUserHighlightingSet.h"
#include "C3JsonUtils.h"
#include "C3TextEditorModeCPPSharedData.h"

#include <QJsonObject>
#include <QJsonArray>
#include <C3TextEditorModePHPSharedData.h>
#include <C3TextEditorModeJavaSharedData.h>

C3SettingsUserHighlightingSet::C3SettingsUserHighlightingSet()
{
}

C3SettingsUserHighlightingSet::~C3SettingsUserHighlightingSet()
{
	qDeleteAll(m_hSubsets);
}

QStringList * C3SettingsUserHighlightingSet::subset(const QString &szId)
{
	QStringList * sl = m_hSubsets.value(szId);
	if(!sl)
	{
		sl = new QStringList();
		m_hSubsets.insert(szId,sl);
	}
	return sl;
}

void C3SettingsUserHighlightingSet::clear()
{
	qDeleteAll(m_hSubsets);
	m_hSubsets.clear();
}

void C3SettingsUserHighlightingSet::copyFrom(C3SettingsUserHighlightingSet &src)
{
	qDeleteAll(m_hSubsets);
	m_hSubsets.clear();
	
	QHash<QString,QStringList *>::Iterator it;
	for(it = src.m_hSubsets.begin();it != src.m_hSubsets.end();++it)
	{
		QStringList * sl = it.value();
		if(sl->isEmpty())
			continue;
		QStringList * sl2 = new QStringList(*sl);
		m_hSubsets.insert(it.key(),sl2);
	}
}

void C3SettingsUserHighlightingSet::saveToJsonObject(QJsonObject &ob) const
{
	QHash<QString,QStringList *>::ConstIterator it;
	for(it = m_hSubsets.begin();it != m_hSubsets.end();++it)
	{
		QStringList * sl = it.value();
		if(sl->isEmpty())
			continue;
		
		ob.insert(it.key(),QJsonArray::fromStringList(*sl));
	}
}

void C3SettingsUserHighlightingSet::loadFromJsonObject(const QJsonObject &ob)
{
	qDeleteAll(m_hSubsets);
	m_hSubsets.clear();

	QJsonObject::ConstIterator it;
	for(it = ob.begin();it != ob.end();++it)
	{
		QJsonValue v = it.value();
		if(!v.isArray())
			continue;
		QJsonArray a = v.toArray();
		if(a.count() < 1)
			continue;
	
		QStringList * sl = new QStringList();
		*sl = C3JsonUtils::jsonArrayToStringList(a);
		
		m_hSubsets.insert(it.key(),sl);
	}
}

void C3SettingsUserHighlightingSet::setDefaults()
{
	C3TextEditorModeCPPSharedData::setDefaultUserHighlightingSet(this);
	C3TextEditorModePHPSharedData::setDefaultUserHighlightingSet(this);
	C3TextEditorModeJavaSharedData::setDefaultUserHighlightingSet(this);
}
