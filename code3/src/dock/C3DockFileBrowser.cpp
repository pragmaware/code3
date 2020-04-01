//=============================================================================
//
//   File : C3DockFileBrowser.cpp
//   Creation Date : mar 20 ott 2015 01:05:45
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

#include "C3DockFileBrowser.h"
#include "C3ExternalCommandManager.h"
#include "C3ExternalToolManager.h"
#include "C3ExternalCommand.h"
#include "C3PixmapCache.h"
#include "C3Workspace.h"
#include "C3EditorFactory.h"
#include "C3Application.h"
#include "C3PixmapCache.h"
#include "C3Editor.h"
#include "C3MainWindow.h"
#include "C3FileUtils.h"
#include "C3DockWidgetTitleBar.h"
#include "C3LinkSelectionDialog.h"

#include <QGridLayout>
#include <QTimer>
#include <QToolBar>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QHash>
#include <QMenu>
#include <QLabel>
#include <QAction>
#include <QFileInfo>
#include <QJsonObject>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

C3DockFileBrowser * C3DockFileBrowser::m_pInstance = NULL;

class C3DockFileBrowserPrivate
{
public:
	QGridLayout * pLayout;
	QWidget * pBase;
	QToolBar * pToolBar;
	QLineEdit * pPathEdit;
	QListWidget * pListWidget;
	
	QHash<QString,QString> hIconMap;
	
	QString szDirectory;
	
	QMenu * pContextMenu;
	
	QAction * pUpAction;
	QAction * pReloadAction;
	QAction * pWorkspaceRootAction;
	QAction * pHomeAction;
	QAction * pFileDirAction;
	
	C3DockWidgetTitleBar * pTitleBar;
};

class C3DockFileBrowserListWidgetItem : public QListWidgetItem
{
public:
	C3DockFileBrowserListWidgetItem()
		: QListWidgetItem()
	{
	}
	
	virtual QVariant data(int role) const
	{
		if(role == Qt::ToolTipRole)
		{
			QHash<QString,QVariant> hData = QListWidgetItem::data(Qt::UserRole).toHash();
			QString szPath = hData.value("path").toString();
			
			QFileInfo inf(szPath);
			
			if(inf.isDir())
			{
				return __utf8(
					"<div><b>%1</b></div>"
					"%2"
				).arg(inf.fileName())
				.arg(__tr("folder"));
			}

			return __utf8(
					"<div><b>%1</b></div>"
					"%L2 bytes"
				).arg(inf.fileName())
				.arg(inf.size());
		}
		
		return QListWidgetItem::data(role);
	}
};

#define _p m_pC3DFB

