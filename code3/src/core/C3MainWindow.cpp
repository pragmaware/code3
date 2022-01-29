//=============================================================================
//
//   File : C3MainWindow.cpp
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

#include "C3MainWindow.h"

#include "C3DockFileBrowser.h"
#include "C3DockWorkspaceBrowser.h"
#include "C3Workspace.h"
#include "C3Editor.h"
#include "C3CentralWidget.h"
#include "C3PixmapCache.h"
#include "C3GeneralActions.h"
#include "C3FindWidget.h"
#include "C3Settings.h"
#include "C3NewDialog.h"
#include "C3SettingsDialog.h"
#include "C3DockTextConsole.h"
#include "C3ExternalCommand.h"
#include "C3ExternalCommandManager.h"
#include "C3LinkHistory.h"
#include "C3SymbolManager.h"
#include "C3ExternalToolManager.h"
#include "C3Application.h"
#include "C3EditorFactory.h"

#include <QStackedWidget>
#include <QSettings>
#include <QStatusBar>
#include <QMenuBar>
#include <QLabel>
#include <QToolBar>
#include <QMenu>
#include <QCloseEvent>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QPixmap>
#include <QComboBox>
#include <QWindow>

C3MainWindow * C3MainWindow::m_pInstance = NULL;

class C3MainWindowPrivate
{
public:
	C3CentralWidget * pCentralWidget;
	QStackedWidget * pStackedWidget;
	
	C3GeneralActions * pGeneralActions;

	C3DockFileBrowser * pFileBrowser;
	C3DockWorkspaceBrowser * pWorkspaceBrowser;
	C3DockTextConsole * pTextConsole;
	
	C3LinkHistory * pLinkHistory;
	
	C3Workspace * pWorkspace;
	
	C3Settings * pSettings;

	C3ExternalCommandManager * pExternalCommandManager;
	
	C3SymbolManager * pSymbolManager;
	
	QToolBar * pToolBar;
	QComboBox * pExternalCommandsComboBox;
	QWidget * pToolBarSpacer;
	
	
	QStatusBar * pStatusBar;
	QLabel * pFilePathLabel;
	QLabel * pPrivateInfoText1Label;
	QLabel * pPrivateInfoText2Label;
	
	C3Editor * pCurrentEditor;
	
	QMenuBar * pMenuBar;
	
	QMenu * pWorkspaceMenu;
	QMenu * pFileMenu;
	QMenu * pEditMenu;
	QMenu * pViewMenu;
	QMenu * pFXMenu;
	QMenu * pRecentWorkspacesMenu;
	QMenu * pRecentFilesMenu;
	QMenu * pSettingsMenu;
	
	QList<QAction *> lExternalCommandControls;
	
	bool bFirstShow;
	bool bShuttingDown;
};

#define _p m_pC3MW

