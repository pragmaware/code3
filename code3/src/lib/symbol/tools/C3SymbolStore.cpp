//=============================================================================
//
//   File : C3SymbolStore.cpp
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

#include "C3SymbolStore.h"

#include "C3SymbolNamespace.h"
#include "C3StringUtils.h"

#include <QStringList>
#include <QElapsedTimer>

#define DEBUG_SYMBOL_STORE

C3SymbolStore::C3SymbolStore()
	:
	m_oRootFile(QString())
{
	for(int i=0;i<C3Symbol::LanguageCount;i++)
		m_aSymbolsByLanguage[i] = NULL;
}

C3SymbolStore::~C3SymbolStore()
{
	for(int i=0;i<C3Symbol::LanguageCount;i++)
	{
		if(m_aSymbolsByLanguage[i])
			delete m_aSymbolsByLanguage[i];
	}

	//QHash<QString,C3SymbolFile *>::Iterator end = m_hFiles.end();
	//for(QHash<QString,C3SymbolFile *>::Iterator it = m_hFiles.begin();it != end;++it)
	//{
	//	qDebug("Delete symbol file %s",it.key().toUtf8().data());
	//	delete it.value();
	//}
	qDeleteAll(m_hFiles);
	//m_hFiles.clear();
}

void C3SymbolStore::rebuildSymbolsByLanguage(C3Symbol::Language eLanguage)
{
	// FIXME: In Release mode the cost of rebuilding symbols drops to 30%

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols for language %d",eLanguage);
#endif

#ifdef DEBUG_SYMBOL_STORE
	QElapsedTimer oTimer;
	oTimer.start();
#endif

	if(m_aSymbolsByLanguage[eLanguage])
	{
		// This delete actually runs a clear() operation on the symbol tree.
		// FIXME: Really ? Keep something? Clear only? Doh?
		delete m_aSymbolsByLanguage[eLanguage];
	}

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: checkpoint 0: %lld msecs",oTimer.elapsed());
#endif

	m_aSymbolsByLanguage[eLanguage] = new C3SymbolsByLanguage(&m_oRootFile,eLanguage);

	// Gather the super-mega-list of symbols to build
	C3SymbolList lSymbols;
	
	quint16 uLanguageFlag = 1 << eLanguage;

	QHash<QString,C3SymbolFile *>::Iterator end = m_hFiles.end();
	QHash<QString,C3SymbolFile *>::Iterator it;
	for(it = m_hFiles.begin();it != end;++it)
	{
		C3SymbolFile * pFile = it.value();
		if(!pFile->containsLanguageFlag(uLanguageFlag))
			continue;

		C3SymbolList & lLocalSymbols = it.value()->symbols();
		
		// FIXME: The assertion below must be checked!
		
		// Should not be needed anymore: qDeleteAll() above will trigger a clear of the
		// global namespace of each file so will also clear any symbol present in there
		//foreach(C3Symbol * pSymbol,lLocalSymbols)
		//	pSymbol->clear();
		
		// FIXME: The assertion above must be checked!
		
		if(pFile->containsOnlyLanguageFlag(uLanguageFlag))
		{
			lSymbols += lLocalSymbols;
		} else {
			// file contains multiple languages!
			Q_FOREACH(C3Symbol * pSymm,lLocalSymbols)
			{
				if(pSymm->language() == eLanguage)
					lSymbols.append(pSymm);
			}
		}
	}

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: checkpoint 1: %lld msecs",oTimer.elapsed());
#endif

	buildSymbolsByLanguageForSymbolList(lSymbols,eLanguage);

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: checkpoint 2: %lld msecs",oTimer.elapsed());
#endif

	// resolve imported scopes

	for(it = m_hFiles.begin();it != end;++it)
	{
		C3SymbolFile * pFile = it.value();
		if(!pFile->containsLanguageFlag(uLanguageFlag))
			continue;

		pFile->clearAdditionalSymbolLookupScopes(eLanguage);

		if(!pFile->hasImportedScopes())
			continue;
	
#ifdef DEBUG_SYMBOL_STORE
		qDebug("File %s has imported scopes",pFile->path().toUtf8().data());
#endif
		resolveImportedScopes(pFile,eLanguage);
	}

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: checkpoint 3: %lld msecs",oTimer.elapsed());
#endif

#if 0
	// resolve base classes

	// THIS IS NOW DONE ON-THE-FLY
	m_aSymbolsByLanguage[eLanguage]->globalScope()->resolveTypedefsAndBaseClasses();
#endif

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: built in %lld msecs",oTimer.elapsed());

	//m_aSymbolsByLanguage[eLanguage]->globalScope()->dump(QString(""));
#endif

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuilding symbols: finished in %lld msecs",oTimer.elapsed());
#endif
}

