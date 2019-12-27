//=============================================================================
//
//   File : C3SymbolManagerRequestDescribeClass.cpp
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

#include "C3SymbolManagerRequestDescribeClass.h"

C3SymbolManagerRequestDescribeClass::C3SymbolManagerRequestDescribeClass(
		const QString &szClassName, // full name, with context
		C3Symbol::Language eLanguage
	)
	: C3SymbolManagerRequest(DescribeClass),
	m_szClassName(szClassName),
	m_eLanguage(eLanguage),
	m_pClassDescription(NULL)
{
}

C3SymbolManagerRequestDescribeClass::~C3SymbolManagerRequestDescribeClass()
{
	if(m_pClassDescription)
		delete m_pClassDescription;
}


