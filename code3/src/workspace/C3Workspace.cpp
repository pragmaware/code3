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
#include "C3MainWindow.h"
#include "C3Application.h"
#include "C3WorkspacePropertiesDialog.h"
#include "C3DockFileBrowser.h"
#include "C3DockWorkspaceBrowser.h"
#include "C3WorkspaceFileScanWorker.h"
#include "C3ExternalToolManager.h"
#include "C3Settings.h"
#include "C3JsonUtils.h"
#include "C3TextEditorModeCPPSharedData.h"
#include "C3Settings.h"
#include "C3GeneralActions.h"
#include "C3FileUtils.h"
#include "C3StringUtils.h"
#include "C3TextEditor.h"

#include "C3Workspace_private.h"

#include <QFileInfo>
#include <QRunnable>
#include <QThreadPool>
#include <QHash>
#include <QMutex>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QByteArray>
#include <QTimer>
#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QDateTime>

C3Workspace::C3Workspace()
	: QObject(),
	C3AlertSource(__tr("Workspace"))
{
	_p = new C3WorkspacePrivate();
	_p->bDying = false;
	_p->iModifiedEditorCount = 0;
	_p->pHeartbeatTimer = new QTimer();
	QObject::connect(_p->pHeartbeatTimer,SIGNAL(timeout()),this,SLOT(slotHeartbeat()));
	_p->pHeartbeatTimer->start(10000); // 10 sec
	_p->pFileCompletionStore = NULL;
	_p->pFileScanWorker = NULL;
	_p->pResetSymbolStoreRequest = NULL;
	_p->tAutoSaveTimer.start();
	_p->iTimeToNextAutoSave = 300000; // 5 mins
	_p->tFileScanTimer.start();
	_p->iTimeToNextFileScan = 360000; // 6 mins
	_p->tSymbolScanTimer.start();
	_p->iTimeToNextSymbolScan = 600000; // 10 mins

	// This is an inter-thread signal-slot connection!
	QObject::connect(this,SIGNAL(signalOpenFileWorkerTerminated(const QVariant &)),this,SLOT(slotOpenFileWorkerTerminated(const QVariant &)));
}

C3Workspace::~C3Workspace()
{
	if(_p->pResetSymbolStoreRequest)
		_p->pResetSymbolStoreRequest->abort();

	delete _p->pHeartbeatTimer;
	_p->bDying = true;
	
	abortAllOpenFileWorkers();
	abortSymbolManagerReset();
	abortFileScanWorker();

	// Now there may be still some workers running, but they shouldn't be able to call us anymore (because of abort() above)
	while(_p->hEditors.count() > 0)
	{
		C3Editor * ed = *(_p->hEditors.begin());
		Q_ASSERT(ed);
		closeEditor(ed,false);
	}

	if(_p->pFileCompletionStore)
		delete _p->pFileCompletionStore;

	qDeleteAll(_p->lExternalCommands);

	delete _p;
}

const QString & C3Workspace::cachedPassword() const
{
	return _p->szCachedPassword;
}

void C3Workspace::setCachedPassword(const QString &szPass)
{
	_p->szCachedPassword = szPass;
}

QVariant C3Workspace::genericSetting(const QString &szKey) const
{
	return _p->hGenericSettings.value(szKey,QVariant());
}

void C3Workspace::setGenericSetting(const QString &szKey,const QVariant &vValue)
{
	_p->hGenericSettings.insert(szKey,vValue);
}

C3SettingsUserHighlightingSet * C3Workspace::userHighlightingSet()
{
	return &_p->hUserHighlightingSet;
}

const QString & C3Workspace::projectName() const
{
	return _p->szProjectName;
}

const QString & C3Workspace::fileHeader() const
{
	return _p->szFileHeader;
}

const QString & C3Workspace::rootPath() const
{
	return _p->szRootPath;
}

const QStringList & C3Workspace::includePaths() const
{
	return _p->lIncludePaths;
}

const QStringList & C3Workspace::fileExclusionPatterns() const
{
	return _p->lFileExclusionPatterns;
}

QString * C3Workspace::projectNameOption()
{
	return &(_p->szProjectName);
}

QString * C3Workspace::fileHeaderOption()
{
	return &(_p->szFileHeader);
}

QStringList * C3Workspace::includePathsOption()
{
	return &(_p->lIncludePaths);
}

QStringList * C3Workspace::fileExclusionPatternsOption()
{
	return &(_p->lFileExclusionPatterns);
}

void C3Workspace::abortSymbolManagerReset()
{
	if(!_p->pResetSymbolStoreRequest)
		return;

	QObject::disconnect(
			_p->pResetSymbolStoreRequest,
			SIGNAL(terminated(C3SymbolManagerRequest *)),
			this,
			SLOT(slotResetSymbolStoreRequestTerminated(C3SymbolManagerRequest *))
		);
	_p->pResetSymbolStoreRequest->abort();
	_p->pResetSymbolStoreRequest = NULL; // will be deleted by the symbol manaer
}


