//=============================================================================
//
//   File : C3Workspace.cpp
//   Creation Date : mar 20 ott 2015 01:01:10
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

#include "C3Workspace.h"
#include "C3WorkspaceOpenFileWorker.h"
#include "C3Link.h"
#include "C3MainWindow.h"
#include "C3Application.h"
#include "C3Settings.h"

#include "C3Workspace_private.h"

#include <QThread>
#include <QThreadPool>

void C3Workspace::abortAllOpenFileWorkers()
{
	_p->oOpenFileWorkerHashMutex.lock();
	C3WorkspaceOpenFileWorker * pWorker;
	QList<C3WorkspaceOpenFileWorker *> lWorkers;
	foreach(pWorker,_p->oOpenFileWorkerHash)
		lWorkers.append(pWorker);
	
	foreach(pWorker,lWorkers)
	{
		int id = pWorker->id();
		if(!pWorker->abort())
		{
			// The abort call failed: the worker is already in its termination call.
			// Unlock the mutex and wait for it to complete.
			_p->oOpenFileWorkerHashMutex.unlock();
			
			//qDebug("[C3Workspace::abortAllOpenFileWorkers] Waiting for open file worker %d to exit",id);
			
			while(_p->oOpenFileWorkerHash.contains(id))
				QThread::msleep(50);

			_p->oOpenFileWorkerHashMutex.lock();
		} else {
			// Abort succeeded. The worker will no longer call us back here.
#if QT_VERSION >= 0x50500
			QThreadPool::globalInstance()->cancel(pWorker);
#endif
		}
	}
	_p->oOpenFileWorkerHash.clear();
	_p->oOpenFileWorkerHashMutex.unlock();
}

void C3Workspace::openFileAsync(const QString &szPath,bool bAddHistoryEntry,bool bActivate)
{
	if(_p->bDying)
		return;

	QString szFileName = szPath;
	QString szLocation;
	
	int idx = szFileName.indexOf(QChar(':'));
	if(idx > 3) // present and not windows root marker
	{
		QString szFull = szFileName;
		szFileName = szFull.left(idx);
		szLocation = szFull.mid(idx+1);
	}

	C3Link oLink(szFileName);
	if(!szLocation.isEmpty())
		oLink.setLocation(szLocation);

	openFileAsync(oLink,bAddHistoryEntry,bActivate);
}

void C3Workspace::openFileAsync(const C3Link &lnk,bool bAddHistoryEntry,bool bActivate)
{
	if(_p->bDying)
		return;

	QString szPath = lnk.path();
	QString szFactoryId = lnk.editorFactoryId();

	C3Editor * pEditor = findEditor(szFactoryId,szPath);

	if(pEditor)
	{
		// already open
		if(bAddHistoryEntry)
			C3MainWindow::instance()->addHistoryEntry();
		if(bActivate)
			activateEditor(pEditor);
		pEditor->repeatedOpen(lnk);
		return;
	}

	C3WorkspaceOpenFileWorker * pWorker = new C3WorkspaceOpenFileWorker(this,lnk);
	
	if(bAddHistoryEntry)
		pWorker->link().insert("addHistoryEntry",true);
	pWorker->link().insert("activateOnOpen",bActivate);
	pWorker->setAutoDelete(true);

	_p->oOpenFileWorkerHashMutex.lock();
	_p->oOpenFileWorkerHash.insert(pWorker->id(),pWorker);
	_p->oOpenFileWorkerHashMutex.unlock();

	QThreadPool::globalInstance()->start(pWorker);
}

void C3Workspace::openFileWorkerTerminated(C3WorkspaceOpenFileWorker * pWorker,const C3Link &oLink)
{
	//qDebug("OPEN FILE WORKER TERMINATED (%s) (data=%d)",vParams.value("path").toString().toUtf8().data(),vParams.value("data").toByteArray().size());

	_p->oOpenFileWorkerHashMutex.lock();
	_p->oOpenFileWorkerHash.remove(pWorker->id());
	_p->oOpenFileWorkerHashMutex.unlock();

	if(_p->bDying)
		return;

	emit signalOpenFileWorkerTerminated(QVariant(oLink));
}


