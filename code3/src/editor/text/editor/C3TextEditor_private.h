#ifndef _C3TextEditorPrivate_h_
#define _C3TextEditorPrivate_h_
//=============================================================================
//
//   File : C3TextEditorPrivate.h
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

#include <QString>
#include <QList>
#include <QLinkedList>
#include <QMutex>
#include <QHash>
#include <QVariant>

#include "C3TextEditorCoreData.h"
#include "C3TextEditorRowColumn.h"
#include "C3TextEditorRange.h"
#include "C3Link.h"

class C3TextEditorFormat;
class C3TextEditorFormatReader;
class C3TextEditor;
class C3TextEditorViewport;
class C3TextEditorMargin;
class C3TextEditorMode;
class C3TextEditorUndo;
class C3TextEditorCompletionWidget;
class C3TextEditorFactory;

class QScrollBar;
class QTimer;

#define MAXIMUM_LINE_WIDTH_INVALID 9999999.0
#define MAXIMUM_LINE_WIDTH_IS_VALID(_pPrivate) (_pPrivate->fMaximumLineWidth < MAXIMUM_LINE_WIDTH_INVALID)
#define MAXIMUM_LINE_WIDTH_IS_INVALID(_pPrivate) (_pPrivate->fMaximumLineWidth >= MAXIMUM_LINE_WIDTH_INVALID)

class C3TextEditorPrivate : public C3TextEditorCoreData
{
public:
	C3Link oLink;

	// UI
	C3TextEditorViewport * pViewport;
	C3TextEditorMargin * pMargin;
	C3TextEditorFactory * pFactory;
	
	C3TextEditorCompletionWidget * pCompletionWidget;
	C3TextEditorRange oCompletionRange;
	
	QScrollBar * pVScrollBar;
	QScrollBar * pHScrollBar;

	C3TextEditorFormat * pFormat;
	C3TextEditorMode * pMode;

	QString szCursorPositionText;
	QString szModeNameText;
	
	QString szCachedRelatedFilePath;
	bool bCheckedRelatedFile;

	// Reader stuff, shared by multiple threads, protected by a mutex
	QMutex oReaderMutex;
	C3TextEditorFormatReader * pReader; // note that this is async: checking it to see if a read is running is NOT safe.
	QString szReadingError; // not empty if reading failed at all
	bool bContentWasAlteredDuringRead;
	QString szReadAlterationReason;

	// The name and "direction" of the variable is a bit wrong.
	// It is more "reading process is not running" and it is valid on the UI thread.
	bool bReadingFinished;
	// lines received from the reader
	QLinkedList<C3TextEditorLine *> lReaderLines;

	bool bIgnoreScrollBarChanges;
	
	int iScrollBarSize;
	// The margin at the left of the text lines
	int iLeftMargin;

	// The maximum line width. It's valid if smaller than MAXIMUM_LINE_WIDTH_INVALID.
	// or if MAXIMUM_LINE_WIDTH_IS_VALID() returns true.
	// If it's valid then all editor modifications should try to
	// take care of it as long as it doesn't require considerable effort.
	// If an editor modification algorithm finds out that a line with maximum width was
	// shortened or removed and the computation of the new maximum requires
	// rescanning the whole buffer then it should set fMaximumLineWidth to
	// MAXIMUM_LINE_WIDTH_INVALID (which is a very large number).
	// In this way the layout() algorithm will update it automatically.
	qreal fMaximumLineWidth;

	// recomputed or (possibly) changed by layout()
	int iTopLine;
	int iXOffset;

	// Recomputed by layout()
	int iViewportWidth;
	int iViewportHeight;
	
	int iContentsWidth;
	int iContentsHeight;
	
	int iVisibleLineCount;
	
	
	C3TextEditorRowColumn oCursor;
	C3TextEditorRowColumn oSelectionAnchor;
	C3TextEditorRange oSelection;
	
	C3TextEditorRange oMark;
	
	// This is used to preserve the X cursor coordinate while moving up or down.
	// If it's -1 then it's assumed to be invalid.
	// It's always invalidated in endVisualStateChange() unless a special flag is passed (in cursorUp() and cursorDown())
	qreal fCursorUpDownX;

	QLinkedList<C3TextEditorUndo *> lUndoStack;
	C3TextEditorUndo * pMultiUndo;

	QTimer * pMouseScrollTimer;

	QTimer * pCursorTimer;
	bool bCursorOn;

	// Find
	int iCurrentFindOperationId;
	C3TextEditorRowColumn oCurrentFindOperationEndPosition;
	bool bCurrentFindOperationFinishedStartRow;

	// Visual state change state
	C3TextEditorRowColumn oVisualStateChangeInitialCursor;
	C3TextEditorRange oVisualStateChangeInitialSelection;
	C3TextEditorRange oVisualStateChangeInitialMark;
	int iVisualStateChangeInitialLineCount;
	qreal fVisualStateChangeInitialMaximumLineWidth;
	int iVisualStateChangeStackCount;
	bool bVisualStateChangeForceFullRepaint;
	bool bCursorMovedWhileHidden;
	
	// identify link state
	bool bJumpToLinkAfterIdentification;
};

#define _p m_pC3TE

#endif //!_C3TextEditorPrivate_h_