C3DockFileBrowser::C3DockFileBrowser(QWidget * pParent)
	: QDockWidget(pParent), C3AlertSource(__tr("File Browser"))
{
	m_pInstance = this;

	setWindowTitle(__tr("File Browser"));
	setObjectName(__utf8("C3DockFileBrowser"));
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetMovable);

	_p = new C3DockFileBrowserPrivate();

	_p->pContextMenu = NULL;

	_p->pTitleBar = new C3DockWidgetTitleBar(this,__tr("File Browser"));
	setTitleBarWidget(_p->pTitleBar);

	_p->pBase = new QWidget(this);

	_p->pLayout = new QGridLayout(_p->pBase);

	createActionsAndToolBar();

	_p->pLayout->addWidget(_p->pToolBar,0,0);

	_p->pPathEdit = new QLineEdit(_p->pBase);
	_p->pPathEdit->setToolTip(__tr("The directory to show. The last component can contain * and ? wildcards to filter the displayed contents."));
	QObject::connect(_p->pPathEdit,SIGNAL(returnPressed()),this,SLOT(slotPathEditReturnPressed()));

	_p->pLayout->addWidget(_p->pPathEdit,1,0);

	_p->pListWidget = new QListWidget(_p->pBase);
	_p->pListWidget->setSelectionMode(QListWidget::ExtendedSelection);
	_p->pListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	_p->pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QObject::connect(_p->pListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(slotItemDoubleClicked(QListWidgetItem *)));
	QObject::connect(_p->pListWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slotContextMenuRequested(const QPoint &)));
	
	_p->pLayout->addWidget(_p->pListWidget,2,0);
	
	//_p->pLayout->setContentsMargins(QMargins(3,0,3,3));
	//_p->pLayout->setSpacing(3);

	_p->pLayout->setContentsMargins(QMargins(0,0,0,0));
	_p->pLayout->setSpacing(0);

	_p->pLayout->setRowStretch(2,100);

	setWidget(_p->pBase);

	_p->hIconMap.insert("aac","aac.png");
	_p->hIconMap.insert("ai","ai.png");
	_p->hIconMap.insert("aiff","aiff.png");
	_p->hIconMap.insert("avi","avi.png");
	_p->hIconMap.insert("bmp","bmp.png");
	_p->hIconMap.insert("cpp","cpp.png");
	_p->hIconMap.insert("cxx","cpp.png");
	_p->hIconMap.insert("css","css.png");
	_p->hIconMap.insert("dat","dat.png");
	_p->hIconMap.insert("dmg","dmg.png");
	_p->hIconMap.insert("doc","doc.png");
	_p->hIconMap.insert("dwg","dwg.png");
	_p->hIconMap.insert("dotx","dotx.png");
	_p->hIconMap.insert("dxf","dxf.png");
	_p->hIconMap.insert("eps","eps.png");
	_p->hIconMap.insert("exe","exe.png");
	_p->hIconMap.insert("flv","flv.png");
	_p->hIconMap.insert("gif","gif.png");
	_p->hIconMap.insert("h","h.png");
	_p->hIconMap.insert("hpp","hpp.png");
	_p->hIconMap.insert("html","html.png");
	_p->hIconMap.insert("htm","html.png");
	_p->hIconMap.insert("ics","ics.png");
	_p->hIconMap.insert("iso","iso.png");
	_p->hIconMap.insert("java","java.png");
	_p->hIconMap.insert("jpg","jpg.png");
	_p->hIconMap.insert("jpeg","jpg.png");
	_p->hIconMap.insert("js","js.png");
	_p->hIconMap.insert("key","key.png");
	_p->hIconMap.insert("less","less.png");
	_p->hIconMap.insert("lxx","cpp.png"); // opencascade headers
	_p->hIconMap.insert("mid","mid.png");
	_p->hIconMap.insert("mp3","mp3.png");
	_p->hIconMap.insert("mp4","mp4.png");
	_p->hIconMap.insert("mpg","mpg.png");
	_p->hIconMap.insert("mpeg","mpeg.png");
	_p->hIconMap.insert("odf","odf.png");
	_p->hIconMap.insert("ods","ods.png");
	_p->hIconMap.insert("odt","odt.png");
	_p->hIconMap.insert("otp","otp.png");
	_p->hIconMap.insert("ots","ots.png");
	_p->hIconMap.insert("ott","ott.png");
	_p->hIconMap.insert("pdf","pdf.png");
	_p->hIconMap.insert("php","php.png");
	_p->hIconMap.insert("png","png.png");
	_p->hIconMap.insert("ppt","ppt.png");
	_p->hIconMap.insert("psd","psd.png");
	_p->hIconMap.insert("py","py.png");
	_p->hIconMap.insert("qt","qt.png");
	_p->hIconMap.insert("rar","rar.png");
	_p->hIconMap.insert("rb","rb.png");
	_p->hIconMap.insert("rtf","rtf.png");
	_p->hIconMap.insert("sass","sass.png");
	_p->hIconMap.insert("scss","scss.png");
	_p->hIconMap.insert("sql","sql.png");
	_p->hIconMap.insert("tga","tga.png");
	_p->hIconMap.insert("tgz","tgz.png");
	_p->hIconMap.insert("tar.gz","tgz.png");
	_p->hIconMap.insert("tar.bz2","tgz.png");
	_p->hIconMap.insert("tbz","tgz.png");
	_p->hIconMap.insert("tar.xz","tgz.png");
	_p->hIconMap.insert("tiff","tiff.png");
	_p->hIconMap.insert("txt","txt.png");
	_p->hIconMap.insert("wav","wav.png");
	_p->hIconMap.insert("xls","xls.png");
	_p->hIconMap.insert("xlsx","xlsx.png");
	_p->hIconMap.insert("xml","xml.png");
	_p->hIconMap.insert("yml","yml.png");
	_p->hIconMap.insert("zip","zip.png");
	
	//setDirectory(QDir::currentPath());
}

C3DockFileBrowser::~C3DockFileBrowser()
{
	if(_p->pContextMenu)
		delete _p->pContextMenu;

	delete _p;
	
	m_pInstance = NULL;
}

void C3DockFileBrowser::saveState(QJsonObject &oObject)
{
	oObject.insert(__utf8("directory"),_p->szDirectory);
}

void C3DockFileBrowser::restoreState(QJsonObject &oObject,bool bRestoreDirectory)
{
	if(bRestoreDirectory)
	{
		QString szPath = oObject.value(__utf8("directory")).toString();

		if(!szPath.isEmpty())
			setDirectory(szPath);
		else
			goToWorkspaceRoot();
	}
	
	// Here we could restore other properties
}

