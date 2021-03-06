#ifndef _C3TextEditorModeCPPMemberFunctionGeneratorDialog_h_
#define _C3TextEditorModeCPPMemberFunctionGeneratorDialog_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPMemberFunctionGeneratorDialog.h
//   Creation Date : 2016/09/23 03:13:59
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

#include "C3OptionsDialog.h"

#include "c3_config.h"

#include "C3TextEditorModeCPPCodeEditDialog.h"

class C3TextEditor;
class C3SymbolManagerRequest;
class C3TextEditorModeCPPMemberFunctionGeneratorDialogPrivate;

class C3TextEditorModeCPPMemberFunctionGeneratorDialog : public C3TextEditorModeCPPCodeEditDialog
{
	Q_OBJECT
public:
	C3TextEditorModeCPPMemberFunctionGeneratorDialog(C3TextEditor * pEditor);
	virtual ~C3TextEditorModeCPPMemberFunctionGeneratorDialog();

private:
	C3TextEditorModeCPPMemberFunctionGeneratorDialogPrivate * m_pC3TEMCPPMGD;

protected:

	virtual bool commit();

	virtual void showEvent(QShowEvent * e);

	void enableDisableButtons();
	bool isValid();

	virtual void onContextIdentified(C3SymbolManagerSymbolDescription * pContextSymbol);

private slots:
	void slotReturnTypeEdited();
	void slotFunctionNameEdited();


}; // class C3TextEditorModeCPPMemberFunctionGeneratorDialog

#endif //!_C3TextEditorModeCPPMemberFunctionGeneratorDialog_h_