#ifndef _C3SymbolClass_h_
#define _C3SymbolClass_h_
//=============================================================================
//
//   File : C3SymbolClass.h
//   Creation Date : 2015/12/09 20:42:37
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

#include "C3SymbolScope.h"

class C3SymbolClass : public C3SymbolScope
{
private:
	QString m_szBaseClassNames;
	QList<C3SymbolClass *> * m_pBaseClasses;
	bool m_bBaseClassResolutionAttempted;

public:
	C3SymbolClass(
			C3SymbolFile * pFile,
			Language eLanguage,
			const QString &szIdentifier,
			const QString &szBaseClassNames, // may contain multiple base classes
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber
		) : C3SymbolScope(
			pFile,
			eLanguage,
			Class,
			szIdentifier,
			szScope,
			eAccess,
			uLineNumber,
			uEndLineNumber
		), m_szBaseClassNames(szBaseClassNames),
		m_pBaseClasses(NULL),
		m_bBaseClassResolutionAttempted(false)
	{
		
	}
	
	virtual ~C3SymbolClass()
	{
		if(m_pBaseClasses)
			delete m_pBaseClasses;
	}

public:

	virtual void clear();

	inline const QString & baseClassNames() const
	{
		return m_szBaseClassNames;
	}

	inline QList<C3SymbolClass *> * baseClasses()
	{
		if(m_pBaseClasses)
			return m_pBaseClasses;
		if(m_bBaseClassResolutionAttempted)
			return NULL;
		return resolveBaseClasses();
	}

	virtual QString description() const;

private:
	QList<C3SymbolClass *> * resolveBaseClasses();

	QStringList splitBaseClassList(const QString &szClasses);

}; // class C3SymbolClass

#endif //!_C3SymbolClass_h_