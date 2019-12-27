#ifndef _C3NewJavaClassDialog_h_
#define _C3NewJavaClassDialog_h_
//=============================================================================
//
//   File : C3NewJavaClassDialog.h
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

class C3NewJavaClassDialogPrivate;

///
/// \class C3NewJavaClassDialog
/// \brief The C3NewJavaClassDialog class
///
/// This class...
///
class C3NewJavaClassDialog : public C3OptionsDialog
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3NewJavaClassDialog
	///
	C3NewJavaClassDialog();

	///
	/// Destroys the instance of C3NewJavaClassDialog
	/// and frees all the relevant resources
	///
	virtual ~C3NewJavaClassDialog();

private:

	C3NewJavaClassDialogPrivate * m_pC3NJavaCD;

protected:

	virtual bool commit();

	bool isValid();

protected:

	virtual void showEvent(QShowEvent * e);

private:

	void enableDisableButtons();

	void enableDisableControlsRelatedToClassType();

	void updateJavaFileHeader();

	QString guessPackage();

private slots:
	void slotPackageEdited();
	void slotClassNameEdited();
	void slotBaseClassNameEdited();
	void slotClassTypeEdited();
	void slotJavaFilePathEdited();
	void slotJavaFilePreviewAboutToShow();

}; // class C3NewJavaClassDialog

#endif //!_C3NewJavaClassDialog_h_