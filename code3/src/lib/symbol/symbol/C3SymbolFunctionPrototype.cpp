//=============================================================================
//
//   File : C3SymbolFunctionPrototype.cpp
//   Creation Date : 2015/12/05 05:11:35
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

#include "C3SymbolFunctionPrototype.h"


QString C3SymbolFunctionPrototype::description() const
{
	switch(language())
	{
		case Cpp:
		{
			QString szSig;
			if(returnType().isEmpty())
			{
				if(unresolvedScope().isEmpty())
					szSig = __utf8("%1(%2)").arg(identifier()).arg(signature());
				else
					szSig = __utf8("%1::%2(%3)").arg(unresolvedScope()).arg(identifier()).arg(signature());
			} else {
				if(unresolvedScope().isEmpty())
					szSig = __utf8("%1 %2(%3)").arg(returnType()).arg(identifier()).arg(signature());
				else
					szSig = __utf8("%1 %2::%3(%4)").arg(returnType()).arg(unresolvedScope()).arg(identifier()).arg(signature());
			}
			
			if(functionFlags() & FunctionIsConst)
				szSig += __ascii(" const");
			
			return szSig;
		}
		break;
		default:
			return C3Symbol::description();
		break;
	}
}

QString C3SymbolFunctionPrototype::completionKey() const
{
	switch(language())
	{
		case Cpp:
			return __ascii("%1(%2)").arg(identifier()).arg(signature());
		break;
		default:
			return C3Symbol::completionKey();
		break;
	}
}
