//=============================================================================
//
//   File : C3Application.cpp
//   Creation Date : mar 20 ott 2015 00:01:32
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

#include "C3Application.h"
#include "C3MainWindow.h"
#include "C3PixmapCache.h"

#include "C3TextEditorFactory.h"
#include "C3ImageEditorFactory.h"
#include "C3ExternalToolManager.h"
#include "C3Workspace.h"
#include "C3Link.h"

#include <QList>
#include <QVariant>
#include <QHash>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStyleFactory>

C3Application * C3Application::m_pInstance = NULL;

class C3ApplicationPrivate
{
public:
	C3MainWindow * pMainWindow;
	C3PixmapCache * pPixmapCache;
	C3ExternalToolManager * pExternalToolManager;
	QList<C3EditorFactory *> lFactories;
};

#define _p m_pC3A

C3Application::C3Application(int & argc,char ** argv)
	: QApplication(argc,argv), C3AlertSource(__tr("Application"))
{
	m_pInstance = this;
	
	setOrganizationName("pragmaware");
	setApplicationName("code3");
	setOrganizationDomain("www.pragmaware.net");

	_p = new C3ApplicationPrivate();
	_p->pMainWindow = NULL;
	_p->pPixmapCache = new C3PixmapCache();
	_p->pExternalToolManager = new C3ExternalToolManager();
	
	_p->lFactories.append(new C3TextEditorFactory());
	_p->lFactories.append(new C3ImageEditorFactory());

	loadStyleSheet();
}

C3Application::~C3Application()
{
	qDeleteAll(_p->lFactories);
	_p->lFactories.clear();
	delete _p->pMainWindow;
	delete _p->pPixmapCache;
	delete _p->pExternalToolManager;
	delete _p;
	m_pInstance = NULL;
}

bool C3Application::init(int argc,char ** argv)
{
	QList< C3Link > lFilesToOpen;
	QStringList lWorkspacesToOpen;

	QString szCurrentDir = QDir::currentPath();

	for(int i=1;i<argc;i++)
	{
		QString szFileName = argv[i];
		if(szFileName.isEmpty())
			continue;
		
		QString szLocation;
		
		int idx = szFileName.indexOf(QChar(':'));
		if(idx >= 0)
		{
			QString szFull = szFileName;
			szFileName = szFull.left(idx);
			szLocation = szFull.mid(idx+1);
		}
		
		if(!QDir::isAbsolutePath(szFileName))
		{
			if(szFileName.startsWith(QDir::separator()) || szCurrentDir.endsWith(QDir::separator()))
				szFileName = __utf8("%1%3").arg(szCurrentDir).arg(szFileName);
			else
				szFileName = __utf8("%1%2%3").arg(szCurrentDir).arg(QDir::separator()).arg(szFileName);
		}
		
		if(
				szFileName.endsWith(__utf8(".c3workspace")) ||
				szFileName.endsWith(__utf8(".c3wsp")) ||
				szFileName.endsWith(__utf8(".ssexwsp")) // Old name
			)
			lWorkspacesToOpen.append(szFileName);
		else {
			C3Link oLink(szFileName);
			if(!szLocation.isEmpty())
				oLink.setLocation(szLocation);
			lFilesToOpen.append(oLink);
		}
	}
	
	// FIXME: If there are any workspace files in the list, open them first!

	_p->pMainWindow = new C3MainWindow();
	_p->pMainWindow->show();

	bool bWorkspaceOpen = false;

	if(lWorkspacesToOpen.count() > 0)
	{
		QString szFirst = lWorkspacesToOpen.at(0);
		QString szError;
		if(_p->pMainWindow->openWorkspace(szFirst,szError))
			bWorkspaceOpen = true;
		else
			error(__tr("Could not open workspace file %1: %2").arg(szFirst).arg(szError));
	}
	
	if(!bWorkspaceOpen)
		_p->pMainWindow->loadInitialDefaultWorkspace();

	C3Workspace * pWorkspace = C3Workspace::currentWorkspace();

	foreach(const C3Link &hParams,lFilesToOpen)
		pWorkspace->openFileAsync(hParams,false); // no history entries

	return true;
}

QList<C3EditorFactory *> & C3Application::editorFactories() const
{
	return _p->lFactories;
}


void C3Application::loadStyleSheet()
{
	QFile f(":qdarkstyle/style.qss");
	if(!f.exists())
	{
		qDebug("WARNING: Unable to set stylesheet, file not found: will run with default style");
		return;
	}
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream ts(&f);
	setStyleSheet(ts.readAll());
	setStyle(QStyleFactory::create("windows")); // this stile hopefully won't override our settings
}
