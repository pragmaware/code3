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
#include "C3TextEditorMode.h"
#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorViewport.h"
#include "C3TextEditorCompletionWidget.h"
#include "C3TextEditorUndo.h"

#include <QFontMetricsF>

#define SANITIZE_CURSOR_COLUMN \
	do { \
		C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row); \
		Q_ASSERT(pLine); \
		if(_p->oCursor.col > pLine->szText.length()) \
			_p->oCursor.col = pLine->szText.length(); \
	} while(0)

#define SANITIZE_CURSOR_ROW \
	do { \
		if(_p->oCursor.row >= _p->lLines.count()) \
			_p->oCursor.row = _p->lLines.count() - 1; \
		else if(_p->oCursor.row < 0) \
			_p->oCursor.row = 0; \
	} while(0)

void C3TextEditor::tab()
{
	if(readOnly())
		return;

	if(_p->oSelection.isEmpty())
	{
		insertText("\t");
		return;
	}

	indent();
}


void C3TextEditor::indent()
{
	if(readOnly())
		return;

	beginVisualStateChange();
	beginMultiUndo();

	SANITIZE_CURSOR_ROW;
	SANITIZE_CURSOR_COLUMN;

	int iFirstRow,iLastRow;
	
	if(!_p->oSelection.isEmpty())
	{
		iFirstRow = _p->oSelection.start.row;
		iLastRow = _p->oSelection.end.row;
		_p->oSelection.end.col++;
		_p->oSelection.start.col++;
	} else {
		iFirstRow = _p->oCursor.row;
		iLastRow = _p->oCursor.row;
	}
	_p->oCursor.col++;

	_p->oMark.clear();

	C3TextEditorLine * pLine;
	
	QChar tab('\t');
	QString szTab = tab;

	qreal fMaximumInitialLineWidth = 0.0;
	qreal fMaximumFinalLineWidth = 0.0;

	{
		C3TextEditorLinesLocker oLocker(_p);

		for(int iRow = iFirstRow;iRow <= iLastRow;iRow++)
		{
			pLine = _p->lLines.at(iRow);
	
			pushUndo(
					new C3TextEditorUndo(
							C3TextEditorUndo::Insert,
							C3TextEditorRange(iRow,0,iRow,1),
							szTab
						)
				);
	
			pLine->szText.insert(0,tab);
	
			if(pLine->fWidth > fMaximumInitialLineWidth)
				fMaximumInitialLineWidth = pLine->fWidth;
	
			pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(
					_p->pFontMetrics,
					pLine->szText,
					_p->fFontTabStopWidth
				);
		
			if(pLine->fWidth > fMaximumFinalLineWidth)
				fMaximumFinalLineWidth = pLine->fWidth;
		}
	}

	_p->pMode->textModified(iFirstRow,iLastRow);

	updateOrInvalidateMaximumLineWidthAfterChange(fMaximumInitialLineWidth,fMaximumFinalLineWidth);
	endMultiUndo();
	endVisualStateChange(ForceFullRepaint);
	setModified(true);
}

