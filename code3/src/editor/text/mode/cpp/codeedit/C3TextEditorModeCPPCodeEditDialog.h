#ifndef _C3TextEditorModeCPPCodeEditDialog_h_
#define _C3TextEditorModeCPPCodeEditDialog_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPCodeEditDialog.h
//   Creation Date : 2016/09/22 00:16:30
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
#include "C3SymbolManagerRequestIdentifyContext.h"
#include "C3SymbolManagerSymbolDescription.h"
#include "C3TextEditorSymbolInfo.h"
#include "C3Symbol.h"

#include <QList>

class C3TextEditor;
class C3SymbolManagerRequest;
class C3TextEditorModeCPPCodeEditDialogPrivate;

class C3TextEditorModeCPPCodeEditDialog : public C3OptionsDialog
{
	Q_OBJECT
public:
	enum IdentificationFlags
	{
		ScanEditor = 1,
		IdentifyContextAtCursor = 2, // implies ScanEditor
		IdentifySymbolAtCursor = 4, // implies ScanEditor
	};
public:
	C3TextEditorModeCPPCodeEditDialog(
			const QString &szTitle,
			C3TextEditor * pEditor,
			unsigned int uIdentificationFlags = IdentifyContextAtCursor,
			unsigned int uWidgetContainerFlags = 0
		);
	virtual ~C3TextEditorModeCPPCodeEditDialog();

private:
	C3TextEditorModeCPPCodeEditDialogPrivate * m_pC3TEMCPPCMD;

protected:

	C3TextEditor * editor();

	virtual void onContextIdentificationFailed();
	// Ownership of pInfoList is transferred to the called function!
	virtual void onContextIdentified(C3SymbolManagerSymbolDescription * pContextSymbol);
	virtual void onAllTasksFinished();
	virtual void onSymbolIdentificationFailed();
	// Ownership of pInfoList is transferred to the called function!
	virtual void onSymbolIdentified(QList<C3TextEditorSymbolInfo *> * pInfoList);

private slots:
	void slotScanEditorFileRequestTerminated(C3SymbolManagerRequest * pReq);
	void slotIdentifyContextRequestTerminated(C3SymbolManagerRequest * pReq);
	void slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest * pReq);

private:
	void doRemainingTasks();

}; // class C3TextEditorModeCPPCodeEditDialog

#endif //!_C3TextEditorModeCPPCodeEditDialog_h_