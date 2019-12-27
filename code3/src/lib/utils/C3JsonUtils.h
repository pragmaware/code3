#ifndef _C3JsonUtils_h_
#define _C3JsonUtils_h_
//=============================================================================
//
//   File : C3JsonUtils.h
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


#include "c3_config.h"

#include <QHash>
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>

///
/// \class C3JsonUtils
/// \brief The C3JsonUtils class
///
/// This class...
///
namespace C3JsonUtils
{
	// QJsonObject::toVariantHash() has been introduced in Qt 5.5
	QHash<QString,QVariant> jsonObjectToVariantHash(const QJsonObject &ob);

	// QJsonObject::fromVariantHash() has been introduced in Qt 5.5
	QJsonObject variantHashToJsonObject(const QHash<QString,QVariant> &hHash);

	QHash<QString,QString> jsonObjectToStringHash(const QJsonObject &ob);
	QJsonObject stringHashToJsonObject(const QHash<QString,QString> &hHash);

	// QJsonArray::toStringList() has been introduced in Qt 5.5
	QStringList jsonArrayToStringList(const QJsonArray &a);

	// QJsonArray::fromStringList() has been introduced in Qt 5.5
	QJsonArray stringListToJsonArray(const QStringList &s);

} // class C3JsonUtils

#endif //!_C3JsonUtils_h_