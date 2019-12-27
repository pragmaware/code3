//=============================================================================
//
//   File : C3OptionsWidgetContainer.cpp
//   Creation Date : 2015/11/18 01:23:04
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

#include "C3OptionsWidgetContainer.h"

#include <QTreeWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QList>
#include <QPushButton>

#include "C3OptionsWidget.h"
#include "C3OptionsWidgetDescriptor.h"

class C3OptionsWidgetContainerPrivate
{
public:
	QTreeWidget * pTreeWidget;
	
	QStackedWidget * pStackedWidget;
	
	QGridLayout * pLayout;

	QList<C3OptionsWidgetDescriptor *> lDescriptors;
	
	bool bTreeFilled;
	
	unsigned int uFlags;
	
	QPushButton * pAddButton;
	QPushButton * pRemoveButton;
	QPushButton * pSetDefaultsButton;
};

#define _p m_pC3OWC

C3OptionsWidgetContainer::C3OptionsWidgetContainer(QWidget * pParent,unsigned int uFlags)
	: QWidget(pParent)
{
	_p = new C3OptionsWidgetContainerPrivate();

	_p->uFlags = uFlags;
	_p->bTreeFilled = false;

	_p->pLayout = new QGridLayout(this);

	_p->pTreeWidget = new QTreeWidget(this);
	_p->pTreeWidget->setHeaderHidden(true);
	_p->pTreeWidget->setRootIsDecorated(true);
	_p->pTreeWidget->setSortingEnabled(false);
	_p->pTreeWidget->sortByColumn(0,Qt::AscendingOrder);
	_p->pTreeWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
	_p->pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(_p->pTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(slotItemClicked(QTreeWidgetItem *,int)));

	if(_p->uFlags & HideLeftTreeWidget)
		_p->pTreeWidget->hide();
	else
		_p->pLayout->addWidget(_p->pTreeWidget,0,0);

	int iRowCount = 1;

	if(uFlags & SupportAddAndRemove)
	{
		_p->pAddButton = new QPushButton(this);
		_p->pAddButton->setText(__tr("Add"));
		QObject::connect(_p->pAddButton,SIGNAL(clicked()),this,SLOT(slotAddButtonClicked()));

		_p->pLayout->addWidget(_p->pAddButton,iRowCount,0);
		iRowCount++;

		_p->pRemoveButton = new QPushButton(this);
		_p->pRemoveButton->setText(__tr("Remove"));
		QObject::connect(_p->pRemoveButton,SIGNAL(clicked()),this,SLOT(slotRemoveButtonClicked()));

		_p->pLayout->addWidget(_p->pRemoveButton,iRowCount,0);
		iRowCount++;
	} else {
		_p->pAddButton = NULL;
		_p->pRemoveButton = NULL;
	}

	if(uFlags & SupportSetDefaults)
	{
		_p->pSetDefaultsButton = new QPushButton(this);
		_p->pSetDefaultsButton->setText(__tr("Reset to Defaults"));
		QObject::connect(_p->pSetDefaultsButton,SIGNAL(clicked()),this,SLOT(slotSetDefaultsButtonClicked()));

		_p->pLayout->addWidget(_p->pSetDefaultsButton,iRowCount,0);
		iRowCount++;
	} else {
		_p->pSetDefaultsButton = NULL;
	}

	_p->pStackedWidget = new QStackedWidget(this);
	QObject::connect(_p->pStackedWidget,SIGNAL(currentChanged(int)),this,SLOT(slotStackedWidgetCurrentChanged(int)));

	_p->pLayout->addWidget(_p->pStackedWidget,0,1,iRowCount,1);

	_p->pLayout->setMargin(0);
	_p->pLayout->setSpacing(4);

	if(!(_p->uFlags & HideLeftTreeWidget))
	{
		_p->pLayout->setColumnMinimumWidth(0,140);
		_p->pLayout->setColumnStretch(0,100);
		_p->pLayout->setColumnStretch(1,400);
	}
	_p->pLayout->setRowStretch(0,300);
}

C3OptionsWidgetContainer::~C3OptionsWidgetContainer()
{
	qDeleteAll(_p->lDescriptors);
	delete _p;
}

bool C3OptionsWidgetContainer::commit()
{
	foreach(C3OptionsWidgetDescriptor * d,_p->lDescriptors)
	{
		C3OptionsWidgetDescriptor * w = d->commit();
		if(w)
		{
			activateWidget(w);
			return false;
		}
	}
	return true;
}

void C3OptionsWidgetContainer::gatherWidgets(QList<C3OptionsWidget *> &lWidgets)
{
	foreach(C3OptionsWidgetDescriptor * d,_p->lDescriptors)
		d->gatherWidgets(lWidgets);
}

