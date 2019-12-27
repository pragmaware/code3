//=============================================================================
//
//   File : C3OptionEditorStringList.cpp
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

#include "C3OptionEditorStringList.h"
#include "C3OptionsWidget.h"

#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>

class C3OptionEditorStringListPrivate
{
public:
	unsigned int uOptions;
	QLabel * pLabel;
	QTextEdit * pEdit;
	QStringList * pOption;
};

#define _p m_pC3OES

C3OptionEditorStringList::C3OptionEditorStringList(C3OptionsWidget * pWidget)
	: C3OptionEditor(pWidget)
{
	_p = new C3OptionEditorStringListPrivate();
	_p->pOption = NULL;
}

C3OptionEditorStringList::~C3OptionEditorStringList()
{
	delete _p;
}

void C3OptionEditorStringList::create(const QString &szLabel,QStringList * pOption,unsigned int uOptions)
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
	
	pWidget->gridLayout()->setRowStretch(iRow+1,100);
	
	QString szText = pOption->join("\n");
	
	_p->pEdit->setText(szText);

	QObject::connect(_p->pEdit,SIGNAL(textChanged()),this,SLOT(slotTextChanged()));
}

QStringList C3OptionEditorStringList::currentValue()
{
	QString szText = _p->pEdit->toPlainText();
	QStringList lVals = szText.split(QChar('\n'));

	if(_p->uOptions & DontTrim)
		return lVals;
	
	QStringList lTrimmed;
	foreach(QString s,lVals)
	{
		QString tr = s.trimmed();
		if(tr.isEmpty())
			continue;
		lTrimmed.append(tr);
	}
	return lTrimmed;
}

void C3OptionEditorStringList::setCurrentValue(const QStringList &lValue)
{
	_p->pEdit->setText(lValue.join("\n"));
}

bool C3OptionEditorStringList::commit()
{
	if(!_p->pOption)
		return true;
	*(_p->pOption) = currentValue();
	return true;
}

void C3OptionEditorStringList::setEnabled(bool bEnabled)
{
	_p->pLabel->setEnabled(bEnabled);
	_p->pEdit->setEnabled(bEnabled);
}

void C3OptionEditorStringList::slotTextChanged()
{
	emit edited();
}
