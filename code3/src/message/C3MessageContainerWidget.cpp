//=============================================================================
//
//   File : C3MessageContainerWidget.cpp
//   Creation Date : dom 25 ott 2015 02:33:28
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

#include "C3MessageContainerWidget.h"
#include "C3MessageWidget.h"
#include "C3PixmapCache.h"
#include "C3CentralWidget.h"

#include <QList>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QToolButton>

class C3MessageContainerWidgetPrivate
{
public:
	C3CentralWidget * pCentralWidget;

	QWidget * pBase;
	QScrollBar * pScrollBar;
	
	QToolButton * pCloseButton;
	
	QList<C3MessageWidget *> lAlertWidgets;
	
	int iScrollBarWidth;
	
	int iBaseWidth;
	int iBaseHeight;
};

#define _p m_pC3ACW

C3MessageContainerWidget::C3MessageContainerWidget(C3CentralWidget * pParent)
	: QWidget(pParent)
{
	_p = new C3MessageContainerWidgetPrivate();

	_p->pCentralWidget = pParent;

	_p->pBase = new QWidget(this);
	_p->pScrollBar = new QScrollBar(this);
	_p->pScrollBar->hide();
	QObject::connect(_p->pScrollBar,SIGNAL(valueChanged(int)),this,SLOT(slotScrollBarValueChanged(int)));
	
	_p->iScrollBarWidth = _p->pScrollBar->sizeHint().width();
	_p->iBaseWidth = -1;
	
	_p->pCloseButton = new QToolButton(this);
	_p->pCloseButton->setIcon(C3PixmapCache::instance()->pixmap("flat/24px/x.png"));
	_p->pCloseButton->setAutoRaise(true);
	_p->pCloseButton->raise();
	QObject::connect(_p->pCloseButton,SIGNAL(clicked()),this,SLOT(slotCloseClicked()));
}

C3MessageContainerWidget::~C3MessageContainerWidget()
{
	delete _p;
}

void C3MessageContainerWidget::message(
		const QString &szMessage,
		const QString &szIcon,
		const QString &szStyleSheet
	)
{
	C3MessageWidget * pMessageWidget = new C3MessageWidget(_p->pBase);

	_p->lAlertWidgets.append(pMessageWidget);

	_p->iBaseWidth = -1;

	pMessageWidget->showMessage(szMessage,szIcon,szStyleSheet);
	pMessageWidget->show();

	doLayout();
}

int C3MessageContainerWidget::computeBaseHeight(int iWidth)
{
	int h = 0;
	int w = iWidth - _p->iScrollBarWidth - 36; // 36 for close button
	
	foreach(C3MessageWidget * pMessageWidget,_p->lAlertWidgets)
		h += pMessageWidget->heightForWidth(w);
	
	return h;
}

int C3MessageContainerWidget::heightForWidth(int iWidth)
{
	int h = (_p->iBaseWidth == iWidth) ? _p->iBaseHeight : computeBaseHeight(iWidth);

	if(h < 34)
		h = 34;
	else if(h > 200)
		h = 200;
	return h;
}

void C3MessageContainerWidget::doLayout()
{
	if(_p->iBaseWidth != width())
	{
		_p->iBaseHeight = computeBaseHeight(width());
		_p->iBaseWidth = width();
	}
		
	if(_p->iBaseHeight > height())
	{
		_p->pScrollBar->setGeometry(width() - _p->iScrollBarWidth,0,_p->iScrollBarWidth,height());
		_p->pScrollBar->setMaximum(_p->iBaseHeight - height());

		if(!_p->pScrollBar->isVisible())
			_p->pScrollBar->show();

		_p->pBase->setGeometry(0,-_p->pScrollBar->value(),width() - _p->pScrollBar->width(),_p->iBaseHeight);
	} else {
		if(_p->pScrollBar->isVisible())
			_p->pScrollBar->hide();

		_p->pBase->setGeometry(0,0,width(),_p->iBaseHeight);
	}
	
	int w = _p->pBase->width();
	int y = 0;
	foreach(C3MessageWidget * pMessageWidget,_p->lAlertWidgets)
	{
		int h = pMessageWidget->heightForWidth(w);
		pMessageWidget->setGeometry(0,y,w,h);
		y += h;
	}
	
	QSize sh = _p->pCloseButton->sizeHint();
	
	_p->pCloseButton->setGeometry(
			_p->pBase->width() - 5 - sh.width(),
			5,
			sh.width(),
			sh.height()
		);
}

void C3MessageContainerWidget::slotScrollBarValueChanged(int iVal)
{
	doLayout();
}

void C3MessageContainerWidget::resizeEvent(QResizeEvent * e)
{
	doLayout();
}

void C3MessageContainerWidget::slotCloseClicked()
{
	_p->pCentralWidget->closeMessageContainer();
}

