#ifndef _C3OptionEditorStringList_h_
#define _C3OptionEditorStringList_h_
//=============================================================================
//
//   File : C3OptionEditorStringList.h
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

#include <QStringList>


class C3OptionEditorStringListPrivate;

///
/// \class C3OptionEditorStringList
/// \brief The C3OptionEditorStringList class
///
/// This class...
///
class C3OptionEditorStringList : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorStringList
	///
	C3OptionEditorStringList(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorStringList
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorStringList();

public:

	enum Options
	{
		DontTrim = 1
	};

private:

	C3OptionEditorStringListPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	void create(const QString &szLabel,QStringList * pOption,unsigned int uOptions = 0);
	
	QStringList currentValue();
	void setCurrentValue(const QStringList &lValue);

	virtual bool commit();

private slots:

	void slotTextChanged();

}; // class C3OptionEditorStringList

#endif //!_C3OptionEditorStringList_h_
