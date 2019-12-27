//=============================================================================
//
//   File : C3OptionEditorFile.cpp
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

#include "C3OptionEditorFile.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QLineEdit>

class C3OptionEditorFilePrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QLineEdit * pEdit;
	QString * pOption;
};

#define _p m_pC3OES

C3OptionEditorFile::C3OptionEditorFile(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorFilePrivate();
	_p->pOption = NULL;
}

C3OptionEditorFile::~C3OptionEditorFile()
{
	delete _p;
}

void C3OptionEditorFile::create(const QString &szLabel,QString * pOption,unsigned int uOptions)
{
	Q_ASSERT(!_p->pOption);
	Q_ASSERT(pOption);
	_p->pOption = pOption;
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pLabel = pWidget->addLabel(szLabel,iRow,0,1,2);
	_p->pEdit = pWidget->addLineEdit(iRow,2,1,2);
	
	_p->pEdit->setText(*pOption);

	QObject::connect(_p->pEdit,SIGNAL(textEdited(const QString &)),this,SLOT(slotTextEdited(const QString &)));
}

QString C3OptionEditorFile::currentValue()
{
	if(_p->uOptions & DontTrim)
		return _p->pEdit->text();
	return _p->pEdit->text().trimmed();
}

void C3OptionEditorFile::setCurrentValue(const QString &szValue)
{
	_p->pEdit->setText(szValue);
}

bool C3OptionEditorFile::commit()
{
	if(!_p->pOption)
		return true;
	if(_p->uOptions & DontTrim)
		*(_p->pOption) = _p->pEdit->text();
	else
		*(_p->pOption) = _p->pEdit->text().trimmed();
	return true;
}

void C3OptionEditorFile::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pEdit->setEnabled(bEnabled);
}

void C3OptionEditorFile::slotTextEdited(const QString &)
{
	emit edited();
}

