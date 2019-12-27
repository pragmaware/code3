//=============================================================================
//
//   File : C3OptionEditorGroup.cpp
//   Creation Date : mer 11 nov 2015 04:10:31
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

#include "C3OptionEditorGroup.h"
#include "C3OptionsWidget.h"

#include <QGroupBox>
#include <QGridLayout>


class C3OptionEditorGroupPrivate
{
public:
	unsigned int uOptions;

	QGroupBox * pGroupBox;
	
	QGridLayout * pLayout;

	C3OptionsWidget * pChildOptionsWidget;
};

#define _p m_pC3OEG

C3OptionEditorGroup::C3OptionEditorGroup(C3OptionsWidget * pOptionsWidget)
	: C3OptionEditor(pOptionsWidget)
{
	_p = new C3OptionEditorGroupPrivate();
}

C3OptionEditorGroup::~C3OptionEditorGroup()
{
	delete _p;
}

void C3OptionEditorGroup::setChecked(bool bChecked)
{
	_p->pGroupBox->setChecked(bChecked);
}

bool C3OptionEditorGroup::isChecked()
{
	return _p->pGroupBox->isChecked();
}

void C3OptionEditorGroup::create(const QString &szLabel,unsigned int uOptions)
{
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pGroupBox = pWidget->addGroupBox(szLabel,iRow,0,1,4);
	
	if(uOptions & Checkable)
		_p->pGroupBox->setCheckable(true);

	_p->pGroupBox->setAlignment(Qt::AlignLeft);
	
	_p->pLayout = new QGridLayout(_p->pGroupBox);
	
	_p->pChildOptionsWidget = new C3OptionsWidget(QString(),_p->pGroupBox);
	
	_p->pLayout->addWidget(_p->pChildOptionsWidget,0,0);
}

C3OptionsWidget * C3OptionEditorGroup::childOptionsWidget()
{
	return _p->pChildOptionsWidget;
}


bool C3OptionEditorGroup::commit()
{
	return _p->pChildOptionsWidget->commit();
}

