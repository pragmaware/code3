#ifndef _C3PixmapCache_h_
#define _C3PixmapCache_h_
//=============================================================================
//
//   File : C3PixmapCache.h
//   Creation Date : mar 20 ott 2015 02:35:33
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

#include <QPixmap>

class C3PixmapCachePrivate;

///
/// \class C3PixmapCache
/// \brief The C3PixmapCache class
///
/// This class...
///
class C3PixmapCache
{
public:

	///
	/// Creates an instance of C3PixmapCache
	///
	C3PixmapCache();

	///
	/// Destroys the instance of C3PixmapCache
	/// and frees all the relevant resources
	///
	~C3PixmapCache();

private:

	static C3PixmapCache * m_pInstance;

	C3PixmapCachePrivate * m_pC3PC;

public:

	static C3PixmapCache * instance()
	{
		return m_pInstance;
	}

	const QPixmap & pixmapCompletionNamespace() const;
	const QPixmap & pixmapCompletionClass() const;
	const QPixmap & pixmapCompletionFunction() const;
	const QPixmap & pixmapCompletionIncludeFile() const;
	const QPixmap & pixmapCompletionEnumeration() const;
	const QPixmap & pixmapCompletionEnumerationMember() const;
	const QPixmap & pixmapCompletionLocalVariable() const;
	const QPixmap & pixmapCompletionGlobalVariable() const;
	const QPixmap & pixmapCompletionMemberVariable() const;
	const QPixmap & pixmapCompletionMacro() const;
	const QPixmap & pixmapCompletionKeyword() const;
	const QPixmap & pixmapCompletionTypeDefinition() const;

	const QPixmap & pixmapCompletionOther() const;

	const QPixmap & pixmap(const QString &szName);

private:

	QPixmap * tryLoadImage(const QString &szPath,const QString &szName);


}; // class C3PixmapCache

#endif //!_C3PixmapCache_h_
