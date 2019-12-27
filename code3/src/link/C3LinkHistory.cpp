//=============================================================================
//
//   File : C3LinkHistory.cpp
//   Creation Date : 2015/11/28 02:44:53
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

#include "C3LinkHistory.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3GeneralActions.h"

#include <QMenu>
#include <QAction>

C3LinkHistory * C3LinkHistory::m_pInstance = NULL;

#define MAX_ENTRIES 20

class C3LinkHistoryPrivate
{
public:
	QList<C3Link> lBackStack;
	QList<C3Link> lForwardStack;
};

#define _p m_pC3LH

C3LinkHistory::C3LinkHistory()
{
	m_pInstance = this;
	_p = new C3LinkHistoryPrivate();
}

C3LinkHistory::~C3LinkHistory()
{
	delete _p;
	m_pInstance = NULL;
}

void C3LinkHistory::addEntry(const C3Link &lnk)
{
	_p->lBackStack.insert(0,lnk);
	while(_p->lBackStack.count() > MAX_ENTRIES)
		_p->lBackStack.takeLast();
	_p->lForwardStack.clear();

	C3GeneralActions::instance()->linkHistoryChanged(!_p->lBackStack.isEmpty(),false);
}

bool C3LinkHistory::backAvailable() const
{
	return !_p->lBackStack.isEmpty();
}

bool C3LinkHistory::forwardAvailable() const
{
	return !_p->lForwardStack.isEmpty();
}

void C3LinkHistory::back()
{
	if(_p->lBackStack.isEmpty())
		return;

	C3Link oLink(_p->lBackStack.takeFirst());
	
	// add history entry for current file
	C3Link oCurrent;
	C3MainWindow::instance()->createCurrentHistoryEntry(oCurrent);
	_p->lForwardStack.insert(0,oCurrent);

	C3Workspace::currentWorkspace()->openFileAsync(oLink,false);

	C3GeneralActions::instance()->linkHistoryChanged(!_p->lBackStack.isEmpty(),true);
}

void C3LinkHistory::forward()
{
	if(_p->lForwardStack.isEmpty())
		return;

	C3Link oLink(_p->lForwardStack.takeFirst());
	
	// add history entry for current file
	C3Link oCurrent;
	C3MainWindow::instance()->createCurrentHistoryEntry(oCurrent);
	_p->lBackStack.insert(0,oCurrent);

	C3Workspace::currentWorkspace()->openFileAsync(oLink,false);

	C3GeneralActions::instance()->linkHistoryChanged(true,!_p->lForwardStack.isEmpty());
}

void C3LinkHistory::fillHistoryBackMenu(QMenu * pMenu)
{
	pMenu->clear();

	int c = _p->lBackStack.count();
	for(int i=0;i<c;i++)
	{
		C3Link oLink = _p->lBackStack.at(i);
		
		QString szText = oLink.path();
		QString szLocation = oLink.location();
		if(!szLocation.isEmpty())
			szText += __utf8(":%1").arg(szLocation);
		
		QAction * a = pMenu->addAction(szText);
		a->setData(QVariant(i));
	}
}

void C3LinkHistory::fillHistoryForwardMenu(QMenu * pMenu)
{
	pMenu->clear();

	int c = _p->lForwardStack.count();
	for(int i=0;i<c;i++)
	{
		C3Link oLink = _p->lForwardStack.at(i);
		
		QString szText = oLink.path();
		QString szLocation = oLink.location();
		if(!szLocation.isEmpty())
			szText += __utf8(":%1").arg(szLocation);
		
		QAction * a = pMenu->addAction(szText);
		a->setData(QVariant(i));
	}
}


void C3LinkHistory::backMenuActivated(QAction * a)
{
	bool ok;
	int d = a->data().toInt(&ok);
	if(!ok)
		return;
	if(d < 0)
		return;
	if(d >= _p->lBackStack.count())
		return;

	// add history entry for current file
	C3Link oCurrent;
	C3MainWindow::instance()->createCurrentHistoryEntry(oCurrent);
	_p->lForwardStack.insert(0,oCurrent);

	while(d > 0)
	{
		_p->lForwardStack.insert(0,_p->lBackStack.takeFirst());
		d--;
	}

	C3Link oLink(_p->lBackStack.takeFirst());

	C3Workspace::currentWorkspace()->openFileAsync(oLink,false);

	C3GeneralActions::instance()->linkHistoryChanged(!_p->lBackStack.isEmpty(),!_p->lForwardStack.isEmpty());
}

void C3LinkHistory::forwardMenuActivated(QAction * a)
{
	bool ok;
	int d = a->data().toInt(&ok);
	if(!ok)
		return;
	if(d < 0)
		return;
	if(d >= _p->lForwardStack.count())
		return;

	// add history entry for current file
	C3Link oCurrent;
	C3MainWindow::instance()->createCurrentHistoryEntry(oCurrent);
	_p->lBackStack.insert(0,oCurrent);

	while(d > 0)
	{
		_p->lBackStack.insert(0,_p->lForwardStack.takeFirst());
		d--;
	}

	C3Link oLink(_p->lForwardStack.takeFirst());

	C3Workspace::currentWorkspace()->openFileAsync(oLink,false);

	C3GeneralActions::instance()->linkHistoryChanged(!_p->lBackStack.isEmpty(),!_p->lForwardStack.isEmpty());
}

