#ifndef _C3DockFileBrowser_h_
#define _C3DockFileBrowser_h_
//=============================================================================
//
//   File : C3DockFileBrowser.h
//   Creation Date : mar 20 ott 2015 01:05:45
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

#include "C3AlertSource.h"

#include <QDockWidget>

class C3DockFileBrowserPrivate;

class QFileInfo;
class QListWidgetItem;
class QAction;
class QJsonObject;


///
/// \class C3DockFileBrowser
/// \brief The C3DockFileBrowser class
///
/// This class...
///
class C3DockFileBrowser : public QDockWidget, public C3AlertSource
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3DockFileBrowser
	///
	C3DockFileBrowser(QWidget * pParent);

	///
	/// Destroys the instance of C3DockFileBrowser
	/// and frees all the relevant resources
	///
	virtual ~C3DockFileBrowser();

private:

	C3DockFileBrowserPrivate * m_pC3DFB;

	static C3DockFileBrowser * m_pInstance;

public:

	static C3DockFileBrowser * instance()
	{
		return m_pInstance;
	}

	void setDirectory(const QString &szPath,bool bSelectCurrentFileIfPresent = false);
	const QString & directory();
	
	void reloadDelayed();

	void saveState(QJsonObject &oObject);
	void restoreState(QJsonObject &oObject,bool bRestoreDirectory);

	void goToWorkspaceRoot();

public slots:
	void reload();

private:

	QListWidgetItem * addEntry(const QFileInfo &inf);

	void createActionsAndToolBar();

	void newFolder();
	void openAllFilesInCurrentDirectory(bool bRecursive);
	void openAllFilesInDirectory(const QString &szDir,bool bRecursive);
	void renameSelected();
	void svnMvSelected();
	void svnRmSelected();
	void deleteSelected();
	
	void selectItem(const QString &szName);
	
	QListWidgetItem * findItem(const QString &szName);

	bool gatherSelectedItemsAndAskForDeletion(QList<QListWidgetItem *> &lSelected);

private slots:

	void slotWorkspaceRootActionTriggered();
	void slotUpActionTriggered();
	void slotHomeActionTriggered();
	void slotFileDirActionTriggered();
	void slotReloadActionTriggered();
	void slotPathEditReturnPressed();
	void slotItemDoubleClicked(QListWidgetItem * pItem);
	void slotContextMenuRequested(const QPoint &pnt);
	void slotContextMenuActionTriggered(QAction * pAction);

}; // class C3DockFileBrowser

#endif //!_C3DockFileBrowser_h_
