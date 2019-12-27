//=============================================================================
//
//   File : C3GeneralActions.cpp
//   Creation Date : lun 26 ott 2015 01:22:05
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

#include "C3GeneralActions.h"

#include "C3PixmapCache.h"
#include "C3MainWindow.h"
#include "C3Editor.h"
#include "C3Workspace.h"
#include "C3LinkHistory.h"

#include <QAction>
#include <QKeySequence>
#include <QShortcut>
#include <QMenu>

C3GeneralActions * C3GeneralActions::m_pInstance = NULL;

C3GeneralActions::C3GeneralActions(C3MainWindow * pMainWindow)
	: QObject()
{
	m_pInstance = this;

	m_pMainWindow = pMainWindow;
	
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

#define CREATE_ACTION(_pObject,_szIcon,_szText,_szToolTip,_slot,_oShortcut) \
	do { \
		_pObject = new QAction( \
				pPixmapCache->pixmap(__utf8("general/20px/%1").arg(_szIcon)), \
				_szText, \
				this \
			); \
		_pObject->setIconVisibleInMenu(true); \
		_pObject->setToolTip(_szToolTip); \
		_pObject->setStatusTip(_szToolTip); \
		_pObject->setShortcut(_oShortcut); \
		_pObject->setShortcutContext(Qt::ApplicationShortcut); \
		QObject::connect(_pObject,SIGNAL(triggered()),this,_slot); \
	} while(0)

#define CREATE_SHORTCUT(_slot,_oShortcut) \
	do { \
		QShortcut * cut = new QShortcut( \
				QKeySequence(_oShortcut), \
				m_pMainWindow \
			); \
		cut->setContext(Qt::ApplicationShortcut); \
		QObject::connect(cut,SIGNAL(activated()),this,_slot); \
	} while(0)

	CREATE_ACTION(
			m_pDocumentSaveAction,
			"document-save.png",
			__tr("Save"),
			__tr("Save the current file"),
			SLOT(documentSave()),
			Qt::CTRL + Qt::Key_S
		);

	CREATE_ACTION(
			m_pDocumentSaveAsAction,
			"document-save-as.png",
			__tr("Save As..."),
			__tr("Save the current file with a new name or format"),
			SLOT(documentSaveAs()),
			Qt::CTRL + Qt::SHIFT + Qt::Key_S
		);

	CREATE_ACTION(
			m_pDocumentSaveAllAction,
			"document-save-all.png",
			__tr("Save All"),
			__tr("Save all the modified files"),
			SLOT(documentSaveAllSlot()),
			Qt::CTRL + Qt::ALT + Qt::SHIFT + Qt::Key_S
		);

	CREATE_ACTION(
			m_pDocumentOpenAction,
			"document-open.png",
			__tr("Open..."),
			__tr("Open a file from storage"),
			SLOT(documentOpen()),
			Qt::CTRL + Qt::Key_O
		);

	CREATE_ACTION(
			m_pDocumentNewAction,
			"document-new.png",
			__tr("New..."),
			__tr("Create a new document"),
			SLOT(documentNew()),
			Qt::CTRL + Qt::Key_N
		);

	CREATE_ACTION(
			m_pDocumentCloseAction,
			"document-close.png",
			__tr("Close"),
			__tr("Close the current document"),
			SLOT(documentClose()),
			Qt::CTRL + Qt::SHIFT + Qt::Key_Q
		);

	CREATE_ACTION(
			m_pToggleFindWidgetAction,
			"edit-find.png",
			__tr("Find and Replace..."),
			__tr("Toggle the find & replace window"),
			SLOT(toggleFindWidget()),
			Qt::CTRL + Qt::Key_F
		);

	CREATE_ACTION(
			m_pWorkspaceSaveAsAction,
			"document-save-as.png",
			__tr("Save As..."),
			__tr("Saves the current workspace to a new file"),
			SLOT(workspaceSaveAs()),
			Qt::CTRL + Qt::SHIFT + Qt::Key_W
		);

	CREATE_ACTION(
			m_pWorkspaceOpenAction,
			"document-open.png",
			__tr("Open..."),
			__tr("Open a workspace file"),
			SLOT(workspaceOpen()),
			Qt::CTRL + Qt::SHIFT + Qt::Key_Y
		);


	CREATE_ACTION(
			m_pWorkspaceCloseAction,
			"document-close.png",
			__tr("Close"),
			__tr("Closes the current workspace"),
			SLOT(workspaceClose()),
			Qt::CTRL + Qt::SHIFT + Qt::Key_E
		);

	CREATE_ACTION(
			m_pWorkspacePropertiesAction,
			"document-close.png",
			__tr("Properties..."),
			__tr("Edits the properties of the current workspace"),
			SLOT(workspaceProperties()),
			Qt::CTRL + Qt::SHIFT + Qt::Key_R
		);
	
	CREATE_ACTION(
			m_pConfigureC3Action,
			"settings.png",
			__tr("Configure C3..."),
			__tr("Shows the C3 general configuration dialog"),
			SLOT(configureC3()),
			Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_P
		);

	CREATE_ACTION(
			m_pOpenRelatedFileAction,
			"open-related-file.png",
			__tr("Open Related File"),
			__tr("Attempts to open a file related to the current one"),
			SLOT(openRelatedFile()),
			Qt::CTRL + Qt::Key_R
		);
		
	CREATE_ACTION(
			m_pHistoryBackAction,
			"history-previous.png",
			__tr("Back in History"),
			__tr("Navigates to the previous document in the history"),
			SLOT(historyBack()),
			Qt::ALT + Qt::Key_Left
		);

	m_pHistoryBackMenu = new QMenu(C3MainWindow::instance());
	m_pHistoryBackAction->setMenu(m_pHistoryBackMenu);
	QObject::connect(m_pHistoryBackMenu,SIGNAL(aboutToShow()),this,SLOT(slotHistoryBackMenuAboutToShow()));
	QObject::connect(m_pHistoryBackMenu,SIGNAL(triggered(QAction *)),this,SLOT(slotHistoryBackMenuActionTriggered(QAction *)));
	m_pHistoryBackAction->setEnabled(false);
	
	CREATE_ACTION(
			m_pHistoryForwardAction,
			"history-next.png",
			__tr("Back in History"),
			__tr("Navigates to the previous document in the history"),
			SLOT(historyForward()),
			Qt::ALT + Qt::Key_Right
		);

	m_pHistoryForwardMenu = new QMenu(C3MainWindow::instance());
	m_pHistoryForwardAction->setMenu(m_pHistoryForwardMenu);
	QObject::connect(m_pHistoryForwardMenu,SIGNAL(aboutToShow()),this,SLOT(slotHistoryForwardMenuAboutToShow()));
	QObject::connect(m_pHistoryForwardMenu,SIGNAL(triggered(QAction *)),this,SLOT(slotHistoryForwardMenuActionTriggered(QAction *)));
	m_pHistoryForwardAction->setEnabled(false);

	CREATE_ACTION(
			m_pStartCurrentExternalCommandAction,
			"external-command-start.png",
			__tr("Start External Command"),
			__tr("Starts the currently selected external command"),
			SLOT(startCurrentExternalCommand()),
			Qt::ALT + Qt::Key_T
		);

	CREATE_ACTION(
			m_pStopCurrentExternalCommandAction,
			"external-command-stop.png",
			__tr("Stop External Command"),
			__tr("Stops the currently selected external command"),
			SLOT(stopCurrentExternalCommand()),
			Qt::ALT + Qt::Key_Y
		);

}

