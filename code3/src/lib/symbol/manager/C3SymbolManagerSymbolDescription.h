#ifndef _C3SymbolManagerSymbolDescription_h_
#define _C3SymbolManagerSymbolDescription_h_
//=============================================================================
//
//   File : C3SymbolManagerSymbolDescription.h
//   Creation Date : 2016/09/22 03:10:00
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

#include <QString>

#include "C3Symbol.h"

#include "C3SymbolManagerClassDescription.h"

class C3SymbolManagerSymbolDescription
{
public:
	C3Symbol::Type eSymbolType;

	QString szIdentifier;
	QString szScope;
	
	QString szFileName;

	unsigned int uStartLine;
	unsigned int uEndLine;

	// The class description, if the symbol is a class, or the containin class, if any
	C3SymbolManagerClassDescription * pContainingClass;

public:
	C3SymbolManagerSymbolDescription()
		: pContainingClass(NULL)
	{
	}
	
	~C3SymbolManagerSymbolDescription()
	{
		if(pContainingClass)
			delete pContainingClass;
	}
	
	C3SymbolManagerClassDescription * takeContainingClass()
	{
		C3SymbolManagerClassDescription * pClass = pContainingClass;
		pContainingClass = NULL;
		return pClass;
	}

}; // class C3SymbolManagerSymbolDescription

#endif //!_C3SymbolManagerSymbolDescription_h_