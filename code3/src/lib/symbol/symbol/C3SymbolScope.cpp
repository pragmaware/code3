//=============================================================================
//
//   File : C3SymbolScope.cpp
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

#include "C3SymbolScope.h"
#include "C3SymbolClass.h"
#include "C3SymbolNamespace.h"
#include "C3SymbolManager.h"
#include "C3SymbolTypeDefinition.h"

void C3SymbolScope::clear()
{
	C3Symbol::clear();
	if(!m_pSymbols)
		return;
	foreach(C3Symbol * pSymbol,*m_pSymbols)
		pSymbol->clear();

	m_pSymbols->clear();
}

bool C3SymbolScope::isScope() const
{
	return true;
}

#define DEBUG_RESOLVE_TYPE_TO_CLASS 1

C3SymbolScope * C3SymbolScope::resolveTypeToScope(const QString &szType)
{
#ifdef DEBUG_RESOLVE_TYPE_TO_CLASS
	C3_TRACE_FUNCTION;
	
	C3_TRACE("[C3SymbolScope] Resolve type '%s' to scope",szType.toUtf8().data());
#endif

	if(szType.isEmpty())
	{
		qDebug("[C3SymbolScope] Type is empty!");
		return NULL;
	}

	static QString szDoubleColon("::");

	int idx = szType.indexOf(szDoubleColon);

	C3Symbol * pType;
	
	C3SymbolScope * pGlobalScope = C3SymbolManager::instance()->globalScopeForLanguage(language());
	Q_ASSERT(pGlobalScope);

	if(idx >= 0)
	{
		// FIXME: We do not support complex scoped type definitions.
		//    typedef A B;
		//    typedef B::C D;
		// B will be resolved to A but when resolving D B may or may not be already resolved to a type containing C...
	
		QString szType2 = szType;

		C3Symbol * pPart;
		C3SymbolScope * pPartScope = NULL;

		if(idx == 0)
		{
			// global namespace?

			szType2 = szType2.mid(2);
			while(szType2.startsWith(szDoubleColon))
				szType2 = szType2.mid(2); // aargh?
			
			pPartScope = pGlobalScope;

			szType.indexOf(szDoubleColon);
		}

		while(idx >= 0)
		{
			Q_ASSERT(idx > 0);
		
			QString szC = szType2.left(idx);
			
#ifdef DEBUG_RESOLVE_TYPE_TO_CLASS
			C3_TRACE("[C3SymbolScope] - Resolve part %s",szC.toUtf8().data());
#endif
			
			szType2 = szType2.mid(idx + 2);
			while(szType2.startsWith(szDoubleColon))
				szType2 = szType2.mid(2); // aaargh?


			pPart = pPartScope ?
						pPartScope->findSymbolInThisAndBaseClasses(szC,C3Symbol::Class | C3Symbol::Enumeration | C3Symbol::Union | C3Symbol::Namespace | C3Symbol::TypeDefinition) :
						findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(szC,C3Symbol::Class | C3Symbol::Enumeration | C3Symbol::Union | C3Symbol::Namespace | C3Symbol::TypeDefinition,pGlobalScope);
			
			if(pPart)
			{
				if(pPart->type() == C3Symbol::TypeDefinition)
				{
					// scope that is itself a typedef... it may or may not be already resolved here
					pPartScope = static_cast<C3SymbolTypeDefinition *>(pPart)->resolvedScope();
				} else {
					pPartScope = dynamic_cast<C3SymbolScope *>(pPart);
				}

				if(!pPartScope)
				{
					qDebug("[C3SymbolScope] Can't resolve symbol %s (leading to type %s)",szC.toUtf8().data(),szType.toUtf8().data());
					break;
				}
			} else {

				// FIXME: Look also in the imported scopes for the file?

				pPartScope = NULL;
			
				qDebug("[C3SymbolScope] Can't resolve symbol %s (leading to type %s)",szC.toUtf8().data(),szType.toUtf8().data());
				break;
			}

			idx = szType2.indexOf(szDoubleColon);
		}
		
		if(pPartScope && (!szType2.isEmpty()))
			pType = pPartScope->findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(szType2,C3Symbol::Class | C3Symbol::Enumeration | C3Symbol::Union | C3Symbol::Namespace | C3Symbol::TypeDefinition,pGlobalScope);
		else
			pType = NULL;

	} else {
		pType = findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(szType,C3Symbol::Class | C3Symbol::Enumeration | C3Symbol::Union | C3Symbol::Namespace | C3Symbol::TypeDefinition,pGlobalScope);

		// FIXME: Look also in the imported scopes for the file?

#ifdef DEBUG_RESOLVE_TYPE_TO_CLASS
		if(!pType)
			C3_TRACE("[C3SymbolScope] - Type %s not found in scope %s, containing scopes and base classes",szType.toUtf8().data(),identifier().toUtf8().data());
		else
			C3_TRACE("[C3SymbolScope] - Type %s initially resolved to %s",szType.toUtf8().data(),pType->description().toUtf8().data());
#endif
	}
	
	if(pType)
	{
		if(pType->type() == C3Symbol::TypeDefinition)
			return static_cast<C3SymbolTypeDefinition *>(pType)->resolvedScope(); // if it was already resolved then OK, otherwise well'fail here....
		
		Q_ASSERT(pType->isScope());
		return (C3SymbolScope *)pType;
	}

	qDebug("[C3SymbolScope] Failed to resolve type %s",szType.toUtf8().data());
	return NULL;
}

