//=============================================================================
//
//   File : C3TextEditorModeWithSymbolAnalysis.cpp
//   Creation Date : 2015/12/10 22:05:50
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

#include "C3TextEditorModeWithSymbolAnalysis.h"

#include "C3SymbolManagerRequestPerformCompletion.h"
#include "C3SymbolManagerRequestIdentifySymbol.h"
#include "C3SymbolManagerRequestScanEditorFile.h"

#include "C3TextEditor.h"

#include <QElapsedTimer>

class C3TextEditorModeWithSymbolAnalysisPrivate
{
public:
	C3SymbolManagerRequestPerformCompletion * pPerformCompletionRequest;
	C3SymbolManagerRequestIdentifySymbol * pIdentifySymbolRequest;
	C3SymbolManagerRequestScanEditorFile * pScanEditorFileRequest;
	C3Symbol::Language eLanguage;
	QElapsedTimer oLastFileRescan;
	int iSignificantChanges; // number of newlines changed
};

#define _p m_pTEMWSA


C3TextEditorModeWithSymbolAnalysis::C3TextEditorModeWithSymbolAnalysis(
		C3TextEditorFactory * pFactory,
		const QString &szId,
		const QString &szName,
		unsigned int uInterLineFlagMask,
		C3Symbol::Language eLanguage
	)
	: C3TextEditorModeWithInterLineState(
		pFactory,
		szId,
		szName,
		uInterLineFlagMask
	)
{
	_p = new C3TextEditorModeWithSymbolAnalysisPrivate();
	_p->pPerformCompletionRequest = NULL;
	_p->pIdentifySymbolRequest = NULL;
	_p->pScanEditorFileRequest = NULL;
	_p->eLanguage = eLanguage;
	_p->oLastFileRescan.start();
	_p->iSignificantChanges = 0;
}

C3TextEditorModeWithSymbolAnalysis::~C3TextEditorModeWithSymbolAnalysis()
{
	if(_p->pPerformCompletionRequest)
	{
		QObject::disconnect(
				_p->pPerformCompletionRequest,
				SIGNAL(terminated(C3SymbolManagerRequest *)),
				this,
				SLOT(slotPerformCompletionRequestTerminated(C3SymbolManagerRequest *))
			);
		_p->pPerformCompletionRequest->abort();
	}
	if(_p->pIdentifySymbolRequest)
	{
		QObject::disconnect(
				_p->pIdentifySymbolRequest,
				SIGNAL(terminated(C3SymbolManagerRequest *)),
				this,
				SLOT(slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest *))
			);
		_p->pIdentifySymbolRequest->abort();
	}
	if(_p->pScanEditorFileRequest)
	{
		QObject::disconnect(
				_p->pScanEditorFileRequest,
				SIGNAL(terminated(C3SymbolManagerRequest *)),
				this,
				SLOT(slotScanEditorFileRequestTerminated(C3SymbolManagerRequest *))
			);
		_p->pScanEditorFileRequest->abort();
	}

	delete _p;
}

void C3TextEditorModeWithSymbolAnalysis::symbolCompletion(C3TextEditorSymbolContext * pContext,unsigned int uFlags)
{
	//qDebug("RESCAN: %lld > %d ?",_p->oLastFileRescan.elapsed(),(60000 - (_p->iSignificantChanges * 10000)));
	if((!_p->pScanEditorFileRequest) && (_p->oLastFileRescan.elapsed() > (60000 - (_p->iSignificantChanges * 10000))))
		triggerRescanFile();

	if(_p->pPerformCompletionRequest)
	{
		QObject::disconnect(
				_p->pPerformCompletionRequest,
				SIGNAL(terminated(C3SymbolManagerRequest *)),
				this,
				SLOT(slotPerformCompletionRequestTerminated(C3SymbolManagerRequest *))
			);
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::symbolCompletion] Aborting existing request");
		_p->pPerformCompletionRequest->abort();
	}

	_p->pPerformCompletionRequest = new C3SymbolManagerRequestPerformCompletion(m_oParserState.pCoreData->pEditor->path(),pContext,_p->eLanguage,uFlags);

	QObject::connect(
			_p->pPerformCompletionRequest,
			SIGNAL(terminated(C3SymbolManagerRequest *)),
			this,
			SLOT(slotPerformCompletionRequestTerminated(C3SymbolManagerRequest *))
		);
	_p->pPerformCompletionRequest->start();
}

