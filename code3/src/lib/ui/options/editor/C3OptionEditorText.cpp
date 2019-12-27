//=============================================================================
//
//   File : C3OptionEditorText.cpp
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

#include "C3OptionEditorText.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>

class C3OptionEditorTextPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QTextEdit * pEdit;
	QString * pOption;
};

#define _p m_pC3OES

C3OptionEditorText::C3OptionEditorText(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorTextPrivate();
	_p->pOption = NULL;
}

C3OptionEditorText::~C3OptionEditorText()
{
	delete _p;
}

QTextEdit * C3OptionEditorText::textEdit()
{
	return _p->pEdit;
}

void C3OptionEditorText::create(const QString &szLabel,QString * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(2);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,4);
	_p->pLabel->setWordWrap(true);
	_p->pEdit = pWidget->addTextEdit(iRow+1,0,1,4);
	_p->pEdit->setLineWrapMode(QTextEdit::NoWrap);
	if(uOptions & ReadOnly)
		_p->pEdit->setReadOnly(true);
	
	pWidget->gridLayout()->setRowStretch(iRow+1,100);
	
	_p->pEdit->setText(*pOption);

	QObject::connect(_p->pEdit,SIGNAL(textChanged()),this,SLOT(slotTextChanged()));
}

QString C3OptionEditorText::currentValue()
{
	if(_p->uOptions & Trim)
		return _p->pEdit->toPlainText().trimmed();
	return _p->pEdit->toPlainText();
}

void C3OptionEditorText::setCurrentValue(const QString &szValue)
{
	_p->pEdit->setText(szValue);
}

bool C3OptionEditorText::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentValue();
	return true;
}

void C3OptionEditorText::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pEdit->setEnabled(bEnabled);
}

void C3OptionEditorText::slotTextChanged()
{
	emit edited();
}