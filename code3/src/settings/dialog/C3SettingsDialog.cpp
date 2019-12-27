//=============================================================================
//
//   File : C3SettingsDialog.cpp
//   Creation Date : mer 11 nov 2015 01:08:36
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

#include "C3SettingsDialog.h"

#include "C3SettingsWidgetUser.h"
#include "C3SettingsWidgetFileHeader.h"
#include "C3Settings.h"

#include "C3SettingsUserHighlightingSet.h"
#include "C3SettingsUserHighlightingSetOptionsWidget.h"

#include "C3Application.h"
#include "C3EditorFactory.h"

#include "C3OptionsWidgetDescriptor.h"
#include "C3Workspace.h"

#include <QShowEvent>

class C3SettingsDialogPrivate
{
public:

};

#define _p m_pC3SD

C3SettingsDialog::C3SettingsDialog()
	: C3OptionsDialog(__tr("C3 Settings"),0,true)
{
	_p = new C3SettingsDialogPrivate();

	addWidget(
			__tr("User"),
			[](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3SettingsWidgetUser(szLabel,pParent);
			}
		);
	
	C3OptionsWidgetDescriptor * pEditorOptions = addWidget(
			__tr("Editor"),
			[](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3OptionsWidget(szLabel,pParent);
			}
		);
	
	pEditorOptions->setId(__ascii("EditorOptions"));
	
	C3OptionsWidgetDescriptor * pTextEditorOptions = addWidget(
			pEditorOptions,
			__tr("Text"),
			[](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3OptionsWidget(szLabel,pParent);
			}
		);

	pTextEditorOptions->setId(__ascii("TextEditorOptions"));

	addWidget(
			pTextEditorOptions,
			__tr("File Header"),
			[](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3SettingsWidgetFileHeader(szLabel,pParent);
			}
		);

	addWidget(
			pTextEditorOptions,
			__tr("Highlighting"),
			[](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3SettingsUserHighlightingSetOptionsWidget(szLabel,pParent,C3Settings::instance()->userHighlightingSet());
			}
		);

	addWidget(
			pTextEditorOptions,
			__tr("File Exclusion"),
			[](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				C3OptionsWidget * pWidget = new C3OptionsWidget(szLabel,pParent);
				pWidget->addStringListEditor(
						__tr("File Exclusion Patterns.<br><br>The are used when scanning the paths for symbols or grepping in files. Can contain * and ? wildcards and will be matched against the base name of the file. To exclude directories append a trailing slash."),
						C3Settings::instance()->fileExclusionPatternsOption()
					);
				return pWidget;
			}
		);


	QList<C3EditorFactory *> & lFactories = C3Application::instance()->editorFactories();
	
	foreach(C3EditorFactory * pFactory,lFactories)
		pFactory->addWidgetsToSettingsDialog(this);
		
	
	setMinimumSize(QSize(780,580));
}

C3SettingsDialog::~C3SettingsDialog()
{
	delete _p;

}

bool C3SettingsDialog::commit()
{
	if(!C3OptionsDialog::commit())
		return false;
	
	C3Settings * set = C3Settings::instance();
	
	set->saveRecent(); // save recent too
	set->save();
	
	QList<C3EditorFactory *> & lFactories = C3Application::instance()->editorFactories();
	
	foreach(C3EditorFactory * pFactory,lFactories)
		pFactory->saveSettings();

	C3Workspace::currentWorkspace()->generalSettingsChanged();
	
	return true;
}
