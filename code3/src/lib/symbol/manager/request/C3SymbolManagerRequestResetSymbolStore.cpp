//=============================================================================
//
//   File : C3SymbolManagerRequestResetSymbolStore.cpp
//   Creation Date : 2015/12/10 19:40:53
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

#include "C3SymbolManagerRequestResetSymbolStore.h"

C3SymbolManagerRequestResetSymbolStore::C3SymbolManagerRequestResetSymbolStore(const QStringList &lRescanPaths,const QStringList &lExclusionPatterns,unsigned int uFlags)
	: C3SymbolManagerRequest(ResetSymbolStore),
		m_lRescanPaths(lRescanPaths),
		m_lExclusionPatterns(lExclusionPatterns),
		m_uFlags(uFlags)
{
}

C3SymbolManagerRequestResetSymbolStore::~C3SymbolManagerRequestResetSymbolStore()
{
}