void C3TextEditor::unindent()
{
	if(readOnly())
		return;

	beginVisualStateChange();
	beginMultiUndo();

	SANITIZE_CURSOR_ROW;
	SANITIZE_CURSOR_COLUMN;

	int iFirstRow,iLastRow;
	
	if(!_p->oSelection.isEmpty())
	{
		iFirstRow = _p->oSelection.start.row;
		iLastRow = _p->oSelection.end.row;
	} else {
		iFirstRow = _p->oCursor.row;
		iLastRow = _p->oCursor.row;
	}

	_p->oMark.clear();

	C3TextEditorLine * pLine;
	
	QChar tab('\t');
	QString szTab = tab;
	
	qreal fMaximumInitialLineWidth = 0.0;
	qreal fMaximumFinalLineWidth = 0.0;

	{
		C3TextEditorLinesLocker oLocker(_p);
	
		for(int iRow = iFirstRow;iRow <= iLastRow;iRow++)
		{
			pLine = _p->lLines.at(iRow);
	
			if(pLine->szText.startsWith(tab))
			{
				pushUndo(
						new C3TextEditorUndo(
								C3TextEditorUndo::Delete,
								C3TextEditorRange(iRow,0,iRow,1),
								szTab
							)
					);
		
				pLine->szText.remove(0,1);
	
				if((iRow == _p->oSelection.end.row) && (_p->oSelection.end.col > 0))
					_p->oSelection.end.col--;
				if((iRow == _p->oSelection.start.row) && (_p->oSelection.start.col > 0))
					_p->oSelection.start.col--;
				if((iRow == _p->oCursor.row) && (_p->oCursor.col > 0))
					_p->oCursor.col--;
	
				if(pLine->fWidth > fMaximumInitialLineWidth)
					fMaximumInitialLineWidth = pLine->fWidth;
	
				pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(
						_p->pFontMetrics,
						pLine->szText,
						_p->fFontTabStopWidth
					);
	
				if(pLine->fWidth > fMaximumFinalLineWidth)
					fMaximumFinalLineWidth = pLine->fWidth;
			}
		}
	}

	_p->pMode->textModified(iFirstRow,iLastRow);

	updateOrInvalidateMaximumLineWidthAfterChange(fMaximumInitialLineWidth,fMaximumFinalLineWidth);
	endMultiUndo();
	endVisualStateChange(ForceFullRepaint);
	setModified(true);
}

void C3TextEditor::newline()
{
	if(readOnly())
		return;

	// Auto indent
	QString szText = "\n";

	beginVisualStateChange();

	SANITIZE_CURSOR_ROW;
	SANITIZE_CURSOR_COLUMN;

	C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);
	Q_ASSERT(pLine);

	if(!pLine->szText.isEmpty())
	{
		const QChar * b = pLine->szText.unicode();
		const QChar * p = b;
		int l = pLine->szText.length();
		if(l > _p->oCursor.col)
			l = _p->oCursor.col;
		const QChar * e = b + l;
		
		while(p < e)
		{
			ushort u = p->unicode();
			if((u != ' ') && (u != '\t'))
				break;
			p++;
		}
		
		if(p > b)
			szText.append(b,p-b);
		
#if 0
		// This is somewhat disturbing in certain cases. Especially if one is used to insert if()\n{\n} and then edit the contents of {\n}.
		// It should be probably solved at "language formatting" level instead.

		// indent more after {, [ and (
		if(_p->oCursor.col > 0)
		{
			ushort u = pLine->szText.at(_p->oCursor.col - 1).unicode();
			if((u == '{') || (u == '[') || (u == '('))
				szText.append('\t');
		}
#endif
	}

	insertTextInternal(szText);

	endVisualStateChange();
}

void C3TextEditor::backspace()
{
	if(readOnly())
		return;

	beginVisualStateChange();
	
	if(!_p->oSelection.isEmpty())
	{
		deleteRangeInternal(_p->oSelection);
		endVisualStateChange();
		return;
	}

	SANITIZE_CURSOR_ROW;
	SANITIZE_CURSOR_COLUMN;

	if((_p->oCursor.row > 0) || (_p->oCursor.col > 0))
	{
		int iStartRow;
		int iStartCol;
		
		if(_p->oCursor.col > 0)
		{
			iStartRow = _p->oCursor.row;
			iStartCol = _p->oCursor.col - 1;
		} else {
			iStartRow = _p->oCursor.row - 1;
			iStartCol = _p->lLines.at(iStartRow)->szText.length();
		}
		
		deleteRangeInternal(
				C3TextEditorRange(
						iStartRow,iStartCol,
						_p->oCursor.row,_p->oCursor.col
					)
			);
	}

	if(_p->pCompletionWidget && _p->pCompletionWidget->isVisible())
		_p->pCompletionWidget->hide();

	_p->pMode->triggerSymbolCompletion(
			_p->oCursor,
			C3TextEditorMode::SymbolCompletionMatchInTheMiddle | C3TextEditorMode::SymbolCompletionMatchCaseInsensitive
		);

	endVisualStateChange(DontHideCompletionWidget);
}

