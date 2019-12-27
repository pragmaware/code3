#ifndef _C3SymbolScope_h_
#define _C3SymbolScope_h_
//=============================================================================
//
//   File : C3SymbolScope.h
//   Creation Date : 2015/12/05 07:07:30
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

#include <QStringList>

class C3SymbolNamespace;
class C3SymbolScope;

class C3SymbolScope : public C3Symbol
{
private:
	// The variables are NOT owned.
	// The pointer MAY be null.
	C3SymbolMap * m_pSymbols;

public:
	C3SymbolScope(
			C3SymbolFile * pFile,
			Language eLanguage,
			Type eType,
			const QString &szIdentifier,
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber
		) : C3Symbol(
			pFile,
			eLanguage,
			eType,
			szIdentifier,
			szScope,
			eAccess,
			uLineNumber,
			uEndLineNumber
		), m_pSymbols(NULL)
	{
	}
	
	virtual ~C3SymbolScope()
	{
		if(m_pSymbols)
			delete m_pSymbols;
	}

public:

	// This is a recursive clear operation (subscopes will be cleared too!)
	virtual void clear();

	inline void addSymbol(C3Symbol * pSymbol)
	{
		//Q_ASSERT(pSymbol);
		
		//Q_ASSERT(m_pSymbols);
		
		pSymbol->setParentScope(this);
	
		if(!m_pSymbols)
			m_pSymbols = new C3SymbolMap();
	
		m_pSymbols->insert(pSymbol->identifier(),pSymbol);
	}
	
	// The returned pointer MAY be NULL
	C3SymbolMap * symbols()
	{
		return m_pSymbols;
	}
	
	int symbolCount()
	{
		return m_pSymbols ? m_pSymbols->count() : 0;
	}

	// Looks only in this scope
	C3Symbol * findSymbol(const QString &szIdentifier,quint16 uTypeMask);

	// szIdentifier is a SINGLE scope (not a contatenations of multiple levels)
	// Looks only in this scope.
	C3SymbolScope * findScope(const QString &szIdentifier);

	// szIdentifier is a SINGLE scope (not a contatenations of multiple levels)
	// Looks only in this scope.
	C3SymbolNamespace * findNamespace(const QString &szIdentifier);
	
	// Looks in this and in containing scopes (NO BASE CLASSES)
	C3Symbol * findSymbolInThisAndContainingScopes(const QString &szIdentifier,quint16 uTypeMask);

	// Looks in this and in parent class scopes (if this symbol is a class)
	C3Symbol * findSymbolInThisAndBaseClasses(const QString &szIdentifier,quint16 uTypeMask);

#if 0
	// A combination of the above two: look in containing scopes and if they are classes
	// look also in the base classes.
	// Note that if the scope chain is broken in some way then the lookup will not reach global scope.
	// If the scope/base chains are not broken the deeper levels may be repeated and the lookup
	// will be repeated too..
	C3Symbol * findSymbolInThisScopeContainingScopesAndBaseClasses(
			const QString &szIdentifier,
			quint16 uTypeMask,
			bool bLookInContainingScopes = true
		);
#endif

	// This is similar to the one above but it makes sure that the global
	// scope lookup is done ONCE and is done even if the scope/base chain is broken.
	C3Symbol * findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(
			const QString &szIdentifier,
			quint16 uTypeMask,
			C3SymbolScope * pGlobalScope
		);

	// This one NEVER looks up in the global scope.
	C3Symbol * findSymbolInThisScopeContainingScopesBaseClassesAndExcludeGlobalScope(
			const QString &szIdentifier,
			quint16 uTypeMask,
			C3SymbolScope * pGlobalScope
		);

	// This is no longer used: lazy resolution is performed.
	//void resolveTypedefsAndBaseClasses();

	// Attempt to resolve the type to a class/union/namespace/enum.
	// If the type resolves to something else it's simply discarded.
	// The type must be cleaned up (i.e, no const, mutable and other garbage)
	C3SymbolScope * resolveTypeToScope(const QString &szType);

	virtual bool isScope() const;
	
	virtual void dump(const QString &szPrefix);

}; // class C3SymbolScope

typedef QList<C3SymbolScope *> C3SymbolScopeList;

#endif //!_C3SymbolScope_h_
