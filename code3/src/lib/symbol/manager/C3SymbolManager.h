#ifndef _C3SymbolManager_h_
#define _C3SymbolManager_h_
//=============================================================================
//
//   File : C3SymbolManager.h
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

#include "c3_config.h"

#include "C3SymbolManagerBase.h"

#include <QHash>
#include <QString>

#include "C3TextEditor.h"
#include "C3SymbolFile.h"
#include "C3TextEditorCompletion.h"
#include "C3Symbol.h"

class C3SymbolManagerPrivate;
class C3SymbolManagerCTagsWorker;
class C3SymbolManagerRequestResetSymbolStore;
class C3SymbolManagerRequestPerformCompletion;
class C3SymbolManagerRequestIdentifySymbol;
class C3SymbolManagerRequestIdentifyContext;
class C3SymbolManagerRequestScanEditorFile;
class C3SymbolManagerRequestDescribeClass;
class C3SymbolManagerClassDescription;
class C3SymbolClass;
class C3SymbolStore;

class C3SymbolManager : public C3SymbolManagerBase
{
	friend class C3SymbolManagerCTagsWorker;
	Q_OBJECT
public:
	C3SymbolManager(
			const QString &szCTagsToolPath
		);
	virtual ~C3SymbolManager();

private:
	static C3SymbolManager * m_pInstance;

	QString m_szArrow;
	QString m_szSquareArrow;
	QString m_szSquareDot;
	QString m_szDot;
	QString m_szParenthesisArrow;
	QString m_szParenthesisDot;
	QString m_szTwoColons;
	QString m_szNew;
	QString m_szConst;
	QString m_szStatic;
	QString m_szInline;
	QString m_szDelete;
	QString m_szThis;
	QString m_szP;
	QString m_szParenthesis;
	QString m_szOpenAndClosedParenthesis;

	C3SymbolManagerPrivate * m_pC3SM;

public:
	static C3SymbolManager * instance()
	{
		return m_pInstance;
	}

	// This class works mainly with asynchronous requests.
	// However there are a couple of synchronous ones too.
	// YOU MUST FOLLOW A STRICT POLICY WHEN USING THEM BECAUSE THEY WILL LOCK THE SYMBOL STORE!
	// First call lockStore();
	// Then call your storeLocked* functions
	// Then call unlockStore();

	void lockStore();
	void unlockStore();

	void storeLockedIdentifySymbol(
			C3TextEditorSymbolContext * pContext,
			QList<C3TextEditorSymbolInfo * > * pInfoList,
			bool bAddSymbolPointers,
			bool bUseContextFilter,
			C3Symbol::Language eLanguage
		);

	C3SymbolScope * storeLockedResolveTypeOfSymbol(
			C3SymbolFile * pFile,
			C3SymbolScope * pFileContext,
			C3Symbol * pSymbol
		);
		
	C3SymbolScope * storeLockedResolveFileContext(
			const QString &szFileName,
			int uRow
		);

	QString filterCppType(const QString &szType,QStringList * pTemplatePart = NULL);

protected:

private:
	friend class C3SymbolScope;
	
	// SymbolScope uses this from functions that are triggered from our own code
	C3SymbolScope * globalScopeForLanguage(C3Symbol::Language eLang);

private:

	// friend: called by C3SymbolManagerCTagsWorker (on a slave thread!)
	void CTagsWorkerTerminated(C3SymbolManagerCTagsWorker * pWorker,const QString &szError,QHash<QString,C3SymbolFile *> * pSymbolFiles);

private:
	virtual void processRequest(C3SymbolManagerRequest * pRequest);
	virtual void idleStep();

	void abortAllCTagsWorkers();

	void processIdentifyContextRequest(C3SymbolManagerRequestIdentifyContext * pRequest);
	void processResetSymbolStoreRequest(C3SymbolManagerRequestResetSymbolStore * pRequest);
	void processPerformCompletionRequest(C3SymbolManagerRequestPerformCompletion * pRequest);
	void processIdentifySymbolRequest(C3SymbolManagerRequestIdentifySymbol * pRequest);
	void processScanEditorFileRequest(C3SymbolManagerRequestScanEditorFile * pRequest);
	void processDescribeClassRequest(C3SymbolManagerRequestDescribeClass * pRequest);

