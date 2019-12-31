#ifndef _C3SymbolFunctionSignature_h_
#define _C3SymbolFunctionSignature_h_
//=============================================================================
//
//   File : C3SymbolFunctionSignature.h
//   Creation Date : 2015/12/06 04:10:48
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

#include <QString>

class C3SymbolFunctionSignature
{
public:
	enum Flags
	{
		FunctionIsConst = 1,
		FunctionIsVirtual = 2,
		FunctionIsStatic = 4
	};
private:
	QString m_szSignature;
	QString m_szReturnType;
	quint8 m_uFunctionFlags;
public:
	C3SymbolFunctionSignature(
			const QString &szSignature,
			const QString &szReturnType,
			quint8 uFunctionFlags
		)
		: m_szSignature(szSignature),
		m_szReturnType(szReturnType),
		m_uFunctionFlags(uFunctionFlags)
	{
	}
	
	~C3SymbolFunctionSignature()
	{
	}

public:

	inline quint8 functionFlags() const
	{
		return m_uFunctionFlags;
	}

	inline const QString & returnType() const
	{
		return m_szReturnType;
	}

	inline const QString & signature() const
	{
		return m_szSignature;
	}

}; // class C3SymbolFunctionSignature

#endif //!_C3SymbolFunctionSignature_h_
