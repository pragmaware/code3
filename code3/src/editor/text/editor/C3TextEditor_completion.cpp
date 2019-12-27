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

#include "C3TextEditorViewport.h"
#include "C3TextEditorLine.h"
#include "C3TextEditorMode.h"
#include "C3TextEditorCompletionWidget.h"


bool C3TextEditor::tabCompletion()
{
	if(!_p->pCompletionWidget)
		return false;
	if(!_p->pCompletionWidget->isVisible())
		return false;

	QString szText = getRangeText(_p->oCompletionRange);
	if(szText.isEmpty())
		return false;

	QString szResult;
	QString szSuffix;

	if(!_p->pCompletionWidget->tabCompletion(szText,szResult,szSuffix))
		return true;

	beginVisualStateChange();

	int len = szResult.length();

	if(len > 1)
	{
		len--;
	
		C3TextEditorRange oRightRange;
		oRightRange.start = _p->oCompletionRange.end;
		oRightRange.end = _p->oCompletionRange.end;
		oRightRange.end.col += len;
	
		QString szRight = getRangeText(oRightRange);
		
		for(int i=len;i>0;i--)
		{
			if(
				szRight.startsWith(szResult.right(i)) &&
				(
					(szRight.length() == i) ||
					(!szRight.at(i).isLetterOrNumber())
				)
			)
			{
				// eat common part
				_p->oCompletionRange.end.col += i;
				break;
			}
		}
	}

	_p->oSelection = _p->oCompletionRange;
	_p->oCursor = _p->oSelection.end;

	insertTextInternal(szResult + szSuffix);

	_p->pCompletionWidget->hide();
	_p->pMode->triggerSymbolCompletion(
			_p->oCursor,
			C3TextEditorMode::SymbolCompletionMatchInTheMiddle | C3TextEditorMode::SymbolCompletionMatchCaseInsensitive
		);

	endVisualStateChange(DontHideCompletionWidget);
	return true;
}

bool C3TextEditor::enterCompletion()
{
	if(!_p->pCompletionWidget)
		return false;
	if(!_p->pCompletionWidget->isVisible())
		return false;

	QString szText = getRangeText(_p->oCompletionRange);
	//if(szText.isEmpty())
	//	return false;

	QString szResult;
	QString szSuffix;

	if(!_p->pCompletionWidget->enterCompletion(szText,szResult,szSuffix))
		return true;

	beginVisualStateChange();
	
	//qDebug("TEXT(%s) RESULT(%s) SUFFIX(%s)",szText.toUtf8().data(),szResult.toUtf8().data(),szSuffix.toUtf8().data());

#if 1
	int len = szResult.length();

	if(len > 1)
	{
		len--;
	
		C3TextEditorRange oRightRange;
		oRightRange.start = _p->oCompletionRange.end;
		oRightRange.end = _p->oCompletionRange.end;
		oRightRange.end.col += len;
	
		QString szRight = getRangeText(oRightRange);
		
		for(int i=len;i>0;i--)
		{
			if(
				szRight.startsWith(szResult.right(i)) &&
				(
					(szRight.length() == i) ||
					(!szRight.at(i).isLetterOrNumber())
				)
			)
			{
				// eat common part
				_p->oCompletionRange.end.col += i;
				break;
			}
		}
	}

	static QString szPars("()");

	if(szSuffix == szPars)
	{
		// Special case. Avoid inserting () if there is already a () on the right
		// or there is "SIGNAL" in the current line (signals and slots)

		C3TextEditorRange oTmpRange;
		oTmpRange.start = _p->oCompletionRange.end;
		oTmpRange.end = _p->oCompletionRange.end;
		oTmpRange.end.col += 2;

		szText = getRangeText(oTmpRange);
		
		if(szText == szPars)
			szSuffix = "";
		else {
			szText = getLineText(_p->oCompletionRange.end.row);
			if(szText.contains("SIGNAL") || szText.contains("SLOT"))
				szSuffix = "";
		}
	}


	//_p->oCompletionRange.end.col += oResult.uDeleteLength;

	_p->oSelection = _p->oCompletionRange;
	_p->oCursor = _p->oSelection.end;

	insertTextInternal(szResult + szSuffix);

#else

	QString szInsert = szResult + szSuffix;

	int len = szInsert.length();

	if(len > 1)
	{
		len--;
	
		C3TextEditorRange oRightRange;
		oRightRange.start = _p->oCompletionRange.end;
		oRightRange.end = _p->oCompletionRange.end;
		oRightRange.end.col += len;
	
		QString szRight = getRangeText(oRightRange);
		
		for(int i=len;i>0;i--)
		{
			if(
				szRight.startsWith(szInsert.right(i)) &&
				(
					(szRight.length() == i) ||
					(!szRight.at(i).isLetterOrNumber())
				)
			)
			{
				// eat common part
				_p->oCompletionRange.end.col += i;
				break;
			}
		}
	}

	//_p->oCompletionRange.end.col += oResult.uDeleteLength;

	_p->oSelection = _p->oCompletionRange;
	_p->oCursor = _p->oSelection.end;

	insertTextInternal(szInsert);

#endif


	_p->pCompletionWidget->hide();

	endVisualStateChange(DontHideCompletionWidget);
	return true;
}

