#ifndef _C3SymbolsByLanguage_h_
#define _C3SymbolsByLanguage_h_
//=============================================================================
//
//   File : C3SymbolsByLanguage.h
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

#include "c3_config.h"

#include <C3SymbolNamespace.h>

#include <QHash>

class C3SymbolsByLanguage
{
private:

	// Hash of <first-letter> -> sorted C3SymbolList
	QHash<quint16,C3SymbolMap *> m_hAllSymbols;

public:
	C3SymbolsByLanguage(C3Symbol::Language eLanguage);
	~C3SymbolsByLanguage();

public:

	void addToAllSymbols(C3Symbol * pSymbol);

	C3SymbolMap * allSymbolsForFirstLetter(const QString &szText)
	{
		if(szText.isEmpty())
			return NULL;
		return m_hAllSymbols.value(szText.at(0).unicode(),NULL);
	}

	void clear();

}; // class C3SymbolsByLanguage

#endif //!_C3SymbolsByLanguage_h_