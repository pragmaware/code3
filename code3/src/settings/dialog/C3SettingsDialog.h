#ifndef _C3SettingsDialog_h_
#define _C3SettingsDialog_h_
//=============================================================================
//
//   File : C3SettingsDialog.h
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


#include "c3_config.h"

#include "C3OptionsDialog.h"

class C3SettingsDialogPrivate;

///
/// \class C3SettingsDialog
/// \brief The C3SettingsDialog class
///
/// This class...
///
class C3SettingsDialog : public C3OptionsDialog
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3SettingsDialog
	///
	C3SettingsDialog();

	///
	/// Destroys the instance of C3SettingsDialog
	/// and frees all the relevant resources
	///
	virtual ~C3SettingsDialog();

private:

	C3SettingsDialogPrivate * m_pC3SD;

protected:
	virtual bool commit();

}; // class C3SettingsDialog

#endif //!_C3SettingsDialog_h_
