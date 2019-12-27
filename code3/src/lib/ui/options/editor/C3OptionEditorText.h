#ifndef _C3OptionEditorText_h_
#define _C3OptionEditorText_h_
//=============================================================================
//
//   File : C3OptionEditorText.h
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

class C3OptionEditorTextPrivate;
class QTextEdit;

///
/// \class C3OptionEditorText
/// \brief The C3OptionEditorText class
///
/// This class...
///
class C3OptionEditorText : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorText
	///
	C3OptionEditorText(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorText
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorText();

public:

	enum Options
	{
		Trim = 1,
		ReadOnly = 2
	};

private:

	C3OptionEditorTextPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	QTextEdit * textEdit();

	void create(const QString &szLabel,QString * pOption,unsigned int uOptions = 0);
	
	QString currentValue();
	void setCurrentValue(const QString &szValue);

	virtual bool commit();

private slots:

	void slotTextChanged();

}; // class C3OptionEditorText

#endif //!_C3OptionEditorText_h_
