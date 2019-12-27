#ifndef _C3TextEditorModeCPPGenerateEnumSwitchDialog_h_
#define _C3TextEditorModeCPPGenerateEnumSwitchDialog_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPGenerateEnumSwitchDialog.h
//   Creation Date : 2016/10/15 04:03:54
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

#include "C3TextEditorModeCPPCodeEditDialog.h"

class QListWidgetItem;

class C3TextEditorModeCPPGenerateEnumSwitchDialogPrivate;

class C3TextEditorModeCPPGenerateEnumSwitchDialog : public C3TextEditorModeCPPCodeEditDialog
{
public:
	C3TextEditorModeCPPGenerateEnumSwitchDialog(C3TextEditor * pEditor);
	virtual ~C3TextEditorModeCPPGenerateEnumSwitchDialog();

private:
	C3TextEditorModeCPPGenerateEnumSwitchDialogPrivate * m_pC3TEMCPPGESD;

public:

	virtual void onAllTasksFinished();

private:
	
	void enableDisableButtons();

	virtual bool commit();
	virtual void slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *);

}; // class C3TextEditorModeCPPGenerateEnumSwitchDialog

#endif //!_C3TextEditorModeCPPGenerateEnumSwitchDialog_h_