const QString & C3DockFileBrowser::directory()
{
	return _p->szDirectory;
}

void C3DockFileBrowser::reload()
{
	setDirectory(_p->szDirectory);
}

void C3DockFileBrowser::createActionsAndToolBar()
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	_p->pToolBar = new QToolBar(_p->pBase);

#define CREATE_ACTION(_pObject,_szIcon,_szText,_szToolTip,_fnSlot) \
	do { \
		_pObject = new QAction( \
				pPixmapCache->pixmap(__utf8("general/20px/%1").arg(_szIcon)), \
				_szText, \
				this \
			); \
		_pObject->setIconVisibleInMenu(true); \
		_pObject->setToolTip(_szToolTip); \
		_pObject->setStatusTip(_szToolTip); \
		_p->pToolBar->addAction(_pObject); \
		QObject::connect(_pObject,SIGNAL(triggered()),this,SLOT(_fnSlot())); \
	} while(0)

	CREATE_ACTION(_p->pUpAction,"up.png",__tr("Go Up"),__tr("Go to the directory above the current one"),slotUpActionTriggered);
	CREATE_ACTION(_p->pWorkspaceRootAction,"go-workspace-root.png",__tr("Go to Workspace Root"),__tr("Go to the workspace root"),slotWorkspaceRootActionTriggered);
	CREATE_ACTION(_p->pHomeAction,"home.png",__tr("Go Home"),__tr("Go to the home directory"),slotHomeActionTriggered);
	CREATE_ACTION(_p->pFileDirAction,"herefile.png",__tr("Current File"),__tr("Go to the directory containing the current file"),slotFileDirActionTriggered);
	CREATE_ACTION(_p->pReloadAction,"reload.png",__tr("Reload"),__tr("Reload the listing of the current directory"),slotReloadActionTriggered);
}

void C3DockFileBrowser::goToWorkspaceRoot()
{
	C3Workspace * pWorkspace = C3Workspace::currentWorkspace();
	if(pWorkspace->isDefault())
		setDirectory(QDir::homePath());
	else
		setDirectory(pWorkspace->rootPath());
}

void C3DockFileBrowser::slotWorkspaceRootActionTriggered()
{
	goToWorkspaceRoot();
}

void C3DockFileBrowser::slotUpActionTriggered()
{
	QDir d(_p->szDirectory);
	if(!d.cdUp())
	{
		warning(__tr("Could not change to the containing directory"));
		return;
	}
	
	setDirectory(d.path());
}

void C3DockFileBrowser::slotHomeActionTriggered()
{
	setDirectory(QDir::homePath());
}

void C3DockFileBrowser::slotFileDirActionTriggered()
{
	C3Editor * ed = C3MainWindow::instance()->currentEditor();
	if(!ed)
		return;
	
	QString szPath = ed->path();
	if(szPath.isEmpty())
		return;
	
	QFileInfo inf(szPath);

	setDirectory(inf.absolutePath(),true);
}

void C3DockFileBrowser::slotReloadActionTriggered()
{
	setDirectory(_p->szDirectory);
}

