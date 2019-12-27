//=============================================================================
//
//   File : C3TextEditor.cpp
//   Creation Date : gio 22 ott 2015 22:22:57
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

#include "C3TextEditor.h"
#include "C3TextEditor_private.h"

#include "C3TextEditorLine.h"
#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorMargin.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorViewport.h"

#include <QScrollBar>
#include <QFontMetricsF>
#include <QInputDialog>


void C3TextEditor::repaintCursorRow()
{
	repaintRow(_p->oCursor.row);
}

void C3TextEditor::handleCursorMoved(bool bExtendSelection,unsigned int uEndVisualStateChangeFlags)
{
	if(bExtendSelection)
	{
		_p->oSelection.set(_p->oSelectionAnchor,_p->oCursor);
	} else {
		if(!_p->oSelection.isEmpty())
			_p->oSelection.clear();
		_p->oSelectionAnchor = _p->oCursor;
	}

	endVisualStateChange(uEndVisualStateChangeFlags);
}

#define SANITIZE_CURSOR_COLUMN \
	{ \
		C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row); \
		Q_ASSERT(pLine); \
		if(_p->oCursor.col > pLine->szText.length()) \
			_p->oCursor.col = pLine->szText.length(); \
	}

#define SANITIZE_CURSOR_ROW \
	if(_p->oCursor.row >= _p->lLines.count()) \
		_p->oCursor.row = _p->lLines.count() - 1; \
	if(_p->oCursor.row < 0) \
		_p->oCursor.row = 0; \

#define BEGIN_MOVE_CURSOR \
	beginVisualStateChange(); \
	SANITIZE_CURSOR_ROW \
	if(_p->oSelection.isEmpty()) \
		_p->oSelectionAnchor = _p->oCursor;


void C3TextEditor::cursorLeft(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR
	SANITIZE_CURSOR_COLUMN

	if(_p->oCursor.col == 0)
	{
		if(_p->oCursor.row == 0)
		{
			endVisualStateChange();
			return; // can't be done
		}
		
		_p->oCursor.row--;

		C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);
		Q_ASSERT(pLine);
		
		_p->oCursor.col = pLine->szText.length();
	} else {
		_p->oCursor.col--;
	}

	handleCursorMoved(bExtendSelection);
}

void C3TextEditor::cursorRight(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR
	SANITIZE_CURSOR_COLUMN

	C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);

	if(_p->oCursor.col >= pLine->szText.length())
	{
		if(_p->oCursor.row >= (_p->lLines.count()-1))
		{
			endVisualStateChange();
			return; // can't be done
		}
		
		_p->oCursor.row++;
		_p->oCursor.col = 0;
	} else {
		_p->oCursor.col++;
	}
	
	handleCursorMoved(bExtendSelection);
}

void C3TextEditor::cursorUp(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR
	// do NOT sanitize the column!

	if(_p->oCursor.row == 0)
	{
		endVisualStateChange();
		return; // can't be done
	}
	
	if(_p->fCursorUpDownX < 0.0)
		_p->fCursorUpDownX = columnToAbsoluteXPosition(_p->oCursor.row,_p->oCursor.col);
	
	_p->oCursor.row--;

	int iCol;

	if(absoluteXPositionToColumn(_p->oCursor.row,&iCol,_p->fCursorUpDownX))
		_p->oCursor.col = iCol;

	handleCursorMoved(bExtendSelection,DontResetCursorUpDownX);
}

void C3TextEditor::cursorDown(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR
	// do NOT sanitize the column!

	if(_p->oCursor.row >= (_p->lLines.count()-1))
	{
		endVisualStateChange();
		return; // can't be done
	}
	
	if(_p->fCursorUpDownX < 0.0)
		_p->fCursorUpDownX = columnToAbsoluteXPosition(_p->oCursor.row,_p->oCursor.col);
	
	_p->oCursor.row++;

	int iCol;

	if(absoluteXPositionToColumn(_p->oCursor.row,&iCol,_p->fCursorUpDownX))
		_p->oCursor.col = iCol;

	handleCursorMoved(bExtendSelection,DontResetCursorUpDownX);
}

void C3TextEditor::cursorPageUp(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR
	// do NOT sanitize the column!

	if(_p->oCursor.row == 0)
	{
		endVisualStateChange();
		return; // can't be done
	}

	if(_p->fCursorUpDownX < 0.0)
		_p->fCursorUpDownX = columnToAbsoluteXPosition(_p->oCursor.row,_p->oCursor.col);
	
	_p->oCursor.row -= _p->iVisibleLineCount;
	if(_p->oCursor.row < 0)
		_p->oCursor.row = 0;

	int iCol;

	if(absoluteXPositionToColumn(_p->oCursor.row,&iCol,_p->fCursorUpDownX))
		_p->oCursor.col = iCol;

	handleCursorMoved(bExtendSelection,DontResetCursorUpDownX);
}

