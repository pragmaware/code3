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

#include "C3SettingsUserHighlightingSet.h"
#include "C3WorkspaceFileScanWorker.h"
#include "C3CompletionStore.h"
#include "C3SymbolManagerRequestResetSymbolStore.h"
#include "C3ExternalCommand.h"

#include <QMutex>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QStringList>
#include <QVariant>
#include <QElapsedTimer>

class C3WorkspacePrivate
{
public:
	bool bDying;

	// This mutex locks changes to the hEditors hash. It does not protect reads!
	// and is used in lockEditor()
	QMutex oEditorsMutex;
	QHash<int,C3Editor *> hEditors;

	QMutex oOpenFileWorkerHashMutex;
	QHash<int,C3WorkspaceOpenFileWorker *> oOpenFileWorkerHash;
	int iModifiedEditorCount;
	QString szFilePath;
	QString szRootPath;
	QString szName;
	QTimer * pHeartbeatTimer;

	// settings
	QString szProjectName;
	QString szFileHeader;
	QStringList lIncludePaths;
	QStringList lFileExclusionPatterns;
	
	C3SettingsUserHighlightingSet hUserHighlightingSet;

	// generic settings, valid also for default workspaces
	QHash<QString,QVariant> hGenericSettings;

	// this mutex protects pFileScanWorker and pFileCompletionStore
	QMutex oFileCompletionMutex;
	C3WorkspaceFileScanWorker * pFileScanWorker;
	C3CompletionStore * pFileCompletionStore;
	
	QList<C3ExternalCommand *> lExternalCommands;
	QString szCachedPassword; // for external commands
	
	C3SymbolManagerRequestResetSymbolStore * pResetSymbolStoreRequest;
	
	QElapsedTimer tAutoSaveTimer;
	qint64 iTimeToNextAutoSave;
	QElapsedTimer tFileScanTimer;
	qint64 iTimeToNextFileScan;
	QElapsedTimer tSymbolScanTimer;
	qint64 iTimeToNextSymbolScan;
	
	
public:
	C3WorkspacePrivate()
		: oEditorsMutex(QMutex::Recursive)
	{
	}
};

#define _p m_pC3W
