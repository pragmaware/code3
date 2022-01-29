//=============================================================================
//
//   File : C3SymbolManager.cpp
//   Creation Date : 2015/12/06 05:55:33
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

#include "C3SymbolManager.h"
#include "C3SymbolManagerCTagsWorker.h"
#include "C3SymbolStore.h"
#include "C3SymbolManagerRequestResetSymbolStore.h"
#include "C3SymbolManagerRequestPerformCompletion.h"
#include "C3SymbolManagerRequestIdentifySymbol.h"
#include "C3SymbolManagerRequestScanEditorFile.h"
#include "C3SymbolManagerRequestIdentifyContext.h"
#include "C3SymbolManagerRequestDescribeClass.h"
#include "C3SymbolVariable.h"
#include "C3SymbolClass.h"
#include "C3SymbolTypeDefinition.h"
#include "C3SymbolFunctionDefinition.h"
#include "C3SymbolFunctionPrototype.h"
#include "C3TextEditorSymbolInfo.h"

#include <QMutex>
#include <QThreadPool>
#include <QElapsedTimer>
#include <C3TextEditorMode.h>

//#define DEBUG_SYMBOL_MANAGER

#ifdef DEBUG_SYMBOL_MANAGER
	#define SYMBOL_MANAGER_TRACE_FUNCTION C3_TRACE_FUNCTION 
	#define SYMBOL_MANAGER_TRACE_BLOCK(_blk) C3_TRACE_BLOCK(_blk)
	#ifdef __GNUC__
		#define SYMBOL_MANAGER_TRACE(_szFmt,arg...) C3_TRACE(_szFmt,##arg)
	#else
		#define SYMBOL_MANAGER_TRACE(_szFmt,...) C3_TRACE(_szFmt,__VA_ARGS__)
	#endif
#else
	#define SYMBOL_MANAGER_TRACE_FUNCTION do { } while(0)
	#define SYMBOL_MANAGER_TRACE_BLOCK(_blk) do { } while(0)
	#ifdef __GNUC__
		#define SYMBOL_MANAGER_TRACE(_szFmt,arg...) do { } while(0)
	#else
		#define SYMBOL_MANAGER_TRACE(_szFmt,...) do { } while(0)
	#endif
#endif

C3SymbolManager * C3SymbolManager::m_pInstance = NULL;

class C3SymbolManagerPrivate
{
public:
	QMutex oCTagsWorkersMutex; // protects the hCtagsWorkers hash and the hSymbolFiles hash
	QHash<int,C3SymbolManagerCTagsWorker *> hCTagsWorkers;
	int iNextCTagsWorkerId;
	QString szCTagsToolPath;
	bool bDying;
	QHash<QString,int> hTokensToFilterInTypes;
	QStringList lCPPKeywords; // sorted
	QStringList lPHPKeywords; // sorted
	QHash<QString,C3SymbolFile *> hSymbolFiles;

	QMutex oStoreMutex;
	C3SymbolStore * pStore;
	
	// THIS IS VALID ONLY INSIDE processRequest()
	C3SymbolManagerRequest * pCurrentlyProcessedRequest;
};

#define _p m_pC3SM

C3SymbolManager::C3SymbolManager(const QString &szCTagsToolPath)
	: C3SymbolManagerBase()
{
	m_pInstance = this;
	_p = new C3SymbolManagerPrivate();
	_p->pStore = new C3SymbolStore();
	_p->bDying = false;
	_p->iNextCTagsWorkerId = 0;
	_p->szCTagsToolPath = szCTagsToolPath;
	_p->hTokensToFilterInTypes.insert(__literal("const"),1);
	_p->hTokensToFilterInTypes.insert(__literal("enum"),1);
	_p->hTokensToFilterInTypes.insert(__literal("struct"),1);
	_p->hTokensToFilterInTypes.insert(__literal("class"),1);
	_p->hTokensToFilterInTypes.insert(__literal("union"),1);
	_p->hTokensToFilterInTypes.insert(__literal("volatile"),1);
	_p->hTokensToFilterInTypes.insert(__literal("mutable"),1);
	_p->hTokensToFilterInTypes.insert(__literal("static"),1);
	_p->hTokensToFilterInTypes.insert(__literal("explicit"),1);
	_p->hTokensToFilterInTypes.insert(__literal("virtual"),1);
	_p->hTokensToFilterInTypes.insert(__literal("override"),1);
	_p->hTokensToFilterInTypes.insert(__literal("inline"),1);
	_p->hTokensToFilterInTypes.insert(__literal("__inline"),1);
	_p->hTokensToFilterInTypes.insert(__literal("extern"),1);

	m_szArrow = __literal("->");
	m_szSquareArrow = __literal("[]->");
	m_szSquareDot = __literal("[].");
	m_szDot = __literal(".");
	m_szParenthesisArrow = __literal("()->");
	m_szParenthesisDot = __literal("().");
	m_szTwoColons = __literal("::");
	m_szNew = __literal("new");
	m_szConst = __literal("const");
	m_szStatic = __literal("static");
	m_szInline = __literal("inline");
	m_szDelete = __literal("delete");
	m_szThis = __literal("this");
	m_szP = __literal("_p");
	m_szParenthesis = __literal("(");
	m_szOpenAndClosedParenthesis = __literal("()");

}

C3SymbolManager::~C3SymbolManager()
{
	_p->bDying = true;
	abortAllCTagsWorkers();
	_p->oCTagsWorkersMutex.lock();
	qDeleteAll(_p->hSymbolFiles);
	_p->oCTagsWorkersMutex.unlock();
	_p->oStoreMutex.lock();
	delete _p->pStore;
	_p->oStoreMutex.unlock();
	delete _p;
	m_pInstance = NULL;
}

C3SymbolScope * C3SymbolManager::globalScopeForLanguage(C3Symbol::Language eLang)
{
	return _p->pStore->globalScopeForLanguage(eLang);
}

void C3SymbolManager::abortAllCTagsWorkers()
{
	_p->oCTagsWorkersMutex.lock();
	C3SymbolManagerCTagsWorker * pWorker;
	QList<C3SymbolManagerCTagsWorker *> lWorkers;
	foreach(pWorker,_p->hCTagsWorkers)
		lWorkers.append(pWorker);
	
	foreach(pWorker,lWorkers)
	{
		int id = pWorker->id();
		if(!pWorker->abort())
		{
			// The abort call failed: the worker is already in its termination call.
			// Unlock the mutex and wait for it to complete.
			_p->oCTagsWorkersMutex.unlock();
			
			//qDebug("[C3SymbolManager::abortAllCTagsWorkers] Waiting for ctags worker %d to exit",id);
			
			while(_p->hCTagsWorkers.contains(id))
				QThread::msleep(50);

			_p->oCTagsWorkersMutex.lock();
		} else {
			// Abort succeeded. The worker will no longer call us back here.
#if QT_VERSION >= 0x50500
			QThreadPool::globalInstance()->cancel(pWorker);
#endif
		}
	}
	_p->hCTagsWorkers.clear();
	_p->oCTagsWorkersMutex.unlock();
}

void C3SymbolManager::processResetSymbolStoreRequest(C3SymbolManagerRequestResetSymbolStore * pRequest)
{
	//SYMBOL_MANAGER_TRACE_FUNCTION;

	_p->oStoreMutex.lock();
	delete _p->pStore;
	_p->pStore = new C3SymbolStore();
	_p->oStoreMutex.unlock();

	QStringList lRescanPaths = pRequest->rescanPaths();
	if(lRescanPaths.isEmpty())
		return;

	SYMBOL_MANAGER_TRACE("Starting CTags worker for %d rescan paths",lRescanPaths.count());
	
	_p->iNextCTagsWorkerId++;
	
	unsigned int uFlags = 0;
	if(pRequest->flags() & C3SymbolManagerRequestResetSymbolStore::LimitRecursionDepthTo1)
		uFlags |= C3SymbolManagerCTagsWorker::JobLimitRecursionDepthTo1;
	
	C3SymbolManagerCTagsWorker * pWorker = new C3SymbolManagerCTagsWorker(
			_p->iNextCTagsWorkerId,
			this,
			_p->szCTagsToolPath,
			lRescanPaths,
			pRequest->exclusionPatterns(),
			-1,
			uFlags
		);
	
	pWorker->setAutoDelete(true);

	_p->oCTagsWorkersMutex.lock();
	_p->hCTagsWorkers.insert(pWorker->id(),pWorker);
	_p->oCTagsWorkersMutex.unlock();

	QThreadPool::globalInstance()->start(pWorker);
}

QString C3SymbolManager::filterCppType(const QString &szType,QStringList * pTemplatePart)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	SYMBOL_MANAGER_TRACE("Filter cpp type %s",szType.toUtf8().data());
	
	if(szType.isEmpty())
	{
		SYMBOL_MANAGER_TRACE("Type is empty");
		return szType;
	}

	// split words/symbols/spaces
	const QChar * b = szType.unicode();
	const QChar * p = b;
	const QChar * e = b + szType.length();

	int iGroup,iGroup2;
	
	if(p->isSpace() || (p->unicode() == '*') || (p->unicode() == '&'))
		iGroup = 1;
	else
		iGroup = 2;

	p++;

	QString szFiltered;

	int iParts = 0;

	while(p <= e)
	{
		if(p->isSpace() || (p->unicode() == 0) || (p->unicode() == '*') || (p->unicode() == '&'))
			iGroup2 = 1;
		else
			iGroup2 = 2;
	
		if(iGroup2 != iGroup)
		{
			switch(iGroup)
			{
				case 1:
					// skip
				break;
				case 2:
				{
					// keep if good
					const QChar * pp = p - 1;
					
					QString szToken(b,p-b);
					
					if(!_p->hTokensToFilterInTypes.value(szToken))
					{
						//if(iParts > 0)
						//	return QString(); // not simple type
						szFiltered.append(szToken);
						iParts++;
					}
				}
				break;
			}
			b = p;
			iGroup = iGroup2;
		}
		p++;
	}

	int idx = szFiltered.indexOf('<');
	while(idx >= 0)
	{
		int l = szFiltered.length();
		int end = idx + 1;
		int iLevel = 1;
		while(end < l)
		{
			unsigned short uc = szFiltered[end].unicode();
			if(uc == '<')
			{
				iLevel++;
				end++;
				continue;
			}
			if(uc == '>')
			{
				iLevel--;
				if(iLevel <= 0)
					break;
				end++;
				continue;
			}
			end++;
		}

		if(pTemplatePart)
		{
			//qDebug("Type has template arguments!");
			QString szTemplate = szFiltered.mid(idx+1,end - idx - 1);
			if(!szTemplate.isEmpty())
			{
				// FIXME: Split better ? (nested templates possible!)
				QStringList sl = szTemplate.split(',');
				
				foreach(QString ss,sl)
					pTemplatePart->append(ss.trimmed());
					
				//qDebug("Found %d template parts",pTemplatePart->count());
			}
		} else {
			// just cut it
		}

		szFiltered.remove(idx,end-idx+1);
		idx = szFiltered.indexOf('<');
	}
	
	idx = szFiltered.indexOf('[');
	if(idx >= 0)
	{
		// just cut it
		szFiltered = szFiltered.left(idx);
	}

	//qDebug("Returning '%s' (from '%s')",szFiltered.toUtf8().data(),szType.toUtf8().data());

	SYMBOL_MANAGER_TRACE("Returning '%s'",szFiltered.toUtf8().data());
	return szFiltered;
}

