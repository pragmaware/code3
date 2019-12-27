#ifndef _C3CompletionStore_h_
#define _C3CompletionStore_h_
//=============================================================================
//
//   File : C3CompletionStore.h
//   Creation Date : 2015/11/14 02:45:31
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

class C3CompletionStorePrivate;

#include <QString>
#include <QStringList>

class C3CompletionStore
{
public:
	C3CompletionStore();
	~C3CompletionStore();

private:
	C3CompletionStorePrivate * m_pC3CS;

public:

	void clear();

	void addCompletion(const QString &szText);

	void getCompletions(const QString &szText,QStringList &lBuffer);
	bool findExact(const QString &szText);

}; // class C3CompletionStore

#endif //!_C3CompletionStore_h_