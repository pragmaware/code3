//=============================================================================
//
//   File : C3SymbolVariable.cpp
//   Creation Date : 2015/12/05 04:20:01
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

#include "C3SymbolVariable.h"


QString C3SymbolVariable::description() const
{
	switch(language())
	{
		case Cpp:
		{
			QString szSig;
			
			if(typeName().isEmpty())
			{
				if(unresolvedScope().isEmpty())
					szSig = __utf8("<?> %1").arg(identifier());
				else
					szSig = __utf8("<?> %1::%2").arg(unresolvedScope()).arg(identifier());
			} else {
				if(unresolvedScope().isEmpty())
					szSig = __utf8("%1 %2").arg(typeName()).arg(identifier());
				else
					szSig = __utf8("%1 %2::%3").arg(typeName()).arg(unresolvedScope()).arg(identifier());
			}
			
			return szSig;
		}
		break;
		default:
			return C3Symbol::description();
		break;
	}
}

