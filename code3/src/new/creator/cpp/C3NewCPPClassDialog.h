#ifndef _C3NewCPPClassDialog_h_
#define _C3NewCPPClassDialog_h_
//=============================================================================
//
//   File : C3NewCPPClassDialog.h
//   Creation Date : mer 11 nov 2015 00:53:05
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

class C3NewCPPClassDialogPrivate;

///
/// \class C3NewCPPClassDialog
/// \brief The C3NewCPPClassDialog class
///
/// This class...
///
class C3NewCPPClassDialog : public C3OptionsDialog
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3NewCPPClassDialog
	///
	C3NewCPPClassDialog();

	///
	/// Destroys the instance of C3NewCPPClassDialog
	/// and frees all the relevant resources
	///
	virtual ~C3NewCPPClassDialog();

private:

	C3NewCPPClassDialogPrivate * m_pC3NCPPCD;

protected:

	virtual bool commit();

	bool isValid();

protected:

	virtual void showEvent(QShowEvent * e);

private:

	void enableDisableButtons();

	void enableDisableControlsRelatedToClassType();

	void updateCPPFileHeader();
	void updateHFileHeader();

private slots:
	void slotClassNameEdited();
	void slotBaseClassNameEdited();
	void slotClassTypeEdited();
	void slotCPPFilePathEdited();
	void slotHFilePathEdited();
	void slotHFilePreviewAboutToShow();
	void slotCPPFilePreviewAboutToShow();

}; // class C3NewCPPClassDialog

#endif //!_C3NewCPPClassDialog_h_