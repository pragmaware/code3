#ifndef _C3TextEditorModeASM_h_
#define _C3TextEditorModeASM_h_
//=============================================================================
//
//   File : C3TextEditorModeASM.h
//   Creation Date : 2018/02/15 01:25:06
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

#include "C3TextEditorModeWithInterLineState.h"

#include "C3TextEditorLine.h"
#include "C3TextEditorRange.h"

class C3TextEditorModeParserASM;
class C3TextEditorRowColumn;

class C3TextEditorModeASM : public C3TextEditorModeWithInterLineState
{
	Q_OBJECT
public:
	C3TextEditorModeASM(C3TextEditorFactory * pFactory);
	virtual ~C3TextEditorModeASM();

private:

	C3TextEditorModeParserASM * m_pParser;

public:
	virtual void computeMetadata();
	virtual void triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags);
	virtual bool triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags);
	virtual void fillCodeMenu(QMenu * pCodeMenu);
	virtual bool handleKeyEvent(QKeyEvent * e);

private:
	void completeIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock);
	void completePreprocessorToken(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock);
	bool identifyLinkToIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock);

}; // class C3TextEditorModeASM


#endif //!_C3TextEditorModeASM_h_