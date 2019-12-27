//=============================================================================
//
//   File : C3SymbolNamespace.cpp
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

#include "C3SymbolNamespace.h"

void C3SymbolNamespace::clear()
{
	C3SymbolScope::clear();
	m_lAdditionalDefinitions.clear();
}

QString C3SymbolNamespace::description() const
{
	switch(language())
	{
		case Cpp:
		{
			if(unresolvedScope().isEmpty())
				return __utf8("namespace %1").arg(identifier());
			else
				return __utf8("namespace %1::%2").arg(unresolvedScope()).arg(identifier());
		}
		break;
		default:
			return C3Symbol::description();
		break;
	}
}