void C3TextEditor::cursorPageDown(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR
	// do NOT sanitize the column!

	if(_p->oCursor.row >= (_p->lLines.count()-1))
	{
		endVisualStateChange();
		return; // can't be done
	}

	if(_p->fCursorUpDownX < 0.0)
		_p->fCursorUpDownX = columnToAbsoluteXPosition(_p->oCursor.row,_p->oCursor.col);
	
	_p->oCursor.row += _p->iVisibleLineCount;
	if(_p->oCursor.row > (_p->lLines.count()-1))
		_p->oCursor.row = (_p->lLines.count()-1);

	int iCol;

	if(absoluteXPositionToColumn(_p->oCursor.row,&iCol,_p->fCursorUpDownX))
		_p->oCursor.col = iCol;

	handleCursorMoved(bExtendSelection,DontResetCursorUpDownX);
}

void C3TextEditor::cursorHome(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR

	_p->oCursor.col = 0;

	handleCursorMoved(bExtendSelection);
}

void C3TextEditor::cursorEnd(bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR

	C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);

	_p->oCursor.col = pLine->szText.length();

	handleCursorMoved(bExtendSelection);
}

void C3TextEditor::cursorFileEnd(bool bExtendSelection)
{
	int iLastLine = _p->lLines.count() - 1;

	if(iLastLine < 0)
		return;

	C3TextEditorLine * pLine = _p->lLines.at(iLastLine);
	Q_ASSERT(pLine);

	cursorMoveTo(C3TextEditorRowColumn(iLastLine,pLine->szText.length()),bExtendSelection);
}


void C3TextEditor::cursorMoveTo(const C3TextEditorRowColumn &rc,bool bExtendSelection)
{
	BEGIN_MOVE_CURSOR

	_p->oCursor = rc;

	SANITIZE_CURSOR_ROW
	SANITIZE_CURSOR_COLUMN

	handleCursorMoved(bExtendSelection);
}

bool C3TextEditor::cursorMoveTo(const QString &szRowColumnSpec)
{
	if(szRowColumnSpec.isEmpty())
		return false;

	//qDebug("Cursor move to location %s (path=%s)",szRowColumnSpec.toUtf8().data(),path().toUtf8().data());

	QStringList sl;
	
	int idx = szRowColumnSpec.indexOf(QChar(','));
	if(idx >= 0)
	{
		sl = szRowColumnSpec.split(QChar(','));
	} else {
		idx = szRowColumnSpec.indexOf(QChar(':'));
		if(idx >= 0)
			sl = szRowColumnSpec.split(QChar(':'));
		else
			sl.append(szRowColumnSpec);
	}
	
	if(sl.isEmpty())
		return false;
	
	QString szRow = sl.at(0);
	QString szCol = sl.count() > 1 ? sl.at(1) : __utf8("0");
	
	bool ok;
	
	int iRow = szRow.trimmed().toInt(&ok) - 1;
	if(!ok)
		return false;
	
	int iCol = szCol.trimmed().toInt(&ok);
	if(!ok)
		iCol = 0;
	
	if(iCol < 0)
		iCol = 0;
	if(iRow < 0)
		iRow = 0;
	
	BEGIN_MOVE_CURSOR

	_p->oCursor.row = iRow;
	_p->oCursor.col = iCol;

	SANITIZE_CURSOR_ROW
	SANITIZE_CURSOR_COLUMN

	handleCursorMoved(false,PlaceCursorRowInMiddleOfScreen);
	
	return true;
}

void C3TextEditor::goToStart()
{
	cursorMoveTo(
			C3TextEditorRowColumn(
					0,
					0
				),
			false
		);
}

void C3TextEditor::goToEnd()
{
	C3TextEditorLine * pLine = _p->lLines.last();
	if(!pLine)
		return;

	cursorMoveTo(
			C3TextEditorRowColumn(
					_p->lLines.count() - 1,
					pLine->szText.length()
				),
			false
		);
}

void C3TextEditor::goToLine()
{
	bool ok;

	QString szInput = QInputDialog::getText(
			this,
			__tr("Go To Line..."),
			__tr("Please insert the line and column to go to. The format is line[,column]."),
			QLineEdit::Normal,
			__utf8("%1").arg(_p->lLines.count()),
			&ok
		);

	if(!ok)
		return;

	if(szInput.isEmpty())
		return;

	cursorMoveTo(szInput);
}

int C3TextEditor::cursorRow() const
{
	return _p->oCursor.row;
}

int C3TextEditor::cursorColumn() const
{
	return _p->oCursor.col;
}

const C3TextEditorRowColumn & C3TextEditor::cursorPosition() const
{
	return _p->oCursor;
}