void C3DockFileBrowser::slotContextMenuRequested(const QPoint &pnt)
{
	if(!_p->pContextMenu)
	{
		_p->pContextMenu = new QMenu();
		QObject::connect(_p->pContextMenu,SIGNAL(triggered(QAction *)),this,SLOT(slotContextMenuActionTriggered(QAction *)));
	} else {
		_p->pContextMenu->clear();
	}

	QAction * pAction;

#define ADD_ACTION_TO_MENU(_pMenu,_szId,_szText) \
	do { \
		pAction = _pMenu->addAction(_szText); \
		QHash<QString,QVariant> hData; \
		hData.insert("id",_szId); \
		pAction->setData(hData); \
	} while(0)


#define ADD_ACTION(_szId,_szText) \
	ADD_ACTION_TO_MENU(_p->pContextMenu,_szId,_szText)

	//QListWidgetItem * it = _p->pListWidget->itemAt(pnt);
	//if(it)
	//{
	//	if(!it->isSelected())
	//		_p->pListWidget->setSelected(it);
	//}

	QList<QListWidgetItem *> lSelected = _p->pListWidget->selectedItems();
	if(!lSelected.isEmpty())
	{
		bool bGotFiles = false;
		bool bGotDirs = false;
		int iTotalCount = 0;
	
		foreach(QListWidgetItem * it,lSelected)
		{
			QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();
			
			bool bIsDir = hData.value("isDirectory").toBool();
			if(bIsDir)
			{
				if(hData.value("name").toString() == __utf8(".."))
					continue;
	
				bGotDirs = true;
				iTotalCount++;
				if(bGotFiles)
					break;
			} else {
				bGotFiles = true;
				iTotalCount++;
				if(bGotDirs)
					break;
			}
		}

		if(iTotalCount > 0)
		{
			if(bGotFiles)
			{
				ADD_ACTION(__utf8("open"),bGotDirs ? __tr("Open Files") : __tr("Open"));

				QMenu * pMenu = _p->pContextMenu->addMenu(bGotDirs ? __tr("Open Files in...") : __tr("Open in..."));
				//QObject::connect(pMenu,SIGNAL(triggered(QAction *)),this,SLOT(slotContextMenuActionTriggered(QAction *)));
				
				const QList<C3EditorFactory *> &lFactories = C3Application::instance()->editorFactories();
			
				Q_FOREACH(C3EditorFactory * pFactory,lFactories)
				{
					pAction = pMenu->addAction(pFactory->editorName());
					QHash<QString,QVariant> hDatax;
					hDatax.insert("id","open");
					hDatax.insert("factoryId",pFactory->id());
					pAction->setData(hDatax);
				}
				
				pMenu->addSeparator();
				pAction = pMenu->addAction(__tr("System Editor"));
				QHash<QString,QVariant> hDatay;
				hDatay.insert("id","opensystem");
				pAction->setData(hDatay);

				_p->pContextMenu->addSeparator();
			}
	
			if(iTotalCount == 1)
				ADD_ACTION(__utf8("rename"),__tr("Rename"));
	
			ADD_ACTION(__utf8("delete"),__tr("Delete"));
	
			_p->pContextMenu->addSeparator();
		}
	}

	QMenu * svn = _p->pContextMenu->addMenu(__tr("SVN"));
	
	ADD_ACTION_TO_MENU(svn,__utf8("svn mv"),__tr("svn mv"));
	ADD_ACTION_TO_MENU(svn,__utf8("svn rm"),__tr("svn rm"));

	_p->pContextMenu->addSeparator();

	ADD_ACTION(__utf8("newfolder"),__tr("New Folder..."));
	ADD_ACTION(__utf8("openall"),__tr("Open All Files"));
	ADD_ACTION(__utf8("openallrec"),__tr("Open All Files (Recursive)"));

	_p->pContextMenu->popup(_p->pListWidget->mapToGlobal(pnt));
}

void C3DockFileBrowser::slotContextMenuActionTriggered(QAction * pAction)
{
	QHash<QString,QVariant> hData = pAction->data().toHash();
	
	QString szId = hData.value("id").toString();

	if(szId.isEmpty())
		return;

	QList<QListWidgetItem *> lSelected = _p->pListWidget->selectedItems();

	if(szId == __utf8("open"))
	{
		QString szFactoryId = hData.value("factoryId").toString();
		QString szFormatId = hData.value("formatId").toString();

		Q_FOREACH(QListWidgetItem * it,lSelected)
		{
			QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();
			
			if(hData.value("isDirectory").toBool())
				continue;

			QHash<QString,QVariant> hOpenData;

			hOpenData.insert("path",hData.value("path").toString());
			hOpenData.insert("factoryId",szFactoryId);
			hOpenData.insert("formatId",szFormatId);
			
			C3Workspace::currentWorkspace()->openFileAsync(hOpenData);
		}
		return;
	}
	
	if(szId == __utf8("opensystem"))
	{
		Q_FOREACH(QListWidgetItem * it,lSelected)
		{
			QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();
			
			if(hData.value("isDirectory").toBool())
				continue;

			QString szPath = hData.value("path").toString();
			
			QDesktopServices::openUrl(QUrl(__utf8("file://%1").arg(szPath),QUrl::TolerantMode));
		}

		return;
	}
	
	if(szId == __utf8("newfolder"))
	{
		newFolder();
		return;
	}

	if(szId == __utf8("openall"))
	{
		openAllFilesInCurrentDirectory(false);
		return;
	}

	if(szId == __utf8("openallrec"))
	{
		openAllFilesInCurrentDirectory(true);
		return;
	}

	if(szId == __utf8("rename"))
	{
		renameSelected();
		return;
	}

	if(szId == __utf8("delete"))
	{
		deleteSelected();
		return;
	}

	if(szId == __utf8("svn mv"))
	{
		svnMvSelected();
		return;
	}

	if(szId == __utf8("svn rm"))
	{
		svnRmSelected();
		return;
	}
}

