//=============================================================================
//
//   File : C3TextEditorViewport.cpp
//   Creation Date : sab 24 ott 2015 04:02:14
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

#include "C3TextEditorViewport.h"

#include "C3TextEditor_private.h"

#include "C3TextEditorLine.h"
#include "C3TextEditorMode.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorFontMetricsUtils.h"

#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QCursor>

#include <qdrawutil.h>

C3TextEditorViewport::C3TextEditorViewport(
		C3TextEditor * pParent,
		C3TextEditorPrivate * pPrivate
	)
	: QWidget(pParent)
{
	_p = pPrivate;
	
	setFocusPolicy(Qt::StrongFocus);
	setCursor(QCursor(Qt::IBeamCursor));
	setMouseTracking(true);
}

C3TextEditorViewport::~C3TextEditorViewport()
{
}

void C3TextEditorViewport::paintEvent(QPaintEvent * e)
{
	QRect rPaintRect = e->rect();

	//qDebug("Paint event (%d,%d,%d,%d)",rPaintRect.x(),rPaintRect.y(),rPaintRect.width(),rPaintRect.height());

	int iYOffset = _p->iTopLine * _p->iFontLineSpacing;

	QRect rTextRect = QRect(0,0,_p->iContentsWidth - _p->iXOffset,_p->iContentsHeight - iYOffset)
			.intersected(rPaintRect);

	QPainter p(this);

	if((rPaintRect.width() > rTextRect.width()) || (rPaintRect.height() > rTextRect.height()))
		p.fillRect(rPaintRect,_p->pOptions->oEditorBackgroundColor); // FIXME: This could be optimized further!

	p.fillRect(rTextRect,_p->pOptions->oTextBackgroundColor);

	int iCount = _p->lLines.count();
	
	if(iCount < 1)
	{
		//qDebug("Paint event ret 1");
		return; // no lines to paint
	}

	int iMinY = rPaintRect.y();
	int iMaxY = rPaintRect.bottom(); // +1?
	
	int iFirstRow = _p->iTopLine + (iMinY / _p->iFontLineSpacing);
	if(iFirstRow >= iCount)
	{
		//qDebug("Paint event ret 2");
		return; // no lines to paint
	}
	
	int iLastRow = _p->iTopLine + (iMaxY / _p->iFontLineSpacing);
	if(iLastRow >= iCount)
		iLastRow = iCount - 1;

	_p->pMode->ensureMetadataUpToDate(
			iFirstRow,
			iLastRow
		);

	int iTopY = (iFirstRow - _p->iTopLine) * _p->iFontLineSpacing;
	
	p.setFont(_p->pOptions->oTextFont);
	
	qreal fStartX = _p->iLeftMargin - _p->iXOffset;
	
	int iRow = iFirstRow;

	int iSelectionStartRow,iSelectionEndRow;
	
	if(_p->oSelection.isEmpty())
	{
		iSelectionStartRow = -1;
		iSelectionEndRow = -1;
	} else {
		iSelectionStartRow = _p->oSelection.start.row;
		iSelectionEndRow = _p->oSelection.end.row;
	}
	
	int iMarkStartRow,iMarkEndRow;

	if(_p->oMark.isEmpty())
	{
		iMarkStartRow = -1;
		iMarkEndRow = -1;
	} else {
		iMarkStartRow = _p->oMark.start.row;
		iMarkEndRow = _p->oMark.end.row;
	}

	qreal fWidth = width();
	
	while(iRow <= iLastRow)
	{
		int iBottomY = iTopY + _p->iFontLineSpacing;

		C3TextEditorLine * pLine = _p->lLines.at(iRow);

		if(iRow == _p->oCursor.row)
			p.fillRect(QRectF(0,iTopY,_p->iContentsWidth - _p->iXOffset,iBottomY - iTopY),_p->pOptions->oCurrentLineBackgroundColor);

		if((iRow <= iMarkEndRow) && (iRow >= iMarkStartRow)) // first check end, as when selection is invalid it's -1
		{
			qreal fMarkStartX,fMarkEndX;
			
			if(iRow == iMarkStartRow)
			{
				fMarkStartX = _p->pEditor->columnToAbsoluteXPosition(iRow,_p->oMark.start.col) - (qreal)_p->iXOffset;
				if(_p->oMark.start.col == 0)
					fMarkStartX -= 1.0;
			} else {
				fMarkStartX = (qreal)(_p->iLeftMargin - _p->iXOffset - 1);
			}

			if(iRow == iMarkEndRow)
			{
				fMarkEndX = _p->pEditor->columnToAbsoluteXPosition(iRow,_p->oMark.end.col) - _p->iXOffset;
				if(_p->oMark.end.col >= pLine->szText.length())
					fMarkEndX += 1.0;
			} else {
				fMarkEndX = (qreal)(_p->iLeftMargin + pLine->fWidth + 1 - _p->iXOffset);
			}

			p.fillRect(QRectF(fMarkStartX,iTopY,fMarkEndX - fMarkStartX,iBottomY - iTopY),_p->pOptions->oMarkBackgroundColor);
		}

	
		C3TextEditorLine::Block * pBlock;
	
		qreal fX = fStartX;
		foreach(pBlock,pLine->lBlocks)
		{
			if(fX > fWidth)
				break;

			if(pBlock->szText.length() == 0)
				continue;

			qreal fEndX = fX + pBlock->fWidth;

			if(fEndX > 0)
			{
				if(pBlock->uFlags & C3TextEditorLine::Block::IsTabBlock)
				{
					int iCount = pBlock->szText.length();
	
					p.setPen(_p->pOptions->oTabColor);
	
					// FIXME: Paint the tabs!
					float fTabX = (::floor((fX - fStartX) / _p->fFontTabStopWidth) * _p->fFontTabStopWidth) + fStartX;
					while(iCount > 0)
					{
						int iX = qRound(fTabX);
						p.drawLine(iX,iTopY+1,iX,iBottomY-2);
						fTabX += _p->fFontTabStopWidth;
						iCount--;
					}
				} else {
					//qDebug("Draw block %s at %d,%d with color %x",pBlock->szText.toUtf8().data(),iX,iTopY + _p->iFontAscent,pBlock->pColor);
					p.setPen(*(pBlock->pColor));
					p.drawText(QPointF(fX,iTopY + _p->iFontAscent),pBlock->szText);
				}
			}

			fX = fEndX;
		}

		if((iRow <= iSelectionEndRow) && (iRow >= iSelectionStartRow)) // first check end, as when selection is invalid it's -1
		{
			qreal fSelStartX,fSelEndX;
			
			if(iRow == iSelectionStartRow)
			{
				fSelStartX = _p->pEditor->columnToAbsoluteXPosition(iRow,_p->oSelection.start.col) - (qreal)_p->iXOffset;
				if(_p->oSelection.start.col == 0)
					fSelStartX -= 1.0;
			} else {
				fSelStartX = (qreal)(_p->iLeftMargin - _p->iXOffset - 1);
			}

			if(iRow == iSelectionEndRow)
			{
				fSelEndX = _p->pEditor->columnToAbsoluteXPosition(iRow,_p->oSelection.end.col) - _p->iXOffset;
				if(_p->oSelection.end.col >= pLine->szText.length())
					fSelEndX += 1.0;
			} else {
				fSelEndX = (qreal)(_p->iLeftMargin + pLine->fWidth + 1 - _p->iXOffset);
			}
			
			p.setCompositionMode(QPainter::RasterOp_SourceXorDestination); // FIXME: This is not supported on mac!
			p.fillRect(QRectF(fSelStartX,iTopY,fSelEndX - fSelStartX,iBottomY - iTopY),Qt::white);
			p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		}

		if(_p->bCursorOn && (iRow == _p->oCursor.row))
		{
			//qDebug("Cursor at %d,%d",_p->oCursor.row,_p->oCursor.col);

			// draw cursor
			int iX = qRound(_p->pEditor->columnToAbsoluteXPosition(_p->oCursor.row,_p->oCursor.col) - (qreal)_p->iXOffset);

			p.setPen(_p->pOptions->oCursorColor);

			p.drawLine(iX,iTopY,iX,iBottomY-2);
			p.drawLine(iX+1,iTopY,iX+1,iBottomY-2);
			p.drawLine(iX-3,iTopY,iX+4,iTopY);
			p.drawLine(iX-3,iBottomY-1,iX+4,iBottomY-1);
		}

		iRow++;
		iTopY = iBottomY;
	}
}