C3MainWindow::C3MainWindow()
	: QMainWindow()
{
	m_pInstance = this;

	_p = new C3MainWindowPrivate();

	_p->bFirstShow = true;

	_p->pCurrentEditor = NULL;
	_p->pTextConsole = NULL;
	_p->bShuttingDown = false;

	_p->pSettings = new C3Settings();

	_p->pSettings->loadRecent();
	_p->pSettings->load();

	_p->pExternalCommandManager = new C3ExternalCommandManager();

	_p->pLinkHistory = new C3LinkHistory();

	_p->pWorkspace = new C3Workspace(); // empty workspace
	QObject::connect(_p->pWorkspace,SIGNAL(propertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
	
	_p->pSymbolManager = new C3SymbolManager(C3ExternalToolManager::instance()->findTool(__utf8("code3tags")));
	_p->pSymbolManager->start();

	_p->pGeneralActions = new C3GeneralActions(this);

	setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);

	setDockOptions(QMainWindow::VerticalTabs);

	_p->pCentralWidget = new C3CentralWidget(this);

	setCentralWidget(_p->pCentralWidget);
	
	//setDockOptions(QMainWindow::VerticalTabs | QMainWindow::AllowNestedDocks);
	
	_p->pStackedWidget = _p->pCentralWidget->stackedWidget();
	QObject::connect(_p->pStackedWidget,SIGNAL(currentChanged(int)),this,SLOT(slotStackedWidgetCurrentChanged(int)));

	_p->pFileBrowser = new C3DockFileBrowser(this);

	addDockWidget(Qt::LeftDockWidgetArea,_p->pFileBrowser);

	_p->pWorkspaceBrowser = new C3DockWorkspaceBrowser(this);

	addDockWidget(Qt::LeftDockWidgetArea,_p->pWorkspaceBrowser);
	
	_p->pStatusBar = statusBar();
	
	_p->pFilePathLabel = new QLabel(_p->pStatusBar);
	
	_p->pStatusBar->addWidget(_p->pFilePathLabel);
	
	_p->pPrivateInfoText1Label = new QLabel(_p->pStatusBar);
	_p->pPrivateInfoText1Label->setText(__utf8("-"));

	_p->pStatusBar->addPermanentWidget(_p->pPrivateInfoText1Label);

	_p->pPrivateInfoText2Label = new QLabel(_p->pStatusBar);
	_p->pPrivateInfoText2Label->setText(__utf8("-"));

	_p->pStatusBar->addPermanentWidget(_p->pPrivateInfoText2Label);
	
	createMenuBar();

	createToolBar();

	addToolBar(Qt::TopToolBarArea,_p->pToolBar);
	
	loadWindowState();

	_p->pWorkspaceBrowser->attachToWorkspace(workspace());

	updateFilePathLabel(NULL);
	
	_p->pGeneralActions->workspaceChanged(_p->pWorkspace);
	_p->pGeneralActions->currentEditorChanged(NULL);

	updateCaption();
}

C3MainWindow::~C3MainWindow()
{
	_p->bShuttingDown = true;

	_p->pSymbolManager->requestTermination();

	delete _p->pWorkspace;

	qDeleteAll(_p->lExternalCommandControls);

	delete _p->pGeneralActions;
	delete _p->pLinkHistory;
	delete _p->pExternalCommandManager;
	delete _p->pSettings;

	_p->pSymbolManager->wait();
	delete _p->pSymbolManager;

	delete _p;
	
	m_pInstance = NULL;
}

void C3MainWindow::showEvent(QShowEvent * e)
{
	if(_p->bFirstShow)
	{
		windowHandle()->setIcon(C3PixmapCache::instance()->pixmap(__literal("icon/icon64.png")));
		_p->bFirstShow = false;
	}
}

C3DockTextConsole * C3MainWindow::textConsole()
{
	if(!_p->pTextConsole)
	{
		_p->pTextConsole = new C3DockTextConsole(this);
		addDockWidget(Qt::BottomDockWidgetArea,_p->pTextConsole);
	}
	return _p->pTextConsole;
}

void C3MainWindow::closeTextConsole()
{
	if(!_p->pTextConsole)
		return;
	delete _p->pTextConsole;
	_p->pTextConsole = NULL;
}

void C3MainWindow::createMenuBar()
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	_p->pMenuBar = menuBar();


	_p->pWorkspaceMenu = _p->pMenuBar->addMenu(__tr("Workspace"));

	_p->pWorkspaceMenu->addAction(_p->pGeneralActions->workspaceOpenAction());

	_p->pRecentWorkspacesMenu = _p->pWorkspaceMenu->addMenu(pPixmapCache->pixmap(__utf8("general/20px/document-open-recent.png")),__tr("Open Recent"));
	QObject::connect(_p->pRecentWorkspacesMenu,SIGNAL(aboutToShow()),this,SLOT(slotRecentWorkspacesMenuAboutToShow()));

	_p->pWorkspaceMenu->addAction(_p->pGeneralActions->workspaceSaveAsAction());
	_p->pWorkspaceMenu->addAction(_p->pGeneralActions->workspacePropertiesAction());
	_p->pWorkspaceMenu->addAction(_p->pGeneralActions->workspaceCloseAction());
	


	_p->pFileMenu = _p->pMenuBar->addMenu(__tr("File"));

	_p->pFileMenu->addAction(_p->pGeneralActions->documentNewAction());
	_p->pFileMenu->addAction(_p->pGeneralActions->documentOpenAction());


	_p->pRecentFilesMenu = _p->pFileMenu->addMenu(pPixmapCache->pixmap(__utf8("general/20px/document-open-recent.png")),__tr("Open Recent"));
	QObject::connect(_p->pRecentFilesMenu,SIGNAL(aboutToShow()),this,SLOT(slotRecentFilesMenuAboutToShow()));

	_p->pFileMenu->addSeparator();
	_p->pFileMenu->addAction(_p->pGeneralActions->documentSaveAction());
	_p->pFileMenu->addAction(_p->pGeneralActions->documentSaveAsAction());
	_p->pFileMenu->addAction(_p->pGeneralActions->documentSaveAllAction());
	_p->pFileMenu->addSeparator();
	_p->pFileMenu->addAction(_p->pGeneralActions->documentCloseAction());

	_p->pEditMenu = _p->pMenuBar->addMenu(__tr("Edit"));
	_p->pEditMenu->addAction(_p->pGeneralActions->toggleFindWidgetAction());
	_p->pEditMenu->addAction(_p->pGeneralActions->toggleFindWidgetGrepAction());

	QObject::connect(_p->pEditMenu,SIGNAL(aboutToShow()),this,SLOT(slotEditMenuAboutToShow()));

	_p->pViewMenu = _p->pMenuBar->addMenu(__tr("View"));

	QObject::connect(_p->pViewMenu,SIGNAL(aboutToShow()),this,SLOT(slotViewMenuAboutToShow()));

	_p->pFXMenu = _p->pMenuBar->addMenu(__tr("FX"));

	QObject::connect(_p->pFXMenu,SIGNAL(aboutToShow()),this,SLOT(slotFXMenuAboutToShow()));


	_p->pSettingsMenu = _p->pMenuBar->addMenu(__tr("Settings"));

	_p->pSettingsMenu->addAction(_p->pGeneralActions->configureC3Action());

}