void C3DockFileBrowser::openAllFilesInDirectory(const QString &szDir,bool bRecursive)
{
	QDir d(szDir);

	QDir::Filters f = QDir::Files | QDir::NoDotAndDotDot;
	
	if(bRecursive)
		f |= QDir::Dirs;
	
	QFileInfoList fil = d.entryInfoList(f);

	Q_FOREACH(QFileInfo inf,fil)
	{
		if(inf.isDir())
		{
			if(bRecursive)
				openAllFilesInDirectory(inf.absoluteFilePath(),bRecursive);
			continue;
		}

		C3Workspace::currentWorkspace()->openFileAsync(inf.absoluteFilePath());
	}
}

void C3DockFileBrowser::openAllFilesInCurrentDirectory(bool bRecursive)
{
	int c = _p->pListWidget->count();
	int i = 0;
	while(i < c)
	{
		QListWidgetItem * it = _p->pListWidget->item(i);

		i++;

		if(!it)
		 	continue;

		QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

		QString szPath = hData.value("path").toString();

		if(hData.value("isDirectory").toBool())
		{
			if(hData.value("name").toString() == __utf8(".."))
				continue;
		
			if(bRecursive)
				openAllFilesInDirectory(szPath,true);

			continue;
		}

		C3Workspace::currentWorkspace()->openFileAsync(szPath);
	}
}

void C3DockFileBrowser::svnMvSelected()
{
	QList<QListWidgetItem *> lSelected = _p->pListWidget->selectedItems();
	if(lSelected.isEmpty())
		return;

	QListWidgetItem * it = lSelected.first();

	QString szOriginalFileName = it->text();
	QString szFileName;

	for(;;)
	{
		bool ok;
		szFileName = QInputDialog::getText(
				this,
				__tr("SVN Rename Item"),
				__tr("Please provide the new name for the item"),
				QLineEdit::Normal,
				szOriginalFileName,
				&ok
			);
	
		if(!ok)
			return;
		
		if(!szFileName.isEmpty())
			break;

		QMessageBox::StandardButton eButton = QMessageBox::critical(
				this,
				__tr("SVN Rename Item"),
				__tr("The name can't be empty"),
				QMessageBox::Ok
			);
	}

	if(szFileName == szOriginalFileName)
		return;

	QList<C3Editor *> lOpen;

	QString szFullPath = C3FileUtils::mergePathComponents(_p->szDirectory,szOriginalFileName);
	QString szNewFullPath = C3FileUtils::mergePathComponents(_p->szDirectory,szFileName);

	QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

	if(hData.value("isDirectory").toBool())
		C3Workspace::currentWorkspace()->findAllEditorsInDirectory(szFullPath,lOpen);
	else
		C3Workspace::currentWorkspace()->findAllEditors(szFullPath,lOpen);

	if(!lOpen.isEmpty())
	{
		foreach(C3Editor * ed,lOpen)
		{
			if(!C3Workspace::currentWorkspace()->closeEditor(ed,true))
				return;
		}
	}

	QString szGrep = C3ExternalToolManager::instance()->findTool("svn");
	if(szGrep.isEmpty())
	{
		QMessageBox::information(this,__tr("SVN Tool Not Found"),__tr("Could not find svn executable"));
		return;
	}
	
	C3ExternalCommand cmd;
	
	cmd.setType(C3ExternalCommand::ControlledProcessWithArguments);
	cmd.setCommand(szGrep);
	cmd.setSaveAllFiles(false);
	cmd.setHighlightPaths(true);
	cmd.setClearOutputOnStart(true);

	QStringList lArgs;
	lArgs.append(__utf8("mv"));
	lArgs.append(szFullPath);
	lArgs.append(szNewFullPath);

	cmd.setName(__utf8("SVN"));
	cmd.setId(__utf8("internal.svn"));
	cmd.setArguments(lArgs);

	C3ExternalCommandManager::instance()->executeCommand(&cmd);

	reloadDelayed();
	//selectItem(szFileName);

	C3Workspace::currentWorkspace()->notifyRescanNeeded();
}

void C3DockFileBrowser::reloadDelayed()
{
	QTimer::singleShot(200,this,SLOT(reload()));
}