void C3TextEditor::del()
{
	if(readOnly())
		return;

	beginVisualStateChange();
	
	if(!_p->oSelection.isEmpty())
	{
		deleteRangeInternal(_p->oSelection);
		endVisualStateChange();
		return;
	}

	SANITIZE_CURSOR_ROW;
	SANITIZE_CURSOR_COLUMN;

	int iLineCountMinusOne = _p->lLines.count() - 1;

	if((_p->oCursor.row < iLineCountMinusOne) || (_p->oCursor.col < _p->lLines.at(iLineCountMinusOne)->szText.length()))
	{
		int iEndRow;
		int iEndCol;
		
		C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);
		
		if(_p->oCursor.col < pLine->szText.length())
		{
			iEndRow = _p->oCursor.row;
			iEndCol = _p->oCursor.col + 1;
		} else {
			iEndRow = _p->oCursor.row + 1;
			iEndCol = 0;
		}
		
		deleteRangeInternal(
				C3TextEditorRange(
						_p->oCursor.row,_p->oCursor.col,
						iEndRow,iEndCol
					)
			);
	}

	if(_p->pCompletionWidget && _p->pCompletionWidget->isVisible())
		_p->pCompletionWidget->hide();

	_p->pMode->triggerSymbolCompletion(
			_p->oCursor,
			C3TextEditorMode::SymbolCompletionMatchInTheMiddle | C3TextEditorMode::SymbolCompletionMatchCaseInsensitive
		);

	endVisualStateChange(DontHideCompletionWidget);
}

void C3TextEditor::deleteRange(const C3TextEditorRange &oRange)
{
	if(readOnly())
		return;

	if(oRange.isEmpty())
		return;
	
	beginVisualStateChange();

	deleteRangeInternal(oRange);

	endVisualStateChange();
}

