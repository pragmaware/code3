#ifndef _C3SymbolFunctionDefinition_h_
#define _C3SymbolFunctionDefinition_h_
//=============================================================================
//
//   File : C3SymbolFunctionDefinition.h
//   Creation Date : 2015/12/05 05:12:51
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

#include "C3SymbolFunctionSignature.h"
#include "C3SymbolScope.h"

class C3SymbolFunctionDefinition : public C3SymbolScope, public C3SymbolFunctionSignature
{
public:
	C3SymbolFunctionDefinition(
			C3SymbolFile * pFile,
			Language eLanguage,
			const QString &szIdentifier,
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber,
			const QString &szSignature,
			const QString &szReturnType,
			quint8 uFunctionFlags
		) : C3SymbolScope(
			pFile,
			eLanguage,
			FunctionDefinition,
			szIdentifier,
			szScope,
			eAccess,
			uLineNumber,
			uEndLineNumber
		), C3SymbolFunctionSignature(
			szSignature,
			szReturnType,
			uFunctionFlags
		)
	{
	}

	virtual ~C3SymbolFunctionDefinition()
	{
	}

public:

	virtual QString description() const;
	virtual QString completionKey() const;

}; // class C3SymbolFunctionDefinition

#endif //!_C3SymbolFunctionDefinition_h_