/*

THIS IS NOW DONE ON-THE-FLY

void C3SymbolScope::resolveTypedefsAndBaseClasses()
{

	if(!m_pSymbols)
		return;

	foreach(C3Symbol * pSymbol,*m_pSymbols)
	{
		if(pSymbol->type() == C3Symbol::TypeDefinition)
		{
			C3SymbolTypeDefinition * pTypedef = dynamic_cast<C3SymbolTypeDefinition *>(pSymbol);
			Q_ASSERT(pTypedef);
			
			if(pTypedef->type())
				continue; // already resolved? (FIXME: Can it happen?)

			QString szType = C3SymbolManager::instance()->filterCppType(pTypedef->typeName());
			if(szType.isEmpty())
				continue;

			C3SymbolClass * pResolved = resolveTypeToClass(szType);

			if(pResolved)
				pTypedef->setResolvedClass(pResolved);
			else
				qDebug("[C3SymbolScope] Failed to resolve typedef type %s",szType.toUtf8().data());

			continue;
		}

		if(pSymbol->type() == C3Symbol::Class)
		{
			C3SymbolClass * pClass = dynamic_cast<C3SymbolClass *>(pSymbol);
			Q_ASSERT(pClass);

			if(!pClass->baseClasses())
			{
				QString szClasses = pClass->baseClassNames();
	
				if(szClasses.isEmpty())
					continue;
	
				// FIXME: Handle a qualified class names (with ::)
				// FIXME: Handle templates? (they are currently stripped off by splitBaseClassList()).
				
				QStringList lClassNames = splitBaseClassList(szClasses);
	
				Q_FOREACH(QString szClass,lClassNames)
				{
					C3SymbolClass * pBaseClass = resolveTypeToClass(szClass);
	
					if(!pBaseClass)
					{
						qDebug("[C3SymbolScope] Can't resolve base class %s",szClass.toUtf8().data());
						continue;
					}
					
					if(pBaseClass != pClass) // avoid recursion!
						pClass->addBaseClass(pBaseClass);
					else 
						qDebug("[C3SymbolScope] Class %s has itself as base",szClass.toUtf8().data());
				}
			} else {
				// base classes were already resolved (it can happen as we resolve them recursively)
			}

			dynamic_cast<C3SymbolScope *>(pSymbol)->resolveTypedefsAndBaseClasses();
			continue;
		}

		if(pSymbol->isScope())
		{
			dynamic_cast<C3SymbolScope *>(pSymbol)->resolveTypedefsAndBaseClasses();
			continue;
		}
	}
}
*/