void C3MainWindow::slotRecentWorkspacesMenuAboutToShow()
{
	_p->pRecentWorkspacesMenu->clear();

	QStringList sl = _p->pSettings->recentWorkspaces();
	
	foreach(QString s,sl)
	{
		QAction * a = _p->pRecentWorkspacesMenu->addAction(s,this,SLOT(slotOpenRecentWorkspace()));
		a->setData(s);
	}
}

void C3MainWindow::slotRecentFilesMenuAboutToShow()
{
	_p->pRecentFilesMenu->clear();

	QStringList sl = _p->pSettings->recentFiles();
	
	foreach(QString s,sl)
	{
		QAction * a = _p->pRecentFilesMenu->addAction(s,this,SLOT(slotOpenRecentFile()));
		a->setData(s);
	}
}

void C3MainWindow::slotViewMenuAboutToShow()
{
	_p->pViewMenu->clear();
	
	if(!_p->pCurrentEditor)
		return;
	
	_p->pCurrentEditor->fillViewMenu(_p->pViewMenu);
}

void C3MainWindow::slotEditMenuAboutToShow()
{
	_p->pEditMenu->clear();
	
	if(_p->pCurrentEditor)
	{
		_p->pCurrentEditor->fillEditMenu(_p->pEditMenu);
		_p->pEditMenu->addSeparator();
	}

	_p->pEditMenu->addAction(_p->pGeneralActions->toggleFindWidgetAction());
	_p->pEditMenu->addAction(_p->pGeneralActions->toggleFindWidgetGrepAction());
}

void C3MainWindow::slotFXMenuAboutToShow()
{
	_p->pFXMenu->clear();
	
	if(!_p->pCurrentEditor)
		return;
	
	_p->pCurrentEditor->fillFXMenu(_p->pFXMenu);
}

void C3MainWindow::createToolBar()
{
	_p->pToolBarSpacer = NULL;

	_p->pToolBar = new QToolBar(__tr("Main"),this);
	_p->pToolBar->setObjectName("MainToolBar");
	_p->pToolBar->setFloatable(false);
	_p->pToolBar->setMovable(false);

	_p->pToolBar->addAction(_p->pGeneralActions->documentNewAction());
	_p->pToolBar->addAction(_p->pGeneralActions->documentOpenAction());
	_p->pToolBar->addAction(_p->pGeneralActions->documentSaveAction());
	_p->pToolBar->addAction(_p->pGeneralActions->documentSaveAllAction());
	_p->pToolBar->addSeparator();
	_p->pToolBar->addAction(_p->pGeneralActions->historyBackAction());
	_p->pToolBar->addAction(_p->pGeneralActions->historyForwardAction());
	_p->pToolBar->addAction(_p->pGeneralActions->openRelatedFileAction());
	_p->pToolBar->addSeparator();

	_p->pExternalCommandsComboBox = new QComboBox(_p->pToolBar);
	_p->pExternalCommandsComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
	_p->pExternalCommandsComboBox->setMinimumContentsLength(25);
	QObject::connect(_p->pExternalCommandsComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotExternalCommandsComboBoxCurrentIndexChanged(int)));
	_p->pToolBar->addWidget(_p->pExternalCommandsComboBox);
	_p->pToolBar->addAction(_p->pGeneralActions->startCurrentExternalCommandAction());
	_p->pToolBar->addAction(_p->pGeneralActions->stopCurrentExternalCommandAction());
	_p->pToolBar->addSeparator();

	_p->pToolBarSpacer = new QWidget(_p->pToolBar);
	_p->pToolBarSpacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	_p->pToolBar->addWidget(_p->pToolBarSpacer);
	_p->pToolBar->addAction(_p->pGeneralActions->documentCloseAction());
}

