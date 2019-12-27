//=============================================================================
//
//   File : C3TextEditorMargin.cpp
//   Creation Date : mer 28 ott 2015 01:21:08
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

#include "C3TextEditorMargin.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorViewport.h"
#include "C3TextEditor.h"

#include "C3TextEditor_private.h"


#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>

#include <math.h>

#define _p m_pC3TE

C3TextEditorMargin::C3TextEditorMargin(
		C3TextEditor * pParent,
		C3TextEditorPrivate * pPrivate
	)
	: QWidget(pParent)
{
	_p = pPrivate;
}

C3TextEditorMargin::~C3TextEditorMargin()
{

}

#define LEFT_MARGIN 3
#define RIGHT_MARGIN 3

int C3TextEditorMargin::widthHint()
{
	int iLineCount = _p->lLines.count();
	int iDigits;
	
	if(iLineCount >= 100)
	{
		if(iLineCount < 1000)
			iDigits = 3; // common case
		else if(iLineCount < 10000)
			iDigits = 4;
		else if(iLineCount < 100000)
			iDigits = 5;
		else if(iLineCount < 1000000)
			iDigits = 6;
		else if(iLineCount < 10000000)
			iDigits = 7;
		else if(iLineCount < 100000000)
			iDigits = 8;
		else
			iDigits = 9;
	} else {
		if(iLineCount < 10)
			iDigits = 1;
		else
			iDigits = 2;
	}
	
	int iTextWidth = ::ceil(_p->fFontZeroSequenceWidths[iDigits]);

	return LEFT_MARGIN + iTextWidth + RIGHT_MARGIN;
}

void C3TextEditorMargin::paintEvent(QPaintEvent * e)
{
	//qDebug("Paint event");
	QRect rPaintRect = e->rect();

	int iYOffset = _p->iTopLine * _p->iFontLineSpacing;

	int iWidth = width();

	QPainter p(this);

	p.fillRect(rPaintRect,_p->pOptions->oMarginBackgroundColor);

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

	int iTopY = (iFirstRow - _p->iTopLine) * _p->iFontLineSpacing;
	
	p.setFont(_p->pOptions->oTextFont);
	
	int iRow = iFirstRow;

	p.setPen(_p->pOptions->oMarginTextColor);

	while(iRow <= iLastRow)
	{
		int iBottomY = iTopY + _p->iFontLineSpacing;

		QString szNum;
		szNum.setNum(iRow + 1);

		if(iRow == _p->oCursor.row)
		{
			p.fillRect(0,iTopY,iWidth,iBottomY - iTopY,_p->pOptions->oMarginActiveBackgroundColor);
			p.setPen(_p->pOptions->oMarginActiveTextColor);
			//p.drawText(LEFT_MARGIN,iTopY + _p->iFontAscent,szNum);
			p.drawText(LEFT_MARGIN,iTopY,iWidth - LEFT_MARGIN - RIGHT_MARGIN,iBottomY - iTopY,Qt::AlignRight | Qt::AlignTop,szNum);
			p.setPen(_p->pOptions->oMarginTextColor);
		} else {
			//p.drawText(LEFT_MARGIN,iTopY + _p->iFontAscent,szNum);
			p.drawText(LEFT_MARGIN,iTopY,iWidth - LEFT_MARGIN - RIGHT_MARGIN,iBottomY - iTopY,Qt::AlignRight | Qt::AlignTop,szNum);
		}

		iRow++;
		iTopY = iBottomY;
	}
}


void C3TextEditorMargin::mousePressEvent(QMouseEvent * e)
{
	C3TextEditorRowColumn rc;
	
	if(!_p->pEditor->viewportPointToRowColumn(e->pos(),rc,true))
		return; // should never happen because we pass true.

	_p->pEditor->cursorMoveTo(rc,false);
	_p->pViewport->update();

	e->accept();
}

