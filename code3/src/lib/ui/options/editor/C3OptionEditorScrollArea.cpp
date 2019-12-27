//=============================================================================
//
//   File : C3OptionEditorScrollArea.cpp
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

#include "C3OptionEditorScrollArea.h"
#include "C3OptionsWidget.h"

#include <QScrollArea>


class C3OptionEditorScrollAreaPrivate
{
public:
	unsigned int uOptions;

	QScrollArea * pScrollArea;

	C3OptionsWidget * pChildOptionsWidget;
};

#define _p m_pC3OEG

C3OptionEditorScrollArea::C3OptionEditorScrollArea(C3OptionsWidget * pOptionsWidget)
	: C3OptionEditor(pOptionsWidget)
{
	_p = new C3OptionEditorScrollAreaPrivate();
}

C3OptionEditorScrollArea::~C3OptionEditorScrollArea()
{
	delete _p;
}

void C3OptionEditorScrollArea::create(unsigned int uOptions)
{
	_p->uOptions = uOptions;
	C3OptionsWidget * pWidget = optionsWidget();
	int iRow = pWidget->allocateRows(1);

	_p->pScrollArea = pWidget->addScrollArea(iRow,0,1,4);
	
	_p->pChildOptionsWidget = new C3OptionsWidget(QString(),_p->pScrollArea->viewport());
	
	_p->pScrollArea->setWidget(_p->pChildOptionsWidget);
	_p->pScrollArea->setWidgetResizable(true);

}

C3OptionsWidget * C3OptionEditorScrollArea::childOptionsWidget()
{
	return _p->pChildOptionsWidget;
}


bool C3OptionEditorScrollArea::commit()
{
	return _p->pChildOptionsWidget->commit();
}