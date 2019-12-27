//=============================================================================
//
//   File : C3TextEditorOptionsWidgetFonts.cpp
//   Creation Date : mer 11 nov 2015 01:44:42
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

#include "C3TextEditorOptionsWidgetFonts.h"

#include "C3Settings.h"
#include "C3OptionEditorScrollArea.h"
#include "C3OptionEditorFont.h"
#include "C3TextEditorOptions.h"

#include <QGridLayout>

class C3TextEditorOptionsWidgetFontsPrivate
{
public:

};

#define _p m_pC3TEOWF

C3TextEditorOptionsWidgetFonts::C3TextEditorOptionsWidgetFonts(const QString &szLabel,QWidget * pParent,C3TextEditorOptions * pOptions)
	: C3OptionsWidget(szLabel,pParent)
{
	_p = new C3TextEditorOptionsWidgetFontsPrivate();

	C3OptionEditorScrollArea * pArea = addScrollArea();

	C3OptionsWidget * w = pArea->childOptionsWidget();
	w->gridLayout()->setMargin(10);

	w->addFontEditor(__tr("Font"),&(pOptions->oTextFont));
	w->addIntegerEditor(__tr("Tab Size"),&(pOptions->iTabStopSize));
	w->addIntegerEditor(__tr("Additional Line Spacing"),&(pOptions->iAdditionalLineSpacing));
	
	w->addSpaceEater();
}

C3TextEditorOptionsWidgetFonts::~C3TextEditorOptionsWidgetFonts()
{
	delete _p;
}

