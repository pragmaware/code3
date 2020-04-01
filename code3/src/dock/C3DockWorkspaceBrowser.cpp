//=============================================================================
//
//   File : C3DockWorkspaceBrowser.cpp
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

#include "C3DockWorkspaceBrowser.h"
#include "C3MainWindow.h"
#include "C3Workspace.h"
#include "C3Editor.h"
#include "C3StringUtils.h"
#include "C3PixmapCache.h"
#include "C3DockWidgetTitleBar.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QAction>
#include <QJsonObject>
#include <QHash>
#include <QWidgetAction>
#include <QLabel>
#include <QFont>
#include <QGridLayout>

C3DockWorkspaceBrowser * C3DockWorkspaceBrowser::m_pInstance = NULL;

class C3DockWorkspaceBrowserPrivate
{
public:
	bool bIgnoreEditorNotifications; // used during close
	C3DockWidgetTitleBar * pTitleBar;
	QTreeWidget * pTreeWidget;
	QMenu * pContextMenu;
	C3Workspace * pWorkspace; // shallow
	C3DockWorkspaceBrowser::ViewMode eViewMode;
	QHash<QString,QString> hGroupByFileExtension;
	QHash<QString,QTreeWidgetItem *> hParentsByFileGroup;
};

class C3DockWorkspaceBrowserEditorItem : public QTreeWidgetItem
{
private:
	QString m_szTitle;
	C3Editor * m_pEditor;
	C3DockWorkspaceBrowserPrivate * m_pPrivate;
public:
	C3DockWorkspaceBrowserEditorItem(C3DockWorkspaceBrowserPrivate * pPrivate,QTreeWidget * pTreeWidget)
		: QTreeWidgetItem(pTreeWidget), m_pPrivate(pPrivate)
	{
	}

	C3DockWorkspaceBrowserEditorItem(C3DockWorkspaceBrowserPrivate * pPrivate,QTreeWidgetItem * pParent)
		: QTreeWidgetItem(pParent), m_pPrivate(pPrivate)
	{
	}
	
	C3Editor * editor()
	{
		return m_pEditor;
	}
	
	void set(C3Editor * pEditor)
	{
		m_szTitle = pEditor->title();
		m_pEditor = pEditor;
		QString szText = pEditor->modified() ? __utf8("* %1").arg(m_szTitle) : m_szTitle;
		setText(0,szText);
	}
	
	virtual bool operator < (const QTreeWidgetItem &other) const
	{
		const C3DockWorkspaceBrowserEditorItem * it = dynamic_cast<const C3DockWorkspaceBrowserEditorItem *>(&other);
		if(!it)
			return false;
		if(m_pPrivate->eViewMode == C3DockWorkspaceBrowser::ByLastUsage)
			return m_pEditor->lastShowEventTime() > it->m_pEditor->lastShowEventTime();
		return m_szTitle < it->m_szTitle;
	}
};

#define _p m_pC3DWB