C3GeneralActions::~C3GeneralActions()
{
	m_pInstance = NULL;
}

void C3GeneralActions::currentEditorChanged(C3Editor * pEditor)
{
	if(!pEditor)
	{
		m_pDocumentSaveAction->setEnabled(false);
		m_pDocumentSaveAsAction->setEnabled(false);
		m_pDocumentCloseAction->setEnabled(false);
		m_pOpenRelatedFileAction->setEnabled(false);
	} else {
		m_pDocumentSaveAction->setEnabled(pEditor->modified() && (!pEditor->readOnly()));
		m_pDocumentSaveAsAction->setEnabled(true);
		m_pDocumentCloseAction->setEnabled(true);
		m_pOpenRelatedFileAction->setEnabled(pEditor->canOpenRelatedFile());
	}

	m_pDocumentSaveAllAction->setEnabled(C3Workspace::currentWorkspace()->modifiedEditorCount() > 0);
}

void C3GeneralActions::currentEditorModifiedStateChanged(C3Editor * pEditor)
{
	m_pDocumentSaveAction->setEnabled(pEditor->modified());
	m_pDocumentSaveAllAction->setEnabled(C3Workspace::currentWorkspace()->modifiedEditorCount() > 0);
}

void C3GeneralActions::documentSave()
{
	C3Editor * pCurrent = m_pMainWindow->currentEditor();

	if(!pCurrent)
		return;

	pCurrent->save();
}

