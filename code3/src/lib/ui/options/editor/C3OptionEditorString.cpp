//=============================================================================
//
//   File : C3OptionEditorString.cpp
//   Creation Date : mer 11 nov 2015 01:53:07
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

#include "C3OptionEditorString.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class C3OptionEditorStringPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QLineEdit * pEdit;
	QString * pOption;
	QPushButton * pSelectorButton;
};

#define _p m_pC3OES

C3OptionEditorString::C3OptionEditorString(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorStringPrivate();
	_p->pOption = NULL;
}

C3OptionEditorString::~C3OptionEditorString()
{
	delete _p;
}

QLineEdit * C3OptionEditorString::lineEdit()
{
	return _p->pEdit;
}

QPushButton * C3OptionEditorString::selectorButton()
{
	return _p->pSelectorButton;
}

void C3OptionEditorString::create(const QString &szLabel,QString * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,2);
	_p->pEdit = pWidget->addLineEdit(iRow,2,1,(uOptions & AddSelectorButton) ? 1 : 2);
	
	if(uOptions & AddSelectorButton)
		_p->pSelectorButton = pWidget->addPushButton(QString(),iRow,3,1,1);
	
	_p->pEdit->setText(*pOption);

	QObject::connect(_p->pEdit,SIGNAL(textEdited(const QString &)),this,SLOT(slotTextEdited(const QString &)));
}

QString C3OptionEditorString::currentValue()
{
	if(_p->uOptions & DontTrim)
		return _p->pEdit->text();
	return _p->pEdit->text().trimmed();
}

void C3OptionEditorString::setCurrentValue(const QString &szValue)
{
	_p->pEdit->setText(szValue);
}

bool C3OptionEditorString::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentValue();
	return true;
}

void C3OptionEditorString::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pEdit->setEnabled(bEnabled);
}

void C3OptionEditorString::slotTextEdited(const QString &)
{
	emit edited();
}