C3SymbolScope * C3SymbolManager::storeLockedResolveSimpleCppTypeToSymbolScope(
		C3SymbolFile * pFile,
		const QString & szType,
		C3SymbolScope * pTypeScope,
		bool bMayBeNamespace
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;
	
	// FIXME: We could use C3SymbolScole::resolveTypeToScope() here!
	//        It's only missing the additional scope lookup!

	SYMBOL_MANAGER_TRACE("Attempt to resolve type '%s' with type scope '%s'",szType.toUtf8().data(),pTypeScope->description().toUtf8().data());

	QString szRealType = szType;

	int idx = szRealType.indexOf(m_szTwoColons);
	
	C3SymbolScope * pGlobalScope = _p->pStore->globalScopeForLanguage(pTypeScope->language());
	if(!pGlobalScope)
	{
		SYMBOL_MANAGER_TRACE("No global scope for language!");
		return NULL;
	}
	
	// Cut initial ::
	while(idx == 0)
	{
		// global symbol lookup
		pTypeScope = pGlobalScope;
		Q_ASSERT(pTypeScope);
		szRealType = szRealType.mid(2);
		idx = szRealType.indexOf(m_szTwoColons);
	}
	
	if(szRealType.isEmpty())
	{
		SYMBOL_MANAGER_TRACE("Real type empty");
		return NULL;
	}

	QString szLeft;
	QString szRemaining;

	if(idx > 0)
	{
		// There were more ::
		szLeft = szRealType.left(idx); // this is the first type on left
		szRemaining = szRealType.mid(idx+2); // this is the remaining part
	} else {
		szLeft = szRealType; // only one type
	}

	if(szLeft.isEmpty())
	{
		SYMBOL_MANAGER_TRACE("Left empty");
		return NULL;
	}

	unsigned int uKinds = C3Symbol::Class | C3Symbol::Enumeration | C3Symbol::Union | C3Symbol::TypeDefinition;
	if(bMayBeNamespace || (!szRemaining.isEmpty()))
		uKinds |= C3Symbol::Namespace;
	
	C3Symbol * pAuxType = pTypeScope->findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(szLeft,uKinds,pGlobalScope);

	if((!pAuxType) && pTypeScope->file()) // still not found and this is the first part in the qualified name (e.g. A in A::B::C)
	{
		// try additional lookup scopes for file?
		QList<C3SymbolScope *> & lAdditionalScopes = pTypeScope->file()->additionalSymbolLookupScopes();
		Q_FOREACH(C3SymbolScope * pAdditionalScope,lAdditionalScopes)
		{
			pAuxType = pAdditionalScope->findSymbolInThisAndBaseClasses(
					szLeft,
					uKinds
				);
			if(pAuxType)
				break;
		}
	}
	
	if(pAuxType && (pAuxType->type() == C3Symbol::TypeDefinition))
	{
		SYMBOL_MANAGER_TRACE("Found as type definition");

		pAuxType = static_cast<C3SymbolTypeDefinition *>(pAuxType)->resolvedScope();
	}

	if(!pAuxType)
	{
		SYMBOL_MANAGER_TRACE("Not found");
		return NULL;
	}

	Q_ASSERT(pAuxType->type() & (C3Symbol::Class | C3Symbol::Union | C3Symbol::Namespace | C3Symbol::Enumeration));

	if(szRemaining.isEmpty())
	{
		SYMBOL_MANAGER_TRACE("Resolved to '%s'",pAuxType->identifier().toUtf8().data());
		return dynamic_cast<C3SymbolScope *>(pAuxType);
	}

	SYMBOL_MANAGER_TRACE("Resolved to '%s', but still got '%s' to resolve",pAuxType->identifier().toUtf8().data(),szRemaining.toUtf8().data());

	return storeLockedResolveSimpleCppTypeToSymbolScope(
			pFile,
			szRemaining,
			dynamic_cast<C3SymbolScope *>(pAuxType),
			bMayBeNamespace
		);
}

C3SymbolScope * C3SymbolManager::storeLockedResolveGenericCppTypeToSymbolScope(
		C3SymbolFile * pFile,
		const QString & szUnfilteredType,
		C3SymbolScope * pTypeScope,
		bool bMayBeNamespace
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	if(szUnfilteredType.isEmpty())
		return NULL;

	SYMBOL_MANAGER_TRACE("Attempting to resolve unfiltered type '%s'",szUnfilteredType.toUtf8().data());

	QString szType = filterCppType(szUnfilteredType);

	SYMBOL_MANAGER_TRACE("GOT VAR TYPE %s (from %s)",szType.toUtf8().data(),szUnfilteredType.toUtf8().data());
	
	if(szType.isEmpty())
		return NULL;

	return storeLockedResolveSimpleCppTypeToSymbolScope(pFile,szType,pTypeScope,bMayBeNamespace);
}

C3SymbolScope * C3SymbolManager::storeLockedResolveTypeOfSymbol(
		C3SymbolFile * pFile,
		C3SymbolScope * pFileContext,
		C3Symbol * pSymbol
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	SYMBOL_MANAGER_TRACE("Resolve type of symbol '%s'",pSymbol->identifier().toUtf8().data());

	if(pSymbol->type() == C3Symbol::EnumerationMember)
	{
		SYMBOL_MANAGER_TRACE("Symbol is enumeration member from scope %s",pSymbol->resolvedContainingScope()->identifier().toUtf8().data());
		return pSymbol->resolvedContainingScope();
	}

	QString szType;

	C3SymbolScope * pTryThisScopeFirst = NULL;

	if(pSymbol->type() & (C3Symbol::LocalVariable | C3Symbol::GlobalVariable | C3Symbol::MemberVariable))
	{
		C3SymbolVariable * pVar = dynamic_cast<C3SymbolVariable *>(pSymbol);
		if(!pVar)
		{
			SYMBOL_MANAGER_TRACE("Not found");
			return NULL;
		}
		szType = pVar->typeName();
		
		if(pSymbol->type() == C3Symbol::MemberVariable)
			pTryThisScopeFirst = pVar->resolvedContainingScope();;
	}
	
	if(pSymbol->type() & (C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition))
	{
		C3SymbolFunctionSignature * pSig = dynamic_cast<C3SymbolFunctionSignature *>(pSymbol);
		if(!pSig)
		{
			SYMBOL_MANAGER_TRACE("Not found 2");
			return NULL;
		}
		szType = pSig->returnType();

		pTryThisScopeFirst = pSymbol->resolvedContainingScope(); // container of the function
	}

	if(szType.isEmpty())
	{
		SYMBOL_MANAGER_TRACE("Type turned out to be empty");
		return NULL;
	}

	SYMBOL_MANAGER_TRACE("Need to resolve type '%s'",szType.toUtf8().data());

	if(pTryThisScopeFirst)
	{
		C3SymbolScope * pScope = storeLockedResolveGenericCppTypeToSymbolScope(
				pFile,
				szType,
				pTryThisScopeFirst,
				false
			);
		
		if(pScope)
		{
			SYMBOL_MANAGER_TRACE("Resolved to scope '%s'",pScope->identifier().toUtf8().data());
			return pScope;
		}
	}

	return storeLockedResolveGenericCppTypeToSymbolScope(
			pFile,
			szType,
			pFileContext,
			false
		);
}

C3SymbolScope * C3SymbolManager::resolveP(
		C3SymbolFile * pFile,
		C3SymbolScope * pFileContext,
		C3SymbolScope * pGlobalScope
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	// First try via file context.
	if((!pFileContext) || (pFileContext->type() != C3Symbol::FunctionDefinition))
	{
		SYMBOL_MANAGER_TRACE("_p cannot be resolved outside of a function");
		return NULL;
	}

	C3SymbolScope * pTypeScope = pFileContext->resolvedContainingScope();

	// N.B. Containing scope is never a typedef
	while(pTypeScope && (pTypeScope->type() == C3Symbol::FunctionDefinition))
		// lambda
		pTypeScope = pTypeScope->resolvedContainingScope();
	
	if(!pTypeScope)
	{
		SYMBOL_MANAGER_TRACE("Function definition has no resolved containing scope");
		return NULL;
	}

	QString sPrivate = QStringLiteral("%1Private").arg(pTypeScope->identifier());

	C3SymbolScope * pPrivate = dynamic_cast<C3SymbolScope *>(pGlobalScope->findSymbol(sPrivate,C3Symbol::Class));

	if(pPrivate)
		return pPrivate;

	// Not found: either no file context or _p is defined to something else (or just not found)

	SYMBOL_MANAGER_TRACE("_p is not defined as %s",sPrivate.toUtf8().data());

#if 0
	// It would be nice to have the whole #define line to resolve _p directly.. but we don't have it here!

	if(!pFile)
	{
		SYMBOL_MANAGER_TRACE("Aargh.. there is no file");
		return NULL;
	}

	// ordered by line
	Q_FOREACH(C3Symbol * sym,pFile->symbols())
	{
		if((sym->type() == C3Symbol::Macro) && (sym->identifier() == m_szP))
		{
			// gotcha!
			
			break;
		}
	
		if(sym->lineNumber() > 100)
		{
			SYMBOL_MANAGER_TRACE("No _p defined in the first few lines of file");
			return NULL;
		}
	}
#endif

	// Try to look for SomethingElsePrivate * m_... instead

	static QLatin1String sPrivate2("Private *");
	
	if(pTypeScope->symbols())
	{
		Q_FOREACH(C3Symbol * pSym,*(pTypeScope->symbols()))
		{
			if(pSym->type() != C3Symbol::MemberVariable)
				continue;

			if(!static_cast<C3SymbolVariable *>(pSym)->typeName().endsWith(sPrivate2))
				continue;

			QString sType = filterCppType(static_cast<C3SymbolVariable *>(pSym)->typeName());
			pPrivate = dynamic_cast<C3SymbolScope *>(pGlobalScope->findSymbol(sType,C3Symbol::Class));
			if(pPrivate)
			{
				SYMBOL_MANAGER_TRACE("Mapped _p to %s",sType.toUtf8().data());
				return pPrivate;
			}
		}
	}
	
	return NULL;
}