C3DockWorkspaceBrowser::C3DockWorkspaceBrowser(QWidget * pParent)
	: QDockWidget(pParent), C3AlertSource(__tr("Workspace Browser"))
{
	m_pInstance = this;

	_p = new C3DockWorkspaceBrowserPrivate();

	_p->pContextMenu = NULL;
	_p->bIgnoreEditorNotifications = false;
	_p->eViewMode = ByFileType;

	_p->hGroupByFileExtension.insert("cpp",__tr("C/C++ implementation files"));
	_p->hGroupByFileExtension.insert("cxx",__tr("C/C++ implementation files"));
	_p->hGroupByFileExtension.insert("lxx",__tr("C/C++ implementation files"));
	_p->hGroupByFileExtension.insert("cc",__tr("C/C++ implementation files"));
	_p->hGroupByFileExtension.insert("c",__tr("C/C++ implementation files"));
	_p->hGroupByFileExtension.insert("h",__tr("C/C++ header files"));
	_p->hGroupByFileExtension.insert("hpp",__tr("C/C++ header files"));
	_p->hGroupByFileExtension.insert("hxx",__tr("C/C++ header files"));
	_p->hGroupByFileExtension.insert("hh",__tr("C/C++ header files"));
	_p->hGroupByFileExtension.insert("js",__tr("Javascript files"));
	_p->hGroupByFileExtension.insert("json",__tr("JSON files"));
	_p->hGroupByFileExtension.insert("php",__tr("PHP files"));
	_p->hGroupByFileExtension.insert("php3",__tr("PHP files"));
	_p->hGroupByFileExtension.insert("php5",__tr("PHP files"));
	_p->hGroupByFileExtension.insert("jpg",__tr("Image files"));
	_p->hGroupByFileExtension.insert("jpeg",__tr("Image files"));
	_p->hGroupByFileExtension.insert("png",__tr("Image files"));
	_p->hGroupByFileExtension.insert("gif",__tr("Image files"));
	_p->hGroupByFileExtension.insert("tif",__tr("Image files"));
	_p->hGroupByFileExtension.insert("bmp",__tr("Image files"));
	_p->hGroupByFileExtension.insert("txt",__tr("Text files"));
	_p->hGroupByFileExtension.insert("md",__tr("Text files"));
	_p->hGroupByFileExtension.insert("README",__tr("Text files"));
	_p->hGroupByFileExtension.insert("TODO",__tr("Text files"));
	_p->hGroupByFileExtension.insert("py",__tr("Python files"));
	_p->hGroupByFileExtension.insert("pl",__tr("Perl files"));
	_p->hGroupByFileExtension.insert("java",__tr("Java files"));
	_p->hGroupByFileExtension.insert("m",__tr("Objective C files"));
	_p->hGroupByFileExtension.insert("html",__tr("HTML files"));
	_p->hGroupByFileExtension.insert("htm",__tr("HTML files"));
	_p->hGroupByFileExtension.insert("asp",__tr("ASP files"));
	_p->hGroupByFileExtension.insert("aspx",__tr("ASP files"));
	_p->hGroupByFileExtension.insert("ashx",__tr("ASP files"));
	_p->hGroupByFileExtension.insert("sql",__tr("SQL files"));
	_p->hGroupByFileExtension.insert("css",__tr("CSS files"));
	_p->hGroupByFileExtension.insert("less",__tr("CSS files"));
	_p->hGroupByFileExtension.insert("less",__tr("CSS files"));
	_p->hGroupByFileExtension.insert("CMakeLists.txt",__tr("CMake files"));
	_p->hGroupByFileExtension.insert("sh",__tr("Shell scripts"));

	_p->pTitleBar = new C3DockWidgetTitleBar(this,__tr("Workspace Browser"));
	
	QMenu * pMenu = _p->pTitleBar->addMenuButton(__utf8("dock/12px/gear.png"),__tr("View Options"));

	QObject::connect(pMenu,SIGNAL(aboutToShow()),this,SLOT(slotTitleBarMenuAboutToShow()));

	setTitleBarWidget(_p->pTitleBar);

	QWidget * pBase = new QWidget(this); // this is used to match the style margins (couldn't achieve this with the stylesheet :/)

	QGridLayout * pLayout = new QGridLayout(pBase);

	_p->pTreeWidget = new QTreeWidget(pBase);
	_p->pTreeWidget->setHeaderHidden(true);
	_p->pTreeWidget->setRootIsDecorated(false);
	_p->pTreeWidget->setSortingEnabled(true);
	_p->pTreeWidget->sortByColumn(0,Qt::AscendingOrder);
	_p->pTreeWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
	_p->pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	_p->pTreeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	pLayout->addWidget(_p->pTreeWidget,0,0);
	//pLayout->setContentsMargins(QMargins(3,0,3,3));
	pLayout->setContentsMargins(QMargins(0,0,0,0));
	pLayout->setSpacing(0);

	QObject::connect(_p->pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slotContextMenuRequested(const QPoint &)));
	QObject::connect(_p->pTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(slotItemClicked(QTreeWidgetItem *,int)));
	QObject::connect(_p->pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(slotCurrentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

	setWidget(pBase);
	
	setWindowTitle(__tr("Workspace Browser"));
	setObjectName(__utf8("C3DockWorkspaceBrowser"));
	//setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetMovable);
}

C3DockWorkspaceBrowser::~C3DockWorkspaceBrowser()
{
	if(_p->pContextMenu)
		delete _p->pContextMenu;
	delete _p;
	
	m_pInstance = NULL;
}

void C3DockWorkspaceBrowser::slotTitleBarMenuAboutToShow()
{
	QObject * ob = sender();
	if(!ob)
		return;
	QMenu * pMenu = dynamic_cast<QMenu *>(ob);
	if(!pMenu)
		return;

	pMenu->clear();

	QWidgetAction * act = new QWidgetAction(pMenu);
	
	QLabel * l = new QLabel(pMenu);
	l->setText(__tr("View Mode"));

	// FIXME: Does not seem to work?
	l->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	l->setLineWidth(1);
	l->setMidLineWidth(1);

	// FIXME: Does not seem to work?
	l->setAutoFillBackground(true);
	l->setBackgroundRole(QPalette::Window);

	// FIXME: Does not seem to work?
	QPalette pal = l->palette();
	pal.setColor(QPalette::Window,QColor(70,70,70));
	l->setPalette(pal);

	l->setAlignment(Qt::AlignCenter);
	
	QFont fnt = l->font();
	fnt.setBold(true);
	l->setFont(fnt);
	
	//l->setStyleSheet("QLabel { background-color: #404040; color: #ffffff; font-weight: bold; }"); <-- does not work, problems with style?
	
	act->setDefaultWidget(l);
	
	pMenu->addAction(act);
	
	QAction * a = pMenu->addAction(__tr("Flat"));
	a->setCheckable(true);
	a->setChecked(_p->eViewMode == Flat);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotSetViewModeFlat()));
	
	a = pMenu->addAction(__tr("By File Type"));
	a->setCheckable(true);
	a->setChecked(_p->eViewMode == ByFileType);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotSetViewModeByFileType()));

	a = pMenu->addAction(__tr("Modified / Unmodified"));
	a->setCheckable(true);
	a->setChecked(_p->eViewMode == ByModificationState);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotSetViewModeByModificationState()));

	a = pMenu->addAction(__tr("Last Usage"));
	a->setCheckable(true);
	a->setChecked(_p->eViewMode == ByLastUsage);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotSetViewModeByLastUsage()));
}