void C3TextEditorModeWithSymbolAnalysis::slotPerformCompletionRequestTerminated(C3SymbolManagerRequest * pRequest)
{
	C3SymbolManagerRequestPerformCompletion * pReq = dynamic_cast<C3SymbolManagerRequestPerformCompletion *>(pRequest);
	Q_ASSERT(pReq);
	
	Q_ASSERT(_p->pPerformCompletionRequest == pReq);
	
	_p->pPerformCompletionRequest = NULL;

	QList<C3TextEditorCompletion *> * pCompletions = pReq->takeCompletions();

	if(!pCompletions)
	{
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::slotPerformCompletionRequestTerminated] Got no completion pointer");
		if(!pRequest->aborted())
			m_oParserState.pCoreData->pEditor->symbolCompletionFailed();
		return;
	}

	if(!pCompletions->isEmpty())
	{
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::slotPerformCompletionRequestTerminated] Got %d completions",pCompletions->count());
		m_oParserState.pCoreData->pEditor->showCompletionWidget(
				pReq->context()->oRange,
				pCompletions,
				pReq->flags()
			);
	} else {
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::slotPerformCompletionRequestTerminated] Got no completions");
		if(!pRequest->aborted())
			m_oParserState.pCoreData->pEditor->symbolCompletionFailed();
		delete pCompletions;
	}
}

void C3TextEditorModeWithSymbolAnalysis::symbolIdentification(C3TextEditorSymbolContext * pContext,unsigned int uFlags)
{
	//qDebug("RESCAN: %lld > %d ?",_p->oLastFileRescan.elapsed(),(60000 - (_p->iSignificantChanges * 10000)));
	if((!_p->pScanEditorFileRequest) && (_p->oLastFileRescan.elapsed() > (60000 - (_p->iSignificantChanges * 10000))))
		triggerRescanFile();

	if(_p->pIdentifySymbolRequest)
	{
		QObject::disconnect(
				_p->pIdentifySymbolRequest,
				SIGNAL(terminated(C3SymbolManagerRequest *)),
				this,
				SLOT(slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest *))
			);
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::symbolIdentification] Aborting existing request");
		_p->pIdentifySymbolRequest->abort();
	}

	unsigned int uISFlags = 0;
	if(uFlags & SymbolIdentificationNoContextFilter)
		uISFlags |= C3SymbolManagerRequestIdentifySymbol::NoContextFilter;

	_p->pIdentifySymbolRequest = new C3SymbolManagerRequestIdentifySymbol(pContext,_p->eLanguage,uISFlags);

	QObject::connect(
			_p->pIdentifySymbolRequest,
			SIGNAL(terminated(C3SymbolManagerRequest *)),
			this,
			SLOT(slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest *))
		);
	_p->pIdentifySymbolRequest->start();
}

void C3TextEditorModeWithSymbolAnalysis::slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest * pRequest)
{
	C3SymbolManagerRequestIdentifySymbol * pReq = dynamic_cast<C3SymbolManagerRequestIdentifySymbol *>(pRequest);
	Q_ASSERT(pReq);
	
	Q_ASSERT(_p->pIdentifySymbolRequest == pReq);
	
	_p->pIdentifySymbolRequest = NULL;

	QList<C3TextEditorSymbolInfo *> * pInfoList = pReq->takeInfoList();

	if(!pInfoList)
	{
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::slotIdentifySymbolRequestTerminated] Got no info list");
		m_oParserState.pCoreData->pEditor->symbolIdentified(pReq->context()->oRange,NULL);
		return;
	}

	if(!pInfoList->isEmpty())
	{
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::slotIdentifySymbolRequestTerminated] Got %d info links",pInfoList->count());
		m_oParserState.pCoreData->pEditor->symbolIdentified(pReq->context()->oRange,pInfoList);
	} else {
		//qDebug("[C3TextEditorModeWithSymbolAnalysis::slotIdentifySymbolRequestTerminated] Got no info objects");
		delete pInfoList;
		m_oParserState.pCoreData->pEditor->symbolIdentified(pReq->context()->oRange,NULL);
	}
}

