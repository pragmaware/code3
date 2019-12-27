//=============================================================================
//
//   File : C3CentralWidget.cpp
//   Creation Date : dom 25 ott 2015 02:28:43
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

#include "C3CentralWidget.h"
#include "C3MessageContainerWidget.h"
#include "C3MainWindow.h"
#include "C3FindWidget.h"

#include <QStackedWidget>

class C3CentralWidgetPrivate
{
public:
	QStackedWidget * pStackedWidget;
	C3MessageContainerWidget * pMessageContainerWidget;
	C3FindWidget * pFindWidget;
};

#define _p m_pC3CW

C3CentralWidget::C3CentralWidget(C3MainWindow * pParent)
	: QWidget(pParent)
{
	_p = new C3CentralWidgetPrivate();

	_p->pStackedWidget = new QStackedWidget(this);
	_p->pMessageContainerWidget = NULL;
	_p->pFindWidget = NULL;
	
	layout();
}

C3CentralWidget::~C3CentralWidget()
{
	delete _p;
}

QStackedWidget * C3CentralWidget::stackedWidget()
{
	return _p->pStackedWidget;
}

void C3CentralWidget::resizeEvent(QResizeEvent * e)
{
	layout();
}

void C3CentralWidget::closeMessageContainer()
{
	if(!_p->pMessageContainerWidget)
		return;
	delete _p->pMessageContainerWidget;
	_p->pMessageContainerWidget = NULL;

	layout();
}

void C3CentralWidget::showFindWidget()
{
	if(!_p->pFindWidget)
		_p->pFindWidget = new C3FindWidget(this);
	_p->pFindWidget->show();
	layout();
}

bool C3CentralWidget::findWidgetVisible()
{
	return _p->pFindWidget;
}

C3FindWidget * C3CentralWidget::findWidget()
{
	return _p->pFindWidget;
}


void C3CentralWidget::hideFindWidget()
{
	if(!_p->pFindWidget)
		return;
	delete _p->pFindWidget;
	_p->pFindWidget = NULL;
	layout();
}

void C3CentralWidget::message(
		const QString &szMessage,
		const QString &szIcon,
		const QString &szStyleSheet
	)
{
	if(!_p->pMessageContainerWidget)
	{
		_p->pMessageContainerWidget = new C3MessageContainerWidget(this);
		_p->pMessageContainerWidget->show();
	}

	_p->pMessageContainerWidget->message(
			szMessage,
			szIcon,
			szStyleSheet
		);

	layout();
}

void C3CentralWidget::layout()
{
	int w = width();

	int h1 = 0;
	int h2 = 0;

	if(_p->pMessageContainerWidget)
	{
		h1 = _p->pMessageContainerWidget->heightForWidth(w);
		_p->pMessageContainerWidget->setGeometry(0,0,w,h1);
	}
	
	if(_p->pFindWidget)
	{
		h2 = _p->pFindWidget->sizeHint().height();
		_p->pFindWidget->setGeometry(0,height() - h2,w,h2);
	}
	
	_p->pStackedWidget->setGeometry(0,h1,w,height() - h1 - h2);
}

