#ifndef _C3StringUtils_h_
#define _C3StringUtils_h_
//=============================================================================
//
//   File : C3StringUtils.h
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


#include "c3_config.h"

#include <QString>

///
/// \class C3StringUtils
/// \brief The C3StringUtils class
///
/// This class...
///
namespace C3StringUtils
{
	QString fileNameFromPath(const QString &szPath);

	QString fileDirectoryFromPath(const QString &szPath);

	QString fileExtensionFromPath(const QString &szPath);

	QString prefixLines(const QString &szPrefix,const QString &szText);
	
	QString indentLines(const QString &szText);
	
	int commonCharacterCount(const QString &sz1,const QString &sz2);

} // namespace C3StringUtils

#endif //!_C3StringUtils_h_