void C3MainWindow::closeEvent(QCloseEvent * e)
{
	if(!_p->pWorkspace->close(true))
	{
		e->ignore();
		return;
	}

	saveWindowState();
	
	_p->pSettings->saveRecent();
	_p->pSettings->save();

	QMainWindow::closeEvent(e);
}

C3FindWidget * C3MainWindow::showFindWidget(int iFlags)
{
	_p->pCentralWidget->showFindWidget();
	C3GeneralActions::instance()->toggleFindWidgetAction()->setText(__tr("Hide Find and Replace"));
	C3GeneralActions::instance()->toggleFindWidgetGrepAction()->setText(__tr("Hide Find and Replace"));
	if(iFlags & ShowFindWidgetSwitchToGrepInWorkspaceMode)
		_p->pCentralWidget->findWidget()->switchToGrepInWorkspaceMode();
	return _p->pCentralWidget->findWidget();
}

void C3MainWindow::hideFindWidget()
{
	_p->pCentralWidget->hideFindWidget();
	C3GeneralActions::instance()->toggleFindWidgetAction()->setText(__tr("Find and Replace..."));
	C3GeneralActions::instance()->toggleFindWidgetGrepAction()->setText(__tr("Grep in Workspace..."));
}

void C3MainWindow::toggleFindWidget(int iFlags)
{
	if(_p->pCentralWidget->findWidgetVisible())
		hideFindWidget();
	else
		showFindWidget(iFlags);
}

bool C3MainWindow::findWidgetVisible()
{
	return _p->pCentralWidget->findWidgetVisible();
}

void C3MainWindow::showGeneralSettingsDialog()
{
	C3SettingsDialog dlg;
	dlg.exec();
}

QString C3MainWindow::currentDirectory()
{
	return _p->pFileBrowser->directory();
}

QWidget * C3MainWindow::parentForEditors()
{
	return _p->pStackedWidget;
}

void C3MainWindow::killEditor(C3Editor * pEditor)
{
	if(_p->pCurrentEditor == pEditor)
	{
		QObject::disconnect(_p->pCurrentEditor,SIGNAL(privateInfoText1Changed(C3Editor *)),this,SLOT(slotCurrentEditorPrivateInfoText1Changed(C3Editor *)));
		QObject::disconnect(_p->pCurrentEditor,SIGNAL(privateInfoText2Changed(C3Editor *)),this,SLOT(slotCurrentEditorPrivateInfoText2Changed(C3Editor *)));
		_p->pCurrentEditor = NULL;
	}

	_p->pStackedWidget->removeWidget(pEditor);
	_p->pWorkspaceBrowser->editorClosed(pEditor);

	C3FindWidget * w = _p->pCentralWidget->findWidget();
	if(w)
		w->editorClosed(pEditor);

	delete pEditor;
}

void C3MainWindow::addEditor(C3Editor * pEditor,bool bOnTop)
{
	_p->pWorkspaceBrowser->editorOpened(pEditor);
	if(bOnTop)
		_p->pStackedWidget->addWidget(pEditor);
	else
		_p->pStackedWidget->insertWidget(0,pEditor);
}

void C3MainWindow::activateEditor(C3Editor * pEditor)
{
	if(pEditor == _p->pStackedWidget->currentWidget())
	{
		pEditor->setFocus();
		return; // nothing to do
	}

	_p->pStackedWidget->setCurrentWidget(pEditor);

	pEditor->setFocus();
}

void C3MainWindow::slotCurrentEditorPrivateInfoText1Changed(C3Editor * pEditor)
{
	Q_ASSERT(_p->pCurrentEditor == pEditor);

	updatePrivateInfoText1Label(pEditor);
}

void C3MainWindow::slotCurrentEditorPrivateInfoText2Changed(C3Editor * pEditor)
{
	Q_ASSERT(_p->pCurrentEditor == pEditor);

	updatePrivateInfoText2Label(pEditor);
}

