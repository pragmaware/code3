#ifndef _C3SymbolManagerRequestScanEditorFile_h_
#define _C3SymbolManagerRequestScanEditorFile_h_
//=============================================================================
//
//   File : C3SymbolManagerRequestScanEditorFile.h
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

#include "c3_config.h"

#include "C3SymbolManagerRequest.h"

class C3SymbolManagerRequestScanEditorFile : public C3SymbolManagerRequest
{
public:
	C3SymbolManagerRequestScanEditorFile(int iEditorId,bool bWaitForScanToFinish);
	virtual ~C3SymbolManagerRequestScanEditorFile();

private:

	int m_iEditorId;
	bool m_bWaitForScanToFinish;

public:

	int editorId() const
	{
		return m_iEditorId;
	}

	bool waitForScanToFinish() const
	{
		return m_bWaitForScanToFinish;
	}

}; // class C3SymbolManagerRequestScanEditorFile

#endif //!_C3SymbolManagerRequestScanEditorFile_h_