void C3TextEditor::deleteRangeInternal(const C3TextEditorRange &oRange,bool bAddUndo)
{
	if(readOnly())
		return;

	C3TextEditorRange oSafeRange = oRange;

	// Clear the selection after copying oRange (in case oRange was _p->oSelection actually)
	_p->oSelection.clear();
	_p->oMark.clear();

	if(oSafeRange.isEmpty())
		return; // nothing to do

	int iLineCount = _p->lLines.count();
	
	//qDebug("DELETE RANGE %d",_p->oSelection.isEmpty());

	if(oSafeRange.end.row >= iLineCount)
	{
		oSafeRange.end.row = iLineCount - 1;
		oSafeRange.end.col = _p->lLines.at(oSafeRange.end.row)->szText.length();
	} else {
		int iLength = _p->lLines.at(oSafeRange.end.row)->szText.length();
		if(oSafeRange.end.col >= iLength)
			oSafeRange.end.col = iLength;
	}

	if(oSafeRange.isEmpty())
		return;

	int iRow = oSafeRange.start.row;
	int iEndRow = oSafeRange.end.row;

	QString szDeleted;

	// FIXME: Take care of maximum line width!

	C3TextEditorLine * pLine;
	
	qreal fMaximumInitialLineWidth = 0.0;
	qreal fMaximumFinalLineWidth = 0.0;

	{
		C3TextEditorLinesLocker oLocker(_p);

		if(iRow == iEndRow)
		{
			pLine = _p->lLines.at(iRow);
			int iLen = oSafeRange.end.col - oSafeRange.start.col;
			szDeleted = pLine->szText.mid(oSafeRange.start.col,iLen);
			pLine->szText.remove(oSafeRange.start.col,iLen);
			if(pLine->fWidth > fMaximumInitialLineWidth)
				fMaximumInitialLineWidth = pLine->fWidth;
			pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(_p->pFontMetrics,pLine->szText,_p->fFontTabStopWidth);
			if(pLine->fWidth > fMaximumFinalLineWidth)
				fMaximumFinalLineWidth = pLine->fWidth;
	
		} else {
	
			if((iEndRow - iRow) > 1)
				_p->bVisualStateChangeForceFullRepaint = true; // more than the cursor rows, force full repaint anyway.
	
			C3TextEditorLine * pFirstLine = _p->lLines.at(iRow);
			szDeleted.append(pFirstLine->szText.mid(oSafeRange.start.col));
			pFirstLine->szText.truncate(oSafeRange.start.col);
	
			if(pFirstLine->fWidth > fMaximumInitialLineWidth)
				fMaximumInitialLineWidth = pFirstLine->fWidth;
	
			iRow++;
		
			while(iRow < iEndRow)
			{
				C3TextEditorLine * pLine = _p->lLines.at(iRow);
				szDeleted.append(QChar('\n'));
				szDeleted.append(pLine->szText);
	
				if(pLine->fWidth > fMaximumInitialLineWidth)
					fMaximumInitialLineWidth = pLine->fWidth;
	
				delete pLine; // ! (see erase() below)
	
				iRow++;
			}
	
			C3TextEditorLine * pLastLine = _p->lLines.at(iRow);
			szDeleted.append(QChar('\n'));
			szDeleted.append(pLastLine->szText.left(oSafeRange.end.col));
	
			if(pLastLine->fWidth > fMaximumInitialLineWidth)
				fMaximumInitialLineWidth = pLastLine->fWidth;
	
			pFirstLine->szText.append(pLastLine->szText.mid(oSafeRange.end.col));
			pFirstLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(_p->pFontMetrics,pFirstLine->szText,_p->fFontTabStopWidth);
	
			if(pFirstLine->fWidth > fMaximumFinalLineWidth)
				fMaximumFinalLineWidth = pFirstLine->fWidth;
				
			delete pLastLine; // !
	
			QList<C3TextEditorLine *>::iterator it1 = _p->lLines.begin() + oSafeRange.start.row + 1;
			QList<C3TextEditorLine *>::iterator it2 = _p->lLines.begin() + oSafeRange.end.row + 1;
			
			_p->lLines.erase(it1,it2); // erases up to (but not including) it2
		}
	}

	updateOrInvalidateMaximumLineWidthAfterChange(fMaximumInitialLineWidth,fMaximumFinalLineWidth);

	_p->oCursor = oSafeRange.start;

	if(!_p->pMode->textDeleted(szDeleted,oSafeRange.start.row,oSafeRange.end.row))
		_p->bVisualStateChangeForceFullRepaint = true; // line state not preserved

	if(bAddUndo)
	{
		pushUndo(
				new C3TextEditorUndo(
						C3TextEditorUndo::Delete,
						oSafeRange,
						szDeleted
					)
			);
	}

	setModified(true);
}

void C3TextEditor::insertText(const QString &szText)
{
	if(readOnly())
		return;

	//qDebug("INSERT TEXT");
	//qDebug("%s",szText.toUtf8().data());
	//qDebug("INSERT TEXT END");

	bool bHasSelection = !_p->oSelection.isEmpty();

	if(szText.isEmpty() && (!bHasSelection))
		return; // nothing to do

	beginVisualStateChange();
	
	insertTextInternal(szText);

	endVisualStateChange();
}

