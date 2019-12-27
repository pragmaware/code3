#ifndef _C3SymbolManagerRequestIdentifySymbol_h_
#define _C3SymbolManagerRequestIdentifySymbol_h_
//=============================================================================
//
//   File : C3SymbolManagerRequestIdentifySymbol.h
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
#include "C3TextEditorSymbolInfo.h"
#include "C3Symbol.h"

#include <QList>

class C3SymbolManagerRequestIdentifySymbol : public C3SymbolManagerRequest
{
	Q_OBJECT
public:
	enum Flags
	{
		// Do not look at the context.
		NoContextFilter = 1
	};
private:
	C3TextEditorSymbolContext * m_pContext;
	QList<C3TextEditorSymbolInfo *> * m_pInfoList;
	C3Symbol::Language m_eLanguage;
	unsigned int m_uFlags;
public:
	C3SymbolManagerRequestIdentifySymbol(
			C3TextEditorSymbolContext * pContext,
			C3Symbol::Language eLanguage,
			unsigned int uFlags
		);
	virtual ~C3SymbolManagerRequestIdentifySymbol();

public:

	unsigned int flags() const
	{
		return m_uFlags;
	}

	C3Symbol::Language language() const
	{
		return m_eLanguage;
	}

	C3TextEditorSymbolContext * context()
	{
		return m_pContext;
	}

	QList<C3TextEditorSymbolInfo *> * takeInfoList()
	{
		QList<C3TextEditorSymbolInfo  *> * pList = m_pInfoList;
		m_pInfoList = NULL;
		return pList;
	}

	void setInfoList(QList<C3TextEditorSymbolInfo *> * pInfoList)
	{
		if(m_pInfoList)
			delete m_pInfoList;
		m_pInfoList = pInfoList;
	}

}; // class C3SymbolManagerRequestIdentifySymbol

#endif //!_C3SymbolManagerRequestIdentifySymbol_h_