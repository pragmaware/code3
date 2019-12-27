//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetGeneral.cpp
//   Creation Date : mer 11 nov 2015 03:11:37
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

#include "C3WorkspacePropertiesWidgetGeneral.h"

#include "C3Workspace.h"

class C3WorkspacePropertiesWidgetGeneralPrivate
{
public:

};

#define _p m_pC3WPWG

C3WorkspacePropertiesWidgetGeneral::C3WorkspacePropertiesWidgetGeneral(QWidget * pParent)
	: C3OptionsWidget(__tr("General"),pParent)
{
	_p = new C3WorkspacePropertiesWidgetGeneralPrivate();

	C3Workspace * w = C3Workspace::currentWorkspace();

	addStringEditor(__tr("Project Name"),w->projectNameOption());
	
	addSpaceEater();
}

C3WorkspacePropertiesWidgetGeneral::~C3WorkspacePropertiesWidgetGeneral()
{
	delete _p;
}

