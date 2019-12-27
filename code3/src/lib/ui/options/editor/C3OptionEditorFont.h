#ifndef _C3OptionEditorFont_h_
#define _C3OptionEditorFont_h_
//=============================================================================
//
//   File : C3OptionEditorFont.h
//   Creation Date : 2016/04/01 02:44:10
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

#include "C3OptionEditor.h"
#include <QFont>

class C3OptionEditorFontPrivate;

class C3OptionEditorFont : public C3OptionEditor
{
	Q_OBJECT
public:
	C3OptionEditorFont(C3OptionsWidget * pWidget);
	virtual ~C3OptionEditorFont();

public:
	enum Options
	{
		Dummy = 1
	};

private:

	C3OptionEditorFontPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	void create(const QString &szLabel,QFont * pOption,unsigned int uOptions = 0);
	
	QFont currentValue();
	void setCurrentValue(const QFont &oValue);

	virtual bool commit();

private slots:

	void slotButtonClicked();

}; // class C3OptionEditorFont

#endif //!_C3OptionEditorFont_h_