void C3DockWorkspaceBrowser::saveState(QJsonObject &oObject)
{
	switch(_p->eViewMode)
	{
		case ByFileType:
			oObject.insert(__utf8("viewMode"),__utf8("byFileType"));
		break;
		case ByModificationState:
			oObject.insert(__utf8("viewMode"),__utf8("byModificationState"));
		break;
		case ByLastUsage:
			oObject.insert(__utf8("viewMode"),__utf8("byLastUsage"));
		break;
		//case Flat:
		default:
			oObject.insert(__utf8("viewMode"),__utf8("flat"));
		break;
	}
	//oObject.insert(__utf8("directory"),_p->szDirectory);
}

void C3DockWorkspaceBrowser::restoreState(QJsonObject &oObject)
{
	QString szViewMode = oObject.value(__utf8("viewMode")).toString();
	if(szViewMode == __utf8("byFileType"))
		setViewMode(ByFileType);
	else if(szViewMode == __utf8("byModificationState"))
		setViewMode(ByModificationState);
	else if(szViewMode == __utf8("byLastUsage"))
		setViewMode(ByLastUsage);
	else
		setViewMode(Flat);
}

void C3DockWorkspaceBrowser::attachToWorkspace(C3Workspace * pWorkspace)
{
	if(_p->pWorkspace)
	{
		// not dead yet, detach
		QObject::disconnect(_p->pWorkspace,SIGNAL(destroyed()),this,SLOT(slotWorkspaceDestroyed()));
	}

	_p->pWorkspace = pWorkspace;

	if(!pWorkspace)
	{
		refill();
		return;
	}
	
	QObject::connect(_p->pWorkspace,SIGNAL(destroyed()),this,SLOT(slotWorkspaceDestroyed()));
	
	refill();
}