C3OptionsWidgetDescriptor * C3OptionsWidgetContainer::findDescriptor(const QString &szId)
{
	foreach(C3OptionsWidgetDescriptor * d,_p->lDescriptors)
	{
		if(d->id() == szId)
			return d;
		C3OptionsWidgetDescriptor * d2 = d->findChildDescriptor(szId);
		if(d2)
			return d2;
	}
	return NULL;
}


void C3OptionsWidgetContainer::fillTree()
{
	_p->pTreeWidget->clear();

	foreach(C3OptionsWidgetDescriptor * d,_p->lDescriptors)
	{
		QTreeWidgetItem * it = createItemForDescriptor(NULL,d);
		Q_ASSERT(it);
		fillTree(it,d);
	}

	_p->bTreeFilled = true;
	enableDisableButtons();
}

void C3OptionsWidgetContainer::fillTree(QTreeWidgetItem * pParent,C3OptionsWidgetDescriptor * pParentDescriptor)
{
	QList<C3OptionsWidgetDescriptor *> &lDescriptors = pParentDescriptor->childDescriptors();
	foreach(C3OptionsWidgetDescriptor * d,lDescriptors)
	{
		QTreeWidgetItem * it = createItemForDescriptor(pParent,d);
		Q_ASSERT(it);
		fillTree(it,d);
	}
}

void C3OptionsWidgetContainer::enableDisableButtons()
{
	QWidget * w = _p->pStackedWidget->currentWidget();
	if(_p->pRemoveButton)
		_p->pRemoveButton->setEnabled(w);
}

void C3OptionsWidgetContainer::showEvent(QShowEvent * e)
{
	if(!_p->bTreeFilled)
		fillTree();
	QWidget::showEvent(e);
}

QWidget * C3OptionsWidgetContainer::parentForWidgets()
{
	return _p->pStackedWidget;
}

C3OptionsWidgetDescriptor * C3OptionsWidgetContainer::addWidget(
		C3OptionsWidgetDescriptor * pParent,
		const QString &szLabel,
		std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate
	)
{
	C3OptionsWidgetDescriptor * pDescriptor = new C3OptionsWidgetDescriptor(
			this,
			szLabel,
			fnCreate
		);

	addDescriptor(pParent,pDescriptor);

	return pDescriptor;
}

C3OptionsWidgetDescriptor * C3OptionsWidgetContainer::addWidget(
		C3OptionsWidgetDescriptor * pParent,
		C3OptionsWidget * pExisting
	)
{
	C3OptionsWidgetDescriptor * pDescriptor = new C3OptionsWidgetDescriptor(
			this,
			pExisting
		);
		
	addDescriptor(pParent,pDescriptor);

	pExisting->setOptionsWidgetDescriptor(pDescriptor);

	widgetInstanceCreated(pDescriptor,pExisting);

	return pDescriptor;
}

void C3OptionsWidgetContainer::addDescriptor(C3OptionsWidgetDescriptor * pParent,C3OptionsWidgetDescriptor * pDescriptor)
{
	if(pParent)
		pParent->addChildDescriptor(pDescriptor);
	else
		_p->lDescriptors.append(pDescriptor);

	if(_p->bTreeFilled)
		fillTree();

	enableDisableButtons();
}


void C3OptionsWidgetContainer::removeWidget(C3OptionsWidgetDescriptor * pDescriptor)
{
	if(!pDescriptor)
		return;

	C3OptionsWidget * w = pDescriptor->widget(false);
	if(!w)
		return;

	_p->pStackedWidget->removeWidget(w);
	QTreeWidgetItem * it = itemForWidget(w);
	if(it)
		delete it;
	//delete w; <-- will be killed by descriptor
	
	C3OptionsWidgetDescriptor * pParent = pDescriptor->parent();
	if(pParent)
		pParent->removeChildDescriptor(pDescriptor);
	else {
		_p->lDescriptors.removeAll(pDescriptor);
		delete pDescriptor;
	}

	enableDisableButtons();
}

void C3OptionsWidgetContainer::removeWidget(C3OptionsWidget * pWidget)
{
	C3OptionsWidgetDescriptor * pDescriptor = descriptorForWidget(pWidget);
	if(!pDescriptor)
		return;
	removeWidget(pDescriptor);
}

void C3OptionsWidgetContainer::removeAllWidgets()
{
	foreach(C3OptionsWidgetDescriptor * d,_p->lDescriptors)
		d->clear();
	qDeleteAll(_p->lDescriptors);
	_p->lDescriptors.clear();
	fillTree();
	enableDisableButtons();
}

C3OptionsWidget * C3OptionsWidgetContainer::currentWidget()
{
	QWidget * w = _p->pStackedWidget->currentWidget();
	if(!w)
		return NULL;
	return dynamic_cast<C3OptionsWidget *>(w);
}

void C3OptionsWidgetContainer::widgetInstanceCreated(C3OptionsWidgetDescriptor * pDescriptor,C3OptionsWidget * pWidget)
{
	_p->pStackedWidget->addWidget(pWidget);
	QObject::connect(pWidget,SIGNAL(titleChanged(C3OptionsWidget *)),this,SLOT(slotOptionsWidgetTitleChanged(C3OptionsWidget *)));
}

