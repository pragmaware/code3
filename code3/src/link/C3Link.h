#ifndef _C3Link_h_
#define _C3Link_h_
//=============================================================================
//
//   File : C3Link.h
//   Creation Date : 2015/11/24 23:38:26
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
#include <QVariant>
#include <QJsonObject>

//
// The link has the following properties:
//
//    path: full path to file (usually mandatory, except for cetains cases)
//    location: <line>[:<character>] (optional)
//    factoryId: <the id of the editor factory> (optional)
//    modeId: <the id of the text editor mode> (optional)
//
//    custom fields can be added with insert() and retrieved with value()
//

class C3Link : public QHash<QString,QVariant>
{
public:
	C3Link(const QHash<QString,QVariant> &src)
		: QHash<QString,QVariant>(src)
	{
	}

	C3Link(const C3Link &src)
		: QHash<QString,QVariant>(src)
	{
	}

	C3Link(const QString &szPath)
		: QHash<QString,QVariant>()
	{
		insert(QString("path"),QVariant(szPath));
	}

	C3Link()
		: QHash<QString,QVariant>()
	{
	}
	
	C3Link(const QJsonObject &ob);
	
	~C3Link()
	{
	}
	
public:

	QString path() const
	{
		return value(QString("path")).toString();
	}
	
	void setPath(const QString &szPath)
	{
		insert(QString("path"),QVariant(szPath));
	}

	QString description() const
	{
		return value(QString("description")).toString();
	}
	
	void setDescription(const QString &szDescription)
	{
		insert(QString("description"),QVariant(szDescription));
	}
	
	void setText(const QString &szText)
	{
		insert(QString("text"),QVariant(szText));
	}

	QString text()
	{
		return value(QString("text")).toString();
	}

	// In locations the line numbers are 1-based!
	QString location() const
	{
		return value(QString("location")).toString();
	}

	// In locations the line numbers are 1-based!
	void setLocation(const QString &szLocation)
	{
		insert(QString("location"),QVariant(szLocation));
	}
	
	void clearLocation()
	{
		remove(QString("location"));
	}

	QString textEditorFormatId() const
	{
		return value(QString("formatId")).toString();
	}

	void setTextEditorFormatId(const QString &szFormatId)
	{
		insert(QString("formatId"),QVariant(szFormatId));
	}

	QString textEditorModeId() const
	{
		return value(QString("modeId")).toString();
	}

	void setTextEditorModeId(const QString &szModeId)
	{
		insert(QString("modeId"),QVariant(szModeId));
	}
	
	QString editorFactoryId() const
	{
		return value(QString("factoryId")).toString();
	}
	
	void setEditorFactoryId(const QString &szFactoryId)
	{
		insert(QString("factoryId"),QVariant(szFactoryId));
	}

	QJsonObject toJsonObject() const;

	QString toHtml() const;

public:

	C3Link & operator = (const C3Link &other)
	{
		QHash<QString,QVariant>::operator=(other);
		return *this;
	}

	C3Link & operator = (const QHash<QString,QVariant> &other)
	{
		QHash<QString,QVariant>::operator=(other);
		return *this;
	}

}; // class C3Link

#endif //!_C3Link_h_
