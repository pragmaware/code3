#ifndef _C3SettingsWidgetUser_h_
#define _C3SettingsWidgetUser_h_
//=============================================================================
//
//   File : C3SettingsWidgetUser.h
//   Creation Date : mer 11 nov 2015 01:44:42
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

class C3SettingsWidgetUserPrivate;

///
/// \class C3SettingsWidgetUser
/// \brief The C3SettingsWidgetUser class
///
/// This class...
///
class C3SettingsWidgetUser : public C3OptionsWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3SettingsWidgetUser
	///
	C3SettingsWidgetUser(const QString &szLabel,QWidget * pParent);

	///
	/// Destroys the instance of C3SettingsWidgetUser
	/// and frees all the relevant resources
	///
	virtual ~C3SettingsWidgetUser();

private:

	C3SettingsWidgetUserPrivate * m_pC3SWU;

}; // class C3SettingsWidgetUser

#endif //!_C3SettingsWidgetUser_h_