bool C3TextEditorViewport::event(QEvent * e)
{
	// This is used to catch Tab and Shift+Tab events which normally are
	// 'eaten' by the focus management mechanism.
	if(e->type() == QEvent::KeyPress)
	{
		keyPressEvent((QKeyEvent *)e);
		return e->isAccepted();
	}
	
	return QWidget::event(e);
}


void C3TextEditorViewport::keyPressEvent(QKeyEvent * e)
{
	if(_p->pEditor->handleKeyEvent(e))
		e->accept();
	else
		e->ignore();
}

void C3TextEditorViewport::leftMousePressEvent(QMouseEvent * e)
{
	C3TextEditorRowColumn rc;
	
	if(!_p->pEditor->viewportPointToRowColumn(e->pos(),rc,true))
		return; // should never happen because we pass true.

	_p->pEditor->cursorMoveTo(rc,e->modifiers() & Qt::ShiftModifier);

	update(); // always a full update

	if(e->modifiers() & Qt::ControlModifier)
		_p->pEditor->jumpToLinkAtCursor((e->modifiers() & Qt::AltModifier) ? C3TextEditorMode::SymbolIdentificationNoContextFilter : 0);
}

void C3TextEditorViewport::rightMousePressEvent(QMouseEvent * e)
{
	_p->pEditor->showContextMenu(mapToGlobal(e->pos()));
}

