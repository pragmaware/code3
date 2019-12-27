//=============================================================================
//
//   File : C3EditorFactory.cpp
//   Creation Date : mar 20 ott 2015 03:29:13
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

#include "C3EditorFactory.h"
#include "C3SettingsDialog.h"

C3EditorFactory::C3EditorFactory(
		const QString &szId,
		const QString &szEditorName
	) : m_szId(szId),
	m_szEditorName(szEditorName)
{	
}

C3EditorFactory::~C3EditorFactory()
{
	qDeleteAll(m_oFormatMap);
}

void C3EditorFactory::addWidgetsToSettingsDialog(C3SettingsDialog *)
{
}

void C3EditorFactory::saveSettings()
{
}

void C3EditorFactory::settingsChanged()
{
}
