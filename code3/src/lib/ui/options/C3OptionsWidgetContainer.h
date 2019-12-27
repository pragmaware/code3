#ifndef _C3OptionsWidgetContainer_h_
#define _C3OptionsWidgetContainer_h_
//=============================================================================
//
//   File : C3OptionsWidgetContainer.h
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

#include "c3_config.h"

#include <QWidget>

#include <functional>


class C3OptionsWidget;
class QTreeWidgetItem;

class C3OptionsWidgetDescriptor;
class C3OptionsWidgetContainerPrivate;

class C3OptionsWidgetContainer : public QWidget
{
	friend class C3OptionsWidgetDescriptor;
	Q_OBJECT
public:
	enum Flags
	{
		SupportAddAndRemove = 1,
		SupportSetDefaults = 2,
		HideLeftTreeWidget = 4
	};
public:
	C3OptionsWidgetContainer(QWidget * pParent,unsigned int uFlags = 0);
	virtual ~C3OptionsWidgetContainer();

private:
	C3OptionsWidgetContainerPrivate * m_pC3OWC;

public:

	QWidget * parentForWidgets();

	C3OptionsWidgetDescriptor * addWidget(
			C3OptionsWidgetDescriptor * pParent,
			const QString &szLabel,
			std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate
		);

	C3OptionsWidgetDescriptor * addWidget(
			const QString &szLabel,
			std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate
		)
	{
		return addWidget(NULL,szLabel,fnCreate);
	}

	C3OptionsWidgetDescriptor * addWidget(
			C3OptionsWidgetDescriptor * pParent,
			C3OptionsWidget * pExisting
		);

	C3OptionsWidgetDescriptor * addWidget(
			C3OptionsWidget * pExisting
		)
	{
		return addWidget(NULL,pExisting);
	}

	C3OptionsWidgetDescriptor * findDescriptor(const QString &szId);

	void removeWidget(C3OptionsWidgetDescriptor * pDescriptor);
	void removeWidget(C3OptionsWidget * pWidget);
	void removeAllWidgets();

	void activateWidget(C3OptionsWidget * pWidget);
	void activateWidget(C3OptionsWidgetDescriptor * pDescriptor);

	C3OptionsWidget * currentWidget();

	virtual bool commit();

	void gatherWidgets(QList<C3OptionsWidget *> &lWidgets);

protected:

	void fillTree();
	void fillTree(QTreeWidgetItem * pParent,C3OptionsWidgetDescriptor * pParentDescriptor);

	virtual void showEvent(QShowEvent * e);

signals:
	void addButtonClicked();
	void removeButtonClicked();
	void setDefaultsButtonClicked();

private:

	void addDescriptor(C3OptionsWidgetDescriptor * pParent,C3OptionsWidgetDescriptor * pDescriptor);

	QTreeWidgetItem * createItemForDescriptor(QTreeWidgetItem * pParent,C3OptionsWidgetDescriptor * pDescriptor);

	C3OptionsWidget * widgetForItem(QTreeWidgetItem * it);
	QTreeWidgetItem * itemForWidget(C3OptionsWidget * pWidget);
	C3OptionsWidgetDescriptor * descriptorForWidget(C3OptionsWidget * pWidget);
	C3OptionsWidgetDescriptor * descriptorForItem(QTreeWidgetItem * pItem);
	
	void updateItemForDescriptor(C3OptionsWidgetDescriptor * pDescriptor);
	
	void updateItem(QTreeWidgetItem * it,C3OptionsWidgetDescriptor * pDescriptor);

	void enableDisableButtons();

	void widgetInstanceCreated(C3OptionsWidgetDescriptor * pDescriptor,C3OptionsWidget * pWidget);

private slots:

	void slotOptionsWidgetTitleChanged(C3OptionsWidget * pWidget);

	void slotItemClicked(QTreeWidgetItem *it,int column);
	void slotStackedWidgetCurrentChanged(int idx);

	void slotAddButtonClicked();
	void slotRemoveButtonClicked();
	void slotSetDefaultsButtonClicked();

}; // class C3OptionsWidgetContainer

#endif //!_C3OptionsWidgetContainer_h_