void C3DockWorkspaceBrowser::slotWorkspaceDestroyed()
{
	_p->pWorkspace = NULL;
	refill();
}

void C3DockWorkspaceBrowser::editorOpened(C3Editor * pEditor)
{
	createItemForEditor(pEditor);
}

void C3DockWorkspaceBrowser::editorClosed(C3Editor * pEditor)
{
	if(_p->bIgnoreEditorNotifications)
		return;

	C3DockWorkspaceBrowserEditorItem * it = pEditor->workspaceBrowserItem();
	if(!it)
		return; // huh?

	removeItem(it);
}

void C3DockWorkspaceBrowser::removeItem(C3DockWorkspaceBrowserEditorItem * it)
{
	QTreeWidgetItem * p = it->parent();

	delete it;
	
	while(p)
	{
		if(p->childCount() != 0)
			break;

		_p->hParentsByFileGroup.remove(p->text(0));

		QTreeWidgetItem * pa = p->parent();
		
		delete p;
		p = pa;
	}
}

void C3DockWorkspaceBrowser::editorModifiedStateChanged(C3Editor * pEditor)
{
	if(_p->bIgnoreEditorNotifications)
		return;

	updateItemForEditor(pEditor);
}

void C3DockWorkspaceBrowser::editorTitleChanged(C3Editor * pEditor)
{
	if(_p->bIgnoreEditorNotifications)
		return;

	updateItemForEditor(pEditor);
}

void C3DockWorkspaceBrowser::currentEditorChanged(C3Editor * pEditor)
{
	if(_p->bIgnoreEditorNotifications)
		return;

	if(!pEditor)
	{
		_p->pTreeWidget->setCurrentItem(NULL,0,QItemSelectionModel::Clear);
		return;
	}

	if(_p->eViewMode == ByLastUsage)
	{
		updateItemForEditor(pEditor);
		_p->pTreeWidget->sortItems(0,Qt::AscendingOrder);
	}

	C3DockWorkspaceBrowserEditorItem * it = pEditor->workspaceBrowserItem();

	if(!it)
	{
		_p->pTreeWidget->setCurrentItem(NULL,0,QItemSelectionModel::Clear);
		return;
	}
	
	if(_p->pTreeWidget->currentItem() != it)
		_p->pTreeWidget->setCurrentItem(it,0,QItemSelectionModel::Current | QItemSelectionModel::Select | QItemSelectionModel::Clear);

}

void C3DockWorkspaceBrowser::updateItem(C3DockWorkspaceBrowserEditorItem * it,C3Editor * pEditor)
{
	QTreeWidgetItem * par = parentItemForEditor(pEditor);

	if(par != it->parent())
	{
		removeItem(it);
		createItemForEditor(pEditor);
		return;
	}

	it->set(pEditor);
	//if(!pix.isNull())
	//	it->setIcon(pix);
}

void C3DockWorkspaceBrowser::updateItemForEditor(C3Editor * pEditor)
{
	C3DockWorkspaceBrowserEditorItem * it = pEditor->workspaceBrowserItem();
	if(!it)
		return; // huh?

	updateItem(it,pEditor);
}

