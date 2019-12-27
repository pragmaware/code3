//=============================================================================
//
//   File : C3NewDialog.cpp
//   Creation Date : sab 07 nov 2015 05:43:05
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

#include "C3NewDialog.h"

#include "C3NewObjectCreator.h"
#include "C3PixmapCache.h"

#include "C3NewEmptyTextFileCreator.h"
#include "C3NewCPPClassCreator.h"
#include "C3NewPHPFileCreator.h"
#include "C3NewJavaClassCreator.h"

#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QHash>

class C3NewDialogPrivate
{
public:
	QGridLayout * pLayout;

	QListWidget * pListWidget;
	
	QPushButton * pOkButton;
	QPushButton * pCancelButton;
	
	QHash<QString,C3NewObjectCreator *> hCreators;
};

#define _p m_pC3ND

C3NewDialog::C3NewDialog()
	: QDialog()
{
	setWindowTitle(__tr("New Document"));

	_p = new C3NewDialogPrivate();

	_p->pLayout = new QGridLayout(this);

	QLabel * l = new QLabel(this);
	l->setText(__tr("Please select the type of document or object you want to create"));
	l->setMargin(5);
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignCenter);
	
	_p->pLayout->addWidget(l,0,0,1,3);

	_p->pListWidget = new QListWidget(this);

	_p->pListWidget->setSelectionMode(QListWidget::SingleSelection);
	_p->pListWidget->setFlow(QListWidget::LeftToRight);
	_p->pListWidget->setWrapping(true);
	_p->pListWidget->setSpacing(4);
	_p->pListWidget->setSortingEnabled(false);
	_p->pListWidget->setIconSize(QSize(64,64));
	_p->pListWidget->setResizeMode(QListView::Adjust);
	_p->pListWidget->setMovement(QListView::Static);
	_p->pListWidget->setViewMode(QListView::IconMode);

	QObject::connect(_p->pListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(slotItemDoubleClicked(QListWidgetItem *)));
	QObject::connect(_p->pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *)));
	
	_p->pLayout->addWidget(_p->pListWidget,1,0,1,3);
	
	_p->pCancelButton = new QPushButton(this);
	_p->pCancelButton->setText(__tr("Cancel"));
	QObject::connect(_p->pCancelButton,SIGNAL(clicked()),this,SLOT(slotCancelClicked()));
	_p->pLayout->addWidget(_p->pCancelButton,2,1);

	_p->pOkButton = new QPushButton(this);
	_p->pOkButton->setText(__tr("OK"));
	_p->pOkButton->setDefault(true);
	QObject::connect(_p->pOkButton,SIGNAL(clicked()),this,SLOT(slotOkClicked()));
	_p->pLayout->addWidget(_p->pOkButton,2,2);
	
	_p->pLayout->setMargin(10);
	_p->pLayout->setSpacing(5);
	_p->pLayout->setRowStretch(1,100);
	_p->pLayout->setColumnStretch(0,100);
	_p->pLayout->setColumnMinimumWidth(1,80);
	_p->pLayout->setColumnMinimumWidth(2,80);

	setMinimumSize(QSize(450,450));

	fillCreatorHash();

	fillListWidget();

	enableDisableButtons();
}

C3NewDialog::~C3NewDialog()
{
	qDeleteAll(_p->hCreators);

	delete _p;
}

void C3NewDialog::fillCreatorHash()
{
	_p->hCreators.insert("plain",new C3NewEmptyTextFileCreator(__tr("Text File"),__utf8("new-text.png"),__ascii("plain")));
	_p->hCreators.insert("javascript",new C3NewEmptyTextFileCreator(__tr("Javascript File"),__utf8("new-code.png"),__ascii("js")));
	_p->hCreators.insert("css",new C3NewEmptyTextFileCreator(__tr("CSS File"),__utf8("new-css.png"),__ascii("css")));
	_p->hCreators.insert("cpp",new C3NewCPPClassCreator());
	_p->hCreators.insert("java",new C3NewJavaClassCreator());
	_p->hCreators.insert("php",new C3NewPHPFileCreator());
}

void C3NewDialog::fillListWidget()
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	QHash<QString,C3NewObjectCreator *>::Iterator it;
	for(it = _p->hCreators.begin();it != _p->hCreators.end();++it)
	{
		C3NewObjectCreator * pCreator = it.value();
		
		QString szId = it.key();
		QString szName = pCreator->name();
		
		QPixmap pix = C3PixmapCache::instance()->pixmap(__utf8("new/64px/%1").arg(pCreator->icon()));
		
		QListWidgetItem * item = new QListWidgetItem();

		item->setText(pCreator->name());
		if(!pix.isNull())
			item->setIcon(pix);
		
		item->setData(Qt::UserRole,szId);
		_p->pListWidget->addItem(item);
	}
}

void C3NewDialog::slotItemDoubleClicked(QListWidgetItem *it)
{
	slotOkClicked();
}

void C3NewDialog::slotCurrentItemChanged(QListWidgetItem * current,QListWidgetItem * previous)
{
	enableDisableButtons();
}

void C3NewDialog::slotOkClicked()
{
	QListWidgetItem * it = _p->pListWidget->currentItem();
	if(!it)
		return;

	QString szId = it->data(Qt::UserRole).toString();
	
	C3NewObjectCreator * pCreator = _p->hCreators.value(szId);
	if(!pCreator)
		return; // ?

	accept();

	pCreator->run();
}

void C3NewDialog::slotCancelClicked()
{
	reject();
}

void C3NewDialog::enableDisableButtons()
{
	QListWidgetItem * it = _p->pListWidget->currentItem();
	_p->pOkButton->setEnabled(it);
}