void C3MainWindow::currentEditorChanged(C3Editor * pEditor)
{
	if(_p->pCurrentEditor)
	{
		QObject::disconnect(_p->pCurrentEditor,SIGNAL(privateInfoText1Changed(C3Editor *)),this,SLOT(slotCurrentEditorPrivateInfoText1Changed(C3Editor *)));
		QObject::disconnect(_p->pCurrentEditor,SIGNAL(privateInfoText2Changed(C3Editor *)),this,SLOT(slotCurrentEditorPrivateInfoText2Changed(C3Editor *)));
	}

	_p->pCurrentEditor = pEditor;

	if(_p->pCurrentEditor)
	{
		QObject::connect(_p->pCurrentEditor,SIGNAL(privateInfoText1Changed(C3Editor *)),this,SLOT(slotCurrentEditorPrivateInfoText1Changed(C3Editor *)));
		QObject::connect(_p->pCurrentEditor,SIGNAL(privateInfoText2Changed(C3Editor *)),this,SLOT(slotCurrentEditorPrivateInfoText2Changed(C3Editor *)));
	}

	_p->pWorkspaceBrowser->currentEditorChanged(pEditor);

	updatePrivateInfoText1Label(pEditor);
	updatePrivateInfoText2Label(pEditor);
	updateFilePathLabel(pEditor);
	updateCaption();
	
	_p->pGeneralActions->currentEditorChanged(pEditor);
	
	C3FindWidget * w = _p->pCentralWidget->findWidget();
	if(w)
		w->currentEditorChanged();
}

void C3MainWindow::editorModifiedStateChanged(C3Editor * pEditor)
{
	_p->pWorkspaceBrowser->editorModifiedStateChanged(pEditor);
	if(pEditor == _p->pCurrentEditor)
	{
		updateFilePathLabel(pEditor);
		_p->pGeneralActions->currentEditorModifiedStateChanged(pEditor);
		updateCaption();
	}
}

void C3MainWindow::editorReadOnlyStateChanged(C3Editor * pEditor)
{
	//_p->pWorkspaceBrowser->editorModifiedStateChanged(pEditor);
	if(pEditor == _p->pCurrentEditor)
		updateFilePathLabel(pEditor);
}

void C3MainWindow::editorTitleChanged(C3Editor * pEditor)
{
	_p->pWorkspaceBrowser->editorTitleChanged(pEditor);
}

void C3MainWindow::editorPathChanged(C3Editor * pEditor)
{
	if(pEditor == _p->pCurrentEditor)
		updateFilePathLabel(pEditor);
}

void C3MainWindow::updatePrivateInfoText1Label(C3Editor * pEditor)
{
	if(pEditor)
		_p->pPrivateInfoText1Label->setText(pEditor->privateInfoText1());
	else
		_p->pPrivateInfoText1Label->setText(QString());
}

void C3MainWindow::updatePrivateInfoText2Label(C3Editor * pEditor)
{
	if(pEditor)
		_p->pPrivateInfoText2Label->setText(pEditor->privateInfoText2());
	else
		_p->pPrivateInfoText2Label->setText(QString());
}

void C3MainWindow::updateFilePathLabel(C3Editor * pEditor)
{
	if(!pEditor)
	{
		_p->pFilePathLabel->setText(__tr("[no file]"));
		return;
	}

	QString szText;
	
	if(pEditor->modified())
	{
		if(pEditor->readOnly()) // should NEVER happen
			szText = __tr("[modified, readonly] %1").arg(pEditor->path());
		else
			szText = __tr("[modified] %1").arg(pEditor->path());
	} else {
		if(pEditor->readOnly())
			szText = __tr("[readonly] %1").arg(pEditor->path());
		else
			szText = pEditor->path();
	}

	_p->pFilePathLabel->setText(szText);
}

void C3MainWindow::updateCaption()
{
	QString szWorkspace;
	
	if(_p->pWorkspace->isDefault())
	{
		szWorkspace = __utf8("no workspace");
	} else {
		szWorkspace = _p->pWorkspace->projectName();
		if(szWorkspace.isEmpty())
			szWorkspace = _p->pWorkspace->name();
	}

	QString szCaption;
	
	if(_p->pCurrentEditor)
	{
		if(_p->pCurrentEditor->modified())
			szCaption = __utf8("[%1] * %2 -- C3").arg(szWorkspace).arg(_p->pCurrentEditor->title());
		else
			szCaption = __utf8("[%1] %2 -- C3").arg(szWorkspace).arg(_p->pCurrentEditor->title());
	} else { 
		szCaption = __utf8("[%1] -- C3").arg(szWorkspace);
	}
	
	setWindowTitle(szCaption);
}

