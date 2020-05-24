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

//#define DEBUG_SYMBOL_STORE

C3SymbolStore::C3SymbolStore()
	:
	m_oRootFile(QString())
{
	for(int i=0;i<C3Symbol::LanguageCount;i++)
	{
		m_aSortedSymbols[i] = NULL;
		m_aGlobalScopes[i] = NULL;
	}
}

C3SymbolStore::~C3SymbolStore()
{
	for(int i=0;i<C3Symbol::LanguageCount;i++)
	{
		if(m_aSortedSymbols[i])
			delete m_aSortedSymbols[i];
		if(m_aGlobalScopes[i])
			delete m_aGlobalScopes[i];
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
	qDebug("Rebuild symbols for language %d (got %d files)",eLanguage,m_hFiles.count());
#endif

#ifdef DEBUG_SYMBOL_STORE
	QElapsedTimer oTimer;
	oTimer.start();
#endif


#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: checkpoint 0: %lld msecs",oTimer.elapsed());
#endif

	bool bBuildSortedSymbols;

	if(m_aSortedSymbols[eLanguage])
	{
		// If this pointer is non-null then it is up to date (it already contains all the symbols we got)
		bBuildSortedSymbols = false;
#ifdef DEBUG_SYMBOL_STORE
		qDebug("Will not rebuild sorted symbols for language %d",eLanguage);
#endif

	} else {
#ifdef DEBUG_SYMBOL_STORE
		qDebug("Will rebuild sorted symbols for language %d",eLanguage);
#endif
		m_aSortedSymbols[eLanguage] = new C3SortedSymbols(eLanguage);
		bBuildSortedSymbols = true;
	}
	
	if(m_aGlobalScopes[eLanguage])
	{
#ifdef DEBUG_SYMBOL_STORE
		qDebug("Global scope already exists: clearing");
#endif
		m_aGlobalScopes[eLanguage]->clear();
	} else {
#ifdef DEBUG_SYMBOL_STORE
		qDebug("Global scope does not exist yet: creating");
#endif
		m_aGlobalScopes[eLanguage] = new C3SymbolNamespace(
				&m_oRootFile,
				eLanguage,
				__literal("<global>"),
				QString(),
				C3Symbol::AccessLevelPublic,
				0,
				0
			);
	}
	
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

	buildSymbolsByLanguageForSymbolList(lSymbols,eLanguage,bBuildSortedSymbols);

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
	m_aGlobalScopes[eLanguage]->resolveTypedefsAndBaseClasses();
#endif

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuild symbols: built in %lld msecs",oTimer.elapsed());

	//m_aSortedSymbols[eLanguage]->globalScope()->dump(QString(""));
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

	C3SortedSymbols * pSyms = m_aSortedSymbols[eLanguage];
	Q_ASSERT(pSyms);

	C3SymbolNamespace * pGlobalScope = m_aGlobalScopes[eLanguage];
	Q_ASSERT(pGlobalScope);

	// FIXME: Use a hash to speed up resolving scopes

	Q_FOREACH(C3SymbolImportedScope * pScope,lScopes)
	{
		if(pScope->language() != eLanguage)
			continue;

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


void C3SymbolStore::buildSymbolsByLanguageForSymbolList(C3SymbolList &lSymbolList,C3Symbol::Language eLanguage,bool bBuildSortedSymbols)
{
	if(lSymbolList.isEmpty())
		return;

	// FIXME: What happens with typedefs here?

#ifdef DEBUG_SYMBOL_STORE
	qDebug("Rebuilding symbols: %d symbols to analyze",lSymbolList.count());
#endif

#define MAX_SCOPE_LEVELS 20

	C3SortedSymbols * pSymbolsByLanguage = m_aSortedSymbols[eLanguage];
	Q_ASSERT(pSymbolsByLanguage); // Thist MUST have been set up by the caller
	
	C3SymbolScope * pGlobalScope = m_aGlobalScopes[eLanguage];
	Q_ASSERT(pGlobalScope);

	static QString szScopeSeparator1 = __ascii(".");
	static QString szScopeSeparator2 = __ascii("::");
	QString szScopeSeparator = ((eLanguage == C3Symbol::Cpp) || (eLanguage == C3Symbol::Php)) ? szScopeSeparator2 : szScopeSeparator1;

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

		if(bBuildSortedSymbols) // FIXME: Optimize away this check in some way?
			pSymbolsByLanguage->add(pSymbol);

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
		{
			// no symbols in scopes with i parts (say nothing within namespace::class::function)
			if(aScopes[i])
			{
				// but got scopes with i parts (say namespace::class::function exists)
#if 0
				qDebug("Dump at level %d",i);
				QHash<QString,C3SymbolScopeList *>::Iterator itex = aScopes[i]->begin();
				while(itex != aScopes[i]->end())
				{
					qDebug("Scope list with %d items",itex.value()->count());
					Q_FOREACH(C3SymbolScope * ssc,*(itex.value()))
						qDebug("Scope %s",ssc->description().toUtf8().data());
					++itex;
				}
#endif
				qDeleteAll(*(aScopes[i]));
				delete aScopes[i];
			}
			continue;
		}

		if(!aScopes[i])
		{
			qDebug("WARNING: There are no scopes at level %d: %d symbols will remain floating",i,aSymbols[i]->count());
			delete aSymbols[i];
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

		qDeleteAll(*(aScopes[i]));
		delete aSymbols[i];
		delete aScopes[i];
	}
}


void C3SymbolStore::addFiles(QHash<QString,C3SymbolFile *> & hFiles)
{
#ifdef DEBUG_SYMBOL_STORE
	C3_TRACE("Add %d files (internally got %d files)",hFiles.count(),m_hFiles.count());
	
	QElapsedTimer oTimer;
	oTimer.start();
#endif

	// Strategy
	//
	// Above a certain theshold (FIXME: which one?) we destroy both the symbol
	// tree AND the sorted symbol list. Below the treshold we destroy
	// the symbol tree and update the sorted symbol list.
	//
	// FIXME: The threshould should probably be based on the number of symbols,
	//        not the number of files.
	//
	
	bool bDropEverything = (hFiles.count() > (m_hFiles.count() / 4));

	QHash<QString,C3SymbolFile *>::Iterator it;
	QHash<QString,C3SymbolFile *>::Iterator end = hFiles.end();

	quint16 uLanguageFlags = 0;

	// The global namespaces must be cleared before the files are deleted.
	// So we first scan the files, remove them from the store and gather
	// the list of languages. Then, at once, we clear/delete the global namespaces
	// that were present in the files. And finally we delete the files.
	QList<C3SymbolFile *> lToDelete;

	if(bDropEverything)
	{
		for(it = hFiles.begin();it != end;it++)
		{
			C3SymbolFile * pFile = it.value();
	
			uLanguageFlags |= pFile->languageFlags();
	
			C3SymbolFile * pOld = m_hFiles.value(pFile->path(),NULL);
			if(pOld)
			{
				uLanguageFlags |= pOld->languageFlags();
#ifdef DEBUG_SYMBOL_STORE
				if(m_hFiles.remove(pOld->path()) != 1)
					qDebug("WARNING: Remove of file %s did not return 1",pOld->path().toUtf8().data());
#endif
				lToDelete.append(pOld);
			}
		
			m_hFiles.insert(pFile->path(),pFile); // WILL REPLACE pOld (so do NOT call remove() above)
		}
	
	} else {
	
		// We keep (and update) the sorted symbols.
		// This is a bit harder.
	
		for(it = hFiles.begin();it != end;it++)
		{
			C3SymbolFile * pFile = it.value();
	
			uLanguageFlags |= pFile->languageFlags();
	
			C3SymbolFile * pOld = m_hFiles.value(pFile->path(),NULL);
			if(pOld)
			{
				uLanguageFlags |= pOld->languageFlags();

				Q_FOREACH(C3Symbol * pSym,pOld->symbols())
				{
					if(m_aSortedSymbols[pSym->language()])
						m_aSortedSymbols[pSym->language()]->remove(pSym);
				}
#ifdef DEBUG_SYMBOL_STORE
				if(m_hFiles.remove(pOld->path()) != 1)
					qDebug("WARNING: Remove of file %s did not return 1",pOld->path().toUtf8().data());
#endif
				lToDelete.append(pOld);
			}
		
			m_hFiles.insert(pFile->path(),pFile); // WILL REPLACE pOld (so do NOT call remove() above)

			Q_FOREACH(C3Symbol * pSym,pFile->symbols())
			{
				if(m_aSortedSymbols[pSym->language()])
					m_aSortedSymbols[pSym->language()]->add(pSym);
			}
		}
	}

#ifdef DEBUG_SYMBOL_STORE
	C3_TRACE("DropEverything=%d, languageFlags=%d",bDropEverything,uLanguageFlags);
#endif

	for(quint16 i=0;i<C3Symbol::LanguageCount;i++)
	{
		if(!(uLanguageFlags & (1 << i)))
			continue;

		if(bDropEverything && m_aSortedSymbols[i])
		{
#ifdef DEBUG_SYMBOL_STORE
			C3_TRACE("Dropping sorted symbols for language %d",i);
#endif
			delete m_aSortedSymbols[i];
			m_aSortedSymbols[i] = NULL;
		}
		
		if(m_aGlobalScopes[i])
		{
#ifdef DEBUG_SYMBOL_STORE
			C3_TRACE("Dropping global scope for language %d",i);
#endif
			m_aGlobalScopes[i]->clear(); // must be cleared first, so the lists in the scopes will be emptied!
			delete m_aGlobalScopes[i];
			m_aGlobalScopes[i] = NULL;
		}
	}

	qDeleteAll(lToDelete);

#ifdef DEBUG_SYMBOL_STORE
	C3_TRACE("Adding %d files took %lld msecs, now got %d files",hFiles.count(),oTimer.elapsed(),m_hFiles.count());
#endif

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
		if(!m_aGlobalScopes[i])
			rebuildSymbolsByLanguage((C3Symbol::Language)i);
	}
	
	return pFile;
}