void C3TextEditor::markMatchingParenthesisInternal()
{
	// FIXME: There are several special cases that this code cannot handle.
	// For instance, comments which may have any b0rken or ambiguous syntax within them.

	if(_p->oCursor.col < 1)
		return;
	if(_p->oCursor.row < 0)
		return;
	if(_p->oCursor.row >= _p->lLines.count())
		return;
	
	C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);
	
	int iCol = _p->oCursor.col;
	if(iCol >= pLine->szText.length())
		iCol = pLine->szText.length();

	C3TextEditorRowColumn rcEnd = _p->oCursor;
	rcEnd.col = iCol;

	iCol--;
	if(iCol < 0)
		return;

	int iRow = rcEnd.row;

	QChar cEnd = pLine->szText.at(iCol);
	QChar cStart;
	
	switch(cEnd.unicode())
	{
		case '}':
			cStart = '{';
		break;
		case ']':
			cStart = '[';
		break;
		case ')':
			cStart = '(';
		break;
		default:
			return;
		break;
	}

#define IN_STRING 1
#define IN_CHAR 2

	unsigned int uState = 0;

	int iMinRow = iRow - 500;
	if(iMinRow < 0)
		iMinRow = 0;

	int iP1 = 0;
	int iP2 = 0;
	int iP3 = 0;

	for(;;)
	{
		while(iCol > 0)
		{
			iCol--;
			QChar c = pLine->szText.at(iCol);
			
			if((c == cStart) && (uState == 0) && (iP1 == 0) && (iP2 == 0) && (iP3 == 0))
			{
				_p->oMark.end = rcEnd;
				_p->oMark.start.row = iRow;
				_p->oMark.start.col = iCol;
				return;
			}
			
			switch(c.unicode())
			{
				case '"':
					if(!(uState & IN_CHAR))
					{
						if(uState & IN_STRING)
							uState &= ~IN_STRING;
						else
							uState |= IN_STRING;
					}
				break;
				case '\'':
					if(!(uState & IN_STRING))
					{
						if(uState & IN_CHAR)
							uState &= ~IN_CHAR;
						else
							uState |= IN_CHAR;
					}
				break;
				case ')':
					if(uState == 0)
						iP1++;
				break;
				case '(':
					if(uState == 0)
						iP1--;
				break;
				case ']':
					if(uState == 0)
						iP2++;
				break;
				case '[':
					if(uState == 0)
						iP2--;
				break;
				case '}':
					if(uState == 0)
						iP3++;
				break;
				case '{':
					if(uState == 0)
						iP3--;
				break;
			}
		}
		
		if(iRow <= iMinRow)
			return;

		iRow--;
		
		pLine = _p->lLines.at(iRow);
		iCol = pLine->szText.length();
	}
}

void C3TextEditor::insertTypedText(const QString &szText)
{
	if(readOnly())
		return;

	bool bHasSelection = !_p->oSelection.isEmpty();

	if(szText.isEmpty() && (!bHasSelection))
		return; // nothing to do

	beginVisualStateChange();
	
	insertTextInternal(szText);

	//if(_p->pCompletionWidget && _p->pCompletionWidget->isVisible())
	//	_p->pCompletionWidget->hide();

	if(szText.length() == 1)
	{
		static QChar cP1('}');
		static QChar cP2(')');
		static QChar cP3(']');
		
		if(szText.startsWith(cP1) || szText.startsWith(cP2) || szText.startsWith(cP3))
		{
			markMatchingParenthesisInternal();
		} else {
			_p->pMode->triggerSymbolCompletion(
					_p->oCursor,
					C3TextEditorMode::SymbolCompletionMatchInTheMiddle | C3TextEditorMode::SymbolCompletionMatchCaseInsensitive
				);
		}
	}

	endVisualStateChange(DontHideCompletionWidget);
}

