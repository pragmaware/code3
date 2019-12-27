#ifndef _C3NewObjectUtils_h_
#define _C3NewObjectUtils_h_
//=============================================================================
//
//   File : C3NewObjectUtils.h
//   Creation Date : gio 12 nov 2015 19:49:05
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
#include <QHash>

///
/// \class C3NewObjectUtils
/// \brief The C3NewObjectUtils class
///
/// This class...
///
namespace C3NewObjectUtils
{
	enum BuildHeaderCommentStyle
	{
		BuildHeaderCommentStyleNone,
		BuildHeaderCommentStyleCSingleLine,   // //
		BuildHeaderCommentStyleCMultiLine,    // */
		BuildHeaderCommentStyleShell          // #
	};

	QString buildFileHeader(
			const QString &szFilePath,
			BuildHeaderCommentStyle eCommentStyle
		);

} // namespace C3NewObjectUtils

#endif //!_C3NewObjectUtils_h_