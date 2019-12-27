//=============================================================================
//
//   File : C3TextEditorModeCPPCodeEditDialog.cpp
//   Creation Date : 2016/09/22 00:16:30
//   Project : ssex
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

#include "C3SymbolManagerRequestScanEditorFile.h"
#include "C3SymbolManagerRequestIdentifyContext.h"
#include "C3SymbolManagerRequestIdentifySymbol.h"
#include "C3SymbolManager.h"
#include "C3Workspace.h"

enum C3TextEditorModeCPPCodeEditDialogTask
{
	TaskScanEditor = 1,
	TaskIdentifyContext = 2,
	TaskIdentifySymbol = 4
};

class C3TextEditorModeCPPCodeEditDialogPrivate
{
public:
	C3TextEditor * pEditor;

	C3SymbolManagerRequestScanEditorFile * pScanEditorFileRequest;

	// IdentifyContextAtCursor
	C3SymbolManagerRequestIdentifyContext * pIdentifyContextRequest;
	int iIdentifyContextAtCursorPass;
	
	C3SymbolManagerRequestIdentifySymbol * pIdentifySymbolRequest;

	unsigned int uIdentificationFlags;
	
	unsigned int uRemainingTasks;
};

#define _p m_pC3TEMCPPCMD

C3TextEditorModeCPPCodeEditDialog::C3TextEditorModeCPPCodeEditDialog(
		const QString &szTitle,
		C3TextEditor * pEditor,
		unsigned int uIdentificationFlags,
		unsigned int uWidgetContainerFlags
	)
	: C3OptionsDialog(szTitle,uWidgetContainerFlags)
{
	_p = new C3TextEditorModeCPPCodeEditDialogPrivate();

	_p->pEditor = pEditor;
	_p->uIdentificationFlags = uIdentificationFlags;
	
	_p->uRemainingTasks = 0;
	
	if(uIdentificationFlags & ScanEditor)
		_p->uRemainingTasks |= TaskScanEditor;
	if(uIdentificationFlags & IdentifyContextAtCursor)
		_p->uRemainingTasks |= TaskScanEditor | TaskIdentifyContext;
	if(uIdentificationFlags & IdentifySymbolAtCursor)
		_p->uRemainingTasks |= TaskScanEditor | TaskIdentifySymbol;
	

	_p->pIdentifyContextRequest = NULL;
	_p->pIdentifySymbolRequest = NULL;
	_p->pScanEditorFileRequest = NULL;
	_p->iIdentifyContextAtCursorPass = 0;

	doRemainingTasks();
}

C3TextEditorModeCPPCodeEditDialog::~C3TextEditorModeCPPCodeEditDialog()
{
	if(_p->pScanEditorFileRequest)
		_p->pScanEditorFileRequest->abort();
	if(_p->pIdentifyContextRequest)
		_p->pIdentifyContextRequest->abort();
	if(_p->pIdentifySymbolRequest)
		_p->pIdentifySymbolRequest->abort();
	delete _p;
}

C3TextEditor * C3TextEditorModeCPPCodeEditDialog::editor()
{
	return _p->pEditor;
}

void C3TextEditorModeCPPCodeEditDialog::doRemainingTasks()
{
	if(_p->uRemainingTasks & TaskScanEditor)
	{
		_p->pScanEditorFileRequest = new C3SymbolManagerRequestScanEditorFile(_p->pEditor->id(),true);
		QObject::connect(_p->pScanEditorFileRequest,SIGNAL(terminated(C3SymbolManagerRequest *)),this,SLOT(slotScanEditorFileRequestTerminated(C3SymbolManagerRequest *)));
		_p->pScanEditorFileRequest->start();
		_p->uRemainingTasks &= ~TaskScanEditor;
		return;
	}

	if(_p->uRemainingTasks & TaskIdentifyContext)
	{
		_p->pIdentifyContextRequest = new C3SymbolManagerRequestIdentifyContext(
				_p->pEditor->path(),
				_p->pEditor->cursorRow(),
				C3Symbol::Cpp
			);
		QObject::connect(_p->pIdentifyContextRequest,SIGNAL(terminated(C3SymbolManagerRequest *)),this,SLOT(slotIdentifyContextRequestTerminated(C3SymbolManagerRequest *)));
		_p->pIdentifyContextRequest->start();
		_p->uRemainingTasks &= ~TaskIdentifyContext;
		return;
	}
	
	if(_p->uRemainingTasks & TaskIdentifySymbol)
	{
		C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

		_p->uRemainingTasks &= ~TaskIdentifySymbol;

		if(!
			_p->pEditor->computeSymbolContext(
					_p->pEditor->cursorPosition(),
					*pContext,
					C3TextEditor::ExtendSymbolToRight
				)
			)
		{
			delete pContext;
			onSymbolIdentificationFailed();
			doRemainingTasks();
			return;
		}

		_p->pIdentifySymbolRequest = new C3SymbolManagerRequestIdentifySymbol(
				pContext,
				C3Symbol::Cpp,
				0 // NoContextFilter
			);

		QObject::connect(_p->pIdentifySymbolRequest,SIGNAL(terminated(C3SymbolManagerRequest *)),this,SLOT(slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest *)));
		_p->pIdentifySymbolRequest->start();
		return;
	}
	
	onAllTasksFinished();
}

