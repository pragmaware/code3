#ifndef _C3OptionEditorEnumeration_h_
#define _C3OptionEditorEnumeration_h_
//=============================================================================
//
//   File : C3OptionEditorEnumeration.h
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

#include "C3OptionEditorEnumerationAsInt.h"

///
/// \class C3OptionEditorEnumerationAsInt
/// \brief The C3OptionEditorEnumerationAsInt class
///
/// This class...
///
template<typename T> class C3OptionEditorEnumeration : public C3OptionEditorEnumerationAsInt
{
public:

	///
	/// Creates an instance of C3OptionEditorEnumerationAsInt
	///
	C3OptionEditorEnumeration(C3OptionsWidget * pWidget)
		: C3OptionEditorEnumerationAsInt(pWidget)
	{
	}

	///
	/// Destroys the instance of C3OptionEditorEnumerationAsInt
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorEnumeration()
	{
	}

private:

	int m_iFakeOption;
	T * m_pOption;

public:

	void create(
			const QString &szLabel,
			T * pOption,
			const QList< QPair<int,QString> > &lOptions,
			unsigned int uOptions = 0
		)
	{
		m_iFakeOption = (int)*pOption;
		m_pOption = pOption;
		C3OptionEditorEnumerationAsInt::create(szLabel,&m_iFakeOption,lOptions,uOptions);
	}

	T currentValue()
	{
		return (T)currentIntValue();
	}
	
	void setCurrentValue(T v)
	{
		setCurrentIntValue((int)v);
	}
	

	virtual bool commit()
	{
		if(!C3OptionEditorEnumerationAsInt::commit())
			return false;
		*m_pOption = (T)m_iFakeOption;
		return true;
	}

}; // class C3OptionEditorEnumeration

#endif //!_C3OptionEditorEnumeration_h_
