//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetExternalCommands.cpp
//   Creation Date : 2015/11/19 23:28:02
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

#include "C3WorkspacePropertiesWidgetExternalCommands.h"

#include "C3OptionsWidgetContainer.h"
#include "C3ExternalCommandEditorWidget.h"
#include "C3Workspace.h"

#include <QGridLayout>

typedef QPair<C3ExternalCommandEditorWidget *,C3ExternalCommand *> WidgetAndCommand;

class C3WorkspacePropertiesWidgetExternalCommandsPrivate
{
public:
	C3OptionsWidgetContainer * pContainer;
};

#define _p m_pC3WPWEC

C3WorkspacePropertiesWidgetExternalCommands::C3WorkspacePropertiesWidgetExternalCommands(QWidget * pParent)
	: C3OptionsWidget(__tr("External Commands"),pParent)
{
	_p = new C3WorkspacePropertiesWidgetExternalCommandsPrivate();
	
	_p->pContainer = new C3OptionsWidgetContainer(this,C3OptionsWidgetContainer::SupportAddAndRemove);
	
	QObject::connect(_p->pContainer,SIGNAL(addButtonClicked()),this,SLOT(slotAddButtonClicked()));
	QObject::connect(_p->pContainer,SIGNAL(removeButtonClicked()),this,SLOT(slotRemoveButtonClicked()));
	
	gridLayout()->addWidget(_p->pContainer,0,0,1,4);
	
	const QList<C3ExternalCommand *> & lCommands = C3Workspace::currentWorkspace()->externalCommands();
	foreach(C3ExternalCommand * cmd,lCommands)
	{
		C3ExternalCommandEditorWidget * pWidget = new C3ExternalCommandEditorWidget(_p->pContainer->parentForWidgets(),*cmd);
		_p->pContainer->addWidget(pWidget);
	}
}

C3WorkspacePropertiesWidgetExternalCommands::~C3WorkspacePropertiesWidgetExternalCommands()
{
	delete _p;
}

bool C3WorkspacePropertiesWidgetExternalCommands::commit()
{
	if(!_p->pContainer->commit())
		return false;
	
	QList<C3ExternalCommand *> lCommands;

	QList<C3OptionsWidget *> lWidgets;
	
	 _p->pContainer->gatherWidgets(lWidgets);

	foreach(C3OptionsWidget * w,lWidgets)
	{
		C3ExternalCommandEditorWidget * ww = dynamic_cast<C3ExternalCommandEditorWidget *>(w);
		if(!ww)
			continue;
		lCommands.append(new C3ExternalCommand(ww->command()));
	}

	C3Workspace::currentWorkspace()->setExternalCommands(lCommands);
	return true;
}


void C3WorkspacePropertiesWidgetExternalCommands::slotAddButtonClicked()
{
	C3ExternalCommand oCommand;
	oCommand.setName(__tr("unnamed"));
	C3ExternalCommandEditorWidget * pWidget = new C3ExternalCommandEditorWidget(_p->pContainer->parentForWidgets(),oCommand);
	_p->pContainer->addWidget(pWidget);
}

void C3WorkspacePropertiesWidgetExternalCommands::slotRemoveButtonClicked()
{
	C3OptionsWidget * w = _p->pContainer->currentWidget();
	if(!w)
		return;
	_p->pContainer->removeWidget(w);
}