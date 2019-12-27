#ifndef _C3WorkspacePropertiesWidgetGeneral_h_
#define _C3WorkspacePropertiesWidgetGeneral_h_
//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetGeneral.h
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


#include "c3_config.h"

#include "C3OptionsWidget.h"

class C3WorkspacePropertiesWidgetGeneralPrivate;

///
/// \class C3WorkspacePropertiesWidgetGeneral
/// \brief The C3WorkspacePropertiesWidgetGeneral class
///
/// This class...
///
class C3WorkspacePropertiesWidgetGeneral : public C3OptionsWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3WorkspacePropertiesWidgetGeneral
	///
	C3WorkspacePropertiesWidgetGeneral(QWidget * pParent);

	///
	/// Destroys the instance of C3WorkspacePropertiesWidgetGeneral
	/// and frees all the relevant resources
	///
	virtual ~C3WorkspacePropertiesWidgetGeneral();

private:

	C3WorkspacePropertiesWidgetGeneralPrivate * m_pC3WPWG;


}; // class C3WorkspacePropertiesWidgetGeneral

#endif //!_C3WorkspacePropertiesWidgetGeneral_h_
