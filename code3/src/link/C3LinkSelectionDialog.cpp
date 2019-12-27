//=============================================================================
//
//   File : C3LinkSelectionDialog.cpp
//   Creation Date : 2015/11/25 18:17:24
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

#include "C3LinkSelectionDialog.h"
#include "C3RichTextItemDelegate.h"
#include "C3Workspace.h"

#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>

class C3LinkSelectionDialogPrivate
{
public:
	QGridLayout * pLayout;
	QPushButton * pOKButton;
	QPushButton * pCancelButton;
	QListWidget * pListWidget;

	QList<C3Link> lLinks;
	
	C3Link oSelectedLink;
};

#define _p m_pC3LSD

C3LinkSelectionDialog::C3LinkSelectionDialog(
		QWidget * pParent,
		const QString &szTitle,
		const QList<C3Link> &lLinks
	)
	: QDialog(pParent)
{
	setWindowTitle(szTitle);

	_p = new C3LinkSelectionDialogPrivate();

	_p->lLinks = lLinks;
	
	_p->pLayout = new QGridLayout(this);

	_p->pListWidget = new QListWidget(this);
	_p->pListWidget->setItemDelegate(new C3RichTextItemDelegate(_p->pListWidget));
	QObject::connect(_p->pListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(slotItemDoubleClicked(QListWidgetItem *)));
	QObject::connect(_p->pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *)));
	_p->pLayout->addWidget(_p->pListWidget,0,0,1,3);

	_p->pCancelButton = new QPushButton(this);
	_p->pCancelButton->setText(__tr("Cancel"));
	QObject::connect(_p->pCancelButton,SIGNAL(clicked()),this,SLOT(slotCancelClicked()));
	_p->pLayout->addWidget(_p->pCancelButton,1,1);

	_p->pOKButton = new QPushButton(this);
	_p->pOKButton->setText(__tr("OK"));
	QObject::connect(_p->pOKButton,SIGNAL(clicked()),this,SLOT(slotOKClicked()));
	_p->pLayout->addWidget(_p->pOKButton,1,2);

	_p->pLayout->setMargin(10);
	_p->pLayout->setSpacing(6);

	_p->pLayout->setColumnStretch(0,1000);
	_p->pLayout->setRowStretch(0,1000);
	_p->pLayout->setColumnMinimumWidth(1,80);
	_p->pLayout->setColumnMinimumWidth(2,80);
	
	setMinimumSize(QSize(670,450));
	
	displayLinks();
}

C3LinkSelectionDialog::~C3LinkSelectionDialog()
{
	delete _p;
}

const C3Link & C3LinkSelectionDialog::selectedLink()
{
	return _p->oSelectedLink;
}

void C3LinkSelectionDialog::displayLinks()
{
	int idx = 0;

	C3Workspace * w = C3Workspace::currentWorkspace();

	foreach(const C3Link &oLink,_p->lLinks)
	{
		QListWidgetItem * it = new QListWidgetItem(_p->pListWidget);
		it->setText(oLink.toHtml());
		it->setData(Qt::UserRole,idx);
		
		_p->pListWidget->addItem(it);
		
		idx++;
	}
	
	enableDisableButtons();
}

void C3LinkSelectionDialog::enableDisableButtons()
{
	_p->pCancelButton->setEnabled(_p->pListWidget->currentItem());
}

void C3LinkSelectionDialog::slotOKClicked()
{
	QListWidgetItem * it = _p->pListWidget->currentItem();
	if(!it)
		return;
	bool ok;
	int idx = it->data(Qt::UserRole).toInt(&ok);
	if(!ok)
		return;
	if(idx < 0)
		return;
	if(idx >= _p->lLinks.count())
		return;
	
	_p->oSelectedLink = _p->lLinks.at(idx);

	accept();
}

void C3LinkSelectionDialog::slotCancelClicked()
{
	reject();
}

void C3LinkSelectionDialog::slotItemDoubleClicked(QListWidgetItem * it)
{
	if(!it)
		return;

	bool ok;
	int idx = it->data(Qt::UserRole).toInt(&ok);
	if(!ok)
		return;
	if(idx < 0)
		return;
	if(idx >= _p->lLinks.count())
		return;
	
	_p->oSelectedLink = _p->lLinks.at(idx);

	accept();
}

void C3LinkSelectionDialog::slotCurrentItemChanged(QListWidgetItem * cur,QListWidgetItem * prev)
{
	enableDisableButtons();
}
