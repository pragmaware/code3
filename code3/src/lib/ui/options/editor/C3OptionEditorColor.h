#ifndef _C3OptionEditorColor_h_
#define _C3OptionEditorColor_h_
//=============================================================================
//
//   File : C3OptionEditorColor.h
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


#include "c3_config.h"

#include "C3OptionEditor.h"

#include <QColor>

class C3OptionEditorColorPrivate;

///
/// \class C3OptionEditorColor
/// \brief The C3OptionEditorColor class
///
/// This class...
///
class C3OptionEditorColor : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorColor
	///
	C3OptionEditorColor(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorColor
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorColor();

public:

	enum Options
	{
		Dummy = 1
	};

private:

	C3OptionEditorColorPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	void create(const QString &szLabel,QColor * pOption,unsigned int uOptions = 0);
	
	QColor currentValue();
	void setCurrentValue(const QColor &oValue);
	
	void resetValue();

	virtual bool commit();

private slots:

	void slotButtonClicked();

}; // class C3OptionEditorColor

#endif //!_C3OptionEditorColor_h_