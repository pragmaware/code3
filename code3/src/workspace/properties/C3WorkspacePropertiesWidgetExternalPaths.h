#ifndef _C3WorkspacePropertiesWidgetExternalPaths_h_
#define _C3WorkspacePropertiesWidgetExternalPaths_h_
//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetExternalPaths.h
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

#include "c3_config.h"


#include "C3OptionsWidget.h"

class C3WorkspacePropertiesWidgetExternalPaths : public C3OptionsWidget
{
	Q_OBJECT
public:
	C3WorkspacePropertiesWidgetExternalPaths(QWidget * pParent);
	virtual ~C3WorkspacePropertiesWidgetExternalPaths();

}; // class C3WorkspacePropertiesWidgetExternalPaths

#endif //!_C3WorkspacePropertiesWidgetExternalPaths_h_
