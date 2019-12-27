#ifndef _C3SymbolFile_h_
#define _C3SymbolFile_h_
//=============================================================================
//
//   File : C3SymbolFile.h
//   Creation Date : 2015/12/05 00:23:39
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
#include "C3SymbolImportedScope.h"

#include <QString>
#include <QList>
#include <QDateTime>

// A file of symbols.
// This is the actual owner of the symbols. Always.
class C3SymbolFile
{
private:
	QDateTime m_oTimestamp;

	// The path of the file
	QString m_szPath;

	// The list of symbols (owned pointers).
	// These are ordered by file line.
	C3SymbolList m_lSymbols;

	// The using namespace symbols (owned pointers), these are not ordered.
	QList<C3SymbolImportedScope *> m_lImportedScopes;
	
	// The list of additional scopes in that we have to lookup symbols for this file.
	// The list is "unrolled" and "simplified": the scopes are not duplicated
	// and the global scope is removed.
	// The pointers are shallow.
	QList<C3SymbolScope *> m_lAdditionalSymbolLookupScopes;

	// The file context map. This is built on demand.
	// This contains only Class and Function definitions.
	// variables nor function declarations (function definitions
	// are included though).
	// The pointers here are shallow and always refer to this symbol file.
	C3SymbolList m_lContextMap;

	enum Flags : quint16
	{
		ContextMapDirty = 1
	};

	// The internal flags
	quint16 m_uFlags;

	// combination of (1 << C3Symbol::Language)
	quint16 m_uLanguages;
	
	// a symbol used for searching
	C3Symbol m_oSearchSymbol;

public:

	C3SymbolFile(const QString &szPath)
		: m_szPath(szPath),
		m_uFlags(ContextMapDirty),
		m_oSearchSymbol(this),
		m_uLanguages(0)
	{
		m_oTimestamp = QDateTime::currentDateTime();
	}
	
	~C3SymbolFile()
	{
		//foreach(C3Symbol * pSym,m_lSymbols)
		//{
		//	qDebug("Deleting symbol %s",pSym->identifier().toUtf8().data());
		//	delete pSym;
		//}
		qDeleteAll(m_lSymbols);
		qDeleteAll(m_lImportedScopes);
	}

public:

	inline bool containsLanguage(C3Symbol::Language eLanguage) const
	{
		return m_uLanguages & (1 << eLanguage);
	}
	
	// this expect the bit to be already shifted, it's faster
	inline bool containsLanguageFlag(quint16 uFlag) const
	{
		return m_uLanguages & uFlag;
	}

	inline bool containsOnlyLanguageFlag(quint16 uFlag) const
	{
		return (m_uLanguages & (~uFlag)) == 0;
	}

	inline const QDateTime & timestamp() const
	{
		return m_oTimestamp;
	}

	inline const QString & path() const
	{
		return m_szPath;
	}

	C3SymbolList & symbols()
	{
		return m_lSymbols;
	}
	
	bool hasImportedScopes()
	{
		return !m_lImportedScopes.isEmpty();
	}

	QList<C3SymbolImportedScope *> & importedScopes()
	{
		return m_lImportedScopes;
	}
	
	QList<C3SymbolScope *> & additionalSymbolLookupScopes()
	{
		return m_lAdditionalSymbolLookupScopes;
	}
	
	void clearAdditionalSymbolLookupScopes(C3Symbol::Language eLanguage);

	void addAdditionalSymbolLookupScope(C3SymbolScope * pScope)
	{
		m_lAdditionalSymbolLookupScopes.append(pScope);
	}

//#define C3SYMBOLFILE_PARANOID

	// The symbor pointer ownership is transferred
	void addSymbol(C3Symbol * pSymbol)
	{
		Q_ASSERT(pSymbol);
#ifdef C3SYMBOLFILE_PARANOID
		//if(!m_lSymbols.isEmpty())
		//{
			//C3Symbol * pLast = m_lSymbols.last();
			//Q_ASSERT(pLast->lineNumber() <= pSymbol->lineNumber()); <-- this fails in javascript (ctags reports tags out of order)
		//}
#endif //C3SYMBOLFILE_PARANOID
		m_lSymbols.append(pSymbol);

		m_uLanguages |= (1 << pSymbol->language());

#ifdef C3SYMBOLFILE_PARANOID
		// This should be called before the context map is built
		Q_ASSERT(m_uFlags & ContextMapDirty);
#endif //C3SYMBOLFILE_PARANOID
	}

	// These are things like "using namespace X"
	void addImportedScope(C3SymbolImportedScope * pScope)
	{
		Q_ASSERT(pScope);
		
		if(pScope->unresolvedScope().isEmpty())
		{
			// toplevel
			m_lImportedScopes.append(pScope);
			return;
		}
		
		// nested...
		addSymbol(pScope);
	}


	// Find the context preceeding uLineNumber (zero-based)
	C3Symbol * findContext(unsigned int uLineNumber);

	// This is called automatically in findContext().
	// However for certain files you might want to call it manually
	// while still collecting symbols to keep the overhead on a slave
	// thread.
	void buildContextMap();

}; // class C3SymbolFile

#endif //!_C3SymbolFile_h_
