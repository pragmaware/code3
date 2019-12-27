#ifndef _C3Workspace_h_
#define _C3Workspace_h_
//=============================================================================
//
//   File : C3Workspace.h
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


#include "c3_config.h"

#include "C3Link.h"
#include "C3Editor.h"
#include "C3AlertSource.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QVariant>
#include <QStringList>


class C3WorkspacePrivate;
class C3WorkspaceOpenFileWorker;
class C3WorkspaceSymbolManagerReset;
class C3WorkspaceFileScanWorker;
class C3CompletionStore;
class C3ExternalCommand;
class C3SettingsUserHighlightingSet;
class C3SymbolManagerRequest;

///
/// \class C3Workspace
/// \brief The C3Workspace class
///
/// This class...
///
class C3Workspace : public QObject, public C3AlertSource
{
	friend class C3WorkspaceOpenFileWorker;
	friend class C3WorkspaceSymbolManagerReset;
	friend class C3WorkspaceFileScanWorker;
	friend class C3MainWindow;
	friend class C3SettingsDialog;
	Q_OBJECT
public:

	///
	/// Creates an instance of C3Workspace
	///
	C3Workspace();

	///
	/// Destroys the instance of C3Workspace
	/// and frees all the relevant resources
	///
	virtual ~C3Workspace();

private:

	C3WorkspacePrivate * m_pC3W;

public:

	static C3Workspace * currentWorkspace();

	//
	// A default workspace is not saved to disk.
	//
	bool isDefault();

	void openFileAsync(const QString &szPath,bool bAddHistoryEntry = true,bool bActivate = true);
	
	/*
		hParams = {
			path: <full path of the file to open>
			location: <additional location in the file> -> row[,column] optional!
			factoryId: <factory id> -> optional!,
			formatId: <format id> -> optional (but if present then requires factoryId to be set)
		}
	*/
	void openFileAsync(const C3Link &oLink,bool bAddHistoryEntry = true,bool bActivate = true);
	
	/*
		hParams = {
			factoryId: <factory id> -> optional!,
			formatId: <format id> -> optional (but if present then requires factoryId to be set)
		}
	*/
	C3Editor * newFile(const C3Link &oLink,bool bAddHistoryEntry = true);

	void activateEditor(C3Editor * pEditor);
	
	C3Editor * findEditor(const QString &szFactoryId,const QString &szFilePath);
	C3Editor * findFirstEditor(const QString &szFilePath)
	{
		return findEditor(QString(),szFilePath);
	}
	C3Editor * findEditor(int id);
	
	void computeListOfModifiedTextEditors(QList<int> &lIds);
	
	// Returns the specified editor if it is still in the workspace.
	// It can be called concurrently.
	// If the returned pointer is not null you MUST call unlockEditor()
	// as soon as possible. This is mainly used to call C3TextEditor::saveTemporaryFile()
	// from a slave thread.
	C3Editor * lockEditor(int id);
	void unlockEditor(C3Editor * ed);
	
	void findAllEditors(const QString &szFilePath,QList<C3Editor *> &lBuffer);
	void findAllEditorsInDirectory(const QString &szDirectoryPath,QList<C3Editor *> &lBuffer);
	
	// WARNING: NEVER modify the hash itself.
	QHash<int,C3Editor *> & editors();

	bool closeEditor(C3Editor * pEditor,bool bCanAbort);

	int modifiedEditorCount();
	
	bool close(bool bCanAbort);
	
	bool saveAs();
	
	bool save();
		
	bool load(const QString &szPath,QString &szError);

	const QString & path() const;
	
	const QString & name() const;
	
	void editProperties();

	const QString & projectName() const;
	const QString & rootPath() const;
	const QString & fileHeader() const;
	const QStringList & includePaths() const;
	const QStringList & fileExclusionPatterns() const;

	C3SettingsUserHighlightingSet * userHighlightingSet();

	// for properties widgets
	QString * projectNameOption();
	QString * fileHeaderOption();
	QStringList * includePathsOption();
	QStringList * fileExclusionPatternsOption();

	// These are valid also for default workspaces
	QVariant genericSetting(const QString &szKey) const;
	void setGenericSetting(const QString &szKey,const QVariant &vValue);