void C3TextEditor::symbolCompletionFailed()
{
	if(!_p->pCompletionWidget)
		return;
	_p->pCompletionWidget->hide();
}


void C3TextEditor::showCompletionWidget(
		const C3TextEditorRange &oCompletedRange,
		QList<C3TextEditorCompletion * > * pCompletions,
		unsigned int uFlags // C3TextEditorMode::SymbolCompletionFlags
	)
{
	Q_ASSERT(pCompletions);

	QString szText = getRangeText(oCompletedRange);

	C3TextEditorRange oWideRangeLeft(
			oCompletedRange.start.row - 3,
			0,
			oCompletedRange.start.row,
			oCompletedRange.start.col
		);
	C3TextEditorRange oWideRangeRight(
			oCompletedRange.end.row,
			oCompletedRange.end.col,
			oCompletedRange.end.row + 3,
			0
		);

	QString szWideRangeLeft = getRangeText(oWideRangeLeft);
	QString szWideRangeRight = getRangeText(oWideRangeRight);

	/*
		This actually DOES happen.
	if(szText.isEmpty())
	{
		// should never happen
		if(_p->pCompletionWidget)
			_p->pCompletionWidget->hide();
		return;
	}
	*/

	if(pCompletions->isEmpty())
	{
		delete pCompletions;
		if(_p->pCompletionWidget)
			_p->pCompletionWidget->hide();
		return;
	}

	if(!_p->pCompletionWidget)
		_p->pCompletionWidget = new C3TextEditorCompletionWidget(_p->pViewport,this);

	int iMaxLen = 0;

	int iHint = _p->pCompletionWidget->showChoices(
			szText,
			pCompletions,
			&iMaxLen,
			szWideRangeLeft,
			szWideRangeRight,
			uFlags
		);
	
	if(iHint < 1)
	{
		// nothing in the view
		_p->pCompletionWidget->hide();
		return;
	}

	int y = rowToViewportYPosition(oCompletedRange.start.row);
	int x = (int)columnToAbsoluteXPosition(oCompletedRange.start.row,oCompletedRange.start.col) - _p->iXOffset;

	int w = (iMaxLen < 35) ? 350 : ((iMaxLen > 80) ? 800 : (iMaxLen * 10));
	int h = iHint;

	// x-1 because of frame around the widget
	if((y+h) >= _p->iViewportHeight)
		_p->pCompletionWidget->setGeometry(x-1,y-h,w,h);
	else
		_p->pCompletionWidget->setGeometry(x-1,y+_p->iFontLineSpacing,w,h);

	_p->pCompletionWidget->show();
	
	_p->oCompletionRange = oCompletedRange;
}


bool C3TextEditor::completionUp()
{
	if(!_p->pCompletionWidget)
		return false;
	if(!_p->pCompletionWidget->isVisible())
		return false;
	_p->pCompletionWidget->keyUp();
	return true;
}

bool C3TextEditor::completionDown()
{
	if(!_p->pCompletionWidget)
		return false;
	if(!_p->pCompletionWidget->isVisible())
		return false;
	_p->pCompletionWidget->keyDown();
	return true;
}