bool C3DockFileBrowser::gatherSelectedItemsAndAskForDeletion(QList<QListWidgetItem *> &lSelected)
{
	lSelected = _p->pListWidget->selectedItems();
	if(lSelected.isEmpty())
		return false;

	QListWidgetItem * it;

	int iDirs = 0;
	int iFiles = 0;

	foreach(it,lSelected)
	{
		QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

		QString szPath = hData.value("path").toString();

		if(hData.value("isDirectory").toBool())
			iDirs++;
		else
			iFiles++;
	}

	QString szText;

	QString szDirText;
	QString szFileText;

	if(iDirs > 1)
		szDirText = __tr("%1 folders and all of their contents").arg(iDirs);
	else if(iDirs > 0)
		szDirText =  __tr("one folder");

	if(iFiles > 1)
		szFileText = __tr("%1 files").arg(iFiles);
	else if(iFiles > 0)
		szFileText = __tr("one file");

	if((!szFileText.isEmpty()) && (!szDirText.isEmpty()))
		szText = __tr("You're about to delete %1 and %2. Do you want to continue?").arg(szFileText).arg(szDirText);
	else
		szText = __tr("You're about to delete %1. Do you want to continue?").arg(szFileText.isEmpty() ? szDirText : szFileText);

	QMessageBox::StandardButton eButton = QMessageBox::question(
			this,
			__tr("Delete Items"),
			szText,
			QMessageBox::Yes | QMessageBox::No
		);

	return eButton == QMessageBox::Yes;
}

void C3DockFileBrowser::svnRmSelected()
{
	QList<QListWidgetItem *> lSelected;
	
	if(!gatherSelectedItemsAndAskForDeletion(lSelected))
		return;

	QString szSvn = C3ExternalToolManager::instance()->findTool("svn");
	if(szSvn.isEmpty())
	{
		QMessageBox::information(this,__tr("SVN Tool Not Found"),__tr("Could not find svn executable"));
		return;
	}


	foreach(QListWidgetItem * it,lSelected)
	{
		QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

		QString szPath = hData.value("path").toString();

		QList<C3Editor *> lOpen;

		if(hData.value("isDirectory").toBool())
			C3Workspace::currentWorkspace()->findAllEditorsInDirectory(szPath,lOpen);
		else
			C3Workspace::currentWorkspace()->findAllEditors(szPath,lOpen);
	
		if(!lOpen.isEmpty())
		{
			foreach(C3Editor * ed,lOpen)
			{
				if(!C3Workspace::currentWorkspace()->closeEditor(ed,true))
					return;
			}
		}

		if(hData.value("isDirectory").toBool())
		{
			// stubbornly refuse to attempt deletion of important paths
			if((szPath == "/") || (szPath == "/usr") || (szPath == "/home") || (szPath.startsWith("/etc")) || (szPath.isEmpty()))
				continue;
		}

		C3ExternalCommand cmd;
		
		cmd.setType(C3ExternalCommand::ControlledProcessWithArguments);
		cmd.setCommand(szSvn);
		cmd.setSaveAllFiles(false);
		cmd.setHighlightPaths(true);
		cmd.setClearOutputOnStart(true);
	
		QStringList lArgs;
		lArgs.append(__utf8("rm"));
		lArgs.append(__utf8("--force"));
		lArgs.append(szPath);
	
		cmd.setName(__utf8("SVN"));
		cmd.setId(__utf8("internal.svn"));
		cmd.setArguments(lArgs);
	
		C3ExternalCommandManager::instance()->executeCommand(&cmd);
	}

	reloadDelayed();
}



void C3DockFileBrowser::renameSelected()
{
	QList<QListWidgetItem *> lSelected = _p->pListWidget->selectedItems();
	if(lSelected.isEmpty())
		return;

	QListWidgetItem * it = lSelected.first();

	QString szOriginalFileName = it->text();
	QString szFileName;

	for(;;)
	{
		bool ok;
		szFileName = QInputDialog::getText(
				this,
				__tr("Rename Item"),
				__tr("Please provide the new name for the item"),
				QLineEdit::Normal,
				szOriginalFileName,
				&ok
			);
	
		if(!ok)
			return;
		
		if(!szFileName.isEmpty())
			break;

		QMessageBox::StandardButton eButton = QMessageBox::critical(
				this,
				__tr("Rename Item"),
				__tr("The name can't be empty"),
				QMessageBox::Ok
			);
	}

	if(szFileName == szOriginalFileName)
		return;

	QList<C3Editor *> lOpen;

	QString szFullPath = C3FileUtils::mergePathComponents(_p->szDirectory,szOriginalFileName);

	QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

	if(hData.value("isDirectory").toBool())
		C3Workspace::currentWorkspace()->findAllEditorsInDirectory(szFullPath,lOpen);
	else
		C3Workspace::currentWorkspace()->findAllEditors(szFullPath,lOpen);

	if(!lOpen.isEmpty())
	{
		foreach(C3Editor * ed,lOpen)
		{
			if(!C3Workspace::currentWorkspace()->closeEditor(ed,true))
				return;
		}
	}

	QDir d(_p->szDirectory);
	if(!d.rename(szOriginalFileName,szFileName))
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				this,
				__tr("Rename Item"),
				__tr("Failed to rename the item '%1'").arg(szFileName),
				QMessageBox::Ok
			);
		
		return;
	}

	reload();
	selectItem(szFileName);

	C3Workspace::currentWorkspace()->notifyRescanNeeded();
}

