#ifndef _C3EditorFormat_h_
#define _C3EditorFormat_h_
//=============================================================================
//
//   File : C3EditorFormat.h
//   Creation Date : ven 23 ott 2015 00:02:00
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
/// \class C3EditorFormat
/// \brief The C3EditorFormat class
///
/// This class...
///
class C3EditorFormat
{
public:

	///
	/// Creates an instance of C3EditorFormat
	///
	C3EditorFormat(
			const QString &szId,
			const QString &szName,
			const QString &szFileNamePattern
		);

	///
	/// Destroys the instance of C3EditorFormat
	/// and frees all the relevant resources
	///
	virtual ~C3EditorFormat();

protected:

	QString m_szId;
	QString m_szName;
	QString m_szFileNamePattern;

public:

	const QString & fileNamePattern() const
	{
		return m_szFileNamePattern;
	}

	const QString & id() const
	{
		return m_szId;
	}

	const QString & name() const
	{
		return m_szName;
	}

	QString fileDialogNameFilter() const;


}; // class C3EditorFormat

#endif //!_C3EditorFormat_h_
