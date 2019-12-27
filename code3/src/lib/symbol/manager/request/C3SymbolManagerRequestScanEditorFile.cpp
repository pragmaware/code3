//=============================================================================
//
//   File : C3SymbolManagerRequestScanEditorFile.cpp
//   Creation Date : 2015/12/25 02:00:30
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

#include "C3SymbolManagerRequestScanEditorFile.h"

C3SymbolManagerRequestScanEditorFile::C3SymbolManagerRequestScanEditorFile(int iEditorId,bool bWaitForScanToFinish)
	: C3SymbolManagerRequest(ScanEditorFile), m_iEditorId(iEditorId), m_bWaitForScanToFinish(bWaitForScanToFinish)
{
}

C3SymbolManagerRequestScanEditorFile::~C3SymbolManagerRequestScanEditorFile()
{
}