void C3SymbolStore::resolveImportedScopes(C3SymbolFile * pFile,C3Symbol::Language eLanguage)
{
	Q_ASSERT(pFile);
	
	QList<C3SymbolImportedScope *> & lScopes = pFile->importedScopes();
	if(lScopes.isEmpty())
		return;

	C3SymbolsByLanguage * pSyms = m_aSymbolsByLanguage[eLanguage];
	Q_ASSERT(pSyms);

	// FIXME: Use a hash to speed up resolving scopes

	Q_FOREACH(C3SymbolImportedScope * pScope,lScopes)
	{
		if(pScope->language() != eLanguage)
			continue;

		C3SymbolNamespace * pGlobalScope = pSyms->globalScope();
		
		Q_ASSERT(pGlobalScope);

		QString szToResolve = pScope->identifier();
		szToResolve.replace(__ascii("::"),__ascii("."));

#ifdef DEBUG_SYMBOL_STORE
		qDebug("Resolving imported scope '%s'",szToResolve.toUtf8().data());
#endif

		QHash<void *,C3SymbolScope *> hScopeMap;

		C3SymbolScope * pResolvedScope = NULL;
		C3SymbolScope * pCurrentScope = pGlobalScope;
		
		while(!szToResolve.isEmpty())
		{
			int idx = szToResolve.indexOf(QChar('.'));
			
			QString szResolving;
			
			if(idx >= 0)
			{
				szResolving = szToResolve.left(idx);
				szToResolve = szToResolve.mid(idx+1);
			} else {
				szResolving = szToResolve;
				szToResolve = QString();
			}
			
		
			if(szResolving.isEmpty())
				continue;
		
			// FIXME: Handle nested namespaces!!!!
	
			pResolvedScope = (C3SymbolNamespace *)pCurrentScope->findSymbol(
					szResolving,
					C3Symbol::Namespace
				);
	
			if(!pResolvedScope)
				break;
			
			pCurrentScope = pResolvedScope;
		}

		pScope->setResolvedScope(pResolvedScope);
		
		while(pResolvedScope && (pResolvedScope != pGlobalScope))
		{
			hScopeMap.insert(pResolvedScope,pResolvedScope);
			pResolvedScope = pResolvedScope->resolvedContainingScope();
		}

		QHash<void *,C3SymbolScope *>::Iterator end = hScopeMap.end();
		for(QHash<void *,C3SymbolScope *>::Iterator it = hScopeMap.begin();it != end;++it)
		{
#ifdef DEBUG_SYMBOL_STORE
			qDebug("- Adding additional import scope '%s'",it.value()->identifier().toUtf8().data());
#endif
			pFile->addAdditionalSymbolLookupScope(it.value());
		}
	}
}


