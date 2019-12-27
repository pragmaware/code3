#ifndef _C3OptionEditorInteger_h_
#define _C3OptionEditorInteger_h_
//=============================================================================
//
//   File : C3OptionEditorInteger.h
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

class C3OptionEditorIntegerPrivate;

class QLineEdit;

///
/// \class C3OptionEditorInteger
/// \brief The C3OptionEditorInteger class
///
/// This class...
///
class C3OptionEditorInteger : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorInteger
	///
	C3OptionEditorInteger(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorInteger
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorInteger();

public:

	enum Options
	{
	};

private:

	C3OptionEditorIntegerPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	QLineEdit * lineEdit();

	void create(const QString &szLabel,int * pOption,unsigned int uOptions = 0);
	
	int currentValue(bool * bOk = NULL,int iDefault = 0);
	void setCurrentValue(int iValue);

	virtual bool commit();

private slots:

	void slotTextEdited(const QString &);

}; // class C3OptionEditorInteger

#endif //!_C3OptionEditorInteger_h_