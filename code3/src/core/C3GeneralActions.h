#ifndef _C3GeneralActions_h_
#define _C3GeneralActions_h_
//=============================================================================
//
//   File : C3GeneralActions.h
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


#include "c3_config.h"

#include <QObject>
#include <QAction>

class C3MainWindow;
class C3Editor;
class C3Workspace;
class QMenu;

///
/// \class C3GeneralActions
/// \brief The C3GeneralActions class
///
/// This class...
///
class C3GeneralActions : public QObject
{
	friend class C3MainWindow;
	friend class C3LinkHistory;
	Q_OBJECT
public:

	///
	/// Creates an instance of C3GeneralActions
	///
	C3GeneralActions(C3MainWindow * pMainWindow);

	///
	/// Destroys the instance of C3GeneralActions
	/// and frees all the relevant resources
	///
	virtual ~C3GeneralActions();

private:
	C3MainWindow * m_pMainWindow;

	QAction * m_pDocumentSaveAction;
	QAction * m_pDocumentSaveAsAction;
	QAction * m_pDocumentSaveAllAction;
	QAction * m_pDocumentOpenAction;
	QAction * m_pDocumentNewAction;
	QAction * m_pDocumentCloseAction;
	QAction * m_pToggleFindWidgetAction;

	QAction * m_pWorkspaceOpenAction;
	QAction * m_pWorkspaceCloseAction;
	QAction * m_pWorkspaceSaveAsAction;
	QAction * m_pWorkspacePropertiesAction;
	
	QAction * m_pConfigureC3Action;

	QAction * m_pOpenRelatedFileAction;

	QAction * m_pHistoryBackAction;
	QAction * m_pHistoryForwardAction;
	
	QAction * m_pStartCurrentExternalCommandAction;
	QAction * m_pStopCurrentExternalCommandAction;

	QMenu * m_pHistoryBackMenu;
	QMenu * m_pHistoryForwardMenu;

	static C3GeneralActions * m_pInstance;

public:
	static C3GeneralActions * instance()
	{
		return m_pInstance;
	}

	QAction * startCurrentExternalCommandAction()
	{
		return m_pStartCurrentExternalCommandAction;
	}
	
	QAction * stopCurrentExternalCommandAction()
	{
		return m_pStopCurrentExternalCommandAction;
	}

	QAction * historyBackAction()
	{
		return m_pHistoryBackAction;
	}
	
	QAction * historyForwardAction()
	{
		return m_pHistoryForwardAction;
	}


	QAction * configureC3Action()
	{
		return m_pConfigureC3Action;
	}

	QAction * workspacePropertiesAction()
	{
		return m_pWorkspacePropertiesAction;
	}

	QAction * workspaceOpenAction()
	{
		return m_pWorkspaceOpenAction;
	}

	QAction * workspaceSaveAsAction()
	{
		return m_pWorkspaceSaveAsAction;
	}

	QAction * workspaceCloseAction()
	{
		return m_pWorkspaceCloseAction;
	}

	QAction * toggleFindWidgetAction()
	{
		return m_pToggleFindWidgetAction;
	}

	QAction * documentSaveAction()
	{
		return m_pDocumentSaveAction;
	}

	QAction * documentSaveAsAction()
	{
		return m_pDocumentSaveAsAction;
	}

	QAction * documentSaveAllAction()
	{
		return m_pDocumentSaveAllAction;
	}
	
	QAction * documentOpenAction()
	{
		return m_pDocumentOpenAction;
	}
	
	QAction * documentNewAction()
	{
		return m_pDocumentNewAction;
	}

	QAction * documentCloseAction()
	{
		return m_pDocumentCloseAction;
	}

	QAction * openRelatedFileAction()
	{
		return m_pOpenRelatedFileAction;
	}

protected:

	// friend: called by C3MainWindow
	void currentEditorChanged(C3Editor * pEditor);
	// friend: called by C3MainWindow
	void currentEditorModifiedStateChanged(C3Editor * pEditor);
	// friend: called by C3MainWindow
	void workspaceChanged(C3Workspace * pWorkspace);
	// friend: called by C3LinkHistory
	void linkHistoryChanged(bool bBackAvailable,bool bForwardAvailable);

public:
	bool documentSaveAll();

public slots:
	void documentClose();
	void documentSave();
	void documentSaveAs();
	void documentSaveAllSlot();
	void documentOpen();
	void documentNew();

	void toggleFindWidget();

	void workspaceOpen();
	void workspaceSaveAs();
	void workspaceClose();
	void workspaceProperties();

	void configureC3();
	
	void openRelatedFile();
	
	void historyBack();
	void historyForward();

	void startCurrentExternalCommand();
	void stopCurrentExternalCommand();

private slots:
	void slotHistoryBackMenuAboutToShow();
	void slotHistoryForwardMenuAboutToShow();

	void slotHistoryBackMenuActionTriggered(QAction * a);
	void slotHistoryForwardMenuActionTriggered(QAction * a);

}; // class C3GeneralActions

#endif //!_C3GeneralActions_h_
