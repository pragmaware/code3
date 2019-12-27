//=============================================================================
//
//   File : C3OptionEditorColor.cpp
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

#include "C3OptionEditorColor.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QColorDialog>

class C3OptionEditorColorPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QPushButton * pButton;
	QColor * pOption;
	QColor oColor;
};

#define _p m_pC3OES

C3OptionEditorColor::C3OptionEditorColor(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorColorPrivate();
	_p->pOption = NULL;
}

C3OptionEditorColor::~C3OptionEditorColor()
{
	delete _p;
}

void C3OptionEditorColor::create(const QString &szLabel,QColor * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;

	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,2);
	_p->pButton = pWidget->addPushButton(pOption->name(QColor::HexRgb),iRow,2,1,2);
	
	setCurrentValue(*pOption);

	QObject::connect(_p->pButton,SIGNAL(clicked()),this,SLOT(slotButtonClicked()));
}

QColor C3OptionEditorColor::currentValue()
{
	return _p->oColor;
}

void C3OptionEditorColor::setCurrentValue(const QColor &oValue)
{
	_p->oColor = oValue;
	_p->pButton->setText(oValue.name(QColor::HexRgb));
	
	QColor oInverse(255 - oValue.red(),255 - oValue.green(),255 - oValue.blue());
	
	_p->pButton->setStyleSheet(
			__tr("background-color: %1; color: %2")
				.arg(oValue.name(QColor::HexRgb))
				.arg(oInverse.name(QColor::HexRgb))
		);
}

bool C3OptionEditorColor::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentValue();
	return true;
}

void C3OptionEditorColor::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pButton->setEnabled(bEnabled);
}

void C3OptionEditorColor::slotButtonClicked()
{
	QColor oSelected = QColorDialog::getColor(_p->oColor,optionsWidget(),__tr("Select Color"));
	if(!oSelected.isValid())
		return;

	setCurrentValue(oSelected);

	emit edited();
}

void C3OptionEditorColor::resetValue()
{
	setCurrentValue(*(_p->pOption));
}
