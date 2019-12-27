#ifndef _C3SymbolManagerRequestResetSymbolStore_h_
#define _C3SymbolManagerRequestResetSymbolStore_h_
//=============================================================================
//
//   File : C3SymbolManagerRequestResetSymbolStore.h
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

#include "c3_config.h"

#include "C3SymbolManagerRequest.h"

#include <QStringList>

class C3SymbolManagerRequestResetSymbolStore : public C3SymbolManagerRequest
{
	Q_OBJECT
public:
	enum RequestFlags
	{
		LimitRecursionDepthTo1 = 1
	};

	C3SymbolManagerRequestResetSymbolStore(const QStringList &lRescanPaths,const QStringList &lExclusionPatterns,unsigned int uFlags);
	virtual ~C3SymbolManagerRequestResetSymbolStore();

private:
	QStringList m_lRescanPaths;
	QStringList m_lExclusionPatterns;
	unsigned int m_uFlags;
	
public:

	unsigned int flags() const
	{
		return m_uFlags;
	}

	const QStringList & exclusionPatterns() const
	{
		return m_lExclusionPatterns;
	}

	const QStringList & rescanPaths() const
	{
		return m_lRescanPaths;
	}

}; // class C3SymbolManagerRequestResetSymbolStore

#endif //!_C3SymbolManagerRequestResetSymbolStore_h_