#ifndef _C3OptionsWidgetDescriptor_h_
#define _C3OptionsWidgetDescriptor_h_
//=============================================================================
//
//   File : C3OptionsWidgetDescriptor.h
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

#include "c3_config.h"

#include <QString>
#include <QList>
#include <QWidget>

#include <functional>

class QTreeWidgetItem;
class C3OptionsWidget;
class C3OptionsWidgetContainer;

class C3OptionsWidgetDescriptor
{
private:
	C3OptionsWidgetContainer * m_pContainer;
	QTreeWidgetItem * m_pItem;
	C3OptionsWidget * m_pWidget;
	QString m_szLabel;
	QString m_szId;
	std::function<C3OptionsWidget * (const QString &szLabel,QWidget * pParent)> m_fnCreate;
	QList<C3OptionsWidgetDescriptor *> m_lChildDescriptors;
	C3OptionsWidgetDescriptor * m_pParent;
public:
	C3OptionsWidgetDescriptor(C3OptionsWidgetContainer * pContainer,const QString &szLabel,std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate);
	C3OptionsWidgetDescriptor(C3OptionsWidgetContainer * pContainer,C3OptionsWidget * pExisting);
	~C3OptionsWidgetDescriptor();
public:
	const QString & id() const
	{
		return m_szId;
	}
	
	void setId(const QString &szId)
	{
		m_szId = szId;
	}

	C3OptionsWidgetDescriptor * findChildDescriptor(const QString &szId);

	QList<C3OptionsWidgetDescriptor *> & childDescriptors()
	{
		return m_lChildDescriptors;
	}

	void setParent(C3OptionsWidgetDescriptor * pParent)
	{
		m_pParent = pParent;
	}

	C3OptionsWidgetDescriptor * parent()
	{
		return m_pParent;
	}

	void clear();
	void gatherWidgets(QList<C3OptionsWidget *> &lWidgets);
	QTreeWidgetItem * item();
	void setItem(QTreeWidgetItem * pItem);
	C3OptionsWidget * widget(bool bCreateIfNotExisting = true);
	const QString & label();
	C3OptionsWidgetDescriptor * commit();
	void removeChildDescriptor(C3OptionsWidgetDescriptor * pDescriptor);
	void addChildDescriptor(C3OptionsWidgetDescriptor * pDescriptor);
}; // class C3OptionsWidgetDescriptor

#endif //!_C3OptionsWidgetDescriptor_h_