	enum AddCompletionsOrSymbolInfosFlags
	{
		AllowEmptyText = 1,
		// This is used only for completions
		MatchInTheMiddle = 2,
		// Case insensitive works only with MatchInTheMiddle
		MatchCaseInsensitive = 4,
		// Withing the function definitions / prototypes, match only constructors
		MatchConstructorsOnly = 8
	};
	
	void storeLockedAddCompletions(
			QList<C3TextEditorCompletion *> * pCompletions,
			quint32 uTypeMask,
			C3SymbolMap * pSymbols,
			const QString &szToComplete,
			quint16 uScore,
			unsigned int uFlags = 0
		);

	void addKeywordCompletions(
			QList<C3TextEditorCompletion *> * pCompletions,
			const QStringList &lKeywords,
			const QString &szToComplete,
			quint16 uScore,
			unsigned int uFlags = 0
		);

	void storeLockedAddSymbolInfos(
			QList<C3TextEditorSymbolInfo *> * pInfoList,
			C3TextEditorRange &oRange,
			quint32 uTypeMask,
			C3SymbolMap * pSymbols,
			const QString &szIdentifier,
			quint16 uScore,
			unsigned int uFlags = 0,
			bool bAddSymbolPointers = false
		);

	C3SymbolScope * storeLockedResolveCppArrowOrDotTypeScope(
			C3SymbolFile * pFile,
			C3TextEditorSymbolContext * pContext,
			C3SymbolScope * pFileContext,
			C3SymbolScope * pGlobalScope
		);

	/*
	C3SymbolScope * storeLockedResolveDoubleColonTypeScope(
			C3SymbolFile * pFile,
			C3TextEditorSymbolContext * pContext,
			C3SymbolScope * pFileContext,
			C3SymbolScope * pGlobalScope
		);
	*/
	
	bool storeLockedSelectSymbolsForCpp(
			C3SymbolFile * pFile,
			C3TextEditorSymbolContext * pContext,
			C3SymbolScope * pFileContext,
			C3SymbolScope * pGlobalScope,
			std::function<void(quint32,C3SymbolMap *,const QString &,quint16,unsigned int)> fnSymbolCallback,
			unsigned int uBaseFlags
		);

	void storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInScope(
			C3SymbolFile * pFile,
			C3TextEditorSymbolContext * pContext,
			C3SymbolScope * pClassScope,
			C3SymbolScope * pGlobalScope,
			std::function<void(quint32,C3SymbolMap *,const QString &,quint16,unsigned int)> fnSymbolCallback,
			quint32 uTypeMask,
			unsigned int uBaseFlags
		);

	void storeLockedSelectSymbolsForCppCompleteVisibleSymbolsInBaseClasses(
			QList<C3SymbolClass *> * pBaseClasses,
			C3TextEditorSymbolContext * pContext,
			std::function<void(quint32,C3SymbolMap *,const QString &,quint16,unsigned int)> fnSymbolCallback,
			quint32 uTypeMask,
			quint16 uScore,
			unsigned int uFlags
		);
	
	// Resolved filtered (non template, non const and other stuff) types
	C3SymbolScope * storeLockedResolveSimpleCppTypeToSymbolScope(
			C3SymbolFile * pFile,
			const QString & szType,
			C3SymbolScope * pTypeScope,
			bool bMayBeNamespace = false
		);

	C3SymbolScope * storeLockedResolveGenericCppTypeToSymbolScope(
			C3SymbolFile * pFile,
			const QString & szUnfilteredType,
			C3SymbolScope * pTypeScope,
			bool bMayBeNamespace = false
		);

	C3SymbolScope * resolveP(
			C3SymbolFile * pFile,
			C3SymbolScope * pFileContext,
			C3SymbolScope * pGlobalScope
		);
	

	void buildCPPKeywordList();
	void buildPHPKeywordList();

	C3SymbolManagerClassDescription * storeLockedDescribeClass(const QString &szFullClassName,C3Symbol::Language eLanguage);
	C3SymbolManagerClassDescription * storeLockedDescribeClass(C3SymbolClass * pClass);

}; // class C3SymbolManager

#endif //!_C3SymbolManager_h_
