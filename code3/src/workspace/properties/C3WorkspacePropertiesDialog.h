#ifndef _C3WorkspacePropertiesDialog_h_
#define _C3WorkspacePropertiesDialog_h_
//=============================================================================
//
//   File : C3WorkspacePropertiesDialog.h
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


#include "c3_config.h"

#include "C3OptionsDialog.h"

class C3WorkspacePropertiesDialogPrivate;

///
/// \class C3WorkspacePropertiesDialog
/// \brief The C3WorkspacePropertiesDialog class
///
/// This class...
///
class C3WorkspacePropertiesDialog : public C3OptionsDialog
{
	Q_OBJECT
public:


	///
	/// Creates an instance of C3WorkspacePropertiesDialog
	///
	C3WorkspacePropertiesDialog();

	///
	/// Destroys the instance of C3WorkspacePropertiesDialog
	/// and frees all the relevant resources
	///
	virtual ~C3WorkspacePropertiesDialog();

private:

	C3WorkspacePropertiesDialogPrivate * m_pC3WPD;


}; // class C3WorkspacePropertiesDialog

#endif //!_C3WorkspacePropertiesDialog_h_