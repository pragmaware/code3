//=============================================================================
//
//   File : C3JsonUtils.cpp
//   Creation Date : ven 13 nov 2015 22:38:43
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

#include "C3JsonUtils.h"

namespace C3JsonUtils
{
	QHash<QString,QVariant> jsonObjectToVariantHash(const QJsonObject &ob)
	{
		QHash<QString,QVariant> hHash;
	
		for(QJsonObject::const_iterator it = ob.begin();it != ob.end();++it)
		{
			QJsonValue val2 = it.value();
			hHash.insert(it.key(),val2.toVariant());
		}
		
		return hHash;
	}
	
	QJsonObject variantHashToJsonObject(const QHash<QString,QVariant> &hHash)
	{
		QJsonObject ob;
		
		for(QHash<QString,QVariant>::const_iterator it = hHash.begin();it != hHash.end();++it)
		{
			QVariant v = it.value();
			ob.insert(it.key(),QJsonValue::fromVariant(v));
		}
		
		return ob;
	}

	QHash<QString,QString> jsonObjectToStringHash(const QJsonObject &ob)
	{
		QHash<QString,QString> hHash;
	
		for(QJsonObject::const_iterator it = ob.begin();it != ob.end();++it)
		{
			QJsonValue val2 = it.value();
			hHash.insert(it.key(),val2.toVariant().toString());
		}
		
		return hHash;
	}
	
	QJsonObject stringHashToJsonObject(const QHash<QString,QString> &hHash)
	{
		QJsonObject ob;
		
		for(QHash<QString,QString>::const_iterator it = hHash.begin();it != hHash.end();++it)
			ob.insert(it.key(),QJsonValue(it.value()));
		
		return ob;
	}

	QStringList jsonArrayToStringList(const QJsonArray &a)
	{
		QStringList sl;
		
		for(QJsonArray::const_iterator it = a.begin();it != a.end();++it)
		{
			QJsonValue val = *it;
			sl.append(val.toVariant().toString());
		}
		
		return sl;
	}

	QJsonArray stringListToJsonArray(const QStringList &s)
	{
		QJsonArray a;
		foreach(QString ss,s)
			a.append(QJsonValue(ss));
		return a;
	}

}