void C3Workspace::slotOpenFileWorkerTerminated(const QVariant &vLink)
{
	if(_p->bDying)
		return;

	C3Link oLink(vLink.toHash());

	//qDebug("SLOT OPEN FILE WORKER TERMINATED (%s) (data=%d)",hParams.value("path").toString().toUtf8().data(),hParams.value("data").toByteArray().size());

	QString szPath = oLink.path();

	//qDebug("Open File Async Worker Terminated PATH(%s) LOCATION(%s)",szPath.toUtf8().data(),hParams.value("location").toString().toUtf8().data());

	QString szStatus = oLink.value("status").toString();

	if(szStatus != "ok")
	{
		QString szError = oLink.value("error").toString();
		error(__tr("Could not open file '%1': %2").arg(szPath).arg(szError));
		return;
	}

	bool bAddHistoryEntry = oLink.value("addHistoryEntry").toBool();
	bool bActivate = oLink.value("activateOnOpen").toBool();

	QString szFactoryId = oLink.editorFactoryId();

	C3Editor * pEditor = findEditor(szFactoryId,szPath);

	if(pEditor)
	{
		// already open
		if(bAddHistoryEntry)
			C3MainWindow::instance()->addHistoryEntry();
		if(bActivate)
			activateEditor(pEditor);
		pEditor->repeatedOpen(oLink);
		return;
	}

	const QList<C3EditorFactory *> &lFactories = C3Application::instance()->editorFactories();

	C3EditorFactory * pFactory = NULL;

	Q_FOREACH(C3EditorFactory * pFactory2,lFactories)
	{
		if(pFactory2->id() == szFactoryId)
		{
			pFactory = pFactory2;
			break;
		}
	}

	if(!pFactory)
	{
		error(__tr("Could not open file '%1': editor with factory id '%2' not found").arg(szPath).arg(szFactoryId));
		return;
	}

	pEditor = pFactory->createEditor(
			oLink,
			C3MainWindow::instance()->parentForEditors()
		);

	if(!pEditor)
	{
		error(__tr("Failed to create the editor for file '%1'").arg(szPath));
		return;
	}

	if(bAddHistoryEntry)
		C3MainWindow::instance()->addHistoryEntry();

	QObject::connect(pEditor,SIGNAL(modifiedStateChanged(C3Editor *)),this,SLOT(slotEditorModifiedStateChanged(C3Editor *)));
	QObject::connect(pEditor,SIGNAL(titleChanged(C3Editor *)),this,SLOT(slotEditorTitleChanged(C3Editor *)));
	QObject::connect(pEditor,SIGNAL(readOnlyStateChanged(C3Editor *)),this,SLOT(slotEditorReadOnlyStateChanged(C3Editor *)));
	QObject::connect(pEditor,SIGNAL(pathChanged(C3Editor *)),this,SLOT(slotEditorPathChanged(C3Editor *)));

	_p->oEditorsMutex.lock();
	_p->hEditors.insert(pEditor->id(),pEditor);
	int iCount = _p->hEditors.count();
	_p->oEditorsMutex.unlock();

	//qDebug("CREATED EDITOR FOR PATH (%s) ACTIVATE IS %d",szPath.toUtf8().data(),bActivate);

	C3MainWindow::instance()->addEditor(pEditor,bActivate);

	C3Settings::instance()->addRecentFile(szPath);

	if(bActivate)
		activateEditor(pEditor);
	
	if(isDefault() && (iCount == 1))
	{
		// With a single file reset the symbol manager so the right directory is added
		abortSymbolManagerReset();
		startSymbolManagerReset(); // reset symbol manager right away
	}
}
