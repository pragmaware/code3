//=============================================================================
//
//   File : C3OptionEditorFont.cpp
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

#include "C3OptionEditorFont.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QFontDialog>

class C3OptionEditorFontPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QPushButton * pButton;
	QFont * pOption;
	QFont oFont;
};

#define _p m_pC3OES

C3OptionEditorFont::C3OptionEditorFont(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorFontPrivate();
	_p->pOption = NULL;
}

C3OptionEditorFont::~C3OptionEditorFont()
{
	delete _p;
}

void C3OptionEditorFont::create(const QString &szLabel,QFont * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;

	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,2);
	_p->pButton = pWidget->addPushButton(__ascii("%1 %2").arg(pOption->family()).arg(pOption->pointSize()),iRow,2,1,2);

	setCurrentValue(*pOption);

	QObject::connect(_p->pButton,SIGNAL(clicked()),this,SLOT(slotButtonClicked()));
}

QFont C3OptionEditorFont::currentValue()
{
	return _p->oFont;
}

void C3OptionEditorFont::setCurrentValue(const QFont &oValue)
{
	_p->oFont = oValue;
	_p->pButton->setText(__ascii("%1 %2").arg(oValue.family()).arg(oValue.pointSize()));
}

bool C3OptionEditorFont::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentValue();
	return true;
}

void C3OptionEditorFont::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pButton->setEnabled(bEnabled);
}

void C3OptionEditorFont::slotButtonClicked()
{
	bool ok;

	QFont oSelected = QFontDialog::getFont(&ok,_p->oFont,optionsWidget(),__tr("Select Font"));
	
	if(!ok)
		return;
	
	setCurrentValue(oSelected);

	emit edited();
}
