//=============================================================================
//
//   File : C3OptionsDialog.cpp
//   Creation Date : ven 06 nov 2015 03:31:13
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

#include "C3OptionsDialog.h"

#include <QTreeWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QList>

#include "C3OptionsWidgetContainer.h"
#include "C3OptionsWidget.h"

class C3OptionsDialogPrivate
{
public:
	QGridLayout * pLayout;

	C3OptionsWidgetContainer * pContainer;
	
	QPushButton * pCancelButton;
	QPushButton * pApplyButton;
	QPushButton * pOKButton;
};

#define _p m_pC3OD

C3OptionsDialog::C3OptionsDialog(
		const QString &szTitle,
		unsigned int uWidgetContainerFlags,
		bool bSupportApply
	)
	: QDialog()
{
	setWindowTitle(szTitle);

	_p = new C3OptionsDialogPrivate();

	_p->pLayout = new QGridLayout(this);

	_p->pContainer = new C3OptionsWidgetContainer(this,uWidgetContainerFlags);
	_p->pLayout->addWidget(_p->pContainer,0,0,1,bSupportApply ? 4 : 3);

	_p->pCancelButton = new QPushButton(this);
	_p->pCancelButton->setText(__tr("Cancel"));
	QObject::connect(_p->pCancelButton,SIGNAL(clicked()),this,SLOT(slotCancelClicked()));
	_p->pLayout->addWidget(_p->pCancelButton,1,1);

	int iCol = 2;

	if(bSupportApply)
	{
		_p->pApplyButton = new QPushButton(this);
		_p->pApplyButton->setText(__tr("Apply"));
		QObject::connect(_p->pApplyButton,SIGNAL(clicked()),this,SLOT(slotApplyClicked()));
		_p->pLayout->addWidget(_p->pApplyButton,1,iCol);
		iCol++;
	}

	_p->pOKButton = new QPushButton(this);
	_p->pOKButton->setText(__tr("OK"));
	_p->pOKButton->setDefault(true);
	QObject::connect(_p->pOKButton,SIGNAL(clicked()),this,SLOT(slotOKClicked()));
	_p->pLayout->addWidget(_p->pOKButton,1,iCol);

	_p->pLayout->setMargin(10);
	_p->pLayout->setSpacing(6);

	_p->pLayout->setColumnStretch(0,1000);
	_p->pLayout->setRowStretch(0,1000);
	for(int i=1;i<=iCol;i++)
		_p->pLayout->setColumnMinimumWidth(i,80);
}

C3OptionsDialog::~C3OptionsDialog()
{
	delete _p;
}

QPushButton * C3OptionsDialog::OKButton()
{
	return _p->pOKButton;
}

bool C3OptionsDialog::commit()
{
	return _p->pContainer->commit();
}

void C3OptionsDialog::slotOKClicked()
{
	if(!commit())
		return;
	accept();
}

void C3OptionsDialog::slotApplyClicked()
{
	commit();
}

void C3OptionsDialog::slotCancelClicked()
{
	reject();
}

C3OptionsWidgetDescriptor * C3OptionsDialog::addWidget(const QString &szLabel,std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate)
{
	return _p->pContainer->addWidget(szLabel,fnCreate);
}

C3OptionsWidgetDescriptor * C3OptionsDialog::addWidget(C3OptionsWidgetDescriptor * pParent,const QString &szLabel,std::function<C3OptionsWidget * (const QString &szLabel,QWidget *pParent)> fnCreate)
{
	return _p->pContainer->addWidget(pParent,szLabel,fnCreate);
}

C3OptionsWidgetDescriptor * C3OptionsDialog::addWidget(C3OptionsWidget * pExisting)
{
	return _p->pContainer->addWidget(pExisting);
}

C3OptionsWidgetDescriptor * C3OptionsDialog::addWidget(C3OptionsWidgetDescriptor * pParent,C3OptionsWidget * pExisting)
{
	return _p->pContainer->addWidget(pParent,pExisting);
}

C3OptionsWidgetDescriptor * C3OptionsDialog::findDescriptor(const QString &szId)
{
	return _p->pContainer->findDescriptor(szId);
}


void C3OptionsDialog::activateWidget(C3OptionsWidget * pWidget)
{
	_p->pContainer->activateWidget(pWidget);
}

QWidget * C3OptionsDialog::parentForWidgets()
{
	return _p->pContainer->parentForWidgets();
}
