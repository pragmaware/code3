//=============================================================================
//
//   File : C3TextEditorMode.cpp
//   Creation Date : sab 24 ott 2015 23:38:14
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

#include "C3TextEditorMode.h"
#include "C3TextEditorRowColumn.h"
#include "C3TextEditorFactory.h"

#include "C3Workspace.h"

#include <QMenu>
#include <QKeyEvent>

C3TextEditorMode::C3TextEditorMode(C3TextEditorFactory * pFactory,const QString &szId,const QString &szName)
	: QObject(), m_pFactory(pFactory), m_szId(szId), m_szName(szName)
{
}

C3TextEditorMode::~C3TextEditorMode()
{
}

void C3TextEditorMode::triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	// do nothing
}

bool C3TextEditorMode::triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	return false;
}

void C3TextEditorMode::fillCodeMenu(QMenu * pMode)
{
	// do nothing
}

bool C3TextEditorMode::handleKeyEvent(QKeyEvent * e)
{
	return false;
}

void C3TextEditorMode::fileSaved(bool bWasModified)
{
	// do nothing
}
