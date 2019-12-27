#ifndef _C3SymbolManagerRequestPerformCompletion_h_
#define _C3SymbolManagerRequestPerformCompletion_h_
//=============================================================================
//
//   File : C3SymbolManagerRequestPerformCompletion.h
//   Creation Date : 2015/12/10 23:23:00
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

#include "C3SymbolManagerRequest.h"
#include "C3TextEditorSymbolContext.h"
#include "C3TextEditorCompletion.h"
#include "C3Symbol.h"

#include <QList>

class C3SymbolManagerRequestPerformCompletion : public C3SymbolManagerRequest
{
	Q_OBJECT
private:
	C3TextEditorSymbolContext * m_pContext;
	QList<C3TextEditorCompletion *> * m_pCompletions;
	C3Symbol::Language m_eLanguage;
	QString m_szFilePath;
	// C3TextEditorMode::SymbolCompletionFlags
	unsigned int m_uFlags;
public:
	C3SymbolManagerRequestPerformCompletion(
			const QString &szFilePath,
			C3TextEditorSymbolContext * pContext,
			C3Symbol::Language eLanguage,
			// C3TextEditorMode::SymbolCompletionFlags
			unsigned int uFlags
		);
	virtual ~C3SymbolManagerRequestPerformCompletion();

public:

	// C3TextEditorMode::SymbolCompletionFlags
	unsigned int flags() const
	{
		return m_uFlags;
	}

	const QString & filePath() const
	{
		return m_szFilePath;
	}

	C3Symbol::Language language() const
	{
		return m_eLanguage;
	}

	C3TextEditorSymbolContext * context()
	{
		return m_pContext;
	}

	QList<C3TextEditorCompletion *> * takeCompletions()
	{
		QList<C3TextEditorCompletion *> * pList = m_pCompletions;
		m_pCompletions = NULL;
		return pList;
	}

	void setCompletions(QList<C3TextEditorCompletion *> * pCompletions)
	{
		if(m_pCompletions)
			delete m_pCompletions;
		m_pCompletions = pCompletions;
	}

}; // class C3SymbolManagerRequestPerformCompletion

#endif //!_C3SymbolManagerRequestPerformCompletion_h_