void C3GeneralActions::documentSaveAs()
{
	C3Editor * pCurrent = m_pMainWindow->currentEditor();

	if(!pCurrent)
		return;

	pCurrent->saveAs();
}

bool C3GeneralActions::documentSaveAll()
{
	QHash<int,C3Editor *> & hEditors = C3Workspace::currentWorkspace()->editors();
	
	foreach(C3Editor * pEditor,hEditors)
	{
		if(pEditor->modified())
		{
			if(!pEditor->save())
				return false; // canceled
		}
	}
	
	if(!C3Workspace::currentWorkspace()->isDefault())
		C3Workspace::currentWorkspace()->save();
	
	return true;
}

void C3GeneralActions::documentSaveAllSlot()
{
	documentSaveAll();
}

void C3GeneralActions::documentOpen()
{
	C3MainWindow::instance()->openDocument();
}

void C3GeneralActions::documentNew()
{
	C3MainWindow::instance()->newDocument();
}

void C3GeneralActions::documentClose()
{
	C3Editor * pCurrent = m_pMainWindow->currentEditor();

	if(!pCurrent)
		return;

	C3Workspace::currentWorkspace()->closeEditor(pCurrent,true);
}

void C3GeneralActions::toggleFindWidget()
{
	m_pMainWindow->toggleFindWidget();
}

void C3GeneralActions::workspaceSaveAs()
{
	C3MainWindow::instance()->saveWorkspaceAs();
}

void C3GeneralActions::workspaceClose()
{
	C3MainWindow::instance()->closeWorkspace(true,true);
}

void C3GeneralActions::workspaceOpen()
{
	C3MainWindow::instance()->openWorkspace();
}

void C3GeneralActions::workspaceChanged(C3Workspace * pWorkspace)
{
	//m_pWorkspaceSaveAsAction->setEnabled(true);
	m_pWorkspaceCloseAction->setEnabled(!pWorkspace->isDefault());
	m_pWorkspacePropertiesAction->setEnabled(!pWorkspace->isDefault());
}

void C3GeneralActions::workspaceProperties()
{
	C3Workspace::currentWorkspace()->editProperties();
}

void C3GeneralActions::configureC3()
{
	C3MainWindow::instance()->showGeneralSettingsDialog();
}

void C3GeneralActions::openRelatedFile()
{
	C3Editor * pCurrent = m_pMainWindow->currentEditor();

	if(!pCurrent)
		return;

	pCurrent->openRelatedFile();
}

void C3GeneralActions::historyBack()
{
	C3LinkHistory::instance()->back();
}

void C3GeneralActions::historyForward()
{
	C3LinkHistory::instance()->forward();
}

void C3GeneralActions::startCurrentExternalCommand()
{
	C3MainWindow::instance()->startCurrentExternalCommand();
}

void C3GeneralActions::stopCurrentExternalCommand()
{
	C3MainWindow::instance()->stopCurrentExternalCommand();
}

void C3GeneralActions::linkHistoryChanged(bool bBackAvailable,bool bForwardAvailable)
{
	m_pHistoryBackAction->setEnabled(bBackAvailable);
	m_pHistoryForwardAction->setEnabled(bForwardAvailable);
}

void C3GeneralActions::slotHistoryBackMenuAboutToShow()
{
	C3LinkHistory::instance()->fillHistoryBackMenu(m_pHistoryBackMenu);
}

void C3GeneralActions::slotHistoryForwardMenuAboutToShow()
{
	C3LinkHistory::instance()->fillHistoryForwardMenu(m_pHistoryForwardMenu);
}

void C3GeneralActions::slotHistoryBackMenuActionTriggered(QAction * a)
{
	C3LinkHistory::instance()->backMenuActivated(a);
}

void C3GeneralActions::slotHistoryForwardMenuActionTriggered(QAction * a)
{
	C3LinkHistory::instance()->forwardMenuActivated(a);
}

