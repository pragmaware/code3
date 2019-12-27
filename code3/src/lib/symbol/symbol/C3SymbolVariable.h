#ifndef _C3SymbolVariable_h_
#define _C3SymbolVariable_h_
//=============================================================================
//
//   File : C3SymbolVariable.h
//   Creation Date : 2015/12/05 04:20:01
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

#include "C3Symbol.h"

class C3SymbolVariable : public C3Symbol
{
private:
	QString m_szTypeName;
public:
	C3SymbolVariable(
			C3SymbolFile * pFile,
			Language eLanguage,
			Type eType,
			const QString &szIdentifier,
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber,
			const QString &szTypeName
		) : C3Symbol(
			pFile,
			eLanguage,
			eType,
			szIdentifier,
			szScope,
			eAccess,
			uLineNumber,
			uEndLineNumber
		), m_szTypeName(szTypeName)
	{
	}
	
	virtual ~C3SymbolVariable()
	{
	}

public:

	inline const QString & typeName() const
	{
		return m_szTypeName;
	}

	virtual QString description() const;

}; // class C3SymbolVariable

typedef QList<C3SymbolVariable *> C3SymbolVariableList;

#endif //!_C3SymbolVariable_h_
