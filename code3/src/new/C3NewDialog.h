#ifndef _C3NewDialog_h_
#define _C3NewDialog_h_
//=============================================================================
//
//   File : C3NewDialog.h
//   Creation Date : sab 07 nov 2015 05:43:05
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

#include <QDialog>

class C3NewDialogPrivate;

class QListWidgetItem;

///
/// \class C3NewDialog
/// \brief The C3NewDialog class
///
/// This class...
///
class C3NewDialog : public QDialog
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3NewDialog
	///
	C3NewDialog();

	///
	/// Destroys the instance of C3NewDialog
	/// and frees all the relevant resources
	///
	virtual ~C3NewDialog();

private:

	C3NewDialogPrivate * m_pC3ND;

private:

	void enableDisableButtons();
	void fillListWidget();
	void fillCreatorHash();

private slots:

	void slotCurrentItemChanged(QListWidgetItem * current,QListWidgetItem * previous);
	void slotItemDoubleClicked(QListWidgetItem *it);
	void slotOkClicked();
	void slotCancelClicked();
	
}; // class C3NewDialog

#endif //!_C3NewDialog_h_
