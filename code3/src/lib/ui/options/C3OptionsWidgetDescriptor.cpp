//=============================================================================
//
//   File : C3OptionsWidgetDescriptor.cpp
//   Creation Date : 2016/04/02 02:22:33
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

#include "C3OptionsWidgetDescriptor.h"

#include "C3OptionsWidgetContainer.h"
#include "C3OptionsWidget.h"


C3OptionsWidgetDescriptor::C3OptionsWidgetDescriptor(C3OptionsWidgetContainer * pContainer,const QString &szLabel,std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate)
{
	m_pContainer = pContainer;
	m_pItem = NULL;
	m_pWidget = NULL;
	m_szLabel = szLabel;
	m_fnCreate = fnCreate;
	m_pParent = NULL;
}

C3OptionsWidgetDescriptor::C3OptionsWidgetDescriptor(C3OptionsWidgetContainer * pContainer,C3OptionsWidget * pExisting)
{
	Q_ASSERT(pExisting);

	m_pContainer = pContainer;
	m_pItem = NULL;
	m_pWidget = pExisting;
	m_szLabel = pExisting->title();
	m_pParent = NULL;
}

C3OptionsWidgetDescriptor::~C3OptionsWidgetDescriptor()
{
	clear();
}

void C3OptionsWidgetDescriptor::clear()
{
	qDeleteAll(m_lChildDescriptors);
	m_lChildDescriptors.clear();
	if(m_pWidget)
	{
		delete m_pWidget;
		m_pWidget = NULL;
	}
}

void C3OptionsWidgetDescriptor::gatherWidgets(QList<C3OptionsWidget *> &lWidgets)
{
	if(m_pWidget)
		lWidgets.append(m_pWidget);
	foreach(C3OptionsWidgetDescriptor * d,m_lChildDescriptors)
		d->gatherWidgets(lWidgets);
}

QTreeWidgetItem * C3OptionsWidgetDescriptor::item()
{
	return m_pItem;
}

void C3OptionsWidgetDescriptor::setItem(QTreeWidgetItem * pItem)
{
	m_pItem = pItem;
}
	
C3OptionsWidget * C3OptionsWidgetDescriptor::widget(bool bCreateIfNotExisting)
{
	if(!m_pWidget)
	{
		if(!bCreateIfNotExisting)
			return NULL;
		m_pWidget = m_fnCreate(m_szLabel,m_pContainer->parentForWidgets());
		m_pWidget->setOptionsWidgetDescriptor(this);
		m_pContainer->widgetInstanceCreated(this,m_pWidget);
		Q_ASSERT(m_pWidget);
	}
	return m_pWidget;
}
	
const QString & C3OptionsWidgetDescriptor::label()
{
	return m_szLabel;
}
	
C3OptionsWidgetDescriptor * C3OptionsWidgetDescriptor::commit()
{
	if(m_pWidget)
	{
		if(!m_pWidget->commit())
			return this;
	}
	
	foreach(C3OptionsWidgetDescriptor * d,m_lChildDescriptors)
	{
		C3OptionsWidgetDescriptor * failed = d->commit();
		if(failed)
			return failed;
	}
		
	return NULL; // everything ok
}

void C3OptionsWidgetDescriptor::addChildDescriptor(C3OptionsWidgetDescriptor * pDescriptor)
{
	pDescriptor->setParent(this);
	m_lChildDescriptors.append(pDescriptor);
}

void C3OptionsWidgetDescriptor::removeChildDescriptor(C3OptionsWidgetDescriptor * pDescriptor)
{
	m_lChildDescriptors.removeAll(pDescriptor);
	delete pDescriptor;
}

C3OptionsWidgetDescriptor * C3OptionsWidgetDescriptor::findChildDescriptor(const QString &szId)
{
	foreach(C3OptionsWidgetDescriptor * d,m_lChildDescriptors)
	{
		if(d->id() == szId)
			return d;
		C3OptionsWidgetDescriptor * d2 = d->findChildDescriptor(szId);
		if(d2)
			return d2;
	}
	return NULL;
}