QTreeWidgetItem * C3OptionsWidgetContainer::createItemForDescriptor(QTreeWidgetItem * pParent,C3OptionsWidgetDescriptor * pDescriptor)
{
	QTreeWidgetItem * it = pParent ? new QTreeWidgetItem(pParent) : new QTreeWidgetItem(_p->pTreeWidget);
	it->setData(0,Qt::UserRole,QVariant((qulonglong)pDescriptor));
	updateItem(it,pDescriptor);
	if(pParent)
		pParent->addChild(it);
	else
		_p->pTreeWidget->addTopLevelItem(it);
	pDescriptor->setItem(it);
	return it;
}


C3OptionsWidget * C3OptionsWidgetContainer::widgetForItem(QTreeWidgetItem * it)
{
	if(!it)
		return NULL;
	C3OptionsWidgetDescriptor * pDescriptor = descriptorForItem(it);
	if(!pDescriptor)
		return NULL;
	return pDescriptor->widget(true);
}

C3OptionsWidgetDescriptor * C3OptionsWidgetContainer::descriptorForWidget(C3OptionsWidget * pWidget)
{
	if(!pWidget)
		return NULL;
	return pWidget->optionsWidgetDescriptor();
}

C3OptionsWidgetDescriptor * C3OptionsWidgetContainer::descriptorForItem(QTreeWidgetItem * pItem)
{
	if(!pItem)
		return NULL;
	bool ok;
	qulonglong l = pItem->data(0,Qt::UserRole).toULongLong(&ok);
	Q_ASSERT(ok);
	return (C3OptionsWidgetDescriptor *)l;
}

QTreeWidgetItem * C3OptionsWidgetContainer::itemForWidget(C3OptionsWidget * pWidget)
{
	if(!pWidget)
		return NULL;
	C3OptionsWidgetDescriptor * d = pWidget->optionsWidgetDescriptor();
	if(!d)
		return NULL;
	return d->item();
}

void C3OptionsWidgetContainer::updateItemForDescriptor(C3OptionsWidgetDescriptor * pDescriptor)
{
	if(!pDescriptor)
		return;
	QTreeWidgetItem * it = pDescriptor->item();
	if(!it)
		return;
	updateItem(it,pDescriptor);
}

void C3OptionsWidgetContainer::updateItem(QTreeWidgetItem * it,C3OptionsWidgetDescriptor * pDescriptor)
{
	C3OptionsWidget * pWidget = pDescriptor->widget(false);

	if(pWidget)
		it->setText(0,pWidget->title());
	else
		it->setText(0,pDescriptor->label());
}

void C3OptionsWidgetContainer::slotItemClicked(QTreeWidgetItem *it,int column)
{
	C3OptionsWidgetDescriptor * pDescriptor = descriptorForItem(it);
	Q_ASSERT(it);
	Q_ASSERT(pDescriptor);
	if(!pDescriptor)
		return;
	activateWidget(pDescriptor);
}

void C3OptionsWidgetContainer::activateWidget(C3OptionsWidget * pWidget)
{
	C3OptionsWidgetDescriptor * pDescriptor = descriptorForWidget(pWidget);
	if(!pDescriptor)
		return;
	activateWidget(pDescriptor);
}

void C3OptionsWidgetContainer::activateWidget(C3OptionsWidgetDescriptor * pDescriptor)
{
	Q_ASSERT(pDescriptor);

	C3OptionsWidget * w = pDescriptor->widget(true);
	Q_ASSERT(w);

	_p->pStackedWidget->setCurrentWidget(w);
	_p->pTreeWidget->setCurrentItem(pDescriptor->item());

	enableDisableButtons();
}

void C3OptionsWidgetContainer::slotStackedWidgetCurrentChanged(int idx)
{
	QWidget * w = _p->pStackedWidget->widget(idx);
	if(!w)
		return;
	C3OptionsWidget * ow = dynamic_cast<C3OptionsWidget *>(w);
	Q_ASSERT(ow);
	if(!ow)
		return;

	QTreeWidgetItem * it = itemForWidget(ow);
	if(!it) // may be, at startup
		return;

	_p->pTreeWidget->setCurrentItem(it);
	
	enableDisableButtons();
}

void C3OptionsWidgetContainer::slotOptionsWidgetTitleChanged(C3OptionsWidget * pWidget)
{
	updateItemForDescriptor(pWidget->optionsWidgetDescriptor());
}

void C3OptionsWidgetContainer::slotAddButtonClicked()
{
	emit addButtonClicked();
}

void C3OptionsWidgetContainer::slotRemoveButtonClicked()
{
	emit removeButtonClicked();
}

void C3OptionsWidgetContainer::slotSetDefaultsButtonClicked()
{
	emit setDefaultsButtonClicked();
}
