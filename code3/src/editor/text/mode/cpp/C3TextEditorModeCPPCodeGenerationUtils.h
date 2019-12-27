#ifndef _C3TextEditorModeCPPCodeGenerationUtils_h_
#define _C3TextEditorModeCPPCodeGenerationUtils_h_
//=============================================================================
//
//   File : C3TextEditorModeCPPCodeGenerationUtils.h
//   Creation Date : 2016/09/22 04:08:09
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

class C3TextEditor;
class C3SymbolManagerClassDescription;

namespace C3TextEditorModeCPPCodeGenerationUtils
{
	enum InsertCodeInClassLocation
	{
		BestLocationForMemberVariables,
		BestLocationForMemberFunctionPrototypes,
		BestLocationForMemberFunctionDefinitions
	};

	bool insertCodeInClass(
			C3SymbolManagerClassDescription * pClass,
			InsertCodeInClassLocation eLocation,
			C3Symbol::AccessLevel eAccessLevel,
			const QString &szCode
		);

	bool insertIncludeLine(
			C3TextEditor * pEditor,
			const QString &szLine
		);

} // namespace C3TextEditorModeCPPCodeGenerationUtils

#endif //!_C3TextEditorModeCPPCodeGenerationUtils_h_