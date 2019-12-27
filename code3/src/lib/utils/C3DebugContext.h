#ifndef _C3DebugContext_h_
#define _C3DebugContext_h_
//=============================================================================
//
//   File : C3DebugContext.h
//   Creation Date : 2018/01/21 01:37:05
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

class C3DebugContextPrivate;

class C3DebugContext
{
public:
	C3DebugContext(const char * szContext,...);
	~C3DebugContext();
private:
	C3DebugContextPrivate * m_p;
public:
	static void trace(const char * szFmt,...);

}; // class C3DebugContext

#endif //!_C3DebugContext_h_