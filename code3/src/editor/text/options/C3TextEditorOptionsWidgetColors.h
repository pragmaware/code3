#ifndef _C3TextEditorOptionsWidgetColors_h_
#define _C3TextEditorOptionsWidgetColors_h_
//=============================================================================
//
//   File : C3TextEditorOptionsWidgetColors.h
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


#include "c3_config.h"

#include "C3OptionsWidget.h"

class C3TextEditorOptions;
class C3TextEditorOptionsWidgetColorsPrivate;

///
/// class C3TextEditorOptionsWidgetColors
/// rief The C3TextEditorOptionsWidgetColors class
///
/// This class...
///
class C3TextEditorOptionsWidgetColors : public C3OptionsWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorOptionsWidgetColors
	///
	C3TextEditorOptionsWidgetColors(const QString &szLabel,QWidget * pParent,C3TextEditorOptions * pOptions);

	///
	/// Destroys the instance of C3TextEditorOptionsWidgetColors
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorOptionsWidgetColors();

private:

	C3TextEditorOptionsWidgetColorsPrivate * m_pC3SWU;

protected:

	virtual bool commit();
	void resetEditors();

protected slots:

	void saveToFile();
	void loadFromFile();

}; // class C3TextEditorOptionsWidgetColors

#endif //!_C3TextEditorOptionsWidgetColors_h_
