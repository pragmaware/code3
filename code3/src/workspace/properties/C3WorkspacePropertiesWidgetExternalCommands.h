#ifndef _C3WorkspacePropertiesWidgetExternalCommands_h_
#define _C3WorkspacePropertiesWidgetExternalCommands_h_
//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetExternalCommands.h
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

#include "c3_config.h"

#include "C3OptionsWidget.h"

class C3WorkspacePropertiesWidgetExternalCommandsPrivate;

class C3WorkspacePropertiesWidgetExternalCommands : public C3OptionsWidget
{
	Q_OBJECT
public:
	C3WorkspacePropertiesWidgetExternalCommands(QWidget * pParent);
	virtual ~C3WorkspacePropertiesWidgetExternalCommands();

private:
	C3WorkspacePropertiesWidgetExternalCommandsPrivate * m_pC3WPWEC;

protected:

	virtual bool commit();

private slots:

	void slotAddButtonClicked();
	void slotRemoveButtonClicked();


}; // class C3WorkspacePropertiesWidgetExternalCommands

#endif //!_C3WorkspacePropertiesWidgetExternalCommands_h_