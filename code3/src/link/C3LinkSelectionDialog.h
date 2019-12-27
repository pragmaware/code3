#ifndef _C3LinkSelectionDialog_h_
#define _C3LinkSelectionDialog_h_
//=============================================================================
//
//   File : C3LinkSelectionDialog.h
//   Creation Date : 2015/11/25 18:17:24
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

#include <QDialog>

#include <QList>

#include "C3Link.h"

class QListWidgetItem;


class C3LinkSelectionDialogPrivate;

class C3LinkSelectionDialog : public QDialog
{
	Q_OBJECT
public:
	C3LinkSelectionDialog(
			QWidget * pParent,
			const QString &szTitle,
			const QList<C3Link> &lLinks
		);
	virtual ~C3LinkSelectionDialog();

private:
	C3LinkSelectionDialogPrivate * m_pC3LSD;

public:

	const C3Link & selectedLink();

private:

	void displayLinks();
	void enableDisableButtons();

private slots:

	void slotOKClicked();
	void slotCancelClicked();
	void slotItemDoubleClicked(QListWidgetItem * it);
	void slotCurrentItemChanged(QListWidgetItem * cur,QListWidgetItem * prev);
	
}; // class C3LinkSelectionDialog

#endif //!_C3LinkSelectionDialog_h_