void C3Workspace::slotHeartbeat()
{
	if(_p->tAutoSaveTimer.elapsed() > _p->iTimeToNextAutoSave) // 5 mins
	{
		if(isDefault())
		{
			saveDefault();
			return;
		}
	
		QString szError;
	
		if(saveInternal(_p->szFilePath,szError,true))
			return;
			
		error(__tr("Could not auto-save workspace: %1").arg(szError));

		_p->tAutoSaveTimer.restart();
		_p->iTimeToNextAutoSave = 300000; // 5 mins
	}

	if((_p->tFileScanTimer.elapsed() > _p->iTimeToNextFileScan) && (!_p->pFileScanWorker))
	{
		maybeStartFileScanWorker();
	}

	if((_p->tSymbolScanTimer.elapsed() > _p->iTimeToNextSymbolScan) && (!_p->pResetSymbolStoreRequest)) // 10 mins
	{
		startSymbolManagerReset();
	}
}

const QString & C3Workspace::name() const
{
	return _p->szName;
}

const QString & C3Workspace::path() const
{
	return _p->szFilePath;
}

const QList<C3ExternalCommand *> & C3Workspace::externalCommands()
{
	return _p->lExternalCommands;
}

void C3Workspace::setExternalCommands(QList<C3ExternalCommand *> &lCommands)
{
	qDeleteAll(_p->lExternalCommands);
	_p->lExternalCommands = lCommands;
	
	// assign ids
	foreach(C3ExternalCommand * cmd,_p->lExternalCommands)
	{
		if(!cmd->id().isEmpty())
			continue;
		
		for(;;)
		{
			QString szId = QUuid::createUuid().toString();
			bool bFound = false;
			foreach(C3ExternalCommand * oth,_p->lExternalCommands)
			{
				if(oth->id() == szId)
				{
					bFound = true;
					break;
				}
			}
			if(!bFound)
			{
				cmd->setId(szId);
				break;
			}
		}
	}
	
	_p->szCachedPassword = QString(); // clear it
}

const C3ExternalCommand * C3Workspace::externalCommand(const QString &szId)
{
	foreach(C3ExternalCommand * cmd,_p->lExternalCommands)
	{
		if(cmd->id() == szId)
			return cmd;
	}
	return NULL;
}

void C3Workspace::editProperties()
{
	C3WorkspacePropertiesDialog oDialog;

	if(oDialog.exec() != QDialog::Accepted)
		return;

	save();

	emit propertiesChanged();

	// notify all editors

	QHash<int,C3Editor *>::iterator end = _p->hEditors.end();
	for(QHash<int,C3Editor *>::iterator it = _p->hEditors.begin();it != end;++it)
		it.value()->workspacePropertiesChanged();

	abortSymbolManagerReset();
	abortFileScanWorker();
	startSymbolManagerReset();
	maybeStartFileScanWorker(); // FIXME: Don't start with the CTags worker!
}

bool C3Workspace::isDefault()
{
	return _p->szFilePath.isEmpty();
}

QString C3Workspace::defaultWorkspacePath()
{
	QString szPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	QDir dir(szPath);
	if(!dir.exists())
		dir.mkpath(".");

	if(!szPath.endsWith(QDir::separator()))
		szPath.append(QDir::separator());
	szPath.append("default.workspace");

	return szPath;
}

void C3Workspace::saveDefault()
{
	Q_ASSERT(isDefault());
	QString szError;
	
	QString szPath = defaultWorkspacePath();

	//qDebug("Saving default to %s",szPath.toUtf8().data());

	if(!saveInternal(szPath,szError,false))
		warning(__tr("Failed to save the default workspace: %1").arg(szError));
}

void C3Workspace::loadDefault()
{
	Q_ASSERT(isDefault());
	QString szError;
	
	QString szPath = defaultWorkspacePath();

	if(!QFile::exists(szPath))
		return; // not there

	if(!loadInternal(szPath,szError,false))
		warning(__tr("Failed to load the default workspace: %1").arg(szError));

	abortSymbolManagerReset();
	abortFileScanWorker();
	startSymbolManagerReset();
	maybeStartFileScanWorker(); // FIXME: Don't start with the CTags worker!
}

bool C3Workspace::load(const QString &szPath,QString &szError)
{
	if(!loadInternal(szPath,szError,true))
		return false;

	abortSymbolManagerReset();
	abortFileScanWorker();
	startSymbolManagerReset();
	maybeStartFileScanWorker(); // FIXME: Don't start with the CTags worker!
	return true;
}

#define C3_WORKSPACE_CURRENT_VERSION 1

static QStringList known_highlighting_types()
{
	QStringList lHighlightingTypes;

	lHighlightingTypes.append("cppUserKeywords");
	lHighlightingTypes.append("cppUserTypes");
	lHighlightingTypes.append("cppUserObjects");
	lHighlightingTypes.append("cppUserCustom1");
	lHighlightingTypes.append("cppUserCustom2");
	lHighlightingTypes.append("cppUserCustom3");
	
	return lHighlightingTypes;
}

