#ifndef _C3MainWindow_h_
#define _C3MainWindow_h_
//=============================================================================
//
//   File : C3MainWindow.h
//   Creation Date : mar 20 ott 2015 00:03:08
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

#include <QMainWindow>

#include "C3AlertSource.h"

class C3MainWindowPrivate;
class C3DockTextConsole;
class C3Workspace;
class C3Editor;
class C3Link;
class C3FindWidget;

///
/// \class C3MainWindow
/// \brief The C3MainWindow class
///
/// This class...
///
class C3MainWindow : public QMainWindow
{
	friend class C3Workspace;
	Q_OBJECT
public:

	///
	/// Creates an instance of C3MainWindow
	///
	C3MainWindow();

	///
	/// Destroys the instance of C3MainWindow
	/// and frees all the relevant resources
	///
	virtual ~C3MainWindow();

private:

	static C3MainWindow * m_pInstance;

	C3MainWindowPrivate * m_pC3MW;

public:

	static C3MainWindow * instance()
	{
		return m_pInstance;
	}

	void alert(
			C3AlertSource * pSource,
			C3AlertSource::AlertLevel eLevel,
			const QString &szMessage
		);

	C3Workspace * workspace();

	QWidget * parentForEditors();

	void activateEditor(C3Editor * pEditor);

	C3Editor * currentEditor();
	
	// Returns the best guess directory for saving files. This is actually the file browser directory.
	QString currentDirectory();

	void toggleFindWidget();
	C3FindWidget * showFindWidget();
	void hideFindWidget();
	bool findWidgetVisible();
	
	void newDocument();
	void openDocument();

	void updateExternalCommandToolBarControls();
	void startCurrentExternalCommand();
	void stopCurrentExternalCommand();

	bool saveWorkspaceAs();
	bool closeWorkspace(bool bCanAbort,bool bLoadDefault);
	bool openWorkspace();
	bool openWorkspace(const QString &szPath);
	bool openWorkspace(const QString &szPath,QString &szError);

	// This should be called only by C3Application at startup.
	void loadInitialDefaultWorkspace();

	void showGeneralSettingsDialog();
	
	C3DockTextConsole * textConsole();
	void closeTextConsole();

	bool createCurrentHistoryEntry(C3Link &oLink);
	void addHistoryEntry();

signals:
	// This is emitted after C3Workspace::propertiesChanged() but before
	// the editors receive their workspacePropertiesChanged() call.
	void workspacePropertiesChanged();

protected:
	// friend: called by C3Workspace
	void addEditor(C3Editor * pEditor,bool bOnTop);

	// friend: called by C3Workspace
	// Don't use this to close the editor. Use C3Workspace::closeEditor() instead.
	void killEditor(C3Editor * pEditor);

	// friend: called by C3Workspace
	void editorModifiedStateChanged(C3Editor * pEditor);
	void editorTitleChanged(C3Editor * pEditor);
	void editorPathChanged(C3Editor * pEditor);
	void editorReadOnlyStateChanged(C3Editor * pEditor);


private slots:

	void slotStackedWidgetCurrentChanged(int idx);
	void slotCurrentEditorPrivateInfoText1Changed(C3Editor * pEditor);
	void slotCurrentEditorPrivateInfoText2Changed(C3Editor * pEditor);
	void slotViewMenuAboutToShow();
	void slotEditMenuAboutToShow();
	void slotFXMenuAboutToShow();
	void slotRecentWorkspacesMenuAboutToShow();
	void slotOpenRecentWorkspace();
	void slotRecentFilesMenuAboutToShow();
	void slotOpenRecentFile();
	void slotWorkspacePropertiesChanged();
	void slotWorkspaceActionTriggered();
	void slotExternalCommandsComboBoxCurrentIndexChanged(int idx);

private:

	virtual void closeEvent(QCloseEvent * e);
	virtual void showEvent(QShowEvent * e);

	void currentEditorChanged(C3Editor * pEditor);

	void setWorkspace(C3Workspace * pWorkspace);

	void createMenuBar();
	void createToolBar();

	void loadWindowState();
	void saveWindowState();

	void updateFilePathLabel(C3Editor * pEditor);
	void updatePrivateInfoText1Label(C3Editor * pEditor);
	void updatePrivateInfoText2Label(C3Editor * pEditor);
	void updateCaption();

	void rebuildExternalCommandToolBarControls();

	QString currentExternalCommand();

}; // class C3MainWindow

#endif //!_C3MainWindow_h_
