//=============================================================================
//
//   File : C3SymbolTypeDefinition.cpp
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

#include "C3SymbolTypeDefinition.h"
#include "C3SymbolManager.h"
#include "C3SymbolScope.h"


QString C3SymbolTypeDefinition::description() const
{
	switch(language())
	{
		case Cpp:
		{
			QString szSig;

			if(typeName().isEmpty())
			{
				if(unresolvedScope().isEmpty())
					szSig = __utf8("typedef <?> %1").arg(identifier());
				else
					szSig = __utf8("typedef <?> %1::%2").arg(unresolvedScope()).arg(identifier());
			} else {
				if(unresolvedScope().isEmpty())
					szSig = __utf8("typedef %1 %2").arg(typeName()).arg(identifier());
				else
					szSig = __utf8("typedef %1 %2::%3").arg(typeName()).arg(unresolvedScope()).arg(identifier());
			}

			return szSig;
		}
		break;
		default:
			return C3Symbol::description();
		break;
	}
}

C3SymbolScope * C3SymbolTypeDefinition::resolve()
{
	Q_ASSERT(!m_bResolutionAttempted);
	Q_ASSERT(!m_szTypeName.isEmpty());
	
	m_bResolutionAttempted = true;

	C3SymbolScope * pScope = resolvedContainingScope();
	if(!pScope)
	{
		qDebug("[C3SymbolTypeDefinition::resolve] WARNING: Typedef %s->%s has no containing scope",identifier().toUtf8().data(),m_szTypeName.toUtf8().data());
		return NULL;
	}
	
	m_pResolvedScope = pScope->resolveTypeToScope(C3SymbolManager::instance()->filterCppType(m_szTypeName));

	if(!m_pResolvedScope)
		qDebug("[C3SymbolTypeDefinition::resolve] WARNING: Failed to resolve typedef %s->%s",identifier().toUtf8().data(),m_szTypeName.toUtf8().data());
	
	return m_pResolvedScope;
}

void C3SymbolTypeDefinition::clear()
{
	C3Symbol::clear();
	m_bResolutionAttempted = false;
	m_pResolvedScope = NULL;
}