bool C3Workspace::loadInternalUserData(const QString &szPath,QString &szError,bool bLoadingNonDefault)
{
	QFile f(szPath);
	if(!f.open(QFile::ReadOnly))
	{
		szError = __tr("Could not open workspace file for reading: %1").arg(f.error());
		return false;
	}
	
	QByteArray arry = f.readAll();
	
	QJsonParseError err;
	
	QJsonDocument oDoc = QJsonDocument::fromJson(arry,&err);

	if(oDoc.isNull())
	{
		szError = __tr("The workspace user settings file does not seem to be valid: %1").arg(err.errorString());
		return false;
	}

	if(!oDoc.isObject())
	{
		szError = __tr("The workspace user settings file does not seem to be valid: not a JSON object");
		return false;
	}

	QJsonObject oObject = oDoc.object();

	QString szType = oObject.value(__utf8("type")).toString();
	if(szType != __utf8("workspace.user.settings"))
	{
		szError = __tr("The workspace user settings file does not seem to be valid: invalid marker");
		return false;
	}

	int iVersion = oObject.value(__utf8("version")).toInt(0);

	if(iVersion == 0)
	{
		szError = __tr("The workspace user settings file does not seem to be valid: bad or missing version field");
		return false;
	}
	
	if(iVersion > C3_WORKSPACE_CURRENT_VERSION)
	{
		szError = __tr("The workspace user settings file was created by a more recent version of C3");
		return false;
	}

	QFileInfo inf(szPath);

	if(bLoadingNonDefault)
	{
		QJsonValue v = oObject.value(__utf8("editors"));
		if(!v.isArray())
		{
			szError = __tr("The workspace file does not seem to be valid: bad or missing editors array");
			return false;
		}

		QJsonArray aEditors = v.toArray();

		QString szRealPath = inf.canonicalFilePath();
		QString szProjectRoot = C3StringUtils::fileDirectoryFromPath(szRealPath);
		if(!szProjectRoot.endsWith(QDir::separator()))
			szProjectRoot.append(QDir::separator());

		foreach(QJsonValue val,aEditors)
		{
			C3Link lnk(val.toObject());

			QString szFilePath = lnk.path().trimmed();

			if(szFilePath.isEmpty())
				continue;

			// correct the path, if relative to the project root (it should be!)

			if(!C3FileUtils::isAbsolutePath(szFilePath))
			{
				szFilePath = C3FileUtils::mergePathComponents(szProjectRoot,szFilePath);
				lnk.setPath(szFilePath);
			}

			bool bActivate = lnk.value("current").toBool();

			openFileAsync(lnk,false,bActivate); // no history entries
		}
	}

	QJsonObject oFileBrowser = oObject.value(__utf8("fileBrowser")).toObject();
	
	if(C3DockFileBrowser::instance())
		C3DockFileBrowser::instance()->restoreState(oFileBrowser,bLoadingNonDefault);

	QJsonObject oWorkspaceBrowser = oObject.value(__utf8("workspaceBrowser")).toObject();

	if(C3DockWorkspaceBrowser::instance())
		C3DockWorkspaceBrowser::instance()->restoreState(oWorkspaceBrowser);

	return true;
}

bool C3Workspace::loadInternalCommonData(const QString &szPath,QString &szError,bool bLoadingNonDefault)
{
	QFile f(szPath);
	if(!f.open(QFile::ReadOnly))
	{
		szError = __tr("Could not open workspace file for reading: %1").arg(f.error());
		return false;
	}
	
	QByteArray arry = f.readAll();
	
	QJsonParseError err;
	
	QJsonDocument oDoc = QJsonDocument::fromJson(arry,&err);

	if(oDoc.isNull())
	{
		szError = __tr("The workspace file does not seem to be valid: %1").arg(err.errorString());
		return false;
	}

	if(!oDoc.isObject())
	{
		szError = __tr("The workspace file does not seem to be valid: not a JSON object");
		return false;
	}

	QJsonObject oObject = oDoc.object();

	QString szType = oObject.value(__utf8("type")).toString();
	if(szType != __utf8("workspace"))
	{
		szError = __tr("The workspace file does not seem to be valid: invalid marker");
		return false;
	}

	int iVersion = oObject.value(__utf8("version")).toInt(0);

	if(iVersion == 0)
	{
		szError = __tr("The workspace file does not seem to be valid: bad or missing version field");
		return false;
	}
	
	if(iVersion > C3_WORKSPACE_CURRENT_VERSION)
	{
		szError = __tr("The workspace file was created by a more recent version of C3");
		return false;
	}

	QFileInfo inf(szPath);

	QJsonObject oGenericSettings = oObject.value(__utf8("genericSettings")).toObject();
	_p->hGenericSettings = C3JsonUtils::jsonObjectToVariantHash(oGenericSettings);

	qDeleteAll(_p->lExternalCommands);
	_p->lExternalCommands.clear();

	if(bLoadingNonDefault)
	{
		_p->szProjectName = oObject.value(__utf8("projectName")).toString();
		_p->szFileHeader = oObject.value(__utf8("fileHeader")).toString();
		_p->lIncludePaths = C3JsonUtils::jsonArrayToStringList(oObject.value(__utf8("includePaths")).toArray());
		_p->lFileExclusionPatterns = C3JsonUtils::jsonArrayToStringList(oObject.value(__utf8("fileExclusionPatterns")).toArray());
		_p->hUserHighlightingSet.loadFromJsonObject(oObject.value(__utf8("highlighting")).toObject());

		QJsonValue v = oObject.value(__utf8("externalCommands"));
		if(v.isArray())
		{
			QJsonArray aCommands = v.toArray();
			
			foreach(QJsonValue val,aCommands)
			{
				C3ExternalCommand * pCommand = new C3ExternalCommand();
				if(pCommand->decodeJsonObject(val.toObject()))
					_p->lExternalCommands.append(pCommand);
				else
					delete pCommand;
			}
		}

	} else {
		_p->szProjectName = "";
		_p->szFileHeader = C3Settings::instance()->fileHeader();
		_p->hUserHighlightingSet.copyFrom(*C3Settings::instance()->userHighlightingSet());
		_p->lIncludePaths = QStringList();
		_p->lFileExclusionPatterns = C3Settings::instance()->fileExclusionPatterns();
	}

	if(bLoadingNonDefault)
	{
		_p->szFilePath = inf.canonicalFilePath();
		_p->szName = inf.fileName();
		_p->szRootPath = inf.absolutePath();

		C3Settings::instance()->addRecentWorkspace(inf.canonicalFilePath());
	}

	return true;
}

