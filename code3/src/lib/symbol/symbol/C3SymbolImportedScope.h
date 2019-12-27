#ifndef _C3SymbolImportedScope_h_
#define _C3SymbolImportedScope_h_
//=============================================================================
//
//   File : C3SymbolImportedScope.h
//   Creation Date : 2016/04/30 02:48:44
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

#include "C3Symbol.h"

class C3SymbolImportedScope : public C3Symbol
{
private:
	C3SymbolScope * m_pResolvedScope;

public:
	C3SymbolImportedScope(
			C3SymbolFile * pFile,
			Language eLanguage,
			Type eType,
			const QString &szIdentifier,
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber
		)
		: C3Symbol(
			pFile,
			eLanguage,
			eType,
			szIdentifier,
			szScope,
			eAccess,
			uLineNumber,
			uEndLineNumber
		), m_pResolvedScope(NULL)
	{
	}

	virtual ~C3SymbolImportedScope()
	{
	}

public:

	virtual void clear();
	
	void setResolvedScope(C3SymbolScope * pScope)
	{
		m_pResolvedScope = pScope;
	}

}; // class C3SymbolImportedScope

#endif //!_C3SymbolImportedScope_h_