C3Editor * C3MainWindow::currentEditor()
{
	return _p->pCurrentEditor;
}

void C3MainWindow::slotStackedWidgetCurrentChanged(int idx)
{
	if(idx < 0)
	{
		currentEditorChanged(NULL);
		return;
	}

	QWidget * w = _p->pStackedWidget->widget(idx);
	if(!w)
	{
		currentEditorChanged(NULL);
		return;
	}

	C3Editor * ed = dynamic_cast<C3Editor *>(w);
	Q_ASSERT(ed);

	currentEditorChanged(ed);
}

void C3MainWindow::loadInitialDefaultWorkspace()
{
	_p->pWorkspace->loadDefault();

	C3DockFileBrowser::instance()->setDirectory(QDir::currentPath());
}

bool C3MainWindow::openWorkspace()
{
	QStringList lNameFilters;

	lNameFilters << __tr("Code-3 Workspace Files (*.c3workspace)");
	lNameFilters << __tr("Code-3 Old Workspace Files (*.ssexwsp)");

	QFileDialog dlg(
		this,
		__tr("Select the Workspace to Open"),
		currentDirectory()
	);
	
	dlg.setOptions(QFileDialog::DontUseNativeDialog);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilters(lNameFilters);
	dlg.selectNameFilter(lNameFilters.at(0));
	dlg.setDefaultSuffix(__utf8("c3workspace"));

	if(!_p->pWorkspace->path().isEmpty())
		dlg.selectFile(_p->pWorkspace->path());

	if(dlg.exec() != QDialog::Accepted)
		return false;

	QStringList lFiles = dlg.selectedFiles();
	if(lFiles.count() < 1)
		return false; // no files selected?

	QString szPath = lFiles.at(0);
	if(szPath.isEmpty())
		return false; // ???

	return openWorkspace(szPath);
}

bool C3MainWindow::openWorkspace(const QString &szPath)
{
	QString szError;

	if(!openWorkspace(szPath,szError))
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				this,
				__tr("Workspace Opening Failed"),
				__tr("Failed to open the workspace file at '%1': %2").arg(szPath).arg(szError),
				QMessageBox::Ok
			);
		return false;
	}

	return true;
}

bool C3MainWindow::openWorkspace(const QString &szPath,QString &szError)
{
	if(!closeWorkspace(true,false))
		return false;

	if(!_p->pWorkspace->load(szPath.endsWith(".user") ? szPath.mid(0,szPath.length() - 5) : szPath,szError))
	{
		_p->pWorkspace->loadDefault();
		return false;
	}

	_p->pGeneralActions->workspaceChanged(_p->pWorkspace);
	//_p->pWorkspaceBrowser->attachToWorkspace(_p->pWorkspace);
	updateCaption();
	emit workspacePropertiesChanged();
	rebuildExternalCommandToolBarControls();
	return true;
}

void C3MainWindow::slotOpenRecentWorkspace()
{
	QAction * a = dynamic_cast<QAction *>(sender());
	if(!a)
		return;
	
	QString szPath = a->data().toString();
	
	if(szPath.isEmpty())
		return;
	
	if(szPath == _p->pWorkspace->path())
		return; // same workspace
	
	openWorkspace(szPath);
}

bool C3MainWindow::saveWorkspaceAs()
{
	if(!_p->pWorkspace->saveAs())
		return false;

	// reattach
	_p->pGeneralActions->workspaceChanged(_p->pWorkspace);
	_p->pWorkspaceBrowser->attachToWorkspace(_p->pWorkspace);
	updateCaption();
	emit workspacePropertiesChanged();
	rebuildExternalCommandToolBarControls();
	return true;
}

bool C3MainWindow::closeWorkspace(bool bCanAbort,bool bLoadDefault)
{
	if(!_p->pWorkspace->close(bCanAbort))
	{
		if(bCanAbort)
			return false;

		qDebug("ERROR: The workspace didn't close properly: it will be killed");
	}

	C3Workspace * pWorkspace = new C3Workspace();
	if(bLoadDefault)
		pWorkspace->loadDefault();

	setWorkspace(pWorkspace);
	return true;
}

