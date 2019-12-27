#ifndef _C3DockWorkspaceBrowser_h_
#define _C3DockWorkspaceBrowser_h_
//=============================================================================
//
//   File : C3DockWorkspaceBrowser.h
//   Creation Date : mar 20 ott 2015 05:42:44
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

#include <QDockWidget>

#include "C3AlertSource.h"

class C3DockWorkspaceBrowserPrivate;
class C3DockWorkspaceBrowserEditorItem;
class C3Workspace;
class C3Editor;
class QJsonObject;
class QMenu;
class QTreeWidgetItem;

///
/// \class C3DockWorkspaceBrowser
/// \brief The C3DockWorkspaceBrowser class
///
/// This class...
///
class C3DockWorkspaceBrowser : public QDockWidget, public C3AlertSource
{
	friend class C3MainWindow;
	Q_OBJECT
public:

	///
	/// Creates an instance of C3DockWorkspaceBrowser
	///
	C3DockWorkspaceBrowser(QWidget * pParent);

	///
	/// Destroys the instance of C3DockWorkspaceBrowser
	/// and frees all the relevant resources
	///
	virtual ~C3DockWorkspaceBrowser();

public:

	enum ViewMode
	{
		Flat,
		ByFileType,
		ByModificationState,
		ByLastUsage
	};

private:

	C3DockWorkspaceBrowserPrivate * m_pC3DWB;
	
	static C3DockWorkspaceBrowser * m_pInstance;

public:

	static C3DockWorkspaceBrowser * instance()
	{
		return m_pInstance;
	}

	void attachToWorkspace(C3Workspace * pWorkspace);

	void saveState(QJsonObject &oObject);
	void restoreState(QJsonObject &oObject);

	void setViewMode(ViewMode eViewMode);

protected:

	// friend: called by C3MainWindow
	void editorClosed(C3Editor * pEditor);
	// friend: called by C3MainWindow
	void editorOpened(C3Editor * pEditor);
	// friend: called by C3MainWindow
	void currentEditorChanged(C3Editor * pEditor);
	// friend: called by C3MainWindow
	void editorModifiedStateChanged(C3Editor * pEditor);
	// friend: called by C3MainWindow
	void editorTitleChanged(C3Editor * pEditor);

private:
	void refill();

	void createItemForEditor(C3Editor * pEditor);

	void updateItemForEditor(C3Editor * pEditor);
	
	void updateItem(C3DockWorkspaceBrowserEditorItem * it,C3Editor * pEditor);
	
	void removeItem(C3DockWorkspaceBrowserEditorItem * it);

	QTreeWidgetItem * parentItemForEditor(C3Editor * pEditor);

private slots:
	void slotWorkspaceDestroyed();

	void slotItemClicked(QTreeWidgetItem *it,int column);
	void slotCurrentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *);

	void slotContextMenuRequested(const QPoint &pnt);

	void slotContextMenuActionTriggered(QAction * pAction);
	
	void slotTitleBarMenuAboutToShow();
	
	void slotSetViewModeFlat();
	void slotSetViewModeByFileType();
	void slotSetViewModeByModificationState();
	void slotSetViewModeByLastUsage();

}; // class C3DockWorkspaceBrowser

#endif //!_C3DockWorkspaceBrowser_h_