C3SymbolScope * C3SymbolManager::storeLockedResolveCppArrowOrDotTypeScope(
		C3SymbolFile * pFile,
		C3TextEditorSymbolContext * pContext,
		C3SymbolScope * pFileContext,
		C3SymbolScope * pGlobalScope
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;
	
	Q_ASSERT(pContext->lLeftContext.count() >= 1);
	Q_ASSERT(pGlobalScope);

	// FIXME: Optimize the "int", "unsigned int" and other simple cases
	
	// Handle things like dynamic_cast<X *>(var)->

	// Find the leftmost context that has arrow/dot as operator
	
	int c = pContext->lLeftContext.count();

	C3TextEditorSymbolContext::AdditionalContext * pCtx = pContext->lLeftContext.at(0);
	
	int iIdx = 1;
	while(iIdx < c)
	{
		C3TextEditorSymbolContext::AdditionalContext * pCtxOnLeft = pContext->lLeftContext.at(iIdx);

		if(
				pCtxOnLeft->szOperator.isEmpty() ||
				(
					(pCtxOnLeft->szOperator != m_szParenthesisArrow) &&
					(pCtxOnLeft->szOperator != m_szParenthesisDot) &&
					(pCtxOnLeft->szOperator != m_szArrow) &&
					(pCtxOnLeft->szOperator != m_szDot) &&
					(pCtxOnLeft->szOperator != m_szSquareArrow) &&
					(pCtxOnLeft->szOperator != m_szSquareDot) &&
					(pCtxOnLeft->szOperator != m_szTwoColons)
				)
			)
			break;

		pCtx = pCtxOnLeft;

		iIdx++;
	}

	iIdx--; // point to pCtx index

	C3SymbolScope * pTypeScope;
	
	bool bDoAdditionalLookups = false;

	if(pCtx->szText.isEmpty())
	{
		if(pCtx->szOperator != m_szTwoColons)
		{
			SYMBOL_MANAGER_TRACE("Initial context has empty text and operator %s, which is not ::",pCtx->szOperator.toUtf8().data());
			return NULL;
		}

		SYMBOL_MANAGER_TRACE("Initial symbol is empty: assuming global scope");

		// ::Something

		pTypeScope = pGlobalScope;

	} else {
		// Something...

		SYMBOL_MANAGER_TRACE("Look for initial symbol '%s'",pCtx->szText.toUtf8().data());

		if(pCtx->szText == m_szThis)
		{
			if(!pFileContext)
			{
				SYMBOL_MANAGER_TRACE("Symbol is 'this' but there is no file context");
				return NULL;
			}
		
			if(pFileContext->type() != C3Symbol::FunctionDefinition)
			{
				SYMBOL_MANAGER_TRACE("Symbol is 'this' but the file context is not a function definition");
				return NULL;
			}
		
			pTypeScope = pFileContext->resolvedContainingScope();

			// N.B. Containing scope is never a typedef
			while(pTypeScope && (pTypeScope->type() == C3Symbol::FunctionDefinition))
				// lambda
				pTypeScope = pTypeScope->resolvedContainingScope();

		} else if(pCtx->szText == m_szP)
		{
			// magic _p macro :)
			// look for <local class>Private class definition

			pTypeScope = resolveP(pFile,pFileContext,pGlobalScope);

		} else {

			SYMBOL_MANAGER_TRACE("Initial symbol is not 'this' nor '_p'");

			pTypeScope = pFileContext ? pFileContext : pGlobalScope;

			bDoAdditionalLookups = true;
			
			iIdx++; // need to resolve THIS variable. Will iIdx-- inside the while.
		}
	}

	if(!pTypeScope)
	{
		SYMBOL_MANAGER_TRACE("Have no root to start from");
		return NULL;
	}

	QString szLastTypeScopeName;

	while(iIdx > 0)
	{
		iIdx--;
		pCtx = pContext->lLeftContext.at(iIdx);

		SYMBOL_MANAGER_TRACE(
				"Lookup %s in %s with operator %s (additional lookups=%d)",
				pCtx->szText.toUtf8().data(),
				pTypeScope->identifier().toUtf8().data(),
				pCtx->szOperator.toUtf8().data(),
				bDoAdditionalLookups
			);

		quint16 uTypeMask;

		if((pCtx->szOperator == m_szParenthesisArrow) || (pCtx->szOperator == m_szParenthesisDot))
		{
			// function call

			uTypeMask = C3Symbol::FunctionDefinition | C3Symbol::FunctionPrototype;
			
			C3SymbolFunctionSignature * pFun;
			
			if(bDoAdditionalLookups)
				pFun = dynamic_cast<C3SymbolFunctionSignature *>(
						pTypeScope->findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(
								pCtx->szText,
								uTypeMask,
								pGlobalScope
							)
					);
			else
				pFun = dynamic_cast<C3SymbolFunctionSignature *>(
						pTypeScope->findSymbolInThisAndBaseClasses(
								pCtx->szText,
								uTypeMask
							)
					);
	
			if(!pFun)
			{
				SYMBOL_MANAGER_TRACE("No function definition/prototype found");
				return NULL;
			}

			pTypeScope = storeLockedResolveGenericCppTypeToSymbolScope(pFile,pFun->returnType(),pTypeScope);
			if(pTypeScope)
				szLastTypeScopeName = pFun->returnType();

		} else if(pCtx->szOperator == m_szTwoColons)
		{
			// class name / namespace
			
			uTypeMask = C3Symbol::Class | C3Symbol::Union | C3Symbol::Namespace;

			if(bDoAdditionalLookups)
				pTypeScope = dynamic_cast<C3SymbolScope *>(
						pTypeScope->findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(
								pCtx->szText,
								uTypeMask,
								pGlobalScope
							)
					);
			else
				pTypeScope = dynamic_cast<C3SymbolScope *>(
						pTypeScope->findSymbolInThisAndBaseClasses(
								pCtx->szText,
								uTypeMask
							)
					);

			if(pTypeScope)
				szLastTypeScopeName = pCtx->szText;

		} else {
			// variable
			
			uTypeMask = C3Symbol::LocalVariable | C3Symbol::MemberVariable | C3Symbol::GlobalVariable;
			
			C3SymbolVariable * pVar;

			if(bDoAdditionalLookups)
				pVar = dynamic_cast<C3SymbolVariable *>(
						pTypeScope->findSymbolInThisScopeContainingScopesBaseClassesAndGlobalScope(
								pCtx->szText,
								uTypeMask,
								pGlobalScope
							)
					);
			else
				pVar = dynamic_cast<C3SymbolVariable *>(
						pTypeScope->findSymbolInThisAndBaseClasses(
								pCtx->szText,
								uTypeMask
							)
					);


			if(!pVar)
			{
				SYMBOL_MANAGER_TRACE("Variable not found");
				return NULL; // don't know.
			}

			SYMBOL_MANAGER_TRACE("Found variable %s with declared type %s",pVar->identifier().toUtf8().data(),pVar->typeName().toUtf8().data());

			pTypeScope = storeLockedResolveGenericCppTypeToSymbolScope(pFile,pVar->typeName(),pTypeScope,false);

			if(pTypeScope)
				szLastTypeScopeName = pVar->typeName();
		}

		if(!pTypeScope)
		{
			if(bDoAdditionalLookups)
			{
				QList<C3SymbolScope *> & lAdditionalSymbolLookupScopes = pFile->additionalSymbolLookupScopes();
		
				Q_FOREACH(C3SymbolScope * pAdditionalScope,lAdditionalSymbolLookupScopes)
				{
					pTypeScope = dynamic_cast<C3SymbolScope *>(
							pAdditionalScope->findSymbolInThisAndBaseClasses(
									pCtx->szText,
									uTypeMask
								)
						);
					if(pTypeScope)
						break;
				}
			}

			if(!pTypeScope)
			{
				SYMBOL_MANAGER_TRACE("Type scope not found");
				return NULL; // don't know
			}
		}

		bDoAdditionalLookups = false;
	
		if(pCtx->szOperator == m_szArrow)
		{
			// look for operator ->
			C3SymbolFunctionSignature * pOperatorArrow = dynamic_cast<C3SymbolFunctionSignature *>(
					pTypeScope->findSymbolInThisAndBaseClasses("operator ->",C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition)
				);

			if(pOperatorArrow)
			{
				SYMBOL_MANAGER_TRACE("Found operator -> with return type %s",pOperatorArrow->returnType().toUtf8().data());
				C3SymbolScope * pReturnScope = storeLockedResolveGenericCppTypeToSymbolScope(pFile,pOperatorArrow->returnType(),pTypeScope);
				if(pReturnScope)
				{
					SYMBOL_MANAGER_TRACE("Found operator -> return scope!");
					pTypeScope = pReturnScope;
				} else {
					SYMBOL_MANAGER_TRACE("Failed to find operator -> return scope");
					// This is very often a template arg
					if(!szLastTypeScopeName.isEmpty())
					{
						// try template args (FIXME: This is a gross guess!)
						// but... FIXME: This works! It should be ported to other parts of this functions: vars, function calls etc...
						QStringList lTemplateArgs;
						filterCppType(szLastTypeScopeName,&lTemplateArgs);
						if(lTemplateArgs.count() > 0)
						{
							foreach(QString szTemplateArg,lTemplateArgs)
							{
								pReturnScope = storeLockedResolveGenericCppTypeToSymbolScope(pFile,szTemplateArg,pFileContext);
								if(pReturnScope)
								{
									pTypeScope = pReturnScope;
									szLastTypeScopeName = szTemplateArg;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return pTypeScope;
}

void C3SymbolManager::storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInScope(
		C3SymbolFile * pFile,
		C3TextEditorSymbolContext * pContext,
		C3SymbolScope * pScope,
		C3SymbolScope * pGlobalScope,
		std::function<void(quint32,C3SymbolMap *,const QString &,quint16,unsigned int)> fnSymbolCallback,
		quint32 uTypeMask,
		unsigned int uBaseFlags
	)
{
	while(pScope && (pScope != pGlobalScope))
	{
		switch(pScope->type())
		{
			case C3Symbol::FunctionDefinition:
				// local variables
				fnSymbolCallback(
						uTypeMask & (C3Symbol::LocalVariable | C3Symbol::TemplateParameter),
						dynamic_cast<C3SymbolScope *>(pScope)->symbols(),
						pContext->szText,
						3, // score
						uBaseFlags
					);
			break;
			case C3Symbol::Class:
			case C3Symbol::Namespace:
				fnSymbolCallback(
						uTypeMask & 
							(C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition |
							C3Symbol::Namespace | C3Symbol::MemberVariable | C3Symbol::TemplateParameter |
							C3Symbol::FunctionDefinition | C3Symbol::LocalVariable |
							C3Symbol::FunctionPrototype | C3Symbol::Enumeration | C3Symbol::EnumerationMember),
						pScope->symbols(),
						pContext->szText,
						2, // score
						uBaseFlags
					);
		
				if(pScope->type() == C3Symbol::Class)
				{
					// loop thru base classes too
					QList<C3SymbolClass *> * pBaseClasses = dynamic_cast<C3SymbolClass *>(pScope)->baseClasses();
					if(pBaseClasses)
						storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInBaseClasses(
								pBaseClasses,
								pContext,
								fnSymbolCallback,
								uTypeMask & // no template parameters here
									(C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition |
									C3Symbol::Namespace | C3Symbol::MemberVariable | C3Symbol::FunctionDefinition |
									C3Symbol::FunctionPrototype | C3Symbol::Enumeration | C3Symbol::EnumerationMember),
								1, // score
								uBaseFlags
							);
				}
			break;
			default:
				pScope = NULL; // break out
			break;
		}

		// FIXME: should probably filter out overrides?
		if(pScope)
			pScope = pScope->resolvedContainingScope();
	}
	
	fnSymbolCallback(
			uTypeMask &
				(C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition |
				C3Symbol::Namespace | C3Symbol::GlobalVariable | C3Symbol::FunctionDefinition |
				C3Symbol::FunctionPrototype | C3Symbol::Macro | C3Symbol::Enumeration | C3Symbol::EnumerationMember),
			pGlobalScope->symbols(),
			pContext->szText,
			0, // score
			uBaseFlags
		);

	QList<C3SymbolScope *> & lAdditionalSymbolLookupScopes = pFile->additionalSymbolLookupScopes();

	if(lAdditionalSymbolLookupScopes.count() > 0)
	{
		Q_FOREACH(C3SymbolScope * pAdditionalScope,lAdditionalSymbolLookupScopes)
		{
			//qDebug("Additional lookup scope [%s]",pAdditionalScope->description().toUtf8().data());
			fnSymbolCallback(
					uTypeMask &
						(C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition |
						C3Symbol::Namespace | C3Symbol::GlobalVariable | C3Symbol::FunctionDefinition |
						C3Symbol::FunctionPrototype | C3Symbol::Macro | C3Symbol::Enumeration | C3Symbol::EnumerationMember),
					pAdditionalScope->symbols(),
					pContext->szText,
					0, // score
					uBaseFlags
				);
		}
	}

}


void C3SymbolManager::storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInBaseClasses(
		QList<C3SymbolClass *> * pBaseClasses,
		C3TextEditorSymbolContext * pContext,
		std::function<void(quint32,C3SymbolMap *,const QString &,quint16,unsigned int)> fnSymbolCallback,
		quint32 uTypeMask,
		quint16 uScore,
		unsigned int uFlags
	)
{
	Q_ASSERT(pBaseClasses);
		
	Q_FOREACH(C3SymbolClass * pBase,*pBaseClasses)
	{
		Q_ASSERT(pBase);

		fnSymbolCallback(
				uTypeMask,
				pBase->symbols(),
				pContext->szText,
				uScore,
				uFlags
			);

		QList<C3SymbolClass *> * pBaseClasses2 = pBase->baseClasses();
		if(pBaseClasses2)
			storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInBaseClasses(pBaseClasses2,pContext,fnSymbolCallback,uTypeMask,uScore,uFlags);
	}
}


bool C3SymbolManager::storeLockedSelectSymbolsForCpp(
		C3SymbolFile * pFile,
		C3TextEditorSymbolContext * pContext,
		C3SymbolScope * pFileContext,
		C3SymbolScope * pGlobalScope,
		std::function<void(quint32,C3SymbolMap *,const QString &,quint16,unsigned int)> fnSymbolCallback,
		unsigned int uBaseFlags
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	// FIXME: enum X, struct X, union X

	// FIXME: complete language keywords here?
	
	// FIXME: handle case:

#ifdef DEBUG_SYMBOL_MANAGER
	SYMBOL_MANAGER_TRACE("CPP SYMBOL SELECTION");
	SYMBOL_MANAGER_TRACE("  TEXT IS: %s",pContext->szText.toUtf8().data());
	int ccc = pContext->lLeftContext.count();
	for(int iii=0;iii<ccc;iii++)
	{
		SYMBOL_MANAGER_TRACE("  CONTEXT: (%s) (%s)",pContext->lLeftContext.at(iii)->szText.toUtf8().data(),pContext->lLeftContext.at(iii)->szOperator.toUtf8().data());
	}
#endif

	C3TextEditorSymbolContext::AdditionalContext * pLeft = pContext->firstLeftContext();
	
	QList<C3SymbolScope *> & lAdditionalSymbolLookupScopes = pFile->additionalSymbolLookupScopes();

#define DO_GLOBAL_SYMBOL_LOOKUP(_uTypes,_szIdentifier,_uScore,_uFlags) \
		do \
		{ \
			fnSymbolCallback(_uTypes,pGlobalScope->symbols(),_szIdentifier,_uScore,_uFlags); \
			if(lAdditionalSymbolLookupScopes.count() > 0) \
			{ \
				Q_FOREACH(C3SymbolScope * pAdditionalScope,lAdditionalSymbolLookupScopes) \
				{ \
					fnSymbolCallback(_uTypes,pAdditionalScope->symbols(),_szIdentifier,_uScore,_uFlags); \
				} \
			} \
		} while(0)

	if(pLeft)
	{
		// FIXME: maybe use hashes to speed it up?
		if(pLeft->szOperator.isEmpty())
		{
			// no left operator
			if(pLeft->szText == m_szNew)
			{
				// we complete constructors too...
				storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInScope(
						pFile,
						pContext,
						pFileContext,
						pGlobalScope,
						fnSymbolCallback,
						C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition |
							C3Symbol::Macro | C3Symbol::TemplateParameter,
						uBaseFlags | MatchConstructors
					);

				return true;
			}

			if(
					(pLeft->szText == m_szConst) ||
					(pLeft->szText == m_szStatic) ||
					(pLeft->szText == m_szInline)
				)
			{
				storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInScope(
						pFile,
						pContext,
						pFileContext,
						pGlobalScope,
						fnSymbolCallback,
						C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition | C3Symbol::Macro | C3Symbol::TemplateParameter,
						uBaseFlags
					);

				return true;
			}
			
			if(pLeft->szText == m_szDelete)
			{
				DO_GLOBAL_SYMBOL_LOOKUP(
						C3Symbol::GlobalVariable,
						pContext->szText,
						1, // score
						uBaseFlags
					);
	
				DO_GLOBAL_SYMBOL_LOOKUP(
						C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition,
						pContext->szText,
						0, // score
						uBaseFlags
					);
	
				if(pFileContext && (pFileContext->type() == C3Symbol::FunctionDefinition))
				{
					fnSymbolCallback(
							C3Symbol::LocalVariable,
							dynamic_cast<C3SymbolScope *>(pFileContext)->symbols(),
							pContext->szText,
							2, // score
							uBaseFlags
						);
	
					if(pFileContext->resolvedContainingScope())
					{
						fnSymbolCallback(
								C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition |
								C3Symbol::MemberVariable | C3Symbol::LocalVariable, // for lambdas functions may be nested
								pFileContext->resolvedContainingScope()->symbols(),
								pContext->szText,
								0, // score
								uBaseFlags
							);
					}
				}
	
				return true;
			}

			//if(pLeft->szText == m_szEmit)
			//{
				// FIXME: local class functions!
			//}

			goto complete_any_visible_symbol;
		}
		
		if(
				(pLeft->szOperator == m_szParenthesisArrow) ||
				(pLeft->szOperator == m_szParenthesisDot) ||
				(pLeft->szOperator == m_szArrow) ||
				(pLeft->szOperator == m_szDot) ||
				(pLeft->szOperator == m_szSquareArrow) ||
				(pLeft->szOperator == m_szSquareDot)
			)
		{
			C3SymbolScope * pTypeScope = storeLockedResolveCppArrowOrDotTypeScope(
					pFile,
					pContext,
					pFileContext,
					pGlobalScope
				);
	
			if(!pTypeScope)
				return false;
	
			SYMBOL_MANAGER_TRACE(
					"VAR SCOPE FOR OPERATOR [%s] IS [%s][%d symbols] [file:%s, line:%d]",
						pLeft->szOperator.toUtf8().data(),
						pTypeScope->description().toUtf8().data(),
						pTypeScope->symbols() ? pTypeScope->symbols()->count() : -1,
						pTypeScope->filePath().toUtf8().data(),
						pTypeScope->lineNumber()
				);
	
			// FIXME: Disable private members (unless we are in the class itself?)
			fnSymbolCallback(
					C3Symbol::MemberVariable | C3Symbol::FunctionDefinition | C3Symbol::FunctionPrototype,
					pTypeScope->symbols(),
					pContext->szText,
					1, // score
					uBaseFlags | AllowEmptyText // flags
				);

			if(pTypeScope->type() == C3Symbol::Class)
			{
				//qDebug("Base class names are '%s'",dynamic_cast<C3SymbolClass *>(pTypeScope)->baseClassNames().toUtf8().data());
				SYMBOL_MANAGER_TRACE("Type scope is a class");
	
				QList<C3SymbolClass *> * pBaseClasses = dynamic_cast<C3SymbolClass *>(pTypeScope)->baseClasses();
				if(pBaseClasses)
				{
					SYMBOL_MANAGER_TRACE("And there are %d base classes (%s)",pBaseClasses->count(),dynamic_cast<C3SymbolClass *>(pTypeScope)->baseClassNames().toUtf8().data());

					storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInBaseClasses(
							pBaseClasses,
							pContext,
							fnSymbolCallback,
							C3Symbol::MemberVariable | C3Symbol::FunctionDefinition | C3Symbol::FunctionPrototype,
							1,
							uBaseFlags | AllowEmptyText
						);
				}
			}
	
			return true;
		}
		
		if(pLeft->szOperator == m_szTwoColons)
		{
			if(pLeft->szText.isEmpty())
			{
				// jump to global scope
				DO_GLOBAL_SYMBOL_LOOKUP(
						C3Symbol::Class | C3Symbol::Union | C3Symbol::TypeDefinition |
						C3Symbol::Namespace | C3Symbol::GlobalVariable | C3Symbol::FunctionDefinition |
						C3Symbol::FunctionPrototype | C3Symbol::Enumeration | C3Symbol::EnumerationMember,
						pContext->szText,
						0, // score
						uBaseFlags
					);
				return true;
			}
			
			// probably a class name (FIXME: Might have more colon/class names on the left (deep namespace))
	
			if(pFileContext || pGlobalScope)
			{
				//C3SymbolScope * pVarScope = storeLockedResolveDoubleColonTypeScope(
				C3SymbolScope * pVarScope = storeLockedResolveCppArrowOrDotTypeScope(
						pFile,
						pContext,
						pFileContext,
						pGlobalScope
					);
		
				if(pVarScope)
				{
					SYMBOL_MANAGER_TRACE(
							"GOT :: VAR SCOPE [%s][%d symbols] [file:%s, line:%d]",
								pVarScope->description().toUtf8().data(),
								pVarScope->symbols() ? pVarScope->symbols()->count() : -1,
								pVarScope->filePath().toUtf8().data(),
								pVarScope->lineNumber()
						);

					fnSymbolCallback(
							C3Symbol::MemberVariable | C3Symbol::FunctionDefinition |
							C3Symbol::FunctionPrototype | C3Symbol::Namespace | C3Symbol::Class |
							C3Symbol::Union | C3Symbol::Enumeration | C3Symbol::EnumerationMember,
							pVarScope->symbols(),
							pContext->szText,
							1, // score
							uBaseFlags | AllowEmptyText
						);
	
					if(pVarScope->type() == C3Symbol::Class)
					{
						// FIXME: should probably filter out overrides?
	
						QList<C3SymbolClass *> * pBaseClasses = dynamic_cast<C3SymbolClass *>(pVarScope)->baseClasses();
						if(pBaseClasses)
							storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInBaseClasses(
									pBaseClasses,
									pContext,
									fnSymbolCallback,
									C3Symbol::MemberVariable | C3Symbol::FunctionDefinition |
									C3Symbol::FunctionPrototype | C3Symbol::Namespace | C3Symbol::Class |
									C3Symbol::Union | C3Symbol::Enumeration | C3Symbol::EnumerationMember,
									1,
									uBaseFlags | AllowEmptyText
								);
					}
				}

				return true;
			}
			
		}
	}

complete_any_visible_symbol:
	// Anything visible.

	storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInScope(
			pFile,
			pContext,
			pFileContext,
			pGlobalScope,
			fnSymbolCallback,
			0xffffffff,
			uBaseFlags
		);

	return true;
}

void C3SymbolManager::buildCPPKeywordList()
{
	_p->lCPPKeywords.clear();
	
	_p->lCPPKeywords.append(__literal("break"));
	_p->lCPPKeywords.append(__literal("catch"));
	_p->lCPPKeywords.append(__literal("class"));
	_p->lCPPKeywords.append(__literal("const"));
	_p->lCPPKeywords.append(__literal("continue"));
	_p->lCPPKeywords.append(__literal("delete"));
	_p->lCPPKeywords.append(__literal("do"));
	_p->lCPPKeywords.append(__literal("double"));
	_p->lCPPKeywords.append(__literal("dynamic_cast"));
	_p->lCPPKeywords.append(__literal("else"));
	_p->lCPPKeywords.append(__literal("enum"));
	_p->lCPPKeywords.append(__literal("false"));
	_p->lCPPKeywords.append(__literal("float"));
	_p->lCPPKeywords.append(__literal("for"));
	_p->lCPPKeywords.append(__literal("if"));
	_p->lCPPKeywords.append(__literal("inline"));
	_p->lCPPKeywords.append(__literal("int"));
	_p->lCPPKeywords.append(__literal("long"));
	_p->lCPPKeywords.append(__literal("mutable"));
	_p->lCPPKeywords.append(__literal("reinterpret_cast"));
	_p->lCPPKeywords.append(__literal("return"));
	_p->lCPPKeywords.append(__literal("short"));
	_p->lCPPKeywords.append(__literal("sizeof"));
	_p->lCPPKeywords.append(__literal("static_cast"));
	_p->lCPPKeywords.append(__literal("struct"));
	_p->lCPPKeywords.append(__literal("switch"));
	_p->lCPPKeywords.append(__literal("this")); // <-- fixme: we should check context for this?
	_p->lCPPKeywords.append(__literal("throw"));
	_p->lCPPKeywords.append(__literal("true"));
	_p->lCPPKeywords.append(__literal("try"));
	_p->lCPPKeywords.append(__literal("typeof"));
	_p->lCPPKeywords.append(__literal("typeid"));
	_p->lCPPKeywords.append(__literal("unsigned"));
	_p->lCPPKeywords.append(__literal("virtual"));
	_p->lCPPKeywords.append(__literal("void"));
	_p->lCPPKeywords.append(__literal("volatile"));
	_p->lCPPKeywords.append(__literal("while"));
	_p->lCPPKeywords.append(__literal("FALSE"));
	_p->lCPPKeywords.append(__literal("NULL"));
	_p->lCPPKeywords.append(__literal("TRUE"));
}

void C3SymbolManager::buildPHPKeywordList()
{
	_p->lPHPKeywords.clear();
	
	_p->lPHPKeywords.append(__literal("break;"));
	_p->lPHPKeywords.append(__literal("catch("));
	_p->lPHPKeywords.append(__literal("class "));
	_p->lPHPKeywords.append(__literal("continue;"));
	_p->lPHPKeywords.append(__literal("do "));
	_p->lPHPKeywords.append(__literal("false"));
	_p->lPHPKeywords.append(__literal("for("));
	_p->lPHPKeywords.append(__literal("function "));
	_p->lPHPKeywords.append(__literal("private "));
	_p->lPHPKeywords.append(__literal("protected "));
	_p->lPHPKeywords.append(__literal("public "));
	_p->lPHPKeywords.append(__literal("return"));
	_p->lPHPKeywords.append(__literal("switch("));
	_p->lPHPKeywords.append(__literal("throw"));
	_p->lPHPKeywords.append(__literal("true"));
	_p->lPHPKeywords.append(__literal("try"));
	_p->lPHPKeywords.append(__literal("while("));
	_p->lPHPKeywords.append(__literal("FALSE"));
	_p->lPHPKeywords.append(__literal("NULL"));
	_p->lPHPKeywords.append(__literal("TRUE"));
	_p->lPHPKeywords.append(__literal("$GLOBALS"));
	_p->lPHPKeywords.append(__literal("$_GET"));
	_p->lPHPKeywords.append(__literal("$_POST"));
	_p->lPHPKeywords.append(__literal("$_REQUEST"));
	_p->lPHPKeywords.append(__literal("$_SESSION"));
	_p->lPHPKeywords.append(__literal("$_SERVER"));
	_p->lPHPKeywords.append(__literal("$this"));
}

C3SymbolScope * C3SymbolManager::storeLockedResolveFileContext(
		const QString &szFileName,
		int uRow
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;
	
	SYMBOL_MANAGER_TRACE("Resolve %s:%d",szFileName.toUtf8().data(),uRow);

	C3SymbolFile * pFile = _p->pStore->file(szFileName);
	if(!pFile)
	{
		SYMBOL_MANAGER_TRACE("No such file");
		return NULL;
	}

	C3Symbol * pFileContextS = pFile->findContext(uRow);
	if(!pFileContextS)
	{
		SYMBOL_MANAGER_TRACE("No symbol at row %d",uRow);
		return NULL;
	}

	SYMBOL_MANAGER_TRACE("Symbol at row %d is %s",uRow,pFileContextS->description().toUtf8().data());

	if(!pFileContextS->isScope())
	{
		SYMBOL_MANAGER_TRACE("Symbol is not a scope");
		return NULL;
	}

	C3SymbolScope * pFileContext = dynamic_cast<C3SymbolScope *>(pFileContextS);
	if(!pFileContext)
	{
		SYMBOL_MANAGER_TRACE("Dynamic cast failed!");
		return NULL;
	}

	SYMBOL_MANAGER_TRACE(
			"FILECONTEXT[ %s %s in scope %s at %s:%d ]",
			pFileContext->typeString().toUtf8().data(),
			pFileContext->identifier().toUtf8().data(),
			pFileContext->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
			pFileContext->filePath().toUtf8().data(),
			pFileContext->lineNumber()
		);

	return pFileContext;
}

void C3SymbolManager::processPerformCompletionRequest(C3SymbolManagerRequestPerformCompletion * pRequest)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	// no need to lock here, as only we can set it to NULL
	if(!_p->pStore)
		return;

	QMutexLocker oLocker(&(_p->oStoreMutex));

	switch(pRequest->language())
	{
		case C3Symbol::CSharp:
		break;
		case C3Symbol::Cpp:
			if(_p->lCPPKeywords.isEmpty())
				buildCPPKeywordList();
		break;
		case C3Symbol::InvalidLanguage:
		break;
		case C3Symbol::Java:
		break;
		case C3Symbol::Javascript:
		break;
		case C3Symbol::Php:
			if(_p->lPHPKeywords.isEmpty())
				buildPHPKeywordList();
		break;
		case C3Symbol::Python:
		break;
		default:
		break;
	}

	C3TextEditorSymbolContext * pContext = pRequest->context();
	Q_ASSERT(pContext);

	SYMBOL_MANAGER_TRACE("Complete symbol '%s'",pContext->szText.toUtf8().data());

	bool bCompleted = false;
	
	unsigned int uCompletionFlags = 0;
	
	if(pRequest->flags() & C3TextEditorMode::SymbolCompletionMatchCaseInsensitive)
		uCompletionFlags |= MatchCaseInsensitive;
	if(pRequest->flags() & C3TextEditorMode::SymbolCompletionMatchInTheMiddle)
		uCompletionFlags |= MatchInTheMiddle;
	else if(pRequest->flags() & C3TextEditorMode::SymbolCompletionMatchInTheMiddleIfLongerThanOne)
	{
		if(pContext->szText.length() > 1)
			uCompletionFlags |= MatchInTheMiddle;
	}

	QList<C3TextEditorCompletion *> * pCompletions = new QList<C3TextEditorCompletion *>();

	if(!(pRequest->flags() & C3TextEditorMode::SymbolCompletionNoContextFilter))
	{
		C3SymbolScope * pFileContext = storeLockedResolveFileContext(
				pRequest->filePath(),
				pContext->oRange.end.row
			);

		C3SymbolFile * pFile = _p->pStore->file(pRequest->filePath());
		C3SymbolScope * pGlobalScope = _p->pStore->globalScopeForLanguage(pRequest->language());
		if(!pGlobalScope)
		{
			delete pCompletions;
			return; // not ready yet?
		}
		
		// if pointing exactly at the symbol that defines the context, jump one level up
		if(pFileContext && (pFileContext->identifier() == pContext->szText) && pFileContext->resolvedContainingScope())
		{
			SYMBOL_MANAGER_TRACE("File context is exactly the symbol '%s': jumping one level up",pContext->szText.toUtf8().data());
			// this will allow finding exactly this instance and also the other ones defines around (it probably makes sense)
			pFileContext = pFileContext->resolvedContainingScope();
		}

		switch(pRequest->language())
		{
			case C3Symbol::Cpp:
				if(!pContext->szText.isEmpty())
				{
					addKeywordCompletions(
							pCompletions,
							_p->lCPPKeywords,
							pContext->szText,
							0,
							uCompletionFlags
						);
				}
			
				bCompleted = storeLockedSelectSymbolsForCpp(
						pFile ? pFile : _p->pStore->rootFile(),
						pRequest->context(),
						pFileContext,
						pGlobalScope,
						[this,pCompletions](quint32 uTypeMask,C3SymbolMap *pSymbols,const QString &szToComplete,quint16 uScore,unsigned int uFlags)
						{
							storeLockedAddCompletions(
									pCompletions,
									uTypeMask,
									pSymbols,
									szToComplete,
									uScore,
									uFlags
								);
						},
						uCompletionFlags
					);
			break;
			case C3Symbol::Php:
				if(!pContext->szText.isEmpty())
					addKeywordCompletions(
							pCompletions,
							_p->lPHPKeywords,
							pContext->szText,
							0,
							uCompletionFlags
						);

				bCompleted = false;
			break;
			default:
				bCompleted = false;
			break;
		}
	}

	// Flat completion if not completed is problematic: we usually WANT the context filter to work and show
	// that there is no match at all for the text that the user has entered.

	if(!bCompleted)
	//if(pCompletions->isEmpty())
	{
		// do flat completion
		
		// FIXME: This does NOT work for "match in the middle" completions...

		SYMBOL_MANAGER_TRACE("Not completed yet: do flat completion");
		
		C3SymbolMap * pAllSymbols = _p->pStore->allSymbolsForFirstLetter(pContext->szText,pRequest->language());
		if(pAllSymbols)
		{
			storeLockedAddCompletions(
					pCompletions,
					0xffffffff,
					pAllSymbols,
					pContext->szText,
					0,
					uCompletionFlags
				);
		}
	}

	SYMBOL_MANAGER_TRACE("Got %d completions",pCompletions->count());

	if(pCompletions->isEmpty())
	{
		delete pCompletions;
		return;
	}

	// FIXME: Sort them!
	pRequest->setCompletions(pCompletions);
}


static inline void insert_completion(
		QList<C3TextEditorCompletion *> * pCompletions,
		C3TextEditorCompletion * pCompletion
	)
{
	if(pCompletions->isEmpty())
	{
		pCompletions->append(pCompletion);
		return;
	}

	QList<C3TextEditorCompletion *>::iterator end = pCompletions->end();
	QList<C3TextEditorCompletion *>::iterator it = qLowerBound(pCompletions->begin(),end,pCompletion,C3TextEditorCompletion::keyLessThan);

	if(it == end)
	{
		pCompletions->append(pCompletion);
		return;
	}

	C3TextEditorCompletion * pOther = *it;

	// FIXME: This might be problematic if we decide to handle parameter insertions and overloads
	if(
			(pOther->eType == pCompletion->eType) &&
			//(pOther->szTrailer == szTrailer) &&
			//(pOther->szSuffix == szSuffix) &&
			(pOther->szKey == pCompletion->szKey) // &&
			//(pOther->szDescription == pCompletion->szDescription)
		)
	{
		// same symbol: avoid duplicating
		if(pOther->uScore < pCompletion->uScore)
			pOther->uScore = pCompletion->uScore;
		delete pCompletion;
		return;
	}
	
	pCompletions->insert(it,pCompletion);
}

void C3SymbolManager::addKeywordCompletions(
		QList<C3TextEditorCompletion *> * pCompletions,
		const QStringList &lKeywords,
		const QString &szToComplete,
		quint16 uScore,
		unsigned int uFlags
	)
{
	// For now keywords never match with empty text nor are matched in the middle.

	if(szToComplete.isEmpty())
		return;

	if(_p->pCurrentlyProcessedRequest->aborted())
	{
#ifdef DEBUG_SYMBOL_MANAGER
		qDebug("Request is aborted: giving up adding completions");
#endif
		return;
	}

	QStringList::const_iterator it = qLowerBound(lKeywords,szToComplete);
	QStringList::const_iterator end = lKeywords.end();

	while(it != end)
	{
		QString szText = *it;
		++it;
	
		if(!szText.startsWith(szToComplete))
		{
			if(szText > szToComplete)
				break;
			continue;
		}

		C3TextEditorCompletion * pCompletion = new C3TextEditorCompletion(
				C3TextEditorCompletion::Keyword,
				szText,
				QString(),
				uScore,
				szText,
				QString(),
				0,
				szText
			);

		insert_completion(pCompletions,pCompletion);
	}
}

void C3SymbolManager::storeLockedAddCompletions(
		QList<C3TextEditorCompletion *> * pCompletions,
		quint32 uTypeMask,
		C3SymbolMap * pSymbols,
		const QString &szToComplete,
		quint16 uScore,
		unsigned int uFlags
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	if(!pSymbols)
		return;

	if(_p->pCurrentlyProcessedRequest->aborted())
	{
#ifdef DEBUG_SYMBOL_MANAGER
		qDebug("Request is aborted: giving up adding completions");
#endif
		return;
	}

	C3SymbolMap::iterator end = pSymbols->end();
	C3SymbolMap::iterator it;

	bool bEmpty = szToComplete.isEmpty();
	bool bMatchInTheMiddle = uFlags & MatchInTheMiddle;
	Qt::CaseSensitivity eCaseSensitivity = (uFlags & MatchCaseInsensitive) ? Qt::CaseInsensitive : Qt::CaseSensitive;

	if(bMatchInTheMiddle)
	{
		if(bEmpty)
		{
			if(!(uFlags & AllowEmptyText))
				return;
		}

		it = pSymbols->begin();
	} else {
		if(bEmpty)
		{
			if(!(uFlags & AllowEmptyText))
				return;
			it = pSymbols->begin();
		} else {
			it = pSymbols->lowerBound(szToComplete);
		}
	}

	// We try to filter function prototype/definition pairs
	QHash<QString,C3Symbol *> hFunctions;

	int cnt = 0;

	while(it != end)
	{
		C3Symbol * pSym = *it;
		++it;
	
		cnt++;
		if((cnt % 10) == 0)
		{
			if(_p->pCurrentlyProcessedRequest->aborted())
			{
#ifdef DEBUG_SYMBOL_MANAGER
				qDebug("Request is aborted: skipping");
#endif
				return;
			}
		}
	
		if(!(pSym->type() & uTypeMask))
			continue;

		if(bMatchInTheMiddle)
		{
			if((!bEmpty) && !pSym->identifier().contains(szToComplete,eCaseSensitivity))
			{
				continue;
			}
		} else {
			if((!bEmpty) && !pSym->identifier().startsWith(szToComplete))
			{
				if(pSym->identifier() > szToComplete)
					break;
				continue;
			}
		}

		C3TextEditorCompletion::Type eType;

		switch(pSym->type())
		{
			case C3Symbol::Class:
				eType = C3TextEditorCompletion::Class;
			break;
			case C3Symbol::Namespace:
				eType = C3TextEditorCompletion::Namespace;
			break;
			case C3Symbol::Union:
				eType = C3TextEditorCompletion::Union;
			break;
			case C3Symbol::FunctionDefinition:
			{
				// FIXME: Signatures here have default argumens which break this algorithm!
				QString szId = pSym->identifier() + dynamic_cast<C3SymbolFunctionDefinition *>(pSym)->signature();
				if(!hFunctions.contains(szId))
					hFunctions.insert(szId,pSym);
				//else
				//	qDebug("Function definition discarded");
				continue;
			}
			break;
			case C3Symbol::FunctionPrototype:
			{
				// prototypes always overwrite definitions
				// FIXME: Signatures here have default argumens which break this algorithm!
				hFunctions.insert(pSym->identifier() + dynamic_cast<C3SymbolFunctionPrototype *>(pSym)->signature(),pSym);
				continue;
			}
			break;
			case C3Symbol::Enumeration:
				eType = C3TextEditorCompletion::Enumeration;
			break;
			case C3Symbol::EnumerationMember:
				eType = C3TextEditorCompletion::EnumerationMember;
			break;
			case C3Symbol::LocalVariable:
				eType = C3TextEditorCompletion::LocalVariable;
			break;
			case C3Symbol::GlobalVariable:
				eType = C3TextEditorCompletion::GlobalVariable;
			break;
			case C3Symbol::MemberVariable:
				eType = C3TextEditorCompletion::MemberVariable;
			break;
			case C3Symbol::TypeDefinition:
				eType = C3TextEditorCompletion::TypeDefinition;
			break;
			case C3Symbol::Macro:
				eType = C3TextEditorCompletion::Macro;
			break;
			default:
				eType = C3TextEditorCompletion::OtherSymbol;
			break;
		}

		SYMBOL_MANAGER_TRACE(
				"COMPLETION[ %s %s in scope %s at %s:%d basescore %d ]",
				pSym->typeString().toUtf8().data(),
				pSym->identifier().toUtf8().data(),
				pSym->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
				pSym->filePath().toUtf8().data(),
				pSym->lineNumber(),
				uScore
			);


		C3TextEditorCompletion * pCompletion = new C3TextEditorCompletion(
				eType,
				pSym->identifier(),
				QString(),
				uScore, // score
				pSym->description(),
				pSym->filePath(),
				pSym->lineNumber(),
				pSym->completionKey()
			);

		if(pCompletions->isEmpty())
		{
			pCompletions->append(pCompletion);
			continue;
		}

		QList<C3TextEditorCompletion *>::iterator end = pCompletions->end();
		QList<C3TextEditorCompletion *>::iterator it = qLowerBound(pCompletions->begin(),end,pCompletion,C3TextEditorCompletion::keyLessThan);

		if(it == end)
		{
			pCompletions->append(pCompletion);
			continue;
		}

		C3TextEditorCompletion * pOther = *it;
		if(
				(pOther->eType == eType) &&
				//(pOther->szTrailer == szTrailer) &&
				//(pOther->szSuffix == szSuffix) &&
				(pOther->szKey == pCompletion->szKey) // &&
				//(pOther->szDescription == pCompletion->szDescription)
			)
		{
			// same symbol: avoid duplicating
			if(pOther->uScore < uScore)
				pOther->uScore = uScore;
			delete pCompletion;
			continue;
		}
		
		pCompletions->insert(it,pCompletion);
	}

	QHash<QString,C3Symbol *>::iterator hit;
	
	//qDebug("Scanning %d functions",hFunctions.count());
	
	for(hit = hFunctions.begin();hit != hFunctions.end();++hit)
	{
		C3Symbol * pSym = hit.value();
		
		QString szTrailer;
		
		SYMBOL_MANAGER_TRACE(
				"FUNCOMPLETION[ %s %s in scope %s at %s:%d basescore %d ]",
				pSym->typeString().toUtf8().data(),
				pSym->identifier().toUtf8().data(),
				pSym->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
				pSym->filePath().toUtf8().data(),
				pSym->lineNumber(),
				uScore
			);


		switch(pSym->type())
		{
			case C3Symbol::FunctionDefinition:
				if(!dynamic_cast<C3SymbolFunctionDefinition *>(pSym)->returnType().isEmpty()) // filters out constructors
					szTrailer = dynamic_cast<C3SymbolFunctionDefinition *>(pSym)->signature().isEmpty() ? m_szOpenAndClosedParenthesis : m_szParenthesis;
			break;
			case C3Symbol::FunctionPrototype:
				if(!dynamic_cast<C3SymbolFunctionPrototype *>(pSym)->returnType().isEmpty()) // filters out constructors
					szTrailer = dynamic_cast<C3SymbolFunctionPrototype *>(pSym)->signature().isEmpty() ? m_szOpenAndClosedParenthesis : m_szParenthesis;
			break;
			default:
				// should not happen!
				Q_ASSERT(false);
				continue;
			break;
		}
		
		C3TextEditorCompletion * pCompletion = new C3TextEditorCompletion(
				C3TextEditorCompletion::Function,
				pSym->identifier(),
				szTrailer,
				uScore, // score
				pSym->description(),
				pSym->filePath(),
				pSym->lineNumber(),
				pSym->completionKey()
			);

		if(pCompletions->isEmpty())
		{
			pCompletions->append(pCompletion);
			continue;
		}

		QList<C3TextEditorCompletion *>::iterator end = pCompletions->end();
		QList<C3TextEditorCompletion *>::iterator it = qLowerBound(pCompletions->begin(),end,pCompletion,C3TextEditorCompletion::keyLessThan);

		if(it == end)
		{
			pCompletions->append(pCompletion);
			continue;
		}

		C3TextEditorCompletion * pOther = *it;
		if(
				(pOther->eType == C3TextEditorCompletion::Function) &&
				//(pOther->szTrailer == szTrailer) &&
				//(pOther->szSuffix == szSuffix) &&
				(pOther->szKey == pCompletion->szKey) // &&
				//(pOther->szDescription == pCompletion->szDescription)
			)
		{
			// same symbol: avoid duplicating
			if(pOther->uScore < uScore)
				pOther->uScore = uScore;
			delete pCompletion;
			continue;
		}
		
		pCompletions->insert(it,pCompletion);
	}
}

void C3SymbolManager::storeLockedIdentifySymbol(
		C3TextEditorSymbolContext * pContext,
		QList<C3TextEditorSymbolInfo * > * pInfoList,
		bool bAddSymbolPointers,
		bool bUseContextFilter,
		C3Symbol::Language eLanguage
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	bool bCompleted = false;

	SYMBOL_MANAGER_TRACE("Identify symbol '%s' [use context filter=%d]",pContext->szText.toUtf8().data(),bUseContextFilter);

	if(bUseContextFilter)
	{
		C3SymbolScope * pFileContext = storeLockedResolveFileContext(
				pContext->szFilePath,
				pContext->oRange.end.row
			);

		SYMBOL_MANAGER_TRACE("File context: %s",pFileContext ? pFileContext->description().toUtf8().data() : "unidentified");

		C3SymbolFile * pFile = _p->pStore->file(pContext->szFilePath);
	
		C3SymbolScope * pGlobalScope = _p->pStore->globalScopeForLanguage(eLanguage);
		if(!pGlobalScope)
		{
			// not ready yet?
			SYMBOL_MANAGER_TRACE("Global scope not ready yet");
			return;
		}
	
		// if pointing exactly at the symbol that defines the context, jump one level up
		if(pFileContext && (pFileContext->identifier() == pContext->szText) && pFileContext->resolvedContainingScope())
		{
			SYMBOL_MANAGER_TRACE("File context is exactly the symbol '%s': jumping one level up",pContext->szText.toUtf8().data());
			// this will allow finding exactly this instance and also the other ones defines around (it probably makes sense)
			pFileContext = pFileContext->resolvedContainingScope();
		}
	
		// FIXME: Identify locals only in their scope!
	
		switch(eLanguage)
		{
			case C3Symbol::Cpp:
				bCompleted = storeLockedSelectSymbolsForCpp(
						pFile ? pFile : _p->pStore->rootFile(),
						pContext,
						pFileContext,
						pGlobalScope,
						[this,pInfoList,pContext,bAddSymbolPointers](quint32 uTypeMask,C3SymbolMap *pSymbols,const QString &szToComplete,quint16 uScore,unsigned int uFlags)
						{
							if(uTypeMask & (C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition))
								uTypeMask |= (C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition); // always both prototypes and definitions

							storeLockedAddSymbolInfos(
									pInfoList,
									pContext->oRange,
									uTypeMask,
									pSymbols,
									szToComplete,
									uScore,
									uFlags,
									bAddSymbolPointers
								);
						},
						0 // base flags
					);
			break;
			default:
				bCompleted = false;
			break;
		}
	}

	//if(!bCompleted)
	if(pInfoList->isEmpty()) // identification switches automatically to flat completion if not completed at all
	{
		SYMBOL_MANAGER_TRACE("Not identified yet: doing flat completion");

		// do flat completion
		C3SymbolMap * pAllSymbols = _p->pStore->allSymbolsForFirstLetter(pContext->szText,eLanguage);
		if(pAllSymbols)
		{
			storeLockedAddSymbolInfos(
					pInfoList,
					pContext->oRange,
					0xffffffff,
					pAllSymbols,
					pContext->szText,
					0,
					0,
					bAddSymbolPointers
				);
		} else {
			SYMBOL_MANAGER_TRACE("No symbols for first letter");
		}
	}
}

void C3SymbolManager::processIdentifySymbolRequest(C3SymbolManagerRequestIdentifySymbol * pRequest)
{
	if(!_p->pStore)
		return;

	QMutexLocker oLocker(&(_p->oStoreMutex));

	C3TextEditorSymbolContext * pContext = pRequest->context();
	Q_ASSERT(pContext);

	QList<C3TextEditorSymbolInfo *> * pInfoList = new QList<C3TextEditorSymbolInfo *>();

	storeLockedIdentifySymbol(
			pContext,
			pInfoList,
			false,
			!(pRequest->flags() & C3SymbolManagerRequestIdentifySymbol::NoContextFilter),
			pRequest->language()
		);
	
	//qDebug("Got %d link infos",pInfoList->count());

	if(pInfoList->isEmpty())
	{
		delete pInfoList;
		return;
	}

	// FIXME: Sort them ?!
	pRequest->setInfoList(pInfoList);

}

void C3SymbolManager::lockStore()
{
	_p->oStoreMutex.lock();
}

void C3SymbolManager::unlockStore()
{
	_p->oStoreMutex.unlock();
}

void C3SymbolManager::storeLockedAddSymbolInfos(
		QList<C3TextEditorSymbolInfo *> * pInfoList,
		C3TextEditorRange &oRange,
		quint32 uTypeMask,
		C3SymbolMap * pSymbols,
		const QString &szIdentifier,
		quint16 uScore,
		unsigned int uFlags,
		bool bAddSymbolPointers
	)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	if(!pSymbols)
	{
		SYMBOL_MANAGER_TRACE("No symbols");
		return;
	}

	if(szIdentifier.isEmpty())
	{
		SYMBOL_MANAGER_TRACE("Identifier is empty!");
		return;
	}

	SYMBOL_MANAGER_TRACE("Identifier '%s', Type mask %x, Flags %x, Symbol Count %d",szIdentifier.toUtf8().data(),uTypeMask,uFlags,pSymbols->count());

	C3SymbolMap::iterator end = pSymbols->end();

	C3SymbolMap::Iterator it = pSymbols->lowerBound(szIdentifier);

	bool bMatchConstructors = uFlags & MatchConstructors;

	while(it != end)
	{
		C3Symbol * pSym = *it;
		++it;
	
		if(!(pSym->type() & uTypeMask))
			continue;

		if(pSym->identifier() != szIdentifier)
		{
#ifdef DEBUG_SYMBOL_MANAGER
			if(pSym->type() == C3Symbol::Namespace)
			{
				SYMBOL_MANAGER_TRACE(
						"incorrect identifier [ %s %s in scope %s at %s:%d (%d definitions)]",
						pSym->typeString().toUtf8().data(),
						pSym->identifier().toUtf8().data(),
						pSym->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
						pSym->filePath().toUtf8().data(),
						pSym->lineNumber(),
						1 + ((C3SymbolNamespace *)pSym)->additionalDefinitions().count()
					);
			} else {
				SYMBOL_MANAGER_TRACE(
						"incorrect identifier [ %s %s in scope %s at %s:%d ]",
						pSym->typeString().toUtf8().data(),
						pSym->identifier().toUtf8().data(),
						pSym->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
						pSym->filePath().toUtf8().data(),
						pSym->lineNumber()
					);
			}
#endif

			if(pSym->identifier() > szIdentifier)
			{
				SYMBOL_MANAGER_TRACE("Stopping at identifier %s",pSym->identifier().toUtf8().data());
				return;
			}
			continue;
		}

#ifdef DEBUG_SYMBOL_MANAGER
		if(pSym->type() == C3Symbol::Namespace)
		{
			SYMBOL_MANAGER_TRACE(
					"LINK INFO[ %s %s in scope %s at %s:%d (%d definitions)]",
					pSym->typeString().toUtf8().data(),
					pSym->identifier().toUtf8().data(),
					pSym->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
					pSym->filePath().toUtf8().data(),
					pSym->lineNumber(),
					1 + ((C3SymbolNamespace *)pSym)->additionalDefinitions().count()
				);
		} else {
			SYMBOL_MANAGER_TRACE(
					"LINK INFO[ %s %s in scope %s at %s:%d ]",
					pSym->typeString().toUtf8().data(),
					pSym->identifier().toUtf8().data(),
					pSym->bestKnownContainingScopeFullyQualifiedName().toUtf8().data(),
					pSym->filePath().toUtf8().data(),
					pSym->lineNumber()
				);
		}
#endif
	
		C3TextEditorSymbolInfo * li = new C3TextEditorSymbolInfo();
		li->oRange = oRange;
		li->oLink.insert(__literal("symbol"),pSym->identifier());
		li->oLink.insert(__literal("isCpp"),__literal("true"));
		li->oLink.setText(pSym->description());
		li->oLink.setDescription(pSym->typeString());
		li->oLink.setPath(pSym->filePath());
		li->oLink.setLocation(__utf8("%1").arg(pSym->lineNumber()+1));
		li->oLink.setEditorFactoryId("text");
		li->oLink.setTextEditorModeId(QString()); // FIXME!
		li->pSymbol = bAddSymbolPointers ? pSym : NULL;

		pInfoList->append(li);
		
		if(pSym->type() == C3Symbol::Namespace)
		{
			QList<C3SymbolNamespace *> & lOther = dynamic_cast<C3SymbolNamespace *>(pSym)->additionalDefinitions();
			Q_FOREACH(C3SymbolNamespace * pSym2,lOther)
			{
				li = new C3TextEditorSymbolInfo();
				li->oRange = oRange;
				li->oLink.insert(__literal("symbol"),pSym2->identifier());
				li->oLink.insert(__literal("isCpp"),__literal("true"));
				li->oLink.setText(pSym2->description());
				li->oLink.setDescription(pSym2->typeString());
				li->oLink.setPath(pSym2->filePath());
				li->oLink.setLocation(__utf8("%1").arg(pSym2->lineNumber()+1));
				li->oLink.setEditorFactoryId("text");
				li->oLink.setTextEditorModeId(QString()); // FIXME!
				li->pSymbol = bAddSymbolPointers ? pSym : NULL;

				pInfoList->append(li);
			}
		} else if((pSym->type() == C3Symbol::Class) && (bMatchConstructors))
		{
			// add constructors too
			storeLockedAddSymbolInfos(
					pInfoList,
					oRange,
					C3Symbol::FunctionDefinition | C3Symbol::FunctionPrototype,
					((C3SymbolClass *)pSym)->symbols(),
					pSym->identifier(),
					uScore,
					uFlags,
					bAddSymbolPointers
				);
		}
	}
}


void C3SymbolManager::processScanEditorFileRequest(C3SymbolManagerRequestScanEditorFile * pRequest)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	int iEditorId = pRequest->editorId();

	SYMBOL_MANAGER_TRACE("Rescanning file in editor %d",iEditorId);

	if(!_p->pStore)
		return; // no store yet.

	_p->iNextCTagsWorkerId++;
	
	int iId = _p->iNextCTagsWorkerId;
	
	C3SymbolManagerCTagsWorker * pWorker = new C3SymbolManagerCTagsWorker(
			iId,
			this,
			_p->szCTagsToolPath,
			QStringList(),
			QStringList(),
			iEditorId
		);
	
	pWorker->setAutoDelete(true);

	_p->oCTagsWorkersMutex.lock();
	_p->hCTagsWorkers.insert(iId,pWorker);
	_p->oCTagsWorkersMutex.unlock();

	QThreadPool::globalInstance()->start(pWorker);
	
	if(!pRequest->waitForScanToFinish())
		return;

	//qDebug("[C3SymbolManager::processScanEditorFileRequest] Waiting for the rescan to finish...");

	for(;;)
	{
		_p->oCTagsWorkersMutex.lock();
		C3SymbolManagerCTagsWorker * pOtherWorker = _p->hCTagsWorkers.value(iId);
		Q_ASSERT(!pOtherWorker || (pOtherWorker == pWorker));
		int iSymbolFiles = _p->hSymbolFiles.count();
		_p->oCTagsWorkersMutex.unlock();

		if(!pOtherWorker)
		{
			if(iSymbolFiles > 0)
				idleStep(); // process symbol files too

			break;
		}

		if(terminationRequested())
		{
			break;
		}

		QThread::msleep(30);
	}
	
	//qDebug("[C3SymbolManager::processScanEditorFileRequest] Scan file worker finished.");
}

void C3SymbolManager::processIdentifyContextRequest(C3SymbolManagerRequestIdentifyContext * pRequest)
{
	SYMBOL_MANAGER_TRACE_FUNCTION;

	if(!_p->pStore)
		return;

	QMutexLocker oLocker(&(_p->oStoreMutex));

	C3SymbolScope * pFileContext = storeLockedResolveFileContext(
			pRequest->filePath(),
			pRequest->lineNumber()
		);

	C3SymbolFile * pFile = _p->pStore->file(pRequest->filePath());
	
	if(!pFileContext)
	{
		//qDebug("File context %s not found!",pRequest->filePath().toUtf8().data());
		return;
	}

	C3SymbolManagerSymbolDescription * pDescription = new C3SymbolManagerSymbolDescription();

	pDescription->eSymbolType = pFileContext->type();
	pDescription->szIdentifier = pFileContext->identifier();
	pDescription->szScope = pFileContext->unresolvedScope();
	pDescription->szFileName = pFileContext->filePath();
	pDescription->uStartLine = pFileContext->lineNumber();
	pDescription->uEndLine = pFileContext->endLineNumber();
	
	C3SymbolClass * pClass = NULL;

	switch(pFileContext->type())
	{
		case C3SymbolScope::Class:
			SYMBOL_MANAGER_TRACE("File context is class");
			pClass = dynamic_cast<C3SymbolClass *>(pFileContext);
		break;
		case C3SymbolScope::FunctionDefinition:
		case C3SymbolScope::FunctionPrototype:
		case C3SymbolScope::Enumeration:
			pClass = dynamic_cast<C3SymbolClass *>(pFileContext->resolvedContainingScope());
			SYMBOL_MANAGER_TRACE("File context is a member of the class: containing scope is %x",pClass);
		break;
		default:
			// ignore
			SYMBOL_MANAGER_TRACE("File context is something that is not usable");
		break;
	}

	if(pClass)
		pDescription->pContainingClass = storeLockedDescribeClass(pClass);
	
	pRequest->setSymbolDescription(pDescription);
}

C3SymbolManagerClassDescription * C3SymbolManager::storeLockedDescribeClass(const QString &szFullClassName,C3Symbol::Language eLanguage)
{
	C3SymbolScope * pScope = storeLockedResolveSimpleCppTypeToSymbolScope(
			NULL,
			szFullClassName,
			_p->pStore->globalScopeForLanguage(eLanguage),
			false
		);
	
	if(!pScope)
		return NULL;

	if(pScope->type() != C3Symbol::Class)
		return NULL;

	C3SymbolClass * pClass = dynamic_cast<C3SymbolClass *>(pScope);
	Q_ASSERT(pClass);

	return storeLockedDescribeClass(pClass);
}


C3SymbolManagerClassDescription * C3SymbolManager::storeLockedDescribeClass(C3SymbolClass *pClass)
{	
	C3SymbolManagerClassDescription * pDescription = new C3SymbolManagerClassDescription();
	
	pDescription->szClassName = pClass->identifier();
	pDescription->szScopeName = pClass->unresolvedScope().toUtf8().data();

	pDescription->szDeclaratonFile = pClass->filePath();
	pDescription->uDeclarationStartLine = pClass->lineNumber();
	pDescription->uDeclarationEndLine = pClass->endLineNumber();

	for(int i=0;i<C3Symbol::AccessLevelCount;i++)
	{
		pDescription->bFoundMemberFunctionsInDeclaration[i] = false;
		pDescription->bFoundMemberVariableDeclarations[i] = false;
		pDescription->uFirstMemberVariableDeclarationLine[i] = 0xffffffff;
		pDescription->uLastMemberVariableDeclarationLine[i] = 0;
		pDescription->uFirstMemberFunctionInDeclarationLine[i] = 0xffffffff;
		pDescription->uLastMemberFunctionInDeclarationLine[i] = 0;
	}

	if(pClass->symbols())
	{
		foreach(C3Symbol * pSym,*(pClass->symbols()))
		{
			C3Symbol::AccessLevel eAccessLevel = pSym->accessLevel();
		
			switch(pSym->type())
			{
				case C3Symbol::MemberVariable:
					pDescription->bFoundMemberVariableDeclarations[C3Symbol::AccessLevelUnknown] = true;
					if(pDescription->uFirstMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown] > pSym->lineNumber())
						pDescription->uFirstMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown] = pSym->lineNumber();
					if(pDescription->uLastMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown] < pSym->endLineNumber())
						pDescription->uLastMemberVariableDeclarationLine[C3Symbol::AccessLevelUnknown] = pSym->endLineNumber();
	
					if(eAccessLevel != C3Symbol::AccessLevelUnknown)
					{
						pDescription->bFoundMemberVariableDeclarations[eAccessLevel] = true;
						if(pDescription->uFirstMemberVariableDeclarationLine[eAccessLevel] > pSym->lineNumber())
							pDescription->uFirstMemberVariableDeclarationLine[eAccessLevel] = pSym->lineNumber();
						if(pDescription->uLastMemberVariableDeclarationLine[eAccessLevel] < pSym->endLineNumber())
							pDescription->uLastMemberVariableDeclarationLine[eAccessLevel] = pSym->endLineNumber();
					}
				break;
				case C3Symbol::FunctionDefinition:
					if(pSym->filePath() != pDescription->szDeclaratonFile)
					{
						if(pDescription->szDefinitionFile.isEmpty())
							pDescription->szDefinitionFile = pSym->filePath();
						else if(pSym->filePath().length() < pDescription->szDefinitionFile.length())
							pDescription->szDefinitionFile = pSym->filePath();
					} else {
						if(
							(pSym->endLineNumber() <= pDescription->uDeclarationEndLine) &&
							(pSym->lineNumber() >= pDescription->uDeclarationStartLine)
						)
						{
							// it's inside the class declaration!
							pDescription->bFoundMemberFunctionsInDeclaration[C3Symbol::AccessLevelUnknown] = true;
							if(pDescription->uFirstMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] > pSym->lineNumber())
								pDescription->uFirstMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] = pSym->lineNumber();
							if(pDescription->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] < pSym->endLineNumber())
								pDescription->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] = pSym->endLineNumber();
							
							if(eAccessLevel != C3Symbol::AccessLevelUnknown)
							{
								pDescription->bFoundMemberFunctionsInDeclaration[eAccessLevel] = true;
								if(pDescription->uFirstMemberFunctionInDeclarationLine[eAccessLevel] > pSym->lineNumber())
									pDescription->uFirstMemberFunctionInDeclarationLine[eAccessLevel] = pSym->lineNumber();
								if(pDescription->uLastMemberFunctionInDeclarationLine[eAccessLevel] < pSym->endLineNumber())
									pDescription->uLastMemberFunctionInDeclarationLine[eAccessLevel] = pSym->endLineNumber();
							}
						}
					}
				break;
				case C3Symbol::FunctionPrototype:
					pDescription->bFoundMemberFunctionsInDeclaration[C3Symbol::AccessLevelUnknown] = true;
					if(pDescription->uFirstMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] > pSym->lineNumber())
						pDescription->uFirstMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] = pSym->lineNumber();
					if(pDescription->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] < pSym->endLineNumber())
						pDescription->uLastMemberFunctionInDeclarationLine[C3Symbol::AccessLevelUnknown] = pSym->endLineNumber();
					
					if(eAccessLevel != C3Symbol::AccessLevelUnknown)
					{
						pDescription->bFoundMemberFunctionsInDeclaration[eAccessLevel] = true;
						if(pDescription->uFirstMemberFunctionInDeclarationLine[eAccessLevel] > pSym->lineNumber())
							pDescription->uFirstMemberFunctionInDeclarationLine[eAccessLevel] = pSym->lineNumber();
						if(pDescription->uLastMemberFunctionInDeclarationLine[eAccessLevel] < pSym->endLineNumber())
							pDescription->uLastMemberFunctionInDeclarationLine[eAccessLevel] = pSym->endLineNumber();
					}
				break;
			}
		}
	}

	if(pDescription->szDefinitionFile.isEmpty())
		pDescription->szDefinitionFile = pDescription->szDeclaratonFile;

	return pDescription;
}