C3Symbol * C3SymbolScope::findSymbolInThisAndBaseClasses(const QString &szIdentifier,quint16 uTypeMask)
{
	C3Symbol * pSym = findSymbol(szIdentifier,uTypeMask);
	if(pSym)
		return pSym;
	if(type() != C3Symbol::Class)
		return NULL;
	C3SymbolClass * pClass = dynamic_cast<C3SymbolClass *>(this);
	Q_ASSERT(pClass);
	
	QList<C3SymbolClass *> * pBaseClasses = pClass->baseClasses();
	if(!pBaseClasses)
		return NULL;
	Q_FOREACH(C3SymbolClass * pBase,*pBaseClasses)
	{
		pSym = pBase->findSymbolInThisAndBaseClasses(szIdentifier,uTypeMask);
		if(pSym)
			return pSym;
	}
	return NULL;
}

C3Symbol * C3SymbolScope::findSymbolInThisAndContainingScopes(const QString &szIdentifier,quint16 uTypeMask)
{
	//C3_TRACE_FUNCTION;
	
	//C3_TRACE("Lookup %s in %s",szIdentifier.toUtf8().data(),description().toUtf8().data());
	
	C3Symbol * pSym = findSymbol(szIdentifier,uTypeMask);
	if(pSym)
	{
		//C3_TRACE("Got: %s",pSym->description().toUtf8().data());
		return pSym;
	}
	if(!m_pContainingScope)
	{
		//C3_TRACE("No containing scope");
		return NULL;
	}

	return resolvedContainingScope()->findSymbolInThisAndContainingScopes(szIdentifier,uTypeMask);
}

C3Symbol * C3SymbolScope::findSymbolInThisScopeContainingScopesBaseClassesAndExcludeGlobalScope(
		const QString &szIdentifier,
		quint16 uTypeMask,
		C3SymbolScope * pGlobalScope
	)
{
	C3_TRACE_FUNCTION;
	
	C3_TRACE("Lookup %s in %s",szIdentifier.toUtf8().data(),description().toUtf8().data());

	if(this == pGlobalScope)
	{
		C3_TRACE("This is the global scope!");
		return NULL;
	}

	C3Symbol * pSym = findSymbol(szIdentifier,uTypeMask);
	if(pSym)
	{
		C3_TRACE("Got: %s",pSym->description().toUtf8().data());
		return pSym;
	}

	if(type() == C3Symbol::Class)
	{
		C3SymbolClass * pClass = dynamic_cast<C3SymbolClass *>(this);
		Q_ASSERT(pClass);
		
		QList<C3SymbolClass *> * pBaseClasses = pClass->baseClasses();
		
		if(pBaseClasses)
		{
			Q_FOREACH(C3SymbolClass * pBase,*pBaseClasses)
			{
				pSym = pBase->findSymbolInThisScopeContainingScopesBaseClassesAndExcludeGlobalScope(szIdentifier,uTypeMask,pGlobalScope);
				if(pSym)
				{
					C3_TRACE("Got: %s",pSym->description().toUtf8().data());
					return pSym;
				}
			}
		}
	}

	if(!m_pContainingScope)
	{
		C3_TRACE("No containing scope");
		return NULL;
	}
	
	if(m_pContainingScope == pGlobalScope)
	{
		C3_TRACE("Containing scope is global scope");
		return NULL;
	}

	return m_pContainingScope->findSymbolInThisScopeContainingScopesBaseClassesAndExcludeGlobalScope(szIdentifier,uTypeMask,pGlobalScope);
}

C3Symbol * C3SymbolScope::findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(
		const QString &szIdentifier,
		quint16 uTypeMask,
		C3SymbolScope * pGlobalScope
	)
{
	//C3_TRACE_FUNCTION;
	Q_ASSERT(pGlobalScope);

	C3Symbol * pSym = findSymbolInThisScopeContainingScopesBaseClassesAndExcludeGlobalScope(szIdentifier,uTypeMask,pGlobalScope);
	if(pSym)
		return pSym;

	// look in global scope once

	return pGlobalScope->findSymbol(szIdentifier,uTypeMask);
}

