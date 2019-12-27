//=============================================================================
//
//   File : C3DockWidgetTitleBar.cpp
//   Creation Date : 2015/11/15 00:56:59
//   Project : 
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

#include "C3DockWidgetTitleBar.h"
#include "C3PixmapCache.h"

#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QMenu>

class C3DockWidgetTitleBarPrivate
{
public:
	QGridLayout * pLayout;
	
	QLabel * pLabel;
	
	int iLastColumn;
};

#define _p m_pC3DWTB

C3DockWidgetTitleBar::C3DockWidgetTitleBar(QWidget * pParent,const QString &szTitle)
	: QFrame(pParent)
{
	_p = new C3DockWidgetTitleBarPrivate();
	_p->pLayout = new QGridLayout(this);

	_p->pLabel = new QLabel(this);
	//_p->pLabel->setStyleSheet("margin: 0; padding: 0;");
	//_p->pLabel->setMargin(0);
	_p->pLabel->setText(szTitle);
	_p->pLayout->addWidget(_p->pLabel,0,0);
	
	_p->pLayout->setColumnStretch(0,1000);
	_p->pLayout->setMargin(0);
	_p->pLayout->setContentsMargins(QMargins(5,2,1,1));
	_p->pLayout->setSpacing(0);
	
	_p->iLastColumn = 0;
	
	//setStyleSheet("background-color: #ff0000;");
	
	setFrameStyle(QFrame::NoFrame);
}

C3DockWidgetTitleBar::~C3DockWidgetTitleBar()
{
	delete _p;
}

void C3DockWidgetTitleBar::setTitle(const QString &szTitle)
{
	_p->pLabel->setText(szTitle);
}

void C3DockWidgetTitleBar::addSeparator()
{
	QFrame * w = new QFrame(this);

	_p->iLastColumn++;
	_p->pLayout->setColumnMinimumWidth(_p->iLastColumn,4);

	w->setFrameStyle(QFrame::NoFrame);
	w->setMinimumSize(QSize(2,2));
	w->setMaximumSize(QSize(2,200));
	w->setObjectName("separator");

	_p->iLastColumn++;
	_p->pLayout->addWidget(w,0,_p->iLastColumn);

	_p->iLastColumn++;
	_p->pLayout->setColumnMinimumWidth(_p->iLastColumn,4);
}

QToolButton * C3DockWidgetTitleBar::addButton(const QString &szIcon,const QString &szToolTip)
{
	QToolButton * pButton = new QToolButton(this);
	pButton->setStyleSheet("margin: 1; margin-top: 2; padding: 1;");
	pButton->setIcon(C3PixmapCache::instance()->pixmap(szIcon));
	pButton->setIconSize(QSize(12,12));
	if(!szToolTip.isEmpty())
		pButton->setToolTip(szToolTip);
	//_p->pMenuButton->setFixedSize(QSize(20,24));
	pButton->setAutoRaise(true);
	
	_p->iLastColumn++;

	_p->pLayout->addWidget(pButton,0,_p->iLastColumn);
	
	return pButton;
}

QMenu * C3DockWidgetTitleBar::addMenuButton(const QString &szIcon,const QString &szToolTip)
{
	QMenu * pMenu = new QMenu(this);
	
	QToolButton * pMenuButton = new QToolButton(this);
	pMenuButton->setStyleSheet("margin: 1; margin-top: 2; padding: 1; padding-right: 12;");
	//_p->pMenuButton->setArrowType(Qt::DownArrow);
	//_p->pMenuButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");
	pMenuButton->setIcon(C3PixmapCache::instance()->pixmap(szIcon));
	pMenuButton->setPopupMode(QToolButton::InstantPopup);
	pMenuButton->setMenu(pMenu);
	pMenuButton->setIconSize(QSize(12,12));
	if(!szToolTip.isEmpty())
		pMenuButton->setToolTip(szToolTip);
	//_p->pMenuButton->setFixedSize(QSize(20,24));
	pMenuButton->setAutoRaise(true);
	
	_p->iLastColumn++;
	
	_p->pLayout->addWidget(pMenuButton,0,_p->iLastColumn);
	
	return pMenu;
}