void C3SymbolStore::buildSymbolsByLanguageForSymbolList(C3SymbolList &lSymbolList,C3Symbol::Language eLanguage)
{
	if(lSymbolList.isEmpty())
		return;

	// FIXME: What happens with typedefs here?

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuilding symbols: %d symbols to analyze",lSymbolList.count());
#endif

#define MAX_SCOPE_LEVELS 20

	C3SymbolsByLanguage * pSymbolsByLanguage = m_aSymbolsByLanguage[eLanguage];
	Q_ASSERT(pSymbolsByLanguage); // Thist MUST have been set up by the caller
	
	C3SymbolScope * pGlobalScope = pSymbolsByLanguage->globalScope();
	Q_ASSERT(pGlobalScope);

	static QString szScopeSeparator1 = __ascii(".");
	static QString szScopeSeparator2 = __ascii("::");
	QString szScopeSeparator = (eLanguage == C3Symbol::Cpp) ? szScopeSeparator2 : szScopeSeparator1;

	// symbols at each scope level (level 0 is unused because it's processed "on-the-fly")
	C3SymbolList * aSymbols[MAX_SCOPE_LEVELS];
	// scopes with the specified NUMBER OF PARTS (level 0 is unused)
	QHash<QString,C3SymbolScopeList *> * aScopes[MAX_SCOPE_LEVELS];

	int i;
	
	// FIXME: memset to go faster?
	// FIXME: Preallocate the lists to go faster?
	for(i=0;i<MAX_SCOPE_LEVELS;i++)
	{
		aSymbols[i] = NULL;
		aScopes[i] = NULL;
	}

	C3Symbol * pSymbol;

	Q_FOREACH(pSymbol,lSymbolList)
	{
#ifdef DEBUG_SYMBOL_STORE
		Q_ASSERT(pSymbol->language() == eLanguage);
#endif

		// FIXME: This could be probably sped up a bit
		
		// A LOT OF TIME IS SPENT INSIDE THIS FUNCTION (WHICH BUILDS A SYMBOLS-BY-FIRST-LETTER SORTED-LIST MAP)
		// FIXME: When a file is updated we need to rebuild the tree but
		//        we COULD keep (and update incrementally) the SYMBOLS BY FIRST LETTER.
		pSymbolsByLanguage->addToAllSymbols(pSymbol);

		int iScopeParts = pSymbol->unresolvedScopePartCount();
		
		if(iScopeParts >= (MAX_SCOPE_LEVELS-1))
		{
			qDebug(
					"WARNING: Symbol %s has %d scope parts but only %d are supported: will remain unattached",
					pSymbol->identifier().toUtf8().data(),
					iScopeParts,
					MAX_SCOPE_LEVELS-1
				);
			continue;
		}
		
		if(pSymbol->isScope())
		{
			// The symbol is a scope itself.
			//
			// Note that the scope of a scope is always explicitly known
			// (scopes are classes/structs/unions, namespaces, enumerations and function definitions)

			int iQualifiedNameParts = iScopeParts + 1;

			if(!aScopes[iQualifiedNameParts])
				aScopes[iQualifiedNameParts] = new QHash<QString,C3SymbolScopeList *>();

			// FIXME: This could be cached!
			QString sQualifiedName = pSymbol->unresolvedScope();
			if(!sQualifiedName.isEmpty())
				sQualifiedName += szScopeSeparator;
			sQualifiedName += pSymbol->identifier();

			C3SymbolScopeList * pList = aScopes[iQualifiedNameParts]->value(sQualifiedName,NULL);
			if(!pList)
			{
				pList = new C3SymbolScopeList();
				aScopes[iQualifiedNameParts]->insert(sQualifiedName,pList);
				pList->append((C3SymbolScope *)pSymbol);
			} else {
				if(pSymbol->isNamespace())
				{
					// do not duplicate namespaces
					bool bFound = false;
					Q_FOREACH(C3SymbolScope * pOtherScope,*pList)
					{
						if(pOtherScope->isNamespace())
						{
							bFound = true;
							break;
						}
					}
					if(!bFound)
						pList->append((C3SymbolScope *)pSymbol);
				} else {
					pList->append((C3SymbolScope *)pSymbol);
				}
			}
		}

		if(iScopeParts > 0)
		{
			// FIXME: Preallocating could avoid this check
			if(!aSymbols[iScopeParts])
				aSymbols[iScopeParts] = new C3SymbolList();
		
			aSymbols[iScopeParts]->append(pSymbol);
			continue;
		}

		// toplevel symbol
		if(pSymbol->isNamespace())
		{
			// do not duplicate namespaces
			C3SymbolNamespace * pNamespace = pGlobalScope->findNamespace(pSymbol->identifier());
			if(pNamespace)
				pNamespace->addAdditionalDefinition(dynamic_cast<C3SymbolNamespace *>(pSymbol));
			else
				pGlobalScope->addSymbol(pSymbol);
		} else {
			pGlobalScope->addSymbol(pSymbol);
		}
	}

	QString szLastNotFoundScope;

	for(i=1;i<MAX_SCOPE_LEVELS;i++)
	{
		if(!aSymbols[i])
			continue;

		if(!aScopes[i])
		{
			delete aSymbols[i];
			qDebug("WARNING: There are no scopes at level %d: %d symbols will remain floating",i,aSymbols[i]->count());
			continue;
		}

		Q_FOREACH(pSymbol,*(aSymbols[i]))
		{
			QString szScope = pSymbol->unresolvedScope();

#ifdef DEBUG_SYMBOL_STORE
			Q_ASSERT(pSymbol->unresolvedScopePartCount() == i);
#endif

			C3SymbolScopeList * pScopeList = aScopes[i]->value(szScope,NULL);
			if(!pScopeList)
			{
				// Scope not found. Symbol will remain "floating" without a scope.
	
				if(szScope != szLastNotFoundScope)
				{
					qDebug("WARNING: Scope %s not found",szScope.toUtf8().data());
					szLastNotFoundScope = szScope;
				}
				continue;
			}

			Q_ASSERT(pScopeList->count() > 0);

			// FIXME: Match the scope yet better!!!
			C3SymbolScope * pBestScope = NULL;
			
			if(pScopeList->count() > 1)
			{
				foreach(C3SymbolScope * pCandidateScope,*pScopeList)
				{
					// same file, better than other ones
					if(pCandidateScope->file() != pSymbol->file())
						continue;

					if(pCandidateScope->lineNumber() > pSymbol->lineNumber())
						continue; // it's defined after??? doesn't really make sense

					if(!pBestScope)
					{
						pBestScope = pCandidateScope;
						continue;
					}

					// multiple candidates in the same file as the symbol, pick the nearest one
					if(pCandidateScope->lineNumber() > pBestScope->lineNumber())
						pBestScope = pCandidateScope;
				}
				
				if(!pBestScope)
				{
					int maxl = 0;
					foreach(C3SymbolScope * pCandidateScope,*pScopeList)
					{
						int commonLen = C3StringUtils::commonCharacterCount(
								pCandidateScope->filePath(),
								pSymbol->filePath()
							);

						if(commonLen > maxl)
						{
							pBestScope = pCandidateScope;
							maxl = commonLen;
						}
					}
					
					if(!pBestScope)
						pBestScope = pScopeList->first();
				}
			} else {
				pBestScope = pScopeList->first();
			}
			
#ifdef DEBUG_SYMBOL_STORE
			Q_ASSERT(pBestScope);
#endif

			if(pSymbol->isNamespace())
			{
				// do not duplicate namespaces
				C3SymbolNamespace * pFound = pBestScope->findNamespace(pSymbol->identifier());
				if(pFound)
					pFound->addAdditionalDefinition(dynamic_cast<C3SymbolNamespace *>(pSymbol));
				else
					pBestScope->addSymbol(pSymbol);
			} else {
				pBestScope->addSymbol(pSymbol);

				if(pSymbol->type() == C3Symbol::EnumerationMember)
				{
					// add it also to the containing scope!
					// FIXME: This may fail if the enumeration member comes before the enumeration itself (?)
					if(pBestScope->resolvedContainingScope())
						pBestScope->resolvedContainingScope()->addSymbol(pSymbol);
					else
						qDebug("WARNING: Enumeration member %s has no containing scope",pSymbol->identifier().toUtf8().data());
				}
			}
		}

		delete aSymbols[i];
		delete aScopes[i];
	}
}

