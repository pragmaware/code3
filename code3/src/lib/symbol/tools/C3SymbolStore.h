#ifndef _C3SymbolStore_h_
#define _C3SymbolStore_h_
//=============================================================================
//
//   File : C3SymbolStore.h
//   Creation Date : 2015/12/05 00:41:41
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

#include "C3Symbol.h"
#include "C3SymbolFile.h"
#include "C3SymbolScope.h"
#include "C3SortedSymbols.h"

#include <QHash>
#include <QString>

// A container of symbols
class C3SymbolStore
{
private:
	// The hash of symbol files. The files are owned pointers and the files OWN the symbols.
	QHash<QString,C3SymbolFile *> m_hFiles;


	// Global scopes for each language. Each global scope contains the whole symbol subtree.
	// The containers are owned but each container has non owned pointers to C3Symbol instances
	// (which are actually owned by C3SymbolFile).
	C3SymbolNamespace * m_aGlobalScopes[C3Symbol::LanguageCount];

	// The map of C3Symbol::Language -> C3SortedSymbols.
	// The containers are owned but each container has non owned pointers to C3Symbol instances
	// (which are actually owned by C3SymbolFile).
	C3SortedSymbols * m_aSortedSymbols[C3Symbol::LanguageCount];

	C3SymbolFile m_oRootFile;

public:

	C3SymbolStore();
	~C3SymbolStore();

private:

	void resolveImportedScopes(C3SymbolFile * pFile,C3Symbol::Language eLanguage);
	void rebuildSymbolsByLanguage(C3Symbol::Language eLanguage);
	void buildSymbolsByLanguageForSymbolList(C3SymbolList &lSymbolList,C3Symbol::Language eLanguage,bool bBuildSortedSymbols);

public:

	// The returned value may be NULL!
	C3SymbolScope * globalScopeForLanguage(C3Symbol::Language eLanguage)
	{
		C3SymbolNamespace * pNS = m_aGlobalScopes[eLanguage];
	
		if(pNS)
			return pNS;

		rebuildSymbolsByLanguage(eLanguage);

		return m_aGlobalScopes[eLanguage];
	}

	C3SymbolMap * allSymbolsForFirstLetter(const QString &szText,C3Symbol::Language eLanguage)
	{
		C3SortedSymbols * pSyms = m_aSortedSymbols[eLanguage];
		if(!pSyms)
		{
			rebuildSymbolsByLanguage(eLanguage);
			pSyms = m_aSortedSymbols[eLanguage];
			if(!pSyms)
				return NULL;
		}
		return pSyms->allSymbolsForFirstLetter(szText);
	}

	// The returned value may be NULL!
	C3SymbolFile * file(const QString &szName);

	C3SymbolScope * findScope(const QString &szScope,C3Symbol::Language eLanguage);

	// Add a symbol file, eventually removing an existing one.
	void addFiles(QHash<QString,C3SymbolFile *> & hFiles);

	C3SymbolFile * rootFile()
	{
		return &m_oRootFile;
	}

}; // class C3SymbolStore

#endif //!_C3SymbolStore_h_
