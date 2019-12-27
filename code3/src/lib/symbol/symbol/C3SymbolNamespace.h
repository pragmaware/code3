#ifndef _C3SymbolNamespace_h_
#define _C3SymbolNamespace_h_
//=============================================================================
//
//   File : C3SymbolNamespace.h
//   Creation Date : 2015/12/06 04:24:20
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

#include <QList>

#include "C3SymbolScope.h"

class C3SymbolNamespace : public C3SymbolScope
{
public:
	C3SymbolNamespace(
			C3SymbolFile * pFile,
			Language eLanguage,
			const QString &szIdentifier,
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber
		) : C3SymbolScope(
			pFile,
			eLanguage,
			Namespace,
			szIdentifier,
			szScope,
			eAccess,
			uLineNumber,
			uEndLineNumber
		)
	{
	}
	
	virtual ~C3SymbolNamespace()
	{
	}

private:

	QList<C3SymbolNamespace *> m_lAdditionalDefinitions;

public:

	QList<C3SymbolNamespace *> & additionalDefinitions()
	{
		return m_lAdditionalDefinitions;
	}

	void addAdditionalDefinition(C3SymbolNamespace * pNs)
	{
		m_lAdditionalDefinitions.append(pNs);
	}

	virtual void clear();

	virtual QString description() const;


}; // class C3SymbolNamespace

#endif //!_C3SymbolNamespace_h_