/*

// THIS IS AN OLD VERSION THAT WAS WORKING AND (AT THE TIME) HAD COMPARABLE PERFORMANCE BUT WAS A MORE COMPLICATED

void C3SymbolStore::buildSymbolsByLanguageForSymbolListOLD(C3SymbolList &lSymbolList,C3Symbol::Language eLanguage)
{

	if(lSymbolList.isEmpty())
		return;

	// FIXME: What happens with typedefs here?

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuilding symbols: %d symbols to analyze",lSymbolList.count());
#endif

	C3SymbolsByLanguage * pSymbolsByLanguage = m_aSymbolsByLanguage[eLanguage];
	Q_ASSERT(pSymbolsByLanguage); // Thist MUST have been set up by the caller
	
	C3SymbolScope * pGlobalScope = pSymbolsByLanguage->globalScope();
	Q_ASSERT(pGlobalScope);

	C3SymbolList lRemainingSymbols;
	
	C3Symbol * pSymbol;

	QHash<QString,C3SymbolScopeList *> hNextLevelScopes;

	// First pass. Assign anything without a scope.
	foreach(pSymbol,lSymbolList)
	{
#ifdef DEBUG_SYMBOL_STORE
		Q_ASSERT(pSymbol->language() == eLanguage);
#endif

		// FIXME: This could be probably sped up a bit
		pSymbolsByLanguage->addToAllSymbols(pSymbol);

		if(pSymbol->unresolvedScopePartCount() > 0)
		{
			// Has a scope. Will be resolved later.
			lRemainingSymbols.append(pSymbol);
			continue;
		}

		// Has no scope

		if(!pSymbol->isScope())
		{
			// not a scope itself: a global symbol
			pGlobalScope->addSymbol(pSymbol);
			continue;
		}

		// Is toplevel scope.

		C3SymbolScopeList * pList = hNextLevelScopes.value(pSymbol->identifier(),NULL);
		if(!pList)
		{
			pList = new C3SymbolScopeList();
			//qDebug("Add first level scope %s",pSymbol->identifier().toUtf8().data());
			hNextLevelScopes.insert(pSymbol->identifier(),pList);
		}

		if(pSymbol->type() == C3Symbol::Namespace)
		{
			// do not duplicate namespaces
			C3SymbolNamespace * pFound = NULL;
			Q_FOREACH(C3SymbolScope * pDaScope,*pList)
			{
				if(pDaScope->type() == C3Symbol::Namespace)
				{
					pFound = dynamic_cast<C3SymbolNamespace *>(pDaScope);
					Q_ASSERT(pFound);
					break;
				}
			}
			
			if(!pFound)
				pList->append(dynamic_cast<C3SymbolScope *>(pSymbol));

			C3SymbolNamespace * pFound2 = pGlobalScope->findNamespace(pSymbol->identifier());
			if(!pFound2)
			{
				Q_ASSERT(!pFound);
				pGlobalScope->addSymbol(pSymbol);
			} else {
				Q_ASSERT(pFound == pFound2);
				pFound->addAdditionalDefinition(dynamic_cast<C3SymbolNamespace *>(pSymbol));
			}
		} else {
			pList->append(dynamic_cast<C3SymbolScope *>(pSymbol));
			pGlobalScope->addSymbol(pSymbol);
		}
	}

	int iScopeLevel = 1;

	static QString szScopeSeparator1 = __ascii(".");
	static QString szScopeSeparator2 = __ascii("::");
	QString szScopeSeparator = (eLanguage == C3Symbol::Cpp) ? szScopeSeparator2 : szScopeSeparator1;

	QHash<QString,C3SymbolScopeList *> hScopes;

	QString szLastNotFoundScope;

	do {
		// move scopes that were built at the previous level into hScopes
		hNextLevelScopes.swap(hScopes);

		// hScopes now contains keys with exactly iScopeLevel=N parts (<name1>::<name2>::..::<nameN>)
		// that point to the related (resololved scopes).

		// clear anything that was present in hScopes (previous level)
		qDeleteAll(hNextLevelScopes);
		hNextLevelScopes.clear();
		
		// move remaining symbols into lSymbolList
		lRemainingSymbols.swap(lSymbolList);
		// clear anything that was present in lSymbolList
		lRemainingSymbols.clear();

#ifdef DEBUG_SYMBOL_STORE
		qDebug("Rebuilding symbols: next pass has %d symbols to analyze",lSymbolList.count());
#endif

		foreach(pSymbol,lSymbolList)
		{
			QString szScope = pSymbol->unresolvedScope();
			
			Q_ASSERT(!szScope.isEmpty());
			
			int iScopeParts = pSymbol->unresolvedScopePartCount();
			
			Q_ASSERT(iScopeParts >= iScopeLevel);
			
			if(iScopeParts > iScopeLevel)
			{
				// has a scope with more parts
				lRemainingSymbols.append(pSymbol);
				continue;
			}
			
			// has a scope with exactly iScopeLevel parts

			C3SymbolScopeList * pScopeList = hScopes.value(szScope,NULL);
			if(pScopeList)
			{
				Q_ASSERT(pScopeList->count() > 0);

				// FIXME: Match the scope yet better!!!
				C3SymbolScope * pBestScope = NULL;
				
				if(pScopeList->count() > 1)
				{
					foreach(C3SymbolScope * pCandidateScope,*pScopeList)
					{
						// same file, better than other ones
						if(pCandidateScope->file() != pSymbol->file())
							continue;

						if(pCandidateScope->lineNumber() > pSymbol->lineNumber())
							continue; // it's defined after??? doesn't really make sense

						if(!pBestScope)
						{
							pBestScope = pCandidateScope;
							continue;
						}

						// multiple candidates in the same file as the symbol, pick the nearest one
						if(pCandidateScope->lineNumber() > pBestScope->lineNumber())
							pBestScope = pCandidateScope;
					}
					
					if(!pBestScope)
					{
						int maxl = 0;
						foreach(C3SymbolScope * pCandidateScope,*pScopeList)
						{
							int commonLen = C3StringUtils::commonCharacterCount(
									pCandidateScope->filePath(),
									pSymbol->filePath()
								);

							if(commonLen > maxl)
							{
								pBestScope = pCandidateScope;
								maxl = commonLen;
							}
						}
						
						if(!pBestScope)
							pBestScope = pScopeList->first();
					}
					
				} else {
					pBestScope = pScopeList->first();
				}

				Q_ASSERT(pBestScope);
				if(pSymbol->type() == C3Symbol::Namespace)
				{
					// do not duplicate namespaces
					C3SymbolNamespace * pFound = pBestScope->findNamespace(pSymbol->identifier());
					if(pFound)
						pFound->addAdditionalDefinition(dynamic_cast<C3SymbolNamespace *>(pSymbol));
					else
						pBestScope->addSymbol(pSymbol);
				} else {
					pBestScope->addSymbol(pSymbol);

					if(pSymbol->type() == C3Symbol::EnumerationMember)
					{
						// add it also to the containing scope!
						// FIXME: This may fail if the enumeration member comes before the enumeration itself (?)
						if(pBestScope->resolvedContainingScope())
							pBestScope->resolvedContainingScope()->addSymbol(pSymbol);
						else
							qDebug("WARNING: Enumeration member %s has no containing scope",pSymbol->identifier().toUtf8().data());
					}
				}
				
			} else {
				// Scope not found. Symbol will remain "floating" without a scope.

				if(szScope != szLastNotFoundScope)
				{
					qDebug("WARNING: Scope %s not found",szScope.toUtf8().data());
					szLastNotFoundScope = szScope;
				}
			}
			
			if(pSymbol->isScope())
			{
				QString szFullScope = szScope + szScopeSeparator + pSymbol->identifier();
				//qDebug("Add %d level scope %s",iScopeLevel,szFullScope.toUtf8().data());
				C3SymbolScopeList * pList = hNextLevelScopes.value(szFullScope,NULL);
				if(!pList)
				{
					pList = new C3SymbolScopeList();
					hNextLevelScopes.insert(szFullScope,pList);
					pList->append(dynamic_cast<C3SymbolScope *>(pSymbol));
				} else {
					if(pSymbol->type() == C3Symbol::Namespace)
					{
						// do not duplicate namespaces
						bool bFound = false;
						Q_FOREACH(C3SymbolScope * pDaScope,*pList)
						{
							if(pDaScope->type() == C3Symbol::Namespace)
							{
								bFound = true;
								break;
							}
						}
						
						if(!bFound)
							pList->append(dynamic_cast<C3SymbolScope *>(pSymbol));
					} else {
						pList->append(dynamic_cast<C3SymbolScope *>(pSymbol));
					}
				}
			}
		}
		
		iScopeLevel++;

	} while(!lRemainingSymbols.isEmpty());

	qDeleteAll(hNextLevelScopes);
	qDeleteAll(hScopes);
}

*/