void C3MainWindow::setWorkspace(C3Workspace * pWorkspace)
{
	Q_ASSERT(pWorkspace);

	delete _p->pWorkspace;
	_p->pWorkspace = pWorkspace;
	_p->pGeneralActions->workspaceChanged(pWorkspace);
	if(_p->pWorkspaceBrowser) // may be false in the constructor
		_p->pWorkspaceBrowser->attachToWorkspace(pWorkspace);
	updateCaption();
	QObject::connect(pWorkspace,SIGNAL(propertiesChanged()),this,SLOT(slotWorkspacePropertiesChanged()));
	emit workspacePropertiesChanged();
	rebuildExternalCommandToolBarControls();
}

void C3MainWindow::slotWorkspacePropertiesChanged()
{
	// Please note that the editors have not been notified of the change yet,
	// but *WILL* be notified by the workspace itself.
	emit workspacePropertiesChanged();
	updateCaption();
	rebuildExternalCommandToolBarControls();
}

QString C3MainWindow::currentExternalCommand()
{
	QVariant curData = _p->pExternalCommandsComboBox->currentData();
	
	return curData.toString();
}

void C3MainWindow::updateExternalCommandToolBarControls()
{
	if(_p->bShuttingDown)
		return;

	QString szId = currentExternalCommand();

	bool bIsRunning = _p->pExternalCommandManager->isCommandExecuting(szId);

	_p->pGeneralActions->startCurrentExternalCommandAction()->setEnabled(!bIsRunning);
	_p->pGeneralActions->stopCurrentExternalCommandAction()->setEnabled(bIsRunning);
}

void C3MainWindow::startCurrentExternalCommand()
{
	const C3ExternalCommand * pCmd = C3Workspace::currentWorkspace()->externalCommand(currentExternalCommand());
	if(!pCmd)
		return;

	_p->pExternalCommandManager->executeCommand(pCmd);
}

void C3MainWindow::stopCurrentExternalCommand()
{
	_p->pExternalCommandManager->terminateCommandById(currentExternalCommand());
}

void C3MainWindow::slotExternalCommandsComboBoxCurrentIndexChanged(int idx)
{
	updateExternalCommandToolBarControls();
}

void C3MainWindow::rebuildExternalCommandToolBarControls()
{
	//qDebug("Rebuilding workspace actions");
	const QList<C3ExternalCommand *> &lCommands = _p->pWorkspace->externalCommands();

	foreach(QAction * act,_p->lExternalCommandControls)
		removeAction(act);

	qDeleteAll(_p->lExternalCommandControls);
	_p->lExternalCommandControls.clear();
	
	QVariant curData = _p->pExternalCommandsComboBox->currentData();

	QString szId = curData.toString();

	int idx = 0;
	int iCurrent = -1;

	_p->pExternalCommandsComboBox->clear();

	foreach(C3ExternalCommand * cmd,lCommands)
	{
		//qDebug("Workspace action %s",cmd->name().toUtf8().data());

		QIcon icon;
		QString szIcon = cmd->icon();
		if(szIcon.isEmpty())
			icon = C3PixmapCache::instance()->pixmap(__utf8("externalcommand/20px/externalcommand-gear.png"));
		else {
			QPixmap pix = QPixmap(szIcon);
			if(pix.isNull())
			{
				pix = C3PixmapCache::instance()->pixmap(szIcon);
				if(pix.isNull())
				{
					pix = C3PixmapCache::instance()->pixmap(__utf8("externalcommand/20px/externalcommand-%1.png").arg(szIcon));
					if(pix.isNull())
					pix = C3PixmapCache::instance()->pixmap(__utf8("externalcommand/20px/externalcommand-gear.png"));
				}
			}
			icon = pix;
		}
	
		QString szText = cmd->name();
		if(!cmd->shortcut().isEmpty())
			szText += __utf8(" (%1)").arg(cmd->shortcut());
	
		_p->pExternalCommandsComboBox->addItem(icon,szText,cmd->id());
	
		QAction * pAction = new QAction(
				icon,
				szText,
				this
			);
		
		QObject::connect(pAction,SIGNAL(triggered()),this,SLOT(slotWorkspaceActionTriggered()));
		
		addAction(pAction);
		
		if(!cmd->shortcut().isEmpty())
			pAction->setShortcut(QKeySequence(cmd->shortcut()));

		pAction->setData(cmd->id());

		_p->lExternalCommandControls.append(pAction);
		
		if(cmd->id() == szId)
			iCurrent = idx;
		
		idx++;
	}
	
	if(iCurrent >= 0)
		_p->pExternalCommandsComboBox->setCurrentIndex(iCurrent);
	
	updateExternalCommandToolBarControls();
}

