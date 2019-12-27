//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetExternalPaths.cpp
//   Creation Date : 2015/11/15 05:28:15
//   Project : 
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

#include "C3WorkspacePropertiesWidgetExternalPaths.h"

#include "C3Workspace.h"

C3WorkspacePropertiesWidgetExternalPaths::C3WorkspacePropertiesWidgetExternalPaths(QWidget * pParent)
	: C3OptionsWidget(__tr("External Paths"),pParent)
{
	C3Workspace * w = C3Workspace::currentWorkspace();

	addStringListEditor(
			__tr("A list of additional paths to be scanned for file completions and tags. Use one path per line."),
			w->includePathsOption()
		);
}

C3WorkspacePropertiesWidgetExternalPaths::~C3WorkspacePropertiesWidgetExternalPaths()
{
}
