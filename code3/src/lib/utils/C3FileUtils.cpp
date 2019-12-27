//=============================================================================
//
//   File : C3FileUtils.cpp
//   Creation Date : 2015/11/19 22:23:07
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

#include "C3FileUtils.h"

#include <QtGlobal>
#include <QDir>

namespace C3FileUtils
{
	QString mergePathComponents(const QString &szC1,const QString &szC2)
	{
		QString szRet = szC1;
		if(szRet.endsWith(QDir::separator()))
			return szRet + szC2;
		if(szC2.startsWith(QDir::separator()))
			return szRet + szC2;
		return szRet + QDir::separator() + szC2;
	}
	
	QString mergePathComponents(const QString &szC1,const QString &szC2,const QString &szC3)
	{
		return mergePathComponents(mergePathComponents(szC1,szC2),szC3);
	}
	
	QString mergePathComponents(const QString &szC1,const QString &szC2,const QString &szC3,const QString &szC4)
	{
		return mergePathComponents(mergePathComponents(szC1,szC2,szC3),szC4);
	}
	
	QString mergePathComponents(const QString &szC1,const QString &szC2,const QString &szC3,const QString &szC4,const QString &szC5)
	{
		return mergePathComponents(mergePathComponents(szC1,szC2,szC3,szC4),szC5);
	}

	bool isAbsolutePath(const QString &szPath)
	{
		if(szPath.isEmpty())
			return false;
#ifdef Q_OS_WIN
		if(szPath.length() < 2)
			return false;
		if(szPath[0].isLetter() && (szPath[1] == QChar(':')))
			return true;
		return szPath.startsWith(QString("\\\\"));
#else
		return szPath.startsWith(QChar('/'));
#endif
	}

	bool createDirectory(const QString &szPath)
	{
		QDir d(szPath);
		if(d.exists())
			return true;
		return d.mkpath(".");
	}

} // namespace C3FileUtils
