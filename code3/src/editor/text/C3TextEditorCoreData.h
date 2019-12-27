#ifndef _C3TextEditorCoreData_h_
#define _C3TextEditorCoreData_h_
//=============================================================================
//
//   File : C3TextEditorCoreData.h
//   Creation Date : sab 24 ott 2015 01:11:02
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

#include <QList>
#include <QMutex>

#include "C3TextEditorLine.h"

class QScrollBar;
class QFontMetricsF;

class C3TextEditorOptions;
class C3TextEditor;
class C3TextEditorLinesLocker;

//
// This class is used as base class of C3TextEditorPrivate
// and is also passed to the mode functions.
//
class C3TextEditorCoreData
{
	friend class C3TextEditorLinesLocker;
protected:
	// This mutex MUST be locked() in the following cases:
	// - when modifying the lLines array (adding/removing lines)
	// - when modifying the szText member of one of the lines
	// - when reading the lLines array from a thread that is not the main one.
	// Other rules:
	// - The lLines array must never be modified by a thread that is not
	//   the main one. This means that locking when reading the lines from the main
	//   thread is not required as these reads will never be concurrent to a modification.
	// - use the lock in the function you directly access lLines buffer for modification
	// - modifying/reading the blocks, the flags or the other parameters of a line
	//   in a slave thread is forbidden. This also means that there is no need to
	//   lock on such operations.
	// Use C3TextEditorLinesLocker helper when you need to lock.
	//
	// This mutex is recursive.
	QMutex lLinesMutex;

public:
	// The options
	C3TextEditorOptions * pOptions;
	// The editor
	C3TextEditor * pEditor;
	
	// These are copied from options and modified "on the fly"
	int iAdditionalLineSpacing;
	int iTabStopSize;

	// The lines of the editor
	// WARNING: Protected by lLinesMutex above. Read the comments.
	QList<C3TextEditorLine *> lLines;

	// font metrics related, recomputed by computeFontMetrics()
	QFontMetricsF * pFontMetrics;

	// Y dimensions are integer (we always use integer values)
	// X dimensions are float as integer version of QFontMetrics uses rounding
	// and it screws up our width calculations (sum of two rounded widths is not the same as the rounded width of sum)
	int iFontAscent; // height above baseline
	int iFontDescent; // height below baseline
	int iFontHeight; // iFontAscent + iFontDescent + 1 (+1 is for the baseline)
	int iFontLeading; // inter-line spacing
	int iFontLineSpacing; // iFontHeight + iFontLeading
	qreal fFontTabStopWidth; // width of N spaces
	qreal fFontElisionWidth; // with of an elision (three dots unicode character: …)
	qreal fFontZeroSequenceWidths[10]; // width of sequences of zeroes (0,00,000,0000...)
	
public:
	C3TextEditorCoreData()
		: lLinesMutex(QMutex::Recursive)
	{
	}
};

class C3TextEditorLinesLocker
{
private:
	C3TextEditorCoreData * m_pCoreData;
public:
	C3TextEditorLinesLocker(C3TextEditorCoreData * pCoreData)
		: m_pCoreData(pCoreData)
	{
		m_pCoreData->lLinesMutex.lock();
	}

	~C3TextEditorLinesLocker()
	{
		m_pCoreData->lLinesMutex.unlock();
	}
};

#endif //!_C3TextEditorCoreData_h_