void C3DockFileBrowser::deleteSelected()
{
	QList<QListWidgetItem *> lSelected;
	
	if(!gatherSelectedItemsAndAskForDeletion(lSelected))
		return;

	foreach(QListWidgetItem * it,lSelected)
	{
		QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

		QString szPath = hData.value("path").toString();

		QList<C3Editor *> lOpen;

		if(hData.value("isDirectory").toBool())
			C3Workspace::currentWorkspace()->findAllEditorsInDirectory(szPath,lOpen);
		else
			C3Workspace::currentWorkspace()->findAllEditors(szPath,lOpen);
	
		if(!lOpen.isEmpty())
		{
			foreach(C3Editor * ed,lOpen)
			{
				if(!C3Workspace::currentWorkspace()->closeEditor(ed,true))
					return;
			}
		}

		if(hData.value("isDirectory").toBool())
		{
			// stubbornly refuse to attempt deletion of important paths
			if((szPath == "/") || (szPath == "/usr") || (szPath == "/home") || (szPath.startsWith("/etc")) || (szPath.isEmpty()))
				continue;
		
			QDir d(szPath);
			if(!d.removeRecursively())
			{
				QMessageBox::StandardButton eButton = QMessageBox::critical(
						this,
						__tr("Delete Item"),
						__tr("Failed to delete the folder '%1'").arg(it->text()),
						QMessageBox::Ok
					);
			}
		} else {
			if(!QFile::remove(szPath))
			{
				QMessageBox::StandardButton eButton = QMessageBox::critical(
						this,
						__tr("Delete Item"),
						__tr("Failed to delete the file '%1'").arg(it->text()),
						QMessageBox::Ok
					);
			}
		}
	}

	reload();
}


void C3DockFileBrowser::newFolder()
{
	QString szFileName;

	for(;;)
	{
		bool ok;
		szFileName = QInputDialog::getText(
				this,
				__tr("New Folder"),
				__tr("Please provide the name for the new folder"),
				QLineEdit::Normal,
				__tr("newfolder"),
				&ok
			);
	
		if(!ok)
			return;
		
		if(!szFileName.isEmpty())
			break;

		QMessageBox::StandardButton eButton = QMessageBox::critical(
				this,
				__tr("New Folder"),
				__tr("You must specify a folder name"),
				QMessageBox::Ok
			);
	}

	QDir d(_p->szDirectory);
	if(!d.mkdir(szFileName))
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				this,
				__tr("New Folder"),
				__tr("Failed to create the folder '%1'").arg(szFileName),
				QMessageBox::Ok
			);
		
		return;
	}

	reload();
	selectItem(szFileName);
}

void C3DockFileBrowser::selectItem(const QString &szFileName)
{
	QListWidgetItem * it = findItem(szFileName);
	if(!it)
		return;
	_p->pListWidget->setCurrentItem(it);
}

QListWidgetItem * C3DockFileBrowser::findItem(const QString &szFileName)
{
	int c = _p->pListWidget->count();
	int i = 0;
	while(i < c)
	{
		QListWidgetItem * it = _p->pListWidget->item(i);
		if(it && (it->text() == szFileName))
			return it;
		i++;
	}
	
	return NULL;
}

void C3DockFileBrowser::setDirectory(const QString &szPath,bool bSelectCurrentFileIfPresent)
{
	QString szSelectPath;
	
	if(bSelectCurrentFileIfPresent)
	{
		C3Editor * ed = C3MainWindow::instance()->currentEditor();
		if(ed)
		{
			szSelectPath = ed->path();
			if(!szSelectPath.startsWith(szPath))
				szSelectPath = QString();
		}
	}

	QFileInfo inf(szPath);
	
	QString szFilter;
	QString szDir;
	
	if(inf.fileName().contains(QChar('*')) || inf.fileName().contains(QChar('?')))
	{
		szFilter = inf.fileName();
		szDir = inf.absolutePath();
		inf.setFile(szDir);
	} else {
		szDir = szPath;
	}

	if(!inf.exists())
	{
		warning(__tr("Directory '%1' does not exist").arg(szDir));
		return;
	}
	
	if(!inf.isDir())
	{
		warning(__tr("The path '%1' is not a directory").arg(szDir));
		return;
	}

	QString szRealPath = inf.canonicalFilePath();

	QDir d(szRealPath);

	_p->pListWidget->clear();

	QDir d2(szRealPath);
	bool bCanCdUp = d2.cdUp();

	QDir::Filters oFilters = QDir::NoDot | QDir::Dirs | QDir::Files | QDir::Drives | QDir::System | QDir::Hidden;
	
	if(!bCanCdUp)
		oFilters |= QDir::NoDotDot;

	if(!szFilter.isEmpty())
		d.setNameFilters(QStringList(szFilter));

	QFileInfoList fil = d.entryInfoList(oFilters,QDir::Name | QDir::DirsFirst);
	
	QListWidgetItem * pMakeCurrent = NULL;
	
	foreach(QFileInfo fi,fil)
	{
		QListWidgetItem * it = addEntry(fi);
		if(szSelectPath.isEmpty())
			continue;
		if(fi.canonicalFilePath() == szSelectPath)
			pMakeCurrent = it;
	}
	
	if(pMakeCurrent)
		_p->pListWidget->setCurrentItem(pMakeCurrent);

	if(szFilter.isEmpty())
	{
		if(szRealPath.endsWith(QDir::separator()))
			_p->pPathEdit->setText(szRealPath);
		else
			_p->pPathEdit->setText(__utf8("%1%2").arg(szRealPath).arg(QDir::separator()));
	} else {
		_p->pPathEdit->setText(szPath); // contains filter
	}

	_p->szDirectory = szRealPath;

	_p->pHomeAction->setEnabled(szRealPath != QDir::homePath());
	_p->pUpAction->setEnabled(bCanCdUp);
}

QListWidgetItem * C3DockFileBrowser::addEntry(const QFileInfo &inf)
{
	//qDebug("Add entry %s",inf.fileName().toUtf8().data());
	
	QString szIcon;
	
	if(inf.isDir())
	{
		szIcon = "folder.png";
	} else {
		QString szExt = inf.completeSuffix().toLower();
		szIcon = _p->hIconMap.value(szExt);
		if(szIcon.isEmpty())
		{
			if(szExt.contains(QChar('.')))
				szIcon = _p->hIconMap.value(inf.suffix().toLower());
			if(szIcon.isEmpty())
				szIcon = "blank.png";
		}
	}
	
	QPixmap pix = C3PixmapCache::instance()->pixmap(__utf8("fileicons/16px/%1").arg(szIcon));
	
	QListWidgetItem * it = new C3DockFileBrowserListWidgetItem();
	it->setText(inf.fileName());
	if(!pix.isNull())
		it->setIcon(pix);

	QHash<QString,QVariant> hData;

	hData.insert("isDirectory",QVariant(inf.isDir()));
	hData.insert("path",QVariant(inf.absoluteFilePath()));
	hData.insert("name",QVariant(inf.fileName()));
	
	it->setData(Qt::UserRole,hData);
	_p->pListWidget->addItem(it);
	
	return it;
}

void C3DockFileBrowser::slotPathEditReturnPressed()
{
	QString sFile = _p->pPathEdit->text().trimmed();

	QString sLocation;

	int idx = sFile.lastIndexOf(QChar(':'));
	if(idx > 3)
	{
		sLocation = sFile.mid(idx+1);
		sFile = sFile.left(idx);
	}

	QFileInfo inf(sFile);
	if(inf.isDir())
	{
		setDirectory(sFile);
		return;
	}

	if(inf.isFile())
	{
		C3Link oLink(sFile);
		if(!sLocation.isEmpty())
			oLink.setLocation(sLocation);
		C3Workspace::currentWorkspace()->openFileAsync(oLink);
		return;
	}

	QList<QString> lList;

	C3Workspace::currentWorkspace()->matchFiles(sFile,lList);

	if(lList.isEmpty())
	{
		warning(__tr("Directory/file '%1' not found").arg(sFile));
		return;
	}

	QList<C3Link> lLinks;
	
	Q_FOREACH(QString s,lList)
	{
		C3Link lnk;
		lnk.setPath(s);
		
		lLinks.append(lnk);
	}

	C3LinkSelectionDialog dlg(this,__tr("File Matches"),lLinks);
	
	if(dlg.exec() != QDialog::Accepted)
		return;
	
	C3Workspace::currentWorkspace()->openFileAsync(dlg.selectedLink());
}

void C3DockFileBrowser::slotItemDoubleClicked(QListWidgetItem *it)
{
	if(!it)
		return;
	
	QHash<QString,QVariant> hData = it->data(Qt::UserRole).toHash();

	QString szPath = hData.value("path").toString();

	if(hData.value("isDirectory").toBool())
	{
		setDirectory(szPath);
		return;
	}

	C3Workspace::currentWorkspace()->openFileAsync(szPath);
}
