//=============================================================================
//
//   File : C3SymbolsByLanguage.cpp
//   Creation Date : 2015/12/14 21:29:32
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

#include "C3SymbolsByLanguage.h"

C3SymbolsByLanguage::C3SymbolsByLanguage(
		C3SymbolFile * pFile,
		C3Symbol::Language eLanguage
	)
	: m_oNamespace(
			pFile,
			eLanguage,
			__ascii("<global>"),
			QString(),
			C3Symbol::AccessLevelPublic,
			0,
			0
		)
{
	
}

C3SymbolsByLanguage::~C3SymbolsByLanguage()
{
	clear();
}

void C3SymbolsByLanguage::clear()
{
	qDeleteAll(m_hAllSymbols);
	m_hAllSymbols.clear();
	m_oNamespace.clear();
}

void C3SymbolsByLanguage::addToAllSymbols(C3Symbol * pSymbol)
{
	const QString & szText = pSymbol->identifier();

	Q_ASSERT(!szText.isEmpty());

	quint16 uu = szText.at(0).unicode();

	C3SymbolMap * map = m_hAllSymbols.value(uu);
	if(!map)
	{
		map = new C3SymbolMap();
		m_hAllSymbols.insert(uu,map);
	}

	// This is very costly (QMultiMap is sorted)
	map->insert(szText,pSymbol);
}
