//=============================================================================
//
//   File : C3NewEmptyTextFileCreator.cpp
//   Creation Date : sab 07 nov 2015 06:00:52
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

#include "C3NewEmptyTextFileCreator.h"
#include "C3DockFileBrowser.h"
#include "C3TextEditor.h"
#include "C3Editor.h"

#include "C3Workspace.h"

C3NewEmptyTextFileCreator::C3NewEmptyTextFileCreator(
		const QString &szName,
		const QString &szIcon,
		const QString &szMode
	)
	: C3NewObjectCreator(),
	m_szName(szName),
	m_szIcon(szIcon),
	m_szMode(szMode)
{
}

C3NewEmptyTextFileCreator::~C3NewEmptyTextFileCreator()
{
}

QString C3NewEmptyTextFileCreator::name()
{
	return m_szName;
}

QString C3NewEmptyTextFileCreator::icon()
{
	return m_szIcon;
}

void C3NewEmptyTextFileCreator::run()
{
	C3Link oLink;
	oLink.setEditorFactoryId("text");
	C3Editor * pEd = C3Workspace::currentWorkspace()->newFile(oLink);
	if(!pEd)
		return;
	C3TextEditor * pTextEditor = dynamic_cast<C3TextEditor *>(pEd);
	if(!pTextEditor)
		return;
	pTextEditor->setMode(m_szMode);
	pEd->saveAs();
	if(C3DockFileBrowser::instance())
		C3DockFileBrowser::instance()->reload();
}