#ifndef _C3OptionEditorEnumerationAsInt_h_
#define _C3OptionEditorEnumerationAsInt_h_
//=============================================================================
//
//   File : C3OptionEditorEnumerationAsInt.h
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

#include <QPair>
#include <QList>

class C3OptionEditorEnumerationAsIntPrivate;


///
/// \class C3OptionEditorEnumerationAsInt
/// \brief The C3OptionEditorEnumerationAsInt class
///
/// This class...
///
class C3OptionEditorEnumerationAsInt : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorEnumerationAsInt
	///
	C3OptionEditorEnumerationAsInt(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorEnumerationAsInt
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorEnumerationAsInt();

public:

	enum Options
	{
	};

private:

	C3OptionEditorEnumerationAsIntPrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	void create(
			const QString &szLabel,
			int * pOption,
			const QList< QPair<int,QString> > &lOptions,
			unsigned int uOptions = 0
		);
	
	int currentIntValue();
	void setCurrentIntValue(int iValue);

	void updateOptions(const QList< QPair<int,QString> > &lOptions);

	virtual bool commit();

private:

	int valueToIndex(int iValue);
	int indexToValue(int iIndex);

private slots:

	void slotCurrentIndexChanged(int idx);

}; // class C3OptionEditorEnumerationAsInt

#endif //!_C3OptionEditorEnumerationAsInt_h_
