#ifndef _C3Editor_h_
#define _C3Editor_h_
//=============================================================================
//
//   File : C3Editor.h
//   Creation Date : mar 20 ott 2015 01:03:26
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

#include "C3AlertSource.h"
#include "C3FindTarget.h"
#include "C3Link.h"

#include <QWidget>
#include <QHash>
#include <QVariant>

class C3EditorPrivate;
class C3EditorFactory;
class C3EditorFormat;
class C3DockWorkspaceBrowserEditorItem;
class QMenu;
class QJsonObject;

///
/// \class C3Editor
/// \brief The C3Editor class
///
/// This class...
///
class C3Editor : public QWidget, public C3AlertSource, public C3FindTarget
{
	friend class C3Workspace;
	Q_OBJECT
public:

	///
	/// Creates an instance of C3Editor
	///
	C3Editor(C3EditorFactory * pFactory,QWidget * pParent,const QString &szAlertSourceName);

	///
	/// Destroys the instance of C3Editor
	/// and frees all the relevant resources
	///
	virtual ~C3Editor();

private:

	C3EditorPrivate * m_pC3E;

public:

	// Internal payload used by C3DockWorkspaceBrowser
	void setWorkspaceBrowserItem(C3DockWorkspaceBrowserEditorItem * it);
	C3DockWorkspaceBrowserEditorItem * workspaceBrowserItem();

	C3EditorFactory * factory() const;

	virtual C3EditorFormat * format() = 0;

	const QString & factoryId() const;

	const QString & title() const;
	
	bool modified() const;

	bool readOnly() const;
	
	qint64 lastShowEventTime() const;
	
	
	virtual void repeatedOpen(const C3Link &oLink);

	virtual const QString & privateInfoText1() const = 0;
	virtual const QString & privateInfoText2() const = 0;

	// unique editor id
	int id() const;

	const QString & path() const;
	
	QString fileName() const;
	QString directory() const;

	// The editor is going to be destroyed. Close the file.
	// You don't actually need to really close it: you should only save contents, if modified.
	// Return false if the close operation failed or was aborted by the user.
	virtual bool closeFile(bool bCanAbort);

	// Returns false if the operation was aborted by the user.
	virtual bool save();

	// Returns false if the operation was aborted by the user.
	virtual bool saveAs();

	virtual void fillViewMenu(QMenu * pMenu);
	virtual void fillEditMenu(QMenu * pMenu);
	virtual void fillFXMenu(QMenu * pMenu);

	virtual bool supportsFindOperations();
	virtual QString findOperationGetInitialSearchString();
	virtual bool findOperationRun(C3FindOperation * pOperation);
	virtual bool findOperationReplace(C3FindOperation * pOperation,const QString &szText);

	// The state for the workspace save operation.
	// Returns false if the state can't/shouldn't be saved.
	virtual bool saveState(QJsonObject &oObject);
	
	// The link for the history
	// Returns false if the state can't/shouldn't be saved.
	virtual bool createLink(C3Link &link);

	virtual QString relatedFilePath();
	virtual bool canOpenRelatedFile();
	virtual void openRelatedFile();
	
	virtual bool handleKeyEvent(QKeyEvent * e);
	
protected:

	// This is called by the C3Workspace *AFTER* its internal signal propertiesChanged()
	// has been emitted and *AFTER* C3MainWindow has emitted its workspacePropertiesChanged().
	virtual void workspacePropertiesChanged();

	// This is called by the C3Workspace *BEFORE* its internal signal propertiesChanged()
	// has been emitted and *BEFORE* C3MainWindow has emitted its workspacePropertiesChanged().
	virtual void generalSettingsChanged();

	// Reimplement this to save the file.
	// Return false in case of error (and set szError please).
	// Use the specified format or the current/default format if pFormat is NULL.
	virtual bool saveInternal(
			const QString &szPath,
			C3EditorFormat * pFormat,
			QString &szError
		) = 0;

signals:

	void titleChanged(C3Editor * pEditor);
	void modifiedStateChanged(C3Editor * pEditor);
	void pathChanged(C3Editor * pEditor);
	void readOnlyStateChanged(C3Editor * pEditor);
	void privateInfoText1Changed(C3Editor * pEditor);
	void privateInfoText2Changed(C3Editor * pEditor);

protected:

	void setTitle(const QString &szTitle);
	void setModified(bool bModified);
	void setPath(const QString &szPath);
	void setReadOnly(bool bReadOnly);
	void showEvent(QShowEvent * e);

}; // class C3Editor

#endif //!_C3Editor_h_
