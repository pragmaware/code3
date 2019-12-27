#ifndef _C3SymbolManagerRequestDescribeClass_h_
#define _C3SymbolManagerRequestDescribeClass_h_
//=============================================================================
//
//   File : C3SymbolManagerRequestDescribeClass.h
//   Creation Date : 2016/09/22 00:32:51
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

#include "C3SymbolManagerClassDescription.h"
#include "C3SymbolManagerRequest.h"
#include "C3Symbol.h"

class C3SymbolManagerRequestDescribeClass : public C3SymbolManagerRequest
{
public:
	C3SymbolManagerRequestDescribeClass(
			const QString &szClassName, // full name, with context
			C3Symbol::Language eLanguage
		);
	virtual ~C3SymbolManagerRequestDescribeClass();
private:
	QString m_szClassName;
	C3Symbol::Language m_eLanguage;
	C3SymbolManagerClassDescription * m_pClassDescription;

public:
	void setClassDescription(C3SymbolManagerClassDescription * pDescription)
	{
		if(m_pClassDescription)
			delete m_pClassDescription;
		m_pClassDescription = pDescription;
	}

	C3SymbolManagerClassDescription * classDescription()
	{
		return m_pClassDescription;
	}

	const QString & className()
	{
		return m_szClassName;
	}
	
	C3Symbol::Language language()
	{
		return m_eLanguage;
	}

}; // class C3SymbolManagerRequestDescribeClass

#endif //!_C3SymbolManagerRequestDescribeClass_h_