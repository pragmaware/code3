#ifndef _C3Process_h_
#define _C3Process_h_
//=============================================================================
//
//   File : C3Process.h
//   Creation Date : 2015/11/20 22:18:01
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

#include <QProcess>

class C3Process : public QProcess
{
	Q_OBJECT
public:
	C3Process();
	virtual ~C3Process();

protected:
	virtual void setupChildProcess();

}; // class C3Process

#endif //!_C3Process_h_