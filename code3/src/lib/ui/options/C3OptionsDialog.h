#ifndef _C3OptionsDialog_h_
#define _C3OptionsDialog_h_
//=============================================================================
//
//   File : C3OptionsDialog.h
//   Creation Date : ven 06 nov 2015 03:31:13
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

#include <functional>

class C3OptionsDialogPrivate;
class C3OptionsWidget;
class C3OptionsWidgetDescriptor;
class QTreeWidgetItem;

///
/// \class C3OptionsDialog
/// \brief The C3OptionsDialog class
///
/// This class...
///
class C3OptionsDialog : public QDialog
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionsDialog
	///
	C3OptionsDialog(
			const QString &szTitle,
			unsigned int uWidgetContainerFlags = 0,
			bool bSupportApply = false
		);

	///
	/// Destroys the instance of C3OptionsDialog
	/// and frees all the relevant resources
	///
	virtual ~C3OptionsDialog();

private:

	C3OptionsDialogPrivate * m_pC3OD;

public:

	QWidget * parentForWidgets();

	C3OptionsWidgetDescriptor * addWidget(const QString &szLabel,std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate);
	C3OptionsWidgetDescriptor * addWidget(C3OptionsWidgetDescriptor * pParent,const QString &szLabel,std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate);
	C3OptionsWidgetDescriptor * addWidget(C3OptionsWidget * pExisting);
	C3OptionsWidgetDescriptor * addWidget(C3OptionsWidgetDescriptor * pParent,C3OptionsWidget * pExisting);

	C3OptionsWidgetDescriptor * findDescriptor(const QString &szId);

	void activateWidget(C3OptionsWidget * pWidget);

protected:

	virtual bool commit();

	QPushButton * OKButton();

private slots:

	void slotOKClicked();
	void slotApplyClicked();
	void slotCancelClicked();

}; // class C3OptionsDialog

#endif //!_C3OptionsDialog_h_