void C3TextEditorViewport::middleMousePressEvent(QMouseEvent * e)
{
}

void C3TextEditorViewport::mousePressEvent(QMouseEvent * e)
{
	if(e->button() == Qt::LeftButton)
		leftMousePressEvent(e);
	else if(e->button() == Qt::MiddleButton)
		middleMousePressEvent(e);
	else if(e->button() == Qt::RightButton)
		rightMousePressEvent(e);

	e->accept();
}

void C3TextEditorViewport::mouseReleaseEvent(QMouseEvent * e)
{
	_p->pEditor->killMouseScrollTimer();
}

void C3TextEditorViewport::mouseMoveEvent(QMouseEvent * e)
{
	C3TextEditorRowColumn rc;

	if(!(e->buttons() & Qt::LeftButton))
	{
		if(e->modifiers() & Qt::ControlModifier)
		{
			if(!_p->pEditor->viewportPointToRowColumn(e->pos(),rc,true))
				return; // should never happen because we pass true.

			unsigned int uFlags = 0;
			
			if(e->modifiers() & Qt::AltModifier)
				uFlags |= C3TextEditorMode::SymbolCompletionNoContextFilter;

			_p->pEditor->triggerSymbolIdentification(rc,uFlags);
		}
		return; // we ignore anything but the left button
	}

	if(rect().contains(e->pos()))
	{
		if(_p->pMouseScrollTimer)
			_p->pEditor->killMouseScrollTimer();
	
		if(!_p->pEditor->viewportPointToRowColumn(e->pos(),rc,true))
			return; // should never happen because we pass true.

		_p->pEditor->cursorMoveTo(rc,true);
	} else {
		if(!_p->pMouseScrollTimer)
			_p->pEditor->startMouseScrollTimer();
	}

	e->accept();
}

void C3TextEditorViewport::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!(e->buttons() & Qt::LeftButton))
		return; // we ignore anything but the left button

	C3TextEditorRange oRange;

	if(!_p->pEditor->computeNearestTokenRange(_p->oCursor.row,_p->oCursor.col,oRange))
		return;

	_p->pEditor->selectRange(oRange);
}

void C3TextEditorViewport::wheelEvent(QWheelEvent * e)
{
	int iDelta = (e->angleDelta().y() / 30); // delta is usually 120 for every notch, positive if the wheel is rotated "away from the user"

	int iRow;

	if(iDelta > 0)
	{
		iRow = _p->iTopLine - iDelta;
		if(iRow < 0)
			iRow = 0;

	} else {

		iRow = _p->iTopLine + _p->iVisibleLineCount - 1 - iDelta;
		if(iRow >= _p->lLines.count())
			iRow = _p->lLines.count() - 1;
	}

	C3TextEditorRowColumn rc(iRow,_p->oCursor.col);

	_p->pEditor->cursorMoveTo(rc,false);

	e->accept();
}