bool C3Workspace::loadInternal(const QString &szPath,QString &szError,bool bLoadingNonDefault)
{
	if(!loadInternalCommonData(szPath,szError,bLoadingNonDefault))
		return false;
	
	QString szUserPath = QString("%1.user").arg(szPath);
	
	if(QFile::exists(szUserPath))
	{
		if(!loadInternalUserData(szUserPath,szError,bLoadingNonDefault))
		{
			qWarning("Failed to load the user workspace data at %s: ignoring",szUserPath.toUtf8().data());
			if(C3DockFileBrowser::instance())
				C3DockFileBrowser::instance()->goToWorkspaceRoot();
		}
	}

	return true;
}

bool C3Workspace::saveInternalUserData(const QString &szPath,QString &szError,bool bSavingNonDefault)
{
	QJsonObject oObject;
	
	oObject.insert(__utf8("version"),C3_WORKSPACE_CURRENT_VERSION);
	oObject.insert(__utf8("type"),__utf8("workspace.user.settings"));

	QFileInfo inf(szPath);

	// Try to have a canonical path
	QString szRealPath = inf.canonicalFilePath();
	if(szRealPath.isEmpty())
	{
		szRealPath = inf.absoluteFilePath();
		if(szRealPath.isEmpty())
			szRealPath = szPath; // doh
	}

	if(bSavingNonDefault)
	{
		QJsonArray aEditors;
	
		QString szProjectRoot = C3StringUtils::fileDirectoryFromPath(szRealPath);
		if(!szProjectRoot.endsWith(QDir::separator()))
			szProjectRoot.append(QDir::separator());
	
		C3Editor * pCurrentEditor = C3MainWindow::instance()->currentEditor();
	
		foreach(C3Editor * ed,_p->hEditors)
		{
			QJsonObject ob;
			if(!ed->saveState(ob))
				continue; // doh?

			// correct the path to be relative to the project root
			QString szEditorPath = ob.value("path").toString();
			if(szEditorPath.startsWith(szProjectRoot))
			{
				szEditorPath = szEditorPath.mid(szProjectRoot.length());
				ob.insert("path",szEditorPath);
			}

			if(ed == pCurrentEditor)
			{
				//qDebug("Editor %s will be current",szEditorPath.toUtf8().data());
				ob.insert("current",true);
			}

			aEditors.append(ob);
		}

		oObject.insert(__utf8("editors"),aEditors);
	}

	QJsonObject oFileBrowser;

	if(C3DockFileBrowser::instance())
		C3DockFileBrowser::instance()->saveState(oFileBrowser);

	oObject.insert(__utf8("fileBrowser"),oFileBrowser);

	QJsonObject oWorkspaceBrowser;

	if(C3DockWorkspaceBrowser::instance())
		C3DockWorkspaceBrowser::instance()->saveState(oWorkspaceBrowser);

	oObject.insert(__utf8("workspaceBrowser"),oWorkspaceBrowser);

	///////////////////////////////////////////////////////////////////////////////////////////

	QJsonDocument oDoc;
	oDoc.setObject(oObject);

	///////////////////////////////////////////////////////////////////////////////////////////


	QByteArray arry = oDoc.toJson(QJsonDocument::Indented);

	QFile f(szRealPath);
	if(!f.open(QFile::WriteOnly))
	{
		szError = __tr("Could not open workspace file for writing: %1").arg(f.error());
		return false;
	}

	// FIXME: Write to temporary and move!
	if(f.write(arry) != arry.size())
	{
		szError = __tr("Could not write workspace file: %1").arg(f.error());
		return false;
	}

	return true;

}

