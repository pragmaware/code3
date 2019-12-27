#ifndef _C3TextEditorModeWithSymbolAnalysis_h_
#define _C3TextEditorModeWithSymbolAnalysis_h_
//=============================================================================
//
//   File : C3TextEditorModeWithSymbolAnalysis.h
//   Creation Date : 2015/12/10 22:05:50
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

#include "C3TextEditorModeWithInterLineState.h"
#include "C3Symbol.h"

class C3TextEditorModeWithSymbolAnalysisPrivate;
class C3SymbolManagerRequest;
class C3TextEditorSymbolContext;

class C3TextEditorModeWithSymbolAnalysis : public C3TextEditorModeWithInterLineState
{
	Q_OBJECT
public:
	C3TextEditorModeWithSymbolAnalysis(
			C3TextEditorFactory * pFactory,
			const QString &szId,
			const QString &szName,
			unsigned int uInterLineFlagMask,
			C3Symbol::Language eLanguage
		);
	virtual ~C3TextEditorModeWithSymbolAnalysis();

private:

	C3TextEditorModeWithSymbolAnalysisPrivate * m_pTEMWSA;

protected:

	virtual bool textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		);
	virtual bool textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		);

	virtual void fileSaved(bool bWasModified);

	virtual void symbolCompletion(C3TextEditorSymbolContext * pContext,unsigned int uFlags);
	virtual void symbolIdentification(C3TextEditorSymbolContext * pContext,unsigned int uFlags);

	virtual void triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags);
	virtual bool triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags);

	void triggerRescanFile();

private slots:

	void slotPerformCompletionRequestTerminated(C3SymbolManagerRequest * pRequest);
	void slotIdentifySymbolRequestTerminated(C3SymbolManagerRequest * pRequest);
	void slotScanEditorFileRequestTerminated(C3SymbolManagerRequest * pRequest);

}; // class C3TextEditorModeWithSymbolAnalysis

#endif //!_C3TextEditorModeWithSymbolAnalysis_h_