void C3SymbolStore::addFiles(QHash<QString,C3SymbolFile *> & hFiles)
{
	QHash<QString,C3SymbolFile *>::Iterator it;
	QHash<QString,C3SymbolFile *>::Iterator end = hFiles.end();

	quint16 uLanguageFlags = 0;

	for(it = hFiles.begin();it != end;it++)
	{
		C3SymbolFile * pFile = it.value();

		uLanguageFlags |= pFile->languageFlags();

		C3SymbolFile * pOld = m_hFiles.value(pFile->path(),NULL);
		if(pOld)
		{
			m_hFiles.remove(pOld->path());
			delete pOld;
		}
	
		m_hFiles.insert(pFile->path(),pFile);
	}

	for(quint16 i=0;i<C3Symbol::LanguageCount;i++)
	{
		if(!m_aSymbolsByLanguage[i])
			continue;
		
		if(!(uLanguageFlags & (1 << i)))
			continue;

		// FIXME: Really mark as dirty only instead of destroying?
		delete m_aSymbolsByLanguage[i];
		m_aSymbolsByLanguage[i] = NULL;
	}

	// remove it
	
	// FIXME: Here we could check if the file has few (maybe non-root) symbols.
	// If it's the case then we could selectively remove them from their parents
	// and gather their children. We could then purge the children that belong to
	// the file being removed to obtain a list of orphans. Then we insert the
	// new file, readd its symbols and finally reset the orphans.
	// This could work as long as the structure doesn't change too much
	// (that is it doesn't happen that an orphan actually becomes a parent
	// scope of one of the newly inserted symbols...)
	
	// FIXME: We could also separate the "symbols-by-first-letter" and the
	//        "symbol-tree-rooted-at-global-namespace" that are currently
	//        in C3SymbolsByLanguage. "symbols-by-first-letter" could be
	//        updated incrementally while the "symbol-tree" could be
	//        updated all at once."
	
	// WARNING: EnumerationMembers are added to multiple scopes!
	
	// FIXME: C3SymbolMap is actually a QMultiMap, which is a red-black tree
	//        and performs poorly when there are MANY symbols.
	//        It might be worth to try again a sorted-QList + QHash based approach
	//        to see if it's better.

}