bool C3Workspace::saveInternalCommonData(const QString &szPath,QString &szError,bool bSavingNonDefault)
{
	QJsonObject oObject;
	
	oObject.insert(__utf8("version"),C3_WORKSPACE_CURRENT_VERSION);
	oObject.insert(__utf8("type"),__utf8("workspace"));

	QFileInfo inf(szPath);

	// Try to have a canonical path
	QString szRealPath = inf.canonicalFilePath();
	if(szRealPath.isEmpty())
	{
		szRealPath = inf.absoluteFilePath();
		if(szRealPath.isEmpty())
			szRealPath = szPath; // doh
	}

	if(bSavingNonDefault)
	{
		oObject.insert(__utf8("projectName"),_p->szProjectName);
		oObject.insert(__utf8("fileHeader"),_p->szFileHeader);
		oObject.insert(__utf8("includePaths"),QJsonArray::fromStringList(_p->lIncludePaths));
		oObject.insert(__utf8("fileExclusionPatterns"),QJsonArray::fromStringList(_p->lFileExclusionPatterns));

		QJsonObject oHighlighting;
		_p->hUserHighlightingSet.saveToJsonObject(oHighlighting);
		oObject.insert(__utf8("highlighting"),oHighlighting);

		QJsonArray aCommands;
		foreach(C3ExternalCommand * pCommand,_p->lExternalCommands)
			aCommands.append(pCommand->toJsonObject());

		oObject.insert(__utf8("externalCommands"),aCommands);
	}

	oObject.insert(__utf8("genericSettings"),C3JsonUtils::variantHashToJsonObject(_p->hGenericSettings));

	///////////////////////////////////////////////////////////////////////////////////////////

	QJsonDocument oDoc;
	oDoc.setObject(oObject);

	///////////////////////////////////////////////////////////////////////////////////////////

	QByteArray arry = oDoc.toJson(QJsonDocument::Indented);

	QFile f(szRealPath);
	if(!f.open(QFile::WriteOnly))
	{
		szError = __tr("Could not open workspace file for writing: %1").arg(f.error());
		return false;
	}

	// FIXME: Write to temporary and move!
	if(f.write(arry) != arry.size())
	{
		szError = __tr("Could not write workspace file: %1").arg(f.error());
		return false;
	}

	if(bSavingNonDefault)
	{
		_p->szFilePath = szRealPath;
		_p->szName = inf.fileName();
		_p->szRootPath = inf.absolutePath();

		C3Settings::instance()->addRecentWorkspace(szRealPath);
	}

	return true;
}


bool C3Workspace::saveInternal(const QString &szPath,QString &szError,bool bSavingNonDefault)
{
	if(!saveInternalCommonData(szPath,szError,bSavingNonDefault))
		return false;
	
	QString szUserPath = QString("%1.user").arg(szPath);
	
	if(!saveInternalUserData(szUserPath,szError,bSavingNonDefault))
		return false;
	
	return true;
}

bool C3Workspace::save()
{
	if(_p->szFilePath.isEmpty())
		return saveAs();

	QString szError;

	if(saveInternal(_p->szFilePath,szError,true))
		return true;

	QMessageBox::StandardButton eButton = QMessageBox::critical(
			C3MainWindow::instance(),
			__tr("Workspace Saving Failed"),
			__tr("Failed to save the workspace file at '%1': %2").arg(_p->szFilePath).arg(szError),
			QMessageBox::Cancel | QMessageBox::Retry
		);

	switch(eButton)
	{
		case QMessageBox::Retry:
			return saveAs();
		break;
		//case QMessageBox::Cancel:
		default:
			// fall down
		break;
	}

	return false;
}

bool C3Workspace::saveAs()
{
	bool bWasDefault = isDefault();

	for(;;)
	{
		QStringList lNameFilters;
		
		lNameFilters << __tr("Code-3 Workspace Files (*.c3workspace)");
		lNameFilters << __tr("Code-3 Old Workspace Files (*.ssexwsp)");

		QFileDialog dlg(
			C3MainWindow::instance(),
			__tr("Select File Name for Workspace"),
			C3MainWindow::instance()->currentDirectory()
		);
		
		dlg.setOptions(QFileDialog::DontUseNativeDialog);
		dlg.setFileMode(QFileDialog::AnyFile);
		dlg.setAcceptMode(QFileDialog::AcceptSave);
		dlg.setNameFilters(lNameFilters);
		dlg.selectNameFilter(lNameFilters.at(0));
		dlg.setDefaultSuffix(__utf8("c3workspace"));

		if(!_p->szFilePath.isEmpty())
			dlg.selectFile(_p->szFilePath);

		if(dlg.exec() != QDialog::Accepted)
			return false;

		QStringList lFiles = dlg.selectedFiles();
		if(lFiles.count() < 1)
			continue; // no files selected?

		QString szPath = lFiles.at(0);
		if(szPath.isEmpty())
			continue; // ???

		QString szError;

		if(bWasDefault)
			setNonDefaultWorkspaceDefaultValues(szPath);

		if(saveInternal(szPath,szError,true))
			break;

		QMessageBox::StandardButton eButton = QMessageBox::critical(
				C3MainWindow::instance(),
				__tr("Workspace Saving Failed"),
				__tr("Failed to save the workspace file at '%1': %2").arg(szPath).arg(szError),
				QMessageBox::Cancel | QMessageBox::Retry
			);

		switch(eButton)
		{
			case QMessageBox::Cancel:
				return false;
			break;
			//case QMessageBox::Retry:
			default:
				// continue
			break;
		}
		// Try again.
	}

	abortSymbolManagerReset();
	abortFileScanWorker();
	startSymbolManagerReset();
	maybeStartFileScanWorker(); // FIXME: Don't start with the CTags worker!
	return true;
}

void C3Workspace::setNonDefaultWorkspaceDefaultValues(const QString &szPath)
{
	QFileInfo inf(szPath);
	
	_p->szProjectName = inf.baseName();
	_p->szFileHeader = C3Settings::instance()->fileHeader();
	_p->hUserHighlightingSet.copyFrom(*C3Settings::instance()->userHighlightingSet());
	_p->lFileExclusionPatterns = C3Settings::instance()->fileExclusionPatterns();
	_p->lIncludePaths = QStringList();
}

