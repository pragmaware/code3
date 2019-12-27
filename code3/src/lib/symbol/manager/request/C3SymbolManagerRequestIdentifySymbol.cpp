//=============================================================================
//
//   File : C3SymbolManagerRequestIdentifySymbol.cpp
//   Creation Date : 2015/12/10 23:23:00
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

#include "C3SymbolManagerRequestIdentifySymbol.h"

C3SymbolManagerRequestIdentifySymbol::C3SymbolManagerRequestIdentifySymbol(
		C3TextEditorSymbolContext * pContext,
		C3Symbol::Language eLanguage,
		unsigned int uFlags
	)
	: C3SymbolManagerRequest(IdentifySymbol),
	m_pContext(pContext),
	m_eLanguage(eLanguage),
	m_uFlags(uFlags),
	m_pInfoList(NULL)
{
	Q_ASSERT(pContext);
}

C3SymbolManagerRequestIdentifySymbol::~C3SymbolManagerRequestIdentifySymbol()
{
	if(m_pInfoList)
	{
		qDeleteAll(*m_pInfoList);
		delete m_pInfoList;
	}
	delete m_pContext;
}


