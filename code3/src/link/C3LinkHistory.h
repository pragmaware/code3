#ifndef _C3LinkHistory_h_
#define _C3LinkHistory_h_
//=============================================================================
//
//   File : C3LinkHistory.h
//   Creation Date : 2015/11/28 02:44:53
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

class QMenu;
class QAction;

#include "C3Link.h"

class C3LinkHistoryPrivate;

class C3LinkHistory
{
public:
	C3LinkHistory();
	virtual ~C3LinkHistory();

private:
	static C3LinkHistory * m_pInstance;
	C3LinkHistoryPrivate * m_pC3LH;

public:
	static C3LinkHistory * instance()
	{
		return m_pInstance;
	}

	void addEntry(const C3Link &lnk);

	bool backAvailable() const;
	bool forwardAvailable() const;

	void back();
	void forward();

	void fillHistoryBackMenu(QMenu * pMenu);
	void fillHistoryForwardMenu(QMenu * pMenu);

	void backMenuActivated(QAction * a);
	void forwardMenuActivated(QAction * a);

}; // class C3LinkHistory

#endif //!_C3LinkHistory_h_