void C3TextEditorModeCPPCodeEditDialog::slotScanEditorFileRequestTerminated(C3SymbolManagerRequest * pReq)
{
	_p->pScanEditorFileRequest = NULL;

	doRemainingTasks();
}

void C3TextEditorModeCPPCodeEditDialog::slotIdentifyContextRequestTerminated(C3SymbolManagerRequest * pReq)
{
	C3SymbolManagerRequestIdentifyContext * pReq2 = dynamic_cast<C3SymbolManagerRequestIdentifyContext *>(pReq);
	Q_ASSERT(pReq2);

	C3SymbolManagerSymbolDescription * pDescription = _p->pIdentifyContextRequest->symbolDescription();

	_p->pIdentifyContextRequest = NULL;

	if(!pDescription)
	{
		onContextIdentificationFailed();
		doRemainingTasks();
		return;
	}

	if(!pDescription->pContainingClass)
	{
		pReq2->takeSymbolDescription();
		onContextIdentified(pDescription);
		doRemainingTasks();
		return;
	}

	if(_p->iIdentifyContextAtCursorPass > 0)
	{
		// already did it twice
		pReq2->takeSymbolDescription();
		onContextIdentified(pDescription);
		doRemainingTasks();
		return;
	}

	_p->iIdentifyContextAtCursorPass++;
	
	// Rescan the files for the containing class and do identification again, just to be sure
	// Eventually open the files too.

	C3TextEditor * pDeclarationEditor = dynamic_cast<C3TextEditor *>(
			C3Workspace::currentWorkspace()->findEditor(__ascii("text"),pDescription->pContainingClass->szDeclaratonFile)
		);
	C3TextEditor * pDefinitionEditor = dynamic_cast<C3TextEditor *>(
			C3Workspace::currentWorkspace()->findEditor(__ascii("text"),pDescription->pContainingClass->szDefinitionFile)
		);
		
	if(!pDeclarationEditor && !pDefinitionEditor)
	{
		// oops.. might have found the wrong class?
		// FIXME: fail?
	}

	// open the files
	if(!pDeclarationEditor)
	{
		C3Workspace::currentWorkspace()->openFileAsync(pDescription->pContainingClass->szDeclaratonFile,false,false);
	} else {
		if(pDeclarationEditor != _p->pEditor)
			(new C3SymbolManagerRequestScanEditorFile(pDeclarationEditor->id(),true))->start();
	}

	if(!pDefinitionEditor)
	{
		if(pDescription->pContainingClass->szDeclaratonFile != pDescription->pContainingClass->szDefinitionFile)
			C3Workspace::currentWorkspace()->openFileAsync(pDescription->pContainingClass->szDefinitionFile,false,false);
	} else {
		if((pDefinitionEditor != _p->pEditor) && (pDefinitionEditor != pDeclarationEditor))
			(new C3SymbolManagerRequestScanEditorFile(pDefinitionEditor->id(),true))->start();
	}

	// and do it again
	_p->uRemainingTasks |= TaskIdentifyContext;

	Q_ASSERT(!_p->pScanEditorFileRequest);
	_p->pScanEditorFileRequest = new C3SymbolManagerRequestScanEditorFile(_p->pEditor->id(),true);
	QObject::connect(_p->pScanEditorFileRequest,SIGNAL(terminated(C3SymbolManagerRequest *)),this,SLOT(slotScanEditorFileRequestTerminated(C3SymbolManagerRequest *)));
	_p->pScanEditorFileRequest->start();
}

void C3TextEditorModeCPPCodeEditDialog::slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest * pReq)
{
	QList<C3TextEditorSymbolInfo *> * pInfoList = _p->pIdentifySymbolRequest->takeInfoList();

	_p->pIdentifySymbolRequest = NULL;
	
	if(!pInfoList)
	{
		onSymbolIdentificationFailed();
	} else {
		onSymbolIdentified(pInfoList);
	}

	doRemainingTasks();
}

void C3TextEditorModeCPPCodeEditDialog::onSymbolIdentified(QList<C3TextEditorSymbolInfo *> * pInfoList)
{
	if(pInfoList)
		delete pInfoList;
}

void C3TextEditorModeCPPCodeEditDialog::onSymbolIdentificationFailed()
{
	qDebug("Symbol identification failed");
}

void C3TextEditorModeCPPCodeEditDialog::onContextIdentified(C3SymbolManagerSymbolDescription * pContextSymbol)
{
	if(pContextSymbol)
		delete pContextSymbol;
}

void C3TextEditorModeCPPCodeEditDialog::onContextIdentificationFailed()
{
	qDebug("Context identification failed");
}

void C3TextEditorModeCPPCodeEditDialog::onAllTasksFinished()
{

}
