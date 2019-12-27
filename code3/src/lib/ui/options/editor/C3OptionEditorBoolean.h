#ifndef _C3OptionEditorBoolean_h_
#define _C3OptionEditorBoolean_h_
//=============================================================================
//
//   File : C3OptionEditorBoolean.h
//   Creation Date : mer 11 nov 2015 04:32:57
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

class C3OptionEditorBooleanPrivate;
class QCheckBox;

///
/// \class C3OptionEditorBoolean
/// \brief The C3OptionEditorBoolean class
///
/// This class...
///
class C3OptionEditorBoolean : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorBoolean
	///
	C3OptionEditorBoolean(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorBoolean
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorBoolean();

private:

	C3OptionEditorBooleanPrivate * m_pC3OEB;

public:

	enum Options
	{
	};


public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();
	void currentValueChanged(bool bChecked);

public:

	QCheckBox * checkBox();

	void create(const QString &szLabel,bool * pOption,unsigned int uOptions = 0);
	
	bool currentValue();
	void setCurrentValue(bool bValue);

	virtual bool commit();

private slots:

	void slotCheckBoxStateChanged(int iState);

}; // class C3OptionEditorBoolean

#endif //!_C3OptionEditorBoolean_h_