void C3TextEditorModeWithSymbolAnalysis::triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

	if(!m_oParserState.pCoreData->pEditor->computeSymbolContext(rc,*pContext))
	{
		m_oParserState.pCoreData->pEditor->symbolCompletionFailed();
		delete pContext;
		return;
	}

	if(pContext->szText.length() > 0)
	{
		if(pContext->szText[0].isDigit())
		{
			bool ok;
			pContext->szText.toLongLong(&ok);
			if(ok)
			{
				m_oParserState.pCoreData->pEditor->symbolCompletionFailed();
				return; // never complete numbers
			}
		}
	}

	//qDebug("Symbol context is %s %s %s",pContext->szLeftText.toUtf8().data(),pContext->szLeftOperator.toUtf8().data(),pContext->szText.toUtf8().data());

	symbolCompletion(pContext,uFlags);
}

bool C3TextEditorModeWithSymbolAnalysis::triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

	if(!m_oParserState.pCoreData->pEditor->computeSymbolContext(rc,*pContext,C3TextEditor::ExtendSymbolToRight))
	{
		delete pContext;
		return false;
	}

	//qDebug("Symbol context is %s %s %s",pContext->szLeftText.toUtf8().data(),pContext->szLeftOperator.toUtf8().data(),pContext->szText.toUtf8().data());

	symbolIdentification(pContext,uFlags);
	return true;
}

void C3TextEditorModeWithSymbolAnalysis::triggerRescanFile()
{
	if(_p->pScanEditorFileRequest)
		return;

	_p->pScanEditorFileRequest = new C3SymbolManagerRequestScanEditorFile(m_oParserState.pCoreData->pEditor->id(),false);

	//qDebug("Triggering rescan file");

	QObject::connect(
			_p->pScanEditorFileRequest,
			SIGNAL(terminated(C3SymbolManagerRequest *)),
			this,
			SLOT(slotScanEditorFileRequestTerminated(C3SymbolManagerRequest *))
		);
	_p->pScanEditorFileRequest->start();
}

void C3TextEditorModeWithSymbolAnalysis::slotScanEditorFileRequestTerminated(C3SymbolManagerRequest * pRequest)
{
	C3SymbolManagerRequestScanEditorFile * pReq = dynamic_cast<C3SymbolManagerRequestScanEditorFile *>(pRequest);
	Q_ASSERT(pReq);
	
	
	Q_ASSERT(_p->pScanEditorFileRequest == pReq);
	
	_p->pScanEditorFileRequest = NULL;
	
	_p->oLastFileRescan.start();
	_p->iSignificantChanges = 0;

	//qDebug("Rescan file terminated");
}

bool C3TextEditorModeWithSymbolAnalysis::textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		)
{
	if(iFirstLine != iLastLine)
		_p->iSignificantChanges++;
	else {
		if((szText.length() == 1) && szText.contains(QChar(';')))
			_p->iSignificantChanges += 3; // statement separators count quite a lot
	}

	if((!_p->pScanEditorFileRequest) && (_p->oLastFileRescan.elapsed() > (60000 - (_p->iSignificantChanges * 10000))))
		triggerRescanFile();

	return C3TextEditorModeWithInterLineState::textInserted(szText,iFirstLine,iLastLine);
}

bool C3TextEditorModeWithSymbolAnalysis::textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		)
{
	if(iFirstLine != iLastLine)
		_p->iSignificantChanges++;
	return C3TextEditorModeWithInterLineState::textDeleted(szText,iFirstLine,iLastLine);
}

void C3TextEditorModeWithSymbolAnalysis::fileSaved(bool bWasModified)
{
	if(bWasModified && (!_p->pScanEditorFileRequest))
		triggerRescanFile();
}
