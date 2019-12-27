#ifndef _C3TextEditorModeCPPAddIncludeDialog_h_
#define _C3TextEditorModeCPPAddIncludeDialog_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPAddIncludeDialog.h
//   Creation Date : 2016/09/24 02:27:13
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

#include "C3TextEditorModeCPPCodeEditDialog.h"

#include <QList>

class QListWidgetItem;

class C3TextEditorModeCPPAddIncludeDialogPrivate;

class C3TextEditorModeCPPAddIncludeDialog : public C3TextEditorModeCPPCodeEditDialog
{
	Q_OBJECT
public:
	C3TextEditorModeCPPAddIncludeDialog(C3TextEditor * pEditor);
	virtual ~C3TextEditorModeCPPAddIncludeDialog();

private:
	C3TextEditorModeCPPAddIncludeDialogPrivate * m_pC3TEMCPPAID;

protected:

	virtual void showEvent(QShowEvent * e);
	virtual void onSymbolIdentified(QList<C3TextEditorSymbolInfo *> * pInfoList);
	virtual bool commit();

private:
	void enableDisableButtons();

protected slots:

	void slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *);

}; // class C3TextEditorModeCPPAddIncludeDialog

#endif //!_C3TextEditorModeCPPAddIncludeDialog_h_