#ifndef _C3FileUtils_h_
#define _C3FileUtils_h_
//=============================================================================
//
//   File : C3FileUtils.h
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

#include "c3_config.h"

#include <QString>

namespace C3FileUtils
{
	QString mergePathComponents(const QString &szC1,const QString &szC2);
	QString mergePathComponents(const QString &szC1,const QString &szC2,const QString &szC3);
	QString mergePathComponents(const QString &szC1,const QString &szC2,const QString &szC3,const QString &szC4);
	QString mergePathComponents(const QString &szC1,const QString &szC2,const QString &szC3,const QString &szC4,const QString &szC5);

	bool isAbsolutePath(const QString &szPath);
	
	bool createDirectory(const QString &szPath);

} // namespace C3FileUtils

#endif //!_C3FileUtils_h_
