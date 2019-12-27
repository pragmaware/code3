//=============================================================================
//
//   File : C3WorkspacePropertiesDialog.cpp
//   Creation Date : ven 06 nov 2015 04:01:27
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

#include "C3WorkspacePropertiesDialog.h"

#include "C3WorkspacePropertiesWidgetGeneral.h"
#include "C3WorkspacePropertiesWidgetFileHeader.h"
#include "C3WorkspacePropertiesWidgetExternalPaths.h"
#include "C3WorkspacePropertiesWidgetExternalCommands.h"
#include "C3SettingsUserHighlightingSetOptionsWidget.h"

#include "C3Workspace.h"

class C3WorkspacePropertiesDialogPrivate
{
public:

};

#define _p m_pC3WPD

C3WorkspacePropertiesDialog::C3WorkspacePropertiesDialog()
	: C3OptionsDialog(__tr("Workspace Properties"))
{
	_p = new C3WorkspacePropertiesDialogPrivate();

	addWidget(new C3WorkspacePropertiesWidgetGeneral(parentForWidgets()));
	addWidget(new C3WorkspacePropertiesWidgetExternalPaths(parentForWidgets()));

	C3OptionsWidget * pWidget = new C3OptionsWidget(__tr("File Exclusion"),parentForWidgets());
	pWidget->addStringListEditor(__tr("File Exclusion Patterns.<br><br>The are used when scanning the paths for symbols or grepping in files. Can contain * and ? wildcards and will be matched against the base name of the file. To exclude directories append a trailing slash."),C3Workspace::currentWorkspace()->fileExclusionPatternsOption());
	addWidget(pWidget);

	addWidget(new C3WorkspacePropertiesWidgetExternalCommands(parentForWidgets()));
	addWidget(new C3WorkspacePropertiesWidgetFileHeader(parentForWidgets()));
	addWidget(new C3SettingsUserHighlightingSetOptionsWidget(__tr("Highlighting"),parentForWidgets(),C3Workspace::currentWorkspace()->userHighlightingSet()));
}

C3WorkspacePropertiesDialog::~C3WorkspacePropertiesDialog()
{
	delete _p;
}