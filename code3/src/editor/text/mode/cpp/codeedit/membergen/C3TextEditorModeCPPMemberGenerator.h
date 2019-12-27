#ifndef _C3TextEditorModeCPPMemberGenerator_h_
#define _C3TextEditorModeCPPMemberGenerator_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPMemberGenerator.h
//   Creation Date : 2016/09/23 03:27:25
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

class C3TextEditorModeCPPMemberGenerator
{
public:
	enum Location
	{
		AtCursorPosition,
		InContextContainerDeclaration,
		InContextContainerDeclarationAndDefinition
	};

public:
	C3TextEditorModeCPPMemberGenerator();
	virtual ~C3TextEditorModeCPPMemberGenerator();

}; // class C3TextEditorModeCPPMemberGenerator

#endif //!_C3TextEditorModeCPPMemberGenerator_h_