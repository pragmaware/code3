//=============================================================================
//
//   File : C3TextEditorOptions.cpp
//   Creation Date : sab 24 ott 2015 04:54:22
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

#include "C3TextEditorOptions.h"

#include <QSettings>

C3TextEditorOptions::C3TextEditorOptions()
	: C3TextEditorOptionsColors()
{
}

C3TextEditorOptions::~C3TextEditorOptions()
{
}

void C3TextEditorOptions::load()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","ssex.texteditoroptions");

	loadColors(s);

#define LOAD_FONT(_oObject,_szOptionName,_szDefaultFamily,_iDefaultPointSize) \
	do { \
		QVariant v = s.value(_szOptionName); \
		if((!v.isNull()) && v.canConvert<QFont>()) \
		{ \
			_oObject = v.value<QFont>(); \
		} else { \
			_oObject = QFont(_szDefaultFamily,_iDefaultPointSize); \
		} \
	} while(0)

#define LOAD_INT(_oObject,_szOptionName,_iDefault) \
	do { \
		QVariant v = s.value(_szOptionName); \
		if((!v.isNull()) && v.canConvert<int>()) \
		{ \
			_oObject = v.value<int>(); \
		} else { \
			_oObject = _iDefault; \
		} \
	} while(0)

	//LOAD_FONT(oTextFont,"textFont","Ubuntu Mono",13);
	//LOAD_FONT(oTextFont,"textFont","Monospace",11);
	LOAD_FONT(oTextFont,"textFont","Droid Sans Mono",11);

	LOAD_INT(iTabStopSize,"numberOfSpacesPerTab",4);
	LOAD_INT(iAdditionalLineSpacing,"additionalLineSpacing",0);
}

void C3TextEditorOptions::save()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","ssex.texteditoroptions");

	saveColors(s);

#define SAVE_FONT(_oObject,_szOptionName) \
	do { \
		s.setValue(_szOptionName,QVariant(_oObject)); \
	} while(0)

#define SAVE_INT(_oObject,_szOptionName) \
	do { \
		s.setValue(_szOptionName,QVariant(_oObject)); \
	} while(0)

	SAVE_FONT(oTextFont,"textFont");

	SAVE_INT(iTabStopSize,"numberOfSpacesPerTab");
	SAVE_INT(iAdditionalLineSpacing,"additionalLineSpacing");
}