void C3TextEditor::insertTextInternal(const QString &szText,bool bAddUndo,bool bSelectInsertedText)
{
	if(readOnly())
		return;

	bool bHasSelection = !_p->oSelection.isEmpty();

	if(szText.isEmpty() && (!bHasSelection))
		return; // nothing to do

	if(bHasSelection)
	{
		if(bAddUndo)
			beginMultiUndo();
		deleteRangeInternal(_p->oSelection); // will also clear the mark
	} else {
		_p->oMark.clear();
	}

	QStringList lLines = szText.split(QChar('\n'));

	//qDebug("INSERT TEXT(%s)",szText.toUtf8().data());
	//qDebug("LINES %d",lLines.count());

	C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);
	Q_ASSERT(pLine);

	if(_p->oCursor.col > pLine->szText.length())
		_p->oCursor.col = pLine->szText.length();
	if(_p->oCursor.row >= _p->lLines.count())
		_p->oCursor.row = _p->lLines.count() - 1;
	else if(_p->oCursor.row < 0)
		_p->oCursor.row = 0;
	if(_p->oSelection.isEmpty())
		_p->oSelectionAnchor = _p->oCursor;

	int iPreviousCursorRow = _p->oCursor.row;

	int iLineCount = lLines.count();

	qreal fMaximumFinalLineWidth = 0.0;
	qreal fMaximumInitialLineWidth = pLine->fWidth; // this needs to take account only of EXISTING lines.

	C3TextEditorRange oInsertRange;
	oInsertRange.start = _p->oCursor;

	{
		C3TextEditorLinesLocker oLocker(_p);

		if(iLineCount == 1)
		{
			//qDebug("INSTEXT1[%s][%s]",pLine->szText.toUtf8().data(),szText.toUtf8().data());
		
			pLine->szText.insert(_p->oCursor.col,szText);

			//qDebug("INSTEXT2[%s][%s]",pLine->szText.toUtf8().data(),szText.toUtf8().data());

			_p->oCursor.col += szText.length();
	
		} else {
	
			if(iLineCount > 1)
				_p->bVisualStateChangeForceFullRepaint = true; // more than the cursor rows, force full repaint anyway.
	
			QString szLeft = pLine->szText.left(_p->oCursor.col);
			QString szRight = pLine->szText.mid(_p->oCursor.col);
			pLine->szText = szLeft;
			pLine->szText.append(lLines.at(0));
			pLine->clearMetadata();
	
			pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(
					_p->pFontMetrics,
					pLine->szText,
					_p->fFontTabStopWidth
				);
			if(pLine->fWidth > fMaximumFinalLineWidth)
				fMaximumFinalLineWidth = pLine->fWidth;
	
			for(int i=1;i<iLineCount;i++)
			{
				_p->oCursor.row++;
				pLine = new C3TextEditorLine();
				_p->lLines.insert(_p->oCursor.row,pLine);
	
				pLine->szText = lLines.at(i);
	
				pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(
						_p->pFontMetrics,
						pLine->szText,
						_p->fFontTabStopWidth
					);
				if(pLine->fWidth > fMaximumFinalLineWidth)
					fMaximumFinalLineWidth = pLine->fWidth;
			}
	
			_p->oCursor.col = pLine->szText.length();
	
			pLine->szText.append(szRight);
		}
	}

	pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(
			_p->pFontMetrics,
			pLine->szText,
			_p->fFontTabStopWidth
		);

	if(pLine->fWidth > fMaximumFinalLineWidth)
		fMaximumFinalLineWidth = pLine->fWidth;

	oInsertRange.end = _p->oCursor;

	if(bSelectInsertedText)
		_p->oSelection = oInsertRange;

	updateOrInvalidateMaximumLineWidthAfterChange(fMaximumInitialLineWidth,fMaximumFinalLineWidth);

	if(!_p->pMode->textInserted(szText,iPreviousCursorRow,_p->oCursor.row))
		_p->bVisualStateChangeForceFullRepaint = true; // line state not preserved

	if(bAddUndo)
	{
		pushUndo(
				new C3TextEditorUndo(
						C3TextEditorUndo::Insert,
						oInsertRange,
						QString()
					)
			);
			
		if(bHasSelection)
			endMultiUndo();
	}

	setModified(true);
}