void C3MainWindow::slotWorkspaceActionTriggered()
{
	QObject * ob = sender();
	if(!ob)
		return;
	
	QAction * a = dynamic_cast<QAction *>(ob);
	if(!a)
		return;
	
	QString szId = a->data().toString();
	
	const C3ExternalCommand * pCmd = C3Workspace::currentWorkspace()->externalCommand(szId);
	if(!pCmd)
		return;

	_p->pExternalCommandManager->executeCommand(pCmd);
}

C3Workspace * C3MainWindow::workspace()
{
	return _p->pWorkspace; // never null
}

void C3MainWindow::slotOpenRecentFile()
{
	QAction * a = dynamic_cast<QAction *>(sender());
	if(!a)
		return;
	
	QString szPath = a->data().toString();
	
	if(szPath.isEmpty())
		return;

	C3Workspace::currentWorkspace()->openFileAsync(szPath);
}

void C3MainWindow::alert(
		C3AlertSource * pSource,
		C3AlertSource::AlertLevel eLevel,
		const QString &szMessage
	)
{
	// FIXME: Display the alerts in closable popups!
	
	// FIXME: This *MAY* be called from slave threads!!!!!
	
	QString szDesc = pSource->alertLevelDescription(eLevel);
	
	qDebug("%s",__utf8("[%1][%2] %3").arg(szDesc).arg(pSource->name()).arg(szMessage).toUtf8().data());
	
	if(eLevel < C3AlertSource::Warning)
		return; // we show no alerts below the Warning level
	
	QString szIcon;
	QString szStyleSheet;
	
	switch(eLevel)
	{
		case C3AlertSource::Warning:
			szIcon = "message/32px/warning.png";
			szStyleSheet = __utf8("background-color: #505000; color: #ffff00;");
		break;
		case C3AlertSource::Error:
			szIcon = "message/32px/error.png";
			szStyleSheet = __utf8("background-color: #802000; color: #ffff00;");
		break;
		case C3AlertSource::Fatal:
			szIcon = "message/32px/banned.png";
			szStyleSheet = __utf8("background-color: #602000; color: #ffffff;");
		break;
		default:
			szIcon = "message/32px/question.png";
		break;
	}
	
	_p->pCentralWidget->message(
			 __tr("[%1] %2").arg(szDesc.toUpper()).arg(szMessage),
			szIcon,
			szStyleSheet
		);
}


void C3MainWindow::loadWindowState()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.mainwindowstate");

	restoreGeometry(s.value("geometry").toByteArray());
	restoreState(s.value("windowState").toByteArray());
}

void C3MainWindow::saveWindowState()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.mainwindowstate");

	s.setValue("geometry",saveGeometry());
	s.setValue("windowState",saveState());
}


void C3MainWindow::newDocument()
{
	C3NewDialog dlg;
	dlg.exec();
}

void C3MainWindow::openDocument()
{
	QStringList lNameFilters;
	
	QString szNameFilter = __tr("All Files (*)");
	
	lNameFilters << szNameFilter;

	QFileDialog dlg(
		this,
		__tr("Select the File to Open"),
		currentDirectory()
	);
	
	dlg.setOptions(QFileDialog::DontUseNativeDialog);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilters(lNameFilters);
	dlg.selectNameFilter(szNameFilter);

	if(dlg.exec() != QDialog::Accepted)
		return;

	QStringList lFiles = dlg.selectedFiles();
	if(lFiles.count() < 1)
		return; // no files selected?

	QString szPath = lFiles.at(0);
	if(szPath.isEmpty())
		return; // ???

	_p->pWorkspace->openFileAsync(szPath);
}

bool C3MainWindow::createCurrentHistoryEntry(C3Link &oLink)
{
	if(!_p->pCurrentEditor)
		return false;
	return _p->pCurrentEditor->createLink(oLink);
}

void C3MainWindow::addHistoryEntry()
{
	if(!_p->pCurrentEditor)
		return;
	C3Link oLink;
	if(!_p->pCurrentEditor->createLink(oLink))
		return;
	_p->pLinkHistory->addEntry(oLink);
}
