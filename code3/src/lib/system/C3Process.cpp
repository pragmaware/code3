//=============================================================================
//
//   File : C3Process.cpp
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

#include "C3Process.h"

#include <cstdio>

C3Process::C3Process()
	: QProcess()
{
}

C3Process::~C3Process()
{
}

void C3Process::setupChildProcess()
{
	// disable buffering on stdout/stderr
	std::setvbuf(stdout,NULL,_IONBF,0);
	std::setvbuf(stderr,NULL,_IONBF,0);
}
