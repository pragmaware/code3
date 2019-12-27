//=============================================================================
//
//   File : C3OptionEditorEnumerationAsInt.cpp
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

#include "C3OptionEditorEnumerationAsInt.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QComboBox>

class C3OptionEditorEnumerationAsIntPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QComboBox * pComboBox;
	int * pOption;
};

#define _p m_pC3OES

C3OptionEditorEnumerationAsInt::C3OptionEditorEnumerationAsInt(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorEnumerationAsIntPrivate();
	_p->pOption = NULL;
}

C3OptionEditorEnumerationAsInt::~C3OptionEditorEnumerationAsInt()
{
	delete _p;
}

void C3OptionEditorEnumerationAsInt::create(
		const QString &szLabel,
		int * pOption,
		const QList< QPair<int,QString> > &lOptions,
		unsigned int uOptions
	)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,2);
	_p->pComboBox = pWidget->addComboBox(iRow,2,1,2);

	typedef QPair<int,QString> QPIQ;

	foreach(QPIQ p,lOptions)
		_p->pComboBox->addItem(p.second,QVariant(p.first));

	setCurrentIntValue(*pOption);

	QObject::connect(_p->pComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotCurrentIndexChanged(int)));
}

void C3OptionEditorEnumerationAsInt::updateOptions(const QList< QPair<int,QString> > &lOptions)
{
	_p->pComboBox->clear();

	typedef QPair<int,QString> QPIQ;

	foreach(QPIQ p,lOptions)
		_p->pComboBox->addItem(p.second,QVariant(p.first));

	setCurrentIntValue(*(_p->pOption));
}

int C3OptionEditorEnumerationAsInt::valueToIndex(int iValue)
{
	return _p->pComboBox->findData(QVariant(iValue));
}

int C3OptionEditorEnumerationAsInt::indexToValue(int iIndex)
{
	bool ok;
	int iVal = _p->pComboBox->itemData(iIndex).toInt(&ok);
	if(!ok)
		return -1;
	return iVal;
}

int C3OptionEditorEnumerationAsInt::currentIntValue()
{
	return indexToValue(_p->pComboBox->currentIndex());
}

void C3OptionEditorEnumerationAsInt::setCurrentIntValue(int iValue)
{
	int idx = valueToIndex(iValue);
	if(idx < 0)
		return;
	_p->pComboBox->setCurrentIndex(idx);
}

bool C3OptionEditorEnumerationAsInt::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentIntValue();
	return true;
}

void C3OptionEditorEnumerationAsInt::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pComboBox->setEnabled(bEnabled);
}

void C3OptionEditorEnumerationAsInt::slotCurrentIndexChanged(int)
{
	emit edited();
}

