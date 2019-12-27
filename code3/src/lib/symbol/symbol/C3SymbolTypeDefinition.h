#ifndef _C3SymbolTypeDefinition_h_
#define _C3SymbolTypeDefinition_h_
//=============================================================================
//
//   File : C3SymbolTypeDefinition.h
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

class C3SymbolClass;

class C3SymbolTypeDefinition : public C3Symbol
{
private:
	QString m_szTypeName;
	C3SymbolScope * m_pResolvedScope;
	bool m_bResolutionAttempted;
public:
	C3SymbolTypeDefinition(
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
		), m_szTypeName(szTypeName),
		m_pResolvedScope(NULL),
		m_bResolutionAttempted(false)
	{
	}
	
	virtual ~C3SymbolTypeDefinition()
	{
	}

public:

	inline const QString & typeName() const
	{
		return m_szTypeName;
	}

	virtual QString description() const;
	
	// If the typedef resolved to a class/union/enumeration, it's here.
	// If it resolves to some POD type or something we can't figure out, it's NULL.
	C3SymbolScope * resolvedScope()
	{
		if(m_pResolvedScope)
			return m_pResolvedScope;
		if(m_bResolutionAttempted)
			return NULL;
		return resolve();
	}

	C3SymbolScope * resolve();
	
	virtual void clear();

}; // class C3SymbolTypeDefinition

typedef QList<C3SymbolTypeDefinition *> C3SymbolTypeDefinitionList;

#endif //!_C3SymbolTypeDefinition_h_
