//=============================================================================
//
//   File : c3_main.cpp
//   Creation Date : mar 20 ott 2015 00:04:46
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

#include "C3Application.h"

int main(int argc,char ** argv)
{
	C3Application app(argc,argv);

	if(!app.init(argc,argv))
		return -1;
	
	return app.exec();
}