void C3Workspace::generalSettingsChanged()
{
	QHash<int,C3Editor *>::iterator end = _p->hEditors.end();
	QHash<int,C3Editor *>::iterator it;
	for(it = _p->hEditors.begin();it != end;++it)
		it.value()->generalSettingsChanged();

	if(!isDefault())
		return;

	_p->szFileHeader = C3Settings::instance()->fileHeader();
	_p->lFileExclusionPatterns = C3Settings::instance()->fileExclusionPatterns();
	_p->hUserHighlightingSet.copyFrom(*C3Settings::instance()->userHighlightingSet());

	emit propertiesChanged();
	// notify all editors
	
	end = _p->hEditors.end();
	for(it = _p->hEditors.begin();it != end;++it)
		it.value()->workspacePropertiesChanged();
}


bool C3Workspace::close(bool bCanAbort)
{
	if(isDefault())
	{
		saveDefault();
	} else {
		if(!save())
		{
			if(bCanAbort)
				return false;
			
			error("Failed to save the workspace: it will remain unsaved");
		}
	}

	while(_p->hEditors.count() > 0)
	{
		C3Editor * ed = *(_p->hEditors.begin());
		Q_ASSERT(ed);

		if(!closeEditor(ed,bCanAbort))
		{
			if(bCanAbort)
				return false;
			
			error("One of the editors failed to close properly");
		}
	}

	_p->szFilePath = QString();
	_p->szName = QString();
	qDeleteAll(_p->lExternalCommands);
	_p->lExternalCommands.clear();

	abortSymbolManagerReset();
	abortFileScanWorker();

	return true;
}

int C3Workspace::modifiedEditorCount()
{
	return _p->iModifiedEditorCount;
}

C3Workspace * C3Workspace::currentWorkspace()
{
	return C3MainWindow::instance()->workspace();
}

QHash<int,C3Editor *> & C3Workspace::editors()
{
	// By contract the caller should never modify the hash itself.
	return _p->hEditors;
}

void C3Workspace::slotEditorModifiedStateChanged(C3Editor * pEditor)
{
	if(pEditor->modified())
		_p->iModifiedEditorCount++;
	else
		_p->iModifiedEditorCount--;

	C3MainWindow::instance()->editorModifiedStateChanged(pEditor);
}

void C3Workspace::slotEditorTitleChanged(C3Editor * pEditor)
{
	C3MainWindow::instance()->editorTitleChanged(pEditor);
}

void C3Workspace::slotEditorReadOnlyStateChanged(C3Editor * pEditor)
{
	C3MainWindow::instance()->editorReadOnlyStateChanged(pEditor);
}

void C3Workspace::slotEditorPathChanged(C3Editor * pEditor)
{
	C3MainWindow::instance()->editorPathChanged(pEditor);
}

bool C3Workspace::closeEditor(C3Editor * pEditor,bool bCanAbort)
{
	Q_ASSERT(pEditor);

	if(!pEditor->closeFile(bCanAbort))
	{
		if(bCanAbort)
			return false;
	}

	_p->oEditorsMutex.lock();
	_p->hEditors.remove(pEditor->id());
	_p->oEditorsMutex.unlock();

	if(pEditor->modified())
		_p->iModifiedEditorCount--;

	C3MainWindow::instance()->killEditor(pEditor);

	return true;
}

C3Editor * C3Workspace::newFile(const C3Link &oLink,bool bAddHistoryEntry)
{
	if(_p->bDying)
		return NULL;
	
	QString szFactoryId = oLink.editorFactoryId();

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
		error(__tr("Could not create new file: editor with factory id '%2' not found").arg(szFactoryId));
		return NULL;
	}

	C3Editor * pEditor = pFactory->createEditor(
			oLink,
			C3MainWindow::instance()->parentForEditors()
		);

	if(!pEditor)
	{
		error(__tr("Failed to create the editor"));
		return NULL;
	}

	if(bAddHistoryEntry)
		C3MainWindow::instance()->addHistoryEntry();

	QObject::connect(pEditor,SIGNAL(modifiedStateChanged(C3Editor *)),this,SLOT(slotEditorModifiedStateChanged(C3Editor *)));
	QObject::connect(pEditor,SIGNAL(titleChanged(C3Editor *)),this,SLOT(slotEditorTitleChanged(C3Editor *)));
	QObject::connect(pEditor,SIGNAL(readOnlyStateChanged(C3Editor *)),this,SLOT(slotEditorReadOnlyStateChanged(C3Editor *)));
	QObject::connect(pEditor,SIGNAL(pathChanged(C3Editor *)),this,SLOT(slotEditorPathChanged(C3Editor *)));

	_p->oEditorsMutex.lock();
	_p->hEditors.insert(pEditor->id(),pEditor);
	_p->oEditorsMutex.unlock();

	C3MainWindow::instance()->addEditor(pEditor,true);

	activateEditor(pEditor);
	
	return pEditor;
}

void C3Workspace::activateEditor(C3Editor * pEditor)
{
	C3MainWindow::instance()->activateEditor(pEditor);
}


C3Editor * C3Workspace::findEditor(const QString &szFactoryId,const QString &szFilePath)
{
	foreach(C3Editor * e,_p->hEditors)
	{
		if(e->path() != szFilePath)
			continue;
		if(szFactoryId.isEmpty())
			return e;
		if(e->factoryId() == szFactoryId)
			return e;
	}
	
	return NULL;
}

