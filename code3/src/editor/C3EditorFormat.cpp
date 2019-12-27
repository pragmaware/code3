//=============================================================================
//
//   File : C3EditorFormat.cpp
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

#include "C3EditorFormat.h"

C3EditorFormat::C3EditorFormat(
		const QString &szId,
		const QString &szName,
		const QString &szFileNamePattern
	)
	: m_szId(szId),
	m_szName(szName),
	m_szFileNamePattern(szFileNamePattern)
{
}

C3EditorFormat::~C3EditorFormat()
{
}

QString C3EditorFormat::fileDialogNameFilter() const
{
	return __utf8("%1 (%2)").arg(m_szName).arg(m_szFileNamePattern);
}