void C3SymbolManager::processDescribeClassRequest(C3SymbolManagerRequestDescribeClass * pRequest)
{
	if(!_p->pStore)
		return;

	C3SymbolManagerClassDescription * pDescription = storeLockedDescribeClass(pRequest->className(),pRequest->language());

	QMutexLocker oLocker(&(_p->oStoreMutex));
	
	if(pDescription)
		pRequest->setClassDescription(pDescription);
}

void C3SymbolManager::processRequest(C3SymbolManagerRequest * pRequest)
{
	_p->pCurrentlyProcessedRequest = pRequest;

	switch(pRequest->type())
	{
		case C3SymbolManagerRequest::PerformCompletion:
			//qDebug("[C3SymbolManager::processRequest] PerformCompletion");
			processPerformCompletionRequest(dynamic_cast<C3SymbolManagerRequestPerformCompletion *>(pRequest));
		break;
		case C3SymbolManagerRequest::IdentifySymbol:
			//qDebug("[C3SymbolManager::processRequest] IdentifySymbol");
			processIdentifySymbolRequest(dynamic_cast<C3SymbolManagerRequestIdentifySymbol *>(pRequest));
		break;
		case C3SymbolManagerRequest::IdentifyContext:
			//qDebug("[C3SymbolManager::processRequest] IdentifyContext");
			processIdentifyContextRequest(dynamic_cast<C3SymbolManagerRequestIdentifyContext *>(pRequest));
		break;
		case C3SymbolManagerRequest::ResetSymbolStore:
			//qDebug("[C3SymbolManager::processRequest] ResetSymbolStore");
			processResetSymbolStoreRequest(dynamic_cast<C3SymbolManagerRequestResetSymbolStore *>(pRequest));
		break;
		case C3SymbolManagerRequest::ScanEditorFile:
			//qDebug("[C3SymbolManager::processRequest] ScanEditorFile");
			processScanEditorFileRequest(dynamic_cast<C3SymbolManagerRequestScanEditorFile *>(pRequest));
		break;
		case C3SymbolManagerRequest::DescribeClass:
			//qDebug("[C3SymbolManager::processRequest] DescribeClass");
			processDescribeClassRequest(dynamic_cast<C3SymbolManagerRequestDescribeClass *>(pRequest));
		break;
		default:
			qDebug("[C3SymbolManager] Unhandled request type %d",pRequest->type());
		break;
	}
	
	_p->pCurrentlyProcessedRequest = NULL;
}