void C3Workspace::findAllEditors(const QString &szFilePath,QList<C3Editor *> &lBuffer)
{
	foreach(C3Editor * e,_p->hEditors)
	{
		if(e->path() != szFilePath)
			continue;
		lBuffer.append(e);
	}
}

void C3Workspace::findAllEditorsInDirectory(const QString &szDirectoryPath,QList<C3Editor *> &lBuffer)
{
	QString szPath = szDirectoryPath;
	if(!szPath.endsWith(QDir::separator()))
		szPath.append(QDir::separator());

	foreach(C3Editor * e,_p->hEditors)
	{
		if(!e->path().startsWith(szPath))
			continue;
		lBuffer.append(e);
	}
}

C3Editor * C3Workspace::findEditor(int id)
{
	return _p->hEditors.value(id,NULL);
}

C3Editor * C3Workspace::lockEditor(int id)
{
	_p->oEditorsMutex.lock();
	C3Editor * ed = _p->hEditors.value(id,NULL);
	if(!ed)
	{
		_p->oEditorsMutex.unlock();
		return NULL;
	}
	return ed;
}

void C3Workspace::unlockEditor(C3Editor * ed)
{
	if(!ed)
		return;
	Q_ASSERT(_p->hEditors.value(ed->id(),NULL) == ed);
	_p->oEditorsMutex.unlock();
}

void C3Workspace::computeListOfModifiedTextEditors(QList<int> &lIds)
{
	lIds.clear();

	_p->oEditorsMutex.lock();
	for(QHash<int,C3Editor *>::Iterator it = _p->hEditors.begin();it != _p->hEditors.end();++it)
	{
		C3TextEditor * ed = dynamic_cast<C3TextEditor *>(it.value());
		if(!ed)
			continue;
		if(!ed->modified())
			continue;
		lIds.append(ed->id());
	}
	_p->oEditorsMutex.unlock();
}


void C3Workspace::startSymbolManagerReset()
{
	QStringList lPaths;
	
	unsigned int uFlags = 0;
	
	if(isDefault())
	{
		if(_p->hEditors.count() == 1)
		{
			// single editor, scan its path
			_p->oEditorsMutex.lock();
			C3Editor * ed = *(_p->hEditors.begin());
			QString szDir = ed->directory();
			if(!szDir.isEmpty())
				lPaths.append(szDir);
			_p->oEditorsMutex.unlock();
		}
		lPaths.append(QDir::currentPath());
		uFlags |= C3SymbolManagerRequestResetSymbolStore::LimitRecursionDepthTo1;
	} else {
		lPaths.append(_p->szRootPath);
		lPaths.append(_p->lIncludePaths);
	}
	
	if(_p->pResetSymbolStoreRequest)
		abortSymbolManagerReset();
	
	_p->pResetSymbolStoreRequest = new C3SymbolManagerRequestResetSymbolStore(lPaths,_p->lFileExclusionPatterns,uFlags);

	QObject::connect(
			_p->pResetSymbolStoreRequest,
			SIGNAL(terminated(C3SymbolManagerRequest *)),
			this,
			SLOT(slotResetSymbolStoreRequestTerminated(C3SymbolManagerRequest *))
		);

	_p->pResetSymbolStoreRequest->start();

	_p->tSymbolScanTimer.restart();
	_p->iTimeToNextSymbolScan = 600000; // 10 min
}

void C3Workspace::slotResetSymbolStoreRequestTerminated(C3SymbolManagerRequest * pRequest)
{
	Q_ASSERT(_p->pResetSymbolStoreRequest == pRequest);
	_p->pResetSymbolStoreRequest = NULL;
}

void C3Workspace::getFileCompletions(const QString &szPart,QStringList &lBuffer)
{
	_p->oFileCompletionMutex.lock();
	if(!_p->pFileCompletionStore)
	{
		_p->oFileCompletionMutex.unlock();
		return;
	}

	_p->pFileCompletionStore->getCompletions(szPart,lBuffer);
	
	_p->oFileCompletionMutex.unlock();
}

void C3Workspace::matchFiles(const QString &szFilePart,QList<QString> &lRet)
{
	if(isDefault())
	{
		matchFilesRecursive(szFilePart,C3DockFileBrowser::instance()->directory(),lRet,0);
		return;
	}

	matchFilesRecursive(szFilePart,_p->szRootPath,lRet,0);
}


void C3Workspace::matchFilesRecursive(const QString &szFilePart,const QString &szPath,QList<QString> &lRet,int iCurrentDepth)
{
	QDir d(szPath);

	QFileInfoList fil = d.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	foreach(QFileInfo fi,fil)
	{
		if(fi.isDir())
		{
			if(iCurrentDepth < 12)
				matchFilesRecursive(szFilePart,fi.absoluteFilePath(),lRet,iCurrentDepth + 1);
			continue;
		}

		if(fi.fileName().contains(szFilePart) || fi.absoluteFilePath().startsWith(szFilePart))
			lRet.append(fi.absoluteFilePath());
	}
}

QString C3Workspace::checkFileExistence(const QString &szFileName,const QString &szPath)
{
	QString szFullPath = szPath;
	if(!szFullPath.endsWith(QDir::separator()))
		szFullPath.append(QDir::separator());
	szFullPath.append(szFileName);
	if(QFile::exists(szFullPath))
		return szFullPath;
	return QString();
}

