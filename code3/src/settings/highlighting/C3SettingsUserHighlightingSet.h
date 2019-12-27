#ifndef _C3SettingsUserHighlightingSet_h_
#define _C3SettingsUserHighlightingSet_h_
//=============================================================================
//
//   File : C3SettingsUserHighlightingSet.h
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

#include "c3_config.h"

#include <QHash>
#include <QString>
#include <QStringList>

class QJsonObject;

class C3SettingsUserHighlightingSet
{
public:
	C3SettingsUserHighlightingSet();
	~C3SettingsUserHighlightingSet();

private:

	QHash<QString,QStringList *> m_hSubsets;

public:

	bool hasSubset(const QString &szId)
	{
		return m_hSubsets.value(szId) != NULL;
	}

	QStringList * subset(const QString &szId);
	QStringList * subsetIfPresent(const QString &szId)
	{
		return m_hSubsets.value(szId);
	}

	void clear();
	
	void copyFrom(C3SettingsUserHighlightingSet &src);
	
	void saveToJsonObject(QJsonObject &ob) const;
	void loadFromJsonObject(const QJsonObject &ob);

	void setDefaults();

}; // class C3SettingsUserHighlightingSet

#endif //!_C3SettingsUserHighlightingSet_h_
