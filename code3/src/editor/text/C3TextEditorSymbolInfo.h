#ifndef _C3TextEditorSymbolInfo_h_
#define _C3TextEditorSymbolInfo_h_
//=============================================================================
//
//   File : C3TextEditorSymbolInfo.h
//   Creation Date : 2015/12/14 23:08:28
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

#include "C3TextEditorRange.h"
#include "C3Link.h"

class C3Symbol;

#include <QString>

class C3TextEditorSymbolInfo
{
public:
	C3TextEditorRange oRange;
	C3Link oLink;
	// This is set only in synchronous C3SymbolManager requests
	// and can be used only if the symbol store has been locked
	C3Symbol * pSymbol;
}; // class C3TextEditorSymbolInfo

#endif //!_C3TextEditorSymbolInfo_h_