QTreeWidgetItem * C3DockWorkspaceBrowser::parentItemForEditor(C3Editor * pEditor)
{
	QString szGroup;

	switch(_p->eViewMode)
	{
		case ByFileType:
		{
			QString szFileName = pEditor->title();

			szGroup = _p->hGroupByFileExtension.value(szFileName);
			if(szGroup.isEmpty())
			{
				QString szExt = C3StringUtils::fileExtensionFromPath(szFileName).toLower();
				szGroup = _p->hGroupByFileExtension.value(szExt);
				if(szGroup.isEmpty())
					szGroup = __tr("Other files");
			}

		}
		break;
		case ByModificationState:
		{
			szGroup = pEditor->modified() ? __tr("Modified") : __tr("Not Modified");
		}
		break;
		//case ByLastUsage:
		//case Flat:
		default:
			// nuthin
		break;
	}

	if(szGroup.isEmpty())
		return NULL;

	QTreeWidgetItem * it = _p->hParentsByFileGroup.value(szGroup);

	if(!it)
	{
		QPixmap pix = C3PixmapCache::instance()->pixmap(__utf8("fileicons/16px/folder.png"));

		it = new QTreeWidgetItem(_p->pTreeWidget);
		it->setText(0,szGroup);
		it->setExpanded(true);
		//it->setBackground(0,QColor(40,40,40));
		it->setBackground(0,QColor(70,70,70));
		it->setIcon(0,pix);
		it->setFlags(it->flags() & ~Qt::ItemIsSelectable);
		_p->hParentsByFileGroup.insert(szGroup,it);
	}

	return it;
}


void C3DockWorkspaceBrowser::createItemForEditor(C3Editor * pEditor)
{
	QTreeWidgetItem * pParent = parentItemForEditor(pEditor);

	C3DockWorkspaceBrowserEditorItem * it;
	if(!pParent)
		it = new C3DockWorkspaceBrowserEditorItem(_p,_p->pTreeWidget);
	else
		it = new C3DockWorkspaceBrowserEditorItem(_p,pParent);
	
	updateItem(it,pEditor);

	pEditor->setWorkspaceBrowserItem(it);
}

void C3DockWorkspaceBrowser::slotSetViewModeFlat()
{
	setViewMode(Flat);
}

void C3DockWorkspaceBrowser::slotSetViewModeByFileType()
{
	setViewMode(ByFileType);
}

void C3DockWorkspaceBrowser::slotSetViewModeByModificationState()
{
	setViewMode(ByModificationState);
}

void C3DockWorkspaceBrowser::slotSetViewModeByLastUsage()
{
	setViewMode(ByLastUsage);
}

void C3DockWorkspaceBrowser::setViewMode(ViewMode eViewMode)
{
	if(_p->eViewMode == eViewMode)
		return;
	_p->eViewMode = eViewMode;
	refill();
}

void C3DockWorkspaceBrowser::refill()
{
	_p->pTreeWidget->clear();

	_p->hParentsByFileGroup.clear();

	if(!_p->pWorkspace)
		return;
	
	QHash<int,C3Editor *> & hEditors = _p->pWorkspace->editors();

	Q_FOREACH(C3Editor * pEditor,hEditors)
		createItemForEditor(pEditor);
	
	currentEditorChanged(C3MainWindow::instance()->currentEditor());
}

void C3DockWorkspaceBrowser::slotItemClicked(QTreeWidgetItem *it,int column)
{
/*
	C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(it);
	if(!item)
		return;
	_p->pWorkspace->activateEditor(item->editor());
*/
}

void C3DockWorkspaceBrowser::slotCurrentItemChanged(QTreeWidgetItem * cur,QTreeWidgetItem *prev)
{
	if(cur == prev)
		return;
	C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(cur);
	if(!item)
		return;
	_p->pWorkspace->activateEditor(item->editor());
}


