//=============================================================================
//
//   File : C3SortedSymbols.cpp
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

#include "C3SortedSymbols.h"

C3SortedSymbols::C3SortedSymbols(
		C3Symbol::Language eLanguage
	)
{
	
}

C3SortedSymbols::~C3SortedSymbols()
{
	m_hAllSymbols.reserve(128); // at least
	clear();
}

void C3SortedSymbols::clear()
{
	qDeleteAll(m_hAllSymbols);
	m_hAllSymbols.clear();
}

void C3SortedSymbols::add(C3Symbol * pSymbol)
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

void C3SortedSymbols::remove(C3Symbol * pSymbol)
{
	const QString & szText = pSymbol->identifier();

	Q_ASSERT(!szText.isEmpty());

	quint16 uu = szText.at(0).unicode();

	C3SymbolMap * map = m_hAllSymbols.value(uu);
	if(!map)
		return;

	if(map->remove(szText,pSymbol) != 1)
		qDebug("WARNING: Symbol %s was not in the sorted symbols",szText.toUtf8().data());
}
