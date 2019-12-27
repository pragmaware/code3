//=============================================================================
//
//   File : C3SymbolManagerRequestIdentifyContext.cpp
//   Creation Date : 2016/09/13 03:19:12
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

#include "C3SymbolManagerRequestIdentifyContext.h"

C3SymbolManagerRequestIdentifyContext::C3SymbolManagerRequestIdentifyContext(
		const QString &szFilePath,
		unsigned int uLine,
		C3Symbol::Language eLanguage
	)
	: C3SymbolManagerRequest(IdentifyContext),
	m_szFilePath(szFilePath),
	m_uLine(uLine),
	m_eLanguage(eLanguage),
	m_pSymbolDescription(NULL)
{
	
}

C3SymbolManagerRequestIdentifyContext::~C3SymbolManagerRequestIdentifyContext()
{
	if(m_pSymbolDescription)
		delete m_pSymbolDescription;
}


