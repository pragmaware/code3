//=============================================================================
//
//   File : C3OptionEditorInteger.cpp
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

#include "C3OptionEditorInteger.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QLineEdit>

class C3OptionEditorIntegerPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QLineEdit * pEdit;
	int * pOption;
};

#define _p m_pC3OES

C3OptionEditorInteger::C3OptionEditorInteger(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorIntegerPrivate();
	_p->pOption = NULL;
}

C3OptionEditorInteger::~C3OptionEditorInteger()
{
	delete _p;
}

QLineEdit * C3OptionEditorInteger::lineEdit()
{
	return _p->pEdit;
}

void C3OptionEditorInteger::create(const QString &szLabel,int * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,2);
	_p->pEdit = pWidget->addLineEdit(iRow,2,1,2);

	_p->pEdit->setText(__ascii("%1").arg(*pOption));

	QObject::connect(_p->pEdit,SIGNAL(textEdited(const QString &)),this,SLOT(slotTextEdited(const QString &)));
}

int C3OptionEditorInteger::currentValue(bool * bOk,int iDefault)
{
	QString szVal = _p->pEdit->text().trimmed();
	if(szVal.isEmpty())
	{
		if(bOk)
			*bOk = false;
		return iDefault;
	}
	bool ok;
	int iVal = szVal.toInt(&ok);
	if(!ok)
	{
		if(bOk)
			*bOk = false;
	}
	if(bOk)
		*bOk = true;
	return iVal;
}

void C3OptionEditorInteger::setCurrentValue(int iValue)
{
	_p->pEdit->setText(__ascii("%1").arg(iValue));
}

bool C3OptionEditorInteger::commit()
{
	if(!_p->pOption)
		return true;
	bool ok;
	int iVal = currentValue(&ok);
	if(!ok)
		return false;
	*(_p->pOption) = iVal;
	return true;
}

void C3OptionEditorInteger::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pEdit->setEnabled(bEnabled);
}

void C3OptionEditorInteger::slotTextEdited(const QString &)
{
	emit edited();
}
