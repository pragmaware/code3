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
#include "C3TextEditorMode.h"

#include <QScrollBar>
#include <QFontMetricsF>

bool C3TextEditor::viewportPointToRowColumn(const QPoint &pnt,C3TextEditorRowColumn &rc,bool bAllowOutOfTextArea)
{
	int iYOffset = _p->iTopLine * _p->iFontLineSpacing;
	QPoint pnt2(pnt.x() + _p->iXOffset,pnt.y() + iYOffset);
	return absolutePointToRowColumn(pnt2,rc,bAllowOutOfTextArea);
}

bool C3TextEditor::absolutePointToRowColumn(const QPoint &pnt,C3TextEditorRowColumn &rc,bool bAllowOutOfTextArea)
{
	rc.row = pnt.y() / _p->iFontLineSpacing;
	int cnt = _p->lLines.count();

	if(cnt < 1)
		return false;

	if(rc.row >= cnt)
	{
		if(!bAllowOutOfTextArea)
			return false;
		rc.row = cnt - 1;
	} if(rc.row < 0)
	{
		if(!bAllowOutOfTextArea)
			return false;
		rc.row = 0;
	}

	return absoluteXPositionToColumn(rc.row,&rc.col,pnt.x(),bAllowOutOfTextArea);
}

bool C3TextEditor::absoluteXPositionToColumn(int iRow,int *piColumn,qreal fXPos,bool bAllowOutOfTextArea)
{
	Q_ASSERT(iRow >= 0);
	Q_ASSERT(iRow < _p->lLines.count());
	Q_ASSERT(piColumn);

	_p->pMode->ensureMetadataUpToDate(
			iRow,
			iRow
		);

	C3TextEditorLine * pLine = _p->lLines.at(iRow);
	Q_ASSERT(pLine);

	int iCharCount = 0;
	
	fXPos -= _p->iLeftMargin;
	
	int iXPos = fXPos;

	*piColumn = 0;
	
	if(iXPos < 0)
	{
		if((iXPos < -_p->iLeftMargin) && (!bAllowOutOfTextArea))
			return false;
		fXPos = 0;
	}

	qreal fX = 0.0;

	foreach(C3TextEditorLine::Block * pBlock,pLine->lBlocks)
	{
		qreal fEndX = fX + pBlock->fWidth;
	
		if(fXPos <= fEndX)
		{
			// in this block
			qreal fWidth = fXPos - fX;

			if(pBlock->uFlags & C3TextEditorLine::Block::IsTabBlock)
			{
				int ll = C3TextEditorFontMetricsUtils::charCountOfNearestTabStop(
						fWidth,
						fX,
						_p->fFontTabStopWidth
					);

				*piColumn += ll;
				return true;
			}

			// This is very tricky. Qt does not provide a simple mean to figure out the cursor position
			// from an x position on the text. We use QFontMetricsF::elidedText() to guess it.

			// Additionally Qt::ElideNone does not work as expected (see QTBUG-40315): it just ignores clipping altogether.
			// So we use Qt::ElideRight here but we must take into account the width of the elision

			QString szPart = _p->pFontMetrics->elidedText(pBlock->szText,Qt::ElideRight,fWidth + _p->fFontElisionWidth);

			if(szPart.endsWith(__utf8("…")))
				szPart.truncate(szPart.length() - 1); // kill the elision

			// OK, now we have a good starting point

			qreal fPrevWidth = C3TextEditorFontMetricsUtils::widthOfNonTabBlock(
					_p->pFontMetrics,
					szPart
				);
			
			int iBlockLength = pBlock->szText.length();
			
			if(fPrevWidth <= fWidth)
			{
				//qDebug("Initial %f part is %s",fPrevWidth,szPart.toUtf8().data());
				// move up adding characters
				for(;;)
				{
					int iPartLength = szPart.length();
					if(iPartLength == iBlockLength)
					{
						*piColumn += iBlockLength;
						return true;
					}

					szPart = pBlock->szText.left(iPartLength + 1);

					qreal fNextWidth = C3TextEditorFontMetricsUtils::widthOfNonTabBlock(
							_p->pFontMetrics,
							szPart
						);
					//qDebug("Attempt %f up with part %s",fNextWidth,szPart.toUtf8().data());

					if(fNextWidth >= fWidth)
					{
						// gotcha.
						qreal fMiddle = (fPrevWidth + fNextWidth) / 2.0;
						
						if(fWidth < fMiddle)
							*piColumn += iPartLength;
						else
							*piColumn += iPartLength + 1;

						return true;
					}
					
					fPrevWidth = fNextWidth;
				}
			} else {
				//qDebug("Initial %f part is %s",fPrevWidth,szPart.toUtf8().data());

				// move down removing characters
				for(;;)
				{
					int iPartLength = szPart.length();
					if(iPartLength == 0)
						return true;

					szPart = pBlock->szText.left(iPartLength - 1);

					qreal fNextWidth = C3TextEditorFontMetricsUtils::widthOfNonTabBlock(
							_p->pFontMetrics,
							szPart
						);
					//qDebug("Attempt %f down with part %s",fNextWidth,szPart.toUtf8().data());
					
					if(fNextWidth <= fWidth)
					{
						// gotcha.
						qreal fMiddle = (fPrevWidth + fNextWidth) / 2.0;
						
						if(fWidth < fMiddle)
							*piColumn += iPartLength - 1;
						else
							*piColumn += iPartLength;

						return true;
					}
					
					fPrevWidth = fNextWidth;
				}
			}
			Q_ASSERT(false); // not reached
		}

		*piColumn += pBlock->szText.length();
		fX = fEndX;
	}

	return bAllowOutOfTextArea;
}

qreal C3TextEditor::columnToAbsoluteXPosition(int iRow,int iColumn)
{
	C3TextEditorLine * pLine = _p->lLines.at(iRow);
	if(!pLine)
		return 0.0;

	_p->pMode->ensureMetadataUpToDate(
			iRow,
			iRow
		);

	if(iColumn > pLine->szText.length())
		iColumn = pLine->szText.length();
	// find cursor block

	qreal fX = 0.0;
	int iCharCount = 0;

	foreach(C3TextEditorLine::Block * pBlock,pLine->lBlocks)
	{
		int iLen = pBlock->szText.length();
		int iEndCharCount = iCharCount + iLen;
		if(iColumn <= iEndCharCount)
		{
			// in this block
			if(pBlock->uFlags & C3TextEditorLine::Block::IsTabBlock)
			{
				fX += C3TextEditorFontMetricsUtils::widthOfTabBlock(
						iColumn - iCharCount,
						fX,
						_p->fFontTabStopWidth
					);
			} else {
				fX += C3TextEditorFontMetricsUtils::widthOfNonTabBlock(
						_p->pFontMetrics,
						pBlock->szText.left(iColumn - iCharCount)
					);
			}
			return fX +  _p->iLeftMargin;
		}
		fX += pBlock->fWidth;
		iCharCount = iEndCharCount;
	}
	
	//qDebug("Blocks %d Col %d Len %d CharCount %d",pLine->lBlocks.count(),iColumn,pLine->szText.length(),iCharCount);

	Q_ASSERT(false); // not reached
	return (qreal)(_p->iLeftMargin) + pLine->fWidth;
}

int C3TextEditor::rowToViewportYPosition(int iRow)
{
	if(iRow < 0)
		iRow = 0;
	else if(iRow >= _p->lLines.count())
		iRow = _p->lLines.count() - 1;

	return (iRow - _p->iTopLine) * _p->iFontLineSpacing;
}