QString C3Workspace::findFileRecursive(const QString &szFileName,const QString &szPath,int iCurrentDepth)
{
	//qDebug("Find file recursive in %s",szPath.toUtf8().data());

	QString szRet;

	if(iCurrentDepth > 12)
		return szRet; // too deep
	
	szRet = checkFileExistence(szFileName,szPath);
	if(!szRet.isEmpty())
		return szRet;
	
	QDir d(szPath);

	QFileInfoList fil = d.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	foreach(QFileInfo fi,fil)
	{
		if(!fi.isDir())
			continue; // ?
	
		szRet = findFileRecursive(szFileName,fi.absoluteFilePath(),iCurrentDepth + 1);
		if(!szRet.isEmpty())
			return szRet;
	}

	return QString();
}

QString C3Workspace::findFile(const QString &szFileName,const QString &szPathHint,bool bMustExistInCompletionStore)
{
	//qDebug("Find file %s",szFileName.toUtf8().data());

	QString szRet;

	if(szFileName.isEmpty())
		return szRet;
	
	if(!szPathHint.isEmpty())
	{
		szRet = findFileRecursive(szFileName,szPathHint);
		if(!szRet.isEmpty())
			return szRet;
	}
	
	if(isDefault())
		return szRet; // FIXME: maybe look in the current directory (if different than szPathHint) ?

	if(bMustExistInCompletionStore)
	{
		if(!_p->pFileCompletionStore)
			return szRet; // no completion store here
	
		if(!_p->pFileCompletionStore->findExact(szFileName))
		{
			//qDebug("Not found in completion store");
			return szRet; // not there at all.
		}
	}

	szRet = findFileRecursive(szFileName,_p->szRootPath);
	if(!szRet.isEmpty())
		return szRet;

	foreach(QString s,_p->lIncludePaths)
	{
		szRet = findFileRecursive(szFileName,s);
		if(!szRet.isEmpty())
			return szRet;
	}
	
	if(bMustExistInCompletionStore)
	{
		// hum hum... was in the completion store but no longer found???
		qDebug("File %s was in completion store but I can't find it anymore",szFileName.toUtf8().data());
	}
	
	return szRet;
}


QString C3Workspace::substituteVariables(
		const QString &szText,
		QHash<QString,QString> &hAdditionalParams
	)
{
	QString szRet;

	const QChar * b = szText.unicode();
	const QChar * p = b;
	const QChar * e = b + szText.length();
	
	while(p < e)
	{
		if(p->unicode() != (ushort)'%')
		{
			p++;
			continue;
		}

		if(p > b)
		{
			szRet.append(b,p-b);
			b = p;
		}

		p++;
		if(p >= e)
			break;

		if(p->unicode() != (ushort)'%')
		{
			p++;
			continue;
		}

		p++;
		if(p >= e)
			break;

		const QChar * t1 = p;

		while((p < e) && (p->unicode() != (ushort)'%'))
			p++;
		
		if(p >= e)
			break;
		
		const QChar * t2 = p;

		p++;

		if(p >= e)
			break;

		if(p->unicode() != (ushort)'%')
		{
			p++;
			continue;
		}
		
		p++;
		
		// gotcha
		QString szToken = QString(t1,t2-t1).toLower();
		
		QString szValue = hAdditionalParams.value(szToken);
		if(szValue.isEmpty())
		{
			static QString szAuthor = __utf8("author");
			static QString szEMail = __utf8("email");
			static QString szFirstName = __utf8("firstname");
			static QString szLastName = __utf8("lastname");
			static QString szDate = __utf8("date");
			static QString szProject = __utf8("project");
			static QString szWorkspaceRoot = __utf8("workspaceroot");
		
			if(szToken == szAuthor)
				szRet.append(C3Settings::instance()->userFullName());
			else if(szToken == szEMail)
				szRet.append(C3Settings::instance()->userEMail());
			else if(szToken == szFirstName)
				szRet.append(C3Settings::instance()->userFirstName());
			else if(szToken == szLastName)
				szRet.append(C3Settings::instance()->userLastName());
			else if(szToken == szDate)
				szRet.append(QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));
			else if(szToken == szProject)
				szRet.append(C3Workspace::currentWorkspace()->projectName());
			else if(szToken == szWorkspaceRoot)
				szRet.append(C3Workspace::currentWorkspace()->rootPath());
		} else {
			szRet.append(szValue);
		}
		
		b = p;
	}
	
	if(p > b)
		szRet.append(b,p-b);
	
	return szRet;
}


QString C3Workspace::toWorkspaceDependantPath(const QString &szPath)
{
	if(_p->szRootPath.isEmpty())
		return szPath;
	
	QString szProjectRoot = _p->szRootPath;
	if(!szProjectRoot.endsWith(QDir::separator()))
		szProjectRoot.append(QDir::separator());
	
	if(!szPath.startsWith(szProjectRoot))
		return szPath;

	return szPath.mid(szProjectRoot.length());
}

void C3Workspace::notifyRescanNeeded()
{
	// The values below are fixed since this notification is likely to be repeated
	_p->iTimeToNextSymbolScan = 30000; // fixed 30 seconds
	_p->iTimeToNextFileScan = 20000; // fixed 20 seconds
}
