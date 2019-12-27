//=============================================================================
//
//   File : C3SymbolManagerRequestPerformCompletion.cpp
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

#include "C3SymbolManagerRequestPerformCompletion.h"

C3SymbolManagerRequestPerformCompletion::C3SymbolManagerRequestPerformCompletion(
		const QString &szFilePath,
		C3TextEditorSymbolContext * pContext,
		C3Symbol::Language eLanguage,
		unsigned int uFlags
	)
	: C3SymbolManagerRequest(PerformCompletion),
	m_szFilePath(szFilePath),
	m_pContext(pContext),
	m_eLanguage(eLanguage),
	m_uFlags(uFlags),
	m_pCompletions(NULL)
{
	Q_ASSERT(pContext);
}

C3SymbolManagerRequestPerformCompletion::~C3SymbolManagerRequestPerformCompletion()
{
	if(m_pCompletions)
	{
		qDeleteAll(*m_pCompletions);
		delete m_pCompletions;
	}
	delete m_pContext;
}


