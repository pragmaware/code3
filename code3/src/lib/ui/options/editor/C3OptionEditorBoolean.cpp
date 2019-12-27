//=============================================================================
//
//   File : C3OptionEditorBoolean.cpp
//   Creation Date : mer 11 nov 2015 04:32:57
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

#include "C3OptionEditorBoolean.h"
#include "C3OptionsWidget.h"

#include <QCheckBox>

class C3OptionEditorBooleanPrivate
{
public:
	unsigned int uOptions;
	QCheckBox * pCheckBox;
	bool * pOption;
};

#define _p m_pC3OEB

C3OptionEditorBoolean::C3OptionEditorBoolean(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorBooleanPrivate();
	_p->pOption = NULL;
}

C3OptionEditorBoolean::~C3OptionEditorBoolean()
{
	delete _p;
}

void C3OptionEditorBoolean::slotCheckBoxStateChanged(int iState)
{
	emit edited();
	emit currentValueChanged(iState != Qt::Unchecked);
}

QCheckBox * C3OptionEditorBoolean::checkBox()
{
	return _p->pCheckBox;
}

void C3OptionEditorBoolean::create(const QString &szLabel,bool * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pCheckBox = pWidget->addCheckBox(szLabel,iRow,2,1,2);

	_p->pCheckBox->setCheckState(*pOption ? Qt::Checked : Qt::Unchecked);

	QObject::connect(_p->pCheckBox,SIGNAL(stateChanged(int)),this,SLOT(slotCheckBoxStateChanged(int)));
}

bool C3OptionEditorBoolean::currentValue()
{
	return _p->pCheckBox->checkState() != Qt::Unchecked;
}

void C3OptionEditorBoolean::setCurrentValue(bool bValue)
{
	_p->pCheckBox->setCheckState(bValue ? Qt::Checked : Qt::Unchecked);
}

bool C3OptionEditorBoolean::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentValue();
	return true;
}

void C3OptionEditorBoolean::setEnabled(bool bEnabled)
{
	_p->pCheckBox->setEnabled(bEnabled);
}

