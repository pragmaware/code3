#ifndef _C3TextEditorOptionsWidgetFonts_h_
#define _C3TextEditorOptionsWidgetFonts_h_
//=============================================================================
//
//   File : C3TextEditorOptionsWidgetFonts.h
//   Creation Date : 2016/04/01 02:49:27
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

#include "c3_config.h"

#include "C3OptionsWidget.h"

class C3TextEditorOptions;
class C3TextEditorOptionsWidgetFontsPrivate;

class C3TextEditorOptionsWidgetFonts : public C3OptionsWidget
{
public:
	C3TextEditorOptionsWidgetFonts(const QString &szLabel,QWidget * pParent,C3TextEditorOptions * pOptions);
	virtual ~C3TextEditorOptionsWidgetFonts();

private:
	C3TextEditorOptionsWidgetFontsPrivate * m_pC3TEOWF;

}; // class C3TextEditorOptionsWidgetFonts

#endif //!_C3TextEditorOptionsWidgetFonts_h_