#if 0
C3Symbol * C3SymbolScope::findSymbolInThisScopeContainingScopesAndBaseClasses(const QString &szIdentifier,quint16 uTypeMask,bool bLookInContainingScopes)
{
	//C3_TRACE_FUNCTION;
	
	//C3_TRACE("Lookup %s in %s",szIdentifier.toUtf8().data(),description().toUtf8().data());
	
	// FIXME: Would be nice to avoid repeated lookups in the same scopes.
	//        For example, the global scope is looked up starting from each level
	//        of the hierarchy!
	// -> See findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope()
	
	C3Symbol * pSym = findSymbol(szIdentifier,uTypeMask);
	if(pSym)
	{
		//C3_TRACE("Got: %s",pSym->description().toUtf8().data());
		return pSym;
	}

	if(type() == C3Symbol::Class)
	{
		C3SymbolClass * pClass = dynamic_cast<C3SymbolClass *>(this);
		Q_ASSERT(pClass);
		
		QList<C3SymbolClass *> * pBaseClasses = pClass->baseClasses();
		
		if(pBaseClasses)
		{
			Q_FOREACH(C3SymbolClass * pBase,*pBaseClasses)
			{
				pSym = pBase->findSymbolInThisScopeContainingScopesAndBaseClasses(szIdentifier,uTypeMask,bLookInContainingScopes);
				if(pSym)
				{
					//C3_TRACE("Got: %s",pSym->description().toUtf8().data());
					return pSym;
				}
			}
		}
	}

	if(!bLookInContainingScopes)
	{
		return NULL;
	}

	if(!resolvedContainingScope())
	{
		//C3_TRACE("No containing scope");
		return NULL;
	}

	return resolvedContainingScope()->findSymbolInThisScopeContainingScopesAndBaseClasses(szIdentifier,uTypeMask);
}
#endif

C3Symbol * C3SymbolScope::findSymbol(const QString &szIdentifier,quint16 uTypeMask)
{
	if(!m_pSymbols)
		return NULL;


	QList<C3Symbol *> lSyms = m_pSymbols->values(szIdentifier);

	int c = lSyms.count();

	for(int i=0;i<c;i++)
	{
		C3Symbol * pSym = lSyms.value(i);
		if(pSym->type() & uTypeMask)
			return pSym;
	}

	return NULL;
}

C3SymbolScope * C3SymbolScope::findScope(const QString &szIdentifier)
{
	if(!m_pSymbols)
		return NULL;


	QList<C3Symbol *> lSyms = m_pSymbols->values(szIdentifier);

	int c = lSyms.count();

	for(int i=0;i<c;i++)
	{
		C3Symbol * pSym = lSyms.value(i);
		if(pSym->isScope())
			return static_cast<C3SymbolScope *>(pSym);
	}


	return NULL;
}

#if 0
C3SymbolNamespace * C3SymbolScope::findNamespace(const QString &szIdentifier)
{
	if(!m_pSymbols)
		return NULL;

	C3SymbolList::iterator end = m_pSymbols->end();

	C3Symbol oTmp(NULL);
	oTmp.setIdentifier(szIdentifier);

	C3SymbolList::iterator i = qLowerBound(m_pSymbols->begin(),end,&oTmp,C3Symbol::symbolIdentifierLessThan);

	while(i != end)
	{
		if((*i)->type() == C3Symbol::Namespace)
		{
			if((*i)->identifier() == szIdentifier)
				return dynamic_cast<C3SymbolNamespace *>(*i);
		}
		
		if((*i)->identifier() > szIdentifier)
			break;

		++i;
	}

	return NULL;
}
#else
C3SymbolNamespace * C3SymbolScope::findNamespace(const QString &szIdentifier)
{
	return dynamic_cast<C3SymbolNamespace *>(findSymbol(szIdentifier,C3Symbol::Namespace));
}

#endif

void C3SymbolScope::dump(const QString &szPrefix)
{
	C3Symbol::dump(szPrefix);

	if(!m_pSymbols)
		return;
		
	QString szNewPrefix = __utf8("   %1").arg(szPrefix);

	foreach(C3Symbol * pSymbol,*m_pSymbols)
		pSymbol->dump(szNewPrefix);
}