void C3DockWorkspaceBrowser::slotContextMenuRequested(const QPoint &pnt)
{
	QTreeWidgetItem * it = _p->pTreeWidget->itemAt(pnt);
	if(!it)
	{
		return; // ...or we want to do something here?
	}

	//if(!it->isSelected())
	//	_p->pTreeWidget->setSelected(it);

	QList<QTreeWidgetItem *> lSelected = _p->pTreeWidget->selectedItems();
	if(lSelected.isEmpty())
	{
		return; // ...or we want to do something here?
	}

	bool bGotModified = false;

	Q_FOREACH(it,lSelected)
	{
		C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(it);
		if(!item)
			continue;
		if(item->editor()->modified())
		{
			bGotModified = true;
			break;
		}
	}

	if(!_p->pContextMenu)
	{
		_p->pContextMenu = new QMenu();
		QObject::connect(_p->pContextMenu,SIGNAL(triggered(QAction *)),this,SLOT(slotContextMenuActionTriggered(QAction *)));
	} else {
		_p->pContextMenu->clear();
	}

	//int iTotalCount = _p->pTreeWidget->topLevelItemCount();
	int iSelectedCount = lSelected.count();

	QAction * pAction;

#define ADD_ACTION(_szId,_szText) \
	do { \
		pAction = _p->pContextMenu->addAction(_szText); \
		pAction->setData(QVariant(QString(_szId))); \
	} while(0)

	if(bGotModified)
	{
		ADD_ACTION("save",__tr("Save"));
		_p->pContextMenu->addSeparator();
	}

	ADD_ACTION("close",__tr("Close"));

	//if(iTotalCount > iSelectedCount)
	ADD_ACTION("closeother",__tr("Close Other"));

	ADD_ACTION("closeall",__tr("Close All"));


	_p->pContextMenu->popup(_p->pTreeWidget->mapToGlobal(pnt));
}

void C3DockWorkspaceBrowser::slotContextMenuActionTriggered(QAction * pAction)
{
	QString szAction = pAction->data().toString();

	_p->bIgnoreEditorNotifications = true;

	QList<QTreeWidgetItem *> lSelected = _p->pTreeWidget->selectedItems();

	if(szAction == __utf8("save"))
	{
		foreach(QTreeWidgetItem * it,lSelected)
		{
			C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(it);
			if(!item)
				continue;

			if(!item->editor()->save())
				goto finished;
		}
	} else if(szAction == __utf8("close"))
	{
		foreach(QTreeWidgetItem * it,lSelected)
		{
			C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(it);
			if(!item)
				continue;

			if(!_p->pWorkspace->closeEditor(item->editor(),true))
				goto finished;
		}
	} else if(szAction == __utf8("closeall"))
	{
		int c = _p->pTreeWidget->topLevelItemCount();
		int i = 0;
		while(i < c)
		{
			QTreeWidgetItem * it = _p->pTreeWidget->topLevelItem(i);
			i++;

			int cc = it->childCount();
			int ii = 0;
			while(ii < cc)
			{
				QTreeWidgetItem * it2 = it->child(ii);
				ii++;
			
				C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(it2);
				if(!item)
					continue;
	
				if(!_p->pWorkspace->closeEditor(item->editor(),true))
					goto finished;
			}
		}
	} else if(szAction == __utf8("closeother"))
	{
		int c = _p->pTreeWidget->topLevelItemCount();
		int i = 0;
		while(i < c)
		{
			QTreeWidgetItem * it = _p->pTreeWidget->topLevelItem(i);
			i++;

			int cc = it->childCount();
			int ii = 0;
			while(ii < cc)
			{
				QTreeWidgetItem * it2 = it->child(ii);
				ii++;
			
				if(!lSelected.contains(it2))
				{
					C3DockWorkspaceBrowserEditorItem * item = dynamic_cast<C3DockWorkspaceBrowserEditorItem *>(it2);
					if(!item)
						continue;
	
					if(!_p->pWorkspace->closeEditor(item->editor(),true))
						goto finished;
				}
			}
		}
	}

finished:

	_p->bIgnoreEditorNotifications = false;
	
	refill();
}