C3SymbolScope * C3SymbolStore::findScope(const QString &szScope,C3Symbol::Language eLanguage)
{
	C3SymbolScope * pScope = globalScopeForLanguage(eLanguage);
	if(!pScope)
		return pScope;
	if(szScope.isEmpty())
		return pScope;
	
	QStringList lSubScopes = szScope.split(QChar('.'));

	int c = lSubScopes.count();
	int i = 0;
	
	while((i < c) && pScope)
	{
		QString szSubScope = lSubScopes.at(i);
		pScope = pScope->findScope(szSubScope);
		if(!pScope)
			return NULL;
	}
	
	return pScope;
}

C3SymbolFile * C3SymbolStore::file(const QString &szName)
{
	C3SymbolFile * pFile = m_hFiles.value(szName,NULL);
	if(!pFile)
		return NULL;
	
	// Make sure symbols are built!

	// FIXME: We could probably use a global dirty flag telling that SOME languages are dirty?
	// FIXME: This could probably use a list of languages in each file?

	for(int i=0;i<C3Symbol::LanguageCount;i++)
	{
		if(!pFile->containsLanguage((C3Symbol::Language)i))
			continue;
		if(!m_aSymbolsByLanguage[i])
			rebuildSymbolsByLanguage((C3Symbol::Language)i);
	}
	
	return pFile;
}