void C3SymbolManager::idleStep()
{
	_p->oCTagsWorkersMutex.lock();
	if(_p->hSymbolFiles.count() > 0)
	{
		//QElapsedTimer oTimer;
		//oTimer.start();
	
		//qDebug("[C3SymbolManager::idleStep] Got %d symbol files to insert",_p->hSymbolFiles.count());
		_p->oStoreMutex.lock();
		_p->pStore->addFiles(_p->hSymbolFiles);
		_p->oStoreMutex.unlock();

		_p->hSymbolFiles.clear();
		//qDebug("[C3SymbolManager::idleStep] Inserted files in %lld msecs",oTimer.elapsed());
	}
	_p->oCTagsWorkersMutex.unlock();
}

void C3SymbolManager::CTagsWorkerTerminated(C3SymbolManagerCTagsWorker * pWorker,const QString &szError,QHash<QString,C3SymbolFile *> * pSymbolFiles)
{
	// This is called asynchronously!

	//qDebug("CTags worker terminated!");

	_p->oCTagsWorkersMutex.lock();
	_p->hCTagsWorkers.remove(pWorker->id());

	if(pSymbolFiles)
	{
		//qDebug("[C3SymbolManager::CTagsWorkerTerminated] Got %d symbol files",pSymbolFiles->count());

		QHash<QString,C3SymbolFile *>::Iterator it;
		QHash<QString,C3SymbolFile *>::Iterator end = pSymbolFiles->end();
		for(it = pSymbolFiles->begin();it != end;it++)
		{
			C3SymbolFile * pOld = _p->hSymbolFiles.value(it.key(),NULL);
			if(pOld)
				delete pOld;
			_p->hSymbolFiles.insert(it.key(),it.value());
		}
		delete pSymbolFiles;
	} else {
		//qDebug("[C3SymbolManager::CTagsWorkerTerminated] Got no symbol files");
	}

	_p->oCTagsWorkersMutex.unlock();

	if(_p->bDying)
		return;

	// FIXME: SHOULD RESCAN ALL THE MODIFIED BUT UNSAVED FILES HERE!
	//C3Workspace::instance()->computeListOfModifiedTextEditors() <-- this is not really thread safe, (C3Workspace::instance() isn't)

	wakeUpSymbolThread();
}
