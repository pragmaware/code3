#ifndef _C3OptionEditorString_h_
#define _C3OptionEditorString_h_
//=============================================================================
//
//   File : C3OptionEditorString.h
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

class C3OptionEditorStringPrivate;

class QLineEdit;
class QPushButton;

///
/// \class C3OptionEditorString
/// \brief The C3OptionEditorString class
///
/// This class...
///
class C3OptionEditorString : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorString
	///
	C3OptionEditorString(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorString
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorString();

public:

	enum Options
	{
		DontTrim = 1,
		AddSelectorButton = 2
	};

private:

	C3OptionEditorStringPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	QLineEdit * lineEdit();
	QPushButton * selectorButton();

	void create(const QString &szLabel,QString * pOption,unsigned int uOptions = 0);
	
	QString currentValue();
	void setCurrentValue(const QString &szValue);

	virtual bool commit();

private slots:

	void slotTextEdited(const QString &);

}; // class C3OptionEditorString

#endif //!_C3OptionEditorString_h_