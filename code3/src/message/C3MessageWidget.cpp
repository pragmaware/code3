//=============================================================================
//
//   File : C3MessageWidget.cpp
//   Creation Date : dom 25 ott 2015 22:15:50
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

#include "C3MessageWidget.h"
#include "C3PixmapCache.h"

#include <QLabel>
#include <QGridLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

class C3MessageWidgetPrivate
{
public:
	QGridLayout * pLayout;
	
	QLabel * pIconLabel;
	QLabel * pTextLabel;
};

#define _p m_pC3AW

#define MARGIN 4
#define SPACING 2

C3MessageWidget::C3MessageWidget(QWidget * pParent)
	: QWidget(pParent)
{
	_p = new C3MessageWidgetPrivate();

	
	_p->pLayout = new QGridLayout(this);

	_p->pIconLabel = new QLabel(this);
	_p->pLayout->addWidget(_p->pIconLabel,0,0);

	_p->pTextLabel = new QLabel(this);
	_p->pTextLabel->setWordWrap(true);
	_p->pLayout->addWidget(_p->pTextLabel,0,1);
	
	_p->pLayout->setMargin(MARGIN);
	_p->pLayout->setSpacing(SPACING);
	
	QWidget * pSpacer = new QWidget(this);
	_p->pLayout->addWidget(pSpacer,0,2);
	
	_p->pLayout->setColumnStretch(1,100);
	_p->pLayout->setColumnMinimumWidth(2,34);
	
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Background);
}

C3MessageWidget::~C3MessageWidget()
{
	delete _p;
}

void C3MessageWidget::showMessage(
		const QString &szMessage,
		const QString &szIcon,
		const QString &szStyleSheet
	)
{
	if(!szStyleSheet.isEmpty())
		setStyleSheet(szStyleSheet);

	_p->pIconLabel->setPixmap(
			C3PixmapCache::instance()->pixmap(
					szIcon.isEmpty() ? __utf8("message/32px/question.png") : szIcon
				)
		);

	_p->pTextLabel->setText(szMessage);
}

int C3MessageWidget::heightForWidth(int iWidth)
{
	iWidth -= MARGIN + SPACING + MARGIN;

	QSize sh1 = _p->pIconLabel->sizeHint();
	
	iWidth -= sh1.width();
	
	int h = _p->pTextLabel->heightForWidth(iWidth);
	
	if(sh1.height() > h)
		h = sh1.height();

	return MARGIN + h + MARGIN;
}

// This is needed for the style sheet to work
void C3MessageWidget::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

