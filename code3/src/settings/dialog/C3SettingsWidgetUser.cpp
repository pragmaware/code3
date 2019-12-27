//=============================================================================
//
//   File : C3SettingsWidgetUser.cpp
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

#include "C3SettingsWidgetUser.h"

#include "C3Settings.h"

class C3SettingsWidgetUserPrivate
{
public:

};

#define _p m_pC3SWU

C3SettingsWidgetUser::C3SettingsWidgetUser(const QString &szLabel,QWidget * pParent)
	: C3OptionsWidget(szLabel,pParent)
{
	_p = new C3SettingsWidgetUserPrivate();

	C3Settings * pOptions = C3Settings::instance();

	addStringEditor(__tr("First Name"),pOptions->userFirstNameOption());
	addStringEditor(__tr("Last Name"),pOptions->userLastNameOption());
	addStringEditor(__tr("E-Mail"),pOptions->userEMailOption());
	
	addSpaceEater();
}

C3SettingsWidgetUser::~C3SettingsWidgetUser()
{
	delete _p;
}