	// Load a default workspace.
	void loadDefault();

	// Find a file in the workspace (or external include directories)
	QString findFile(const QString &szFileName,const QString &szPathHint = QString(),bool bMustExistInCompletionStore = true);
	QString findFileRecursive(const QString &szFileName,const QString &szPath,int iCurrentDepth = 0);

	void matchFilesRecursive(const QString &szFilePart,const QString &szPath,QList<QString> &lRet,int iCurrentDepth = 0);
	void matchFiles(const QString &szFilePart,QList<QString> &lRet);

	void getFileCompletions(const QString &szPart,QStringList &lBuffer);

	const QList<C3ExternalCommand *> & externalCommands();
	void setExternalCommands(QList<C3ExternalCommand *> &lCommands);
	
	const C3ExternalCommand * externalCommand(const QString &szId);

	//
	// Apply replacements to the szText string.
	// Beside the standard variables (%%author%%, %%project%%, ...)
	// hAdditionalParams may contain a set of additional key->value
	// pairs which will be replaced in the text. For example
	// filepath->path of a file
	//
	static QString substituteVariables(
			const QString &szText,
			QHash<QString,QString> &hAdditionalParams
		);
		
	//
	// If the workspace is a default workspace, return szPath.
	// If the workspace is a real workspace then
	//   If the path is inside the workspace root, make it relative
	//   If the path is outside the workspace root, keep it absolute
	//
	QString toWorkspaceDependantPath(const QString &szPath);

	//
	// Notify the workspace that a file rescan is needed soon.
	// Either new files have been written to disk or some have been renamed.
	//
	void notifyRescanNeeded();


	const QString & cachedPassword() const;
	void setCachedPassword(const QString &szPass);
	

protected:

	// friend: called by C3WorkspaceOpenFileWorker (on a slave thread!)
	void openFileWorkerTerminated(C3WorkspaceOpenFileWorker * pWorker,const C3Link &oLink);
	// friend: called by C3WorkspaceFileScanWorker (on a slave thread!)
	void asyncFileScanWorkerTerminated(C3WorkspaceFileScanWorker * pWorker,const QString &szError,C3CompletionStore * pStore);
	// friend: called by C3MainWindow
	void generalSettingsChanged();
private:

	bool saveInternal(const QString &szPath,QString &szError,bool bSavingNonDefault);
	bool saveInternalCommonData(const QString &szPath,QString &szError,bool bSavingNonDefault);
	bool saveInternalUserData(const QString &szPath,QString &szError,bool bSavingNonDefault);

	bool loadInternal(const QString &szPath,QString &szError,bool bLoadingNonDefault);
	bool loadInternalCommonData(const QString &szPath,QString &szError,bool bLoadingNonDefault);
	bool loadInternalUserData(const QString &szPath,QString &szError,bool bLoadingNonDefault);

	void setNonDefaultWorkspaceDefaultValues(const QString &szPath);

	void abortSymbolManagerReset();
	
	void startSymbolManagerReset();

	void abortFileScanWorker();
	
	void maybeStartFileScanWorker();

	void abortAllOpenFileWorkers();

	void saveDefault();

	QString defaultWorkspacePath();
	
	QString checkFileExistence(const QString &szFileName,const QString &szPath);

signals:
	// internal signal used in inter-thread communication
	void signalOpenFileWorkerTerminated(const QVariant &);

	// This signal is emitted just after the properties dialog has been closed (accepted).
	// Please note that C3Editor instances will also receive a specific call to
	// workspacePropertiesChanged() *after* this signal has been emitted.
	// So this is for everything that needs to update its internal state before
	// the editors notice.
	void propertiesChanged();

protected slots:

	void slotHeartbeat();

	// internal slot used in inter-thread communication
	void slotOpenFileWorkerTerminated(const QVariant &vParams);
	
	void slotResetSymbolStoreRequestTerminated(C3SymbolManagerRequest * pRequest);
	
	void slotEditorModifiedStateChanged(C3Editor * pEditor);
	void slotEditorTitleChanged(C3Editor * pEditor);
	void slotEditorReadOnlyStateChanged(C3Editor * pEditor);
	void slotEditorPathChanged(C3Editor * pEditor);

}; // class C3Workspace

#endif //!_C3Workspace_h_
