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
#include "C3FindOperation.h"


bool C3TextEditor::supportsFindOperations()
{
	return true;
}

bool C3TextEditor::findOperationInitialSearchInSelection()
{
	return !_p->oSelection.isEmpty();
}

QString C3TextEditor::findOperationGetInitialSearchString()
{
	// first line of selection

	QString szSelection;

	C3TextEditorRange oSafeRange = _p->oSelection;

	if(oSafeRange.isEmpty())
		return szSelection;

	int iLineCount = _p->lLines.count();

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
		return szSelection;

	if(oSafeRange.start.row == oSafeRange.end.row)
	{
		C3TextEditorLine * pLine = _p->lLines.at(oSafeRange.start.row);
		int iLen = oSafeRange.end.col - oSafeRange.start.col;
		szSelection = pLine->szText.mid(oSafeRange.start.col,iLen);
	} else {

		C3TextEditorLine * pLine = _p->lLines.at(oSafeRange.start.row);
		szSelection.append(pLine->szText.mid(oSafeRange.start.col));
	}

	return szSelection;
}

// These functions are only available in Qt >= 5.5
// Copied here :)
int QString_indexOf(QString &s,const QRegularExpression &re, int from, QRegularExpressionMatch *rmatch)
{
    if (!re.isValid()) {
        qWarning("QString::indexOf: invalid QRegularExpression object");
        return -1;
    }

    QRegularExpressionMatch match = re.match(s, from);
    if (match.hasMatch()) {
        const int ret = match.capturedStart();
        if (rmatch)
            *rmatch = qMove(match);
        return ret;
    }

    return -1;
}

int QString_lastIndexOf(QString &s,const QRegularExpression &re, int from, QRegularExpressionMatch *rmatch)
{
    if (!re.isValid()) {
        qWarning("QString::lastIndexOf: invalid QRegularExpression object");
        return -1;
    }

    int endpos = (from < 0) ? (s.size() + from + 1) : (from + 1);
    QRegularExpressionMatchIterator iterator = re.globalMatch(s);
    int lastIndex = -1;
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        int start = match.capturedStart();
        if (start < endpos) {
            lastIndex = start;
            if (rmatch)
                *rmatch = qMove(match);
        } else {
            break;
        }
    }

    return lastIndex;
}


bool C3TextEditor::findOperationRun(C3FindOperation * pOperation)
{
	// a find operation always clears our open location, so it will not be applied if
	// we're going to be shown for the first time
	_p->oLink.clearLocation();

	int iLineCount = _p->lLines.count();

	if(iLineCount < 1)
		return false; // doh?

	C3TextEditorRowColumn rc;
	bool bVisualStateChangeStarted = false;

	if(_p->iCurrentFindOperationId != pOperation->id())
	{
		// new find.
		if(pOperation->scope() == C3FindOperation::Selection)
		{
			if(_p->oSelection.isEmpty())
				return false;
			beginVisualStateChange();
			bVisualStateChangeStarted = true;
			_p->oMark = _p->oSelection;
			if(pOperation->direction() == C3FindOperation::Forward)
			{
				_p->oCurrentFindOperationEndPosition = _p->oSelection.end;
				rc = _p->oSelection.start;
			} else {
				_p->oCurrentFindOperationEndPosition = _p->oSelection.start;
				rc = _p->oSelection.end;
			}
		} else {
			_p->oCurrentFindOperationEndPosition = _p->oCursor;
			rc = _p->oCursor;
			beginVisualStateChange();
			bVisualStateChangeStarted = true;
			_p->oMark.clear();
		}
		_p->iCurrentFindOperationId = pOperation->id();
		_p->bCurrentFindOperationFinishedStartRow = false;
	} else {
		if(_p->oCurrentFindOperationEndPosition.row >= iLineCount)
			_p->oCurrentFindOperationEndPosition.row = iLineCount - 1;
		int ll = _p->lLines.at(_p->oCurrentFindOperationEndPosition.row)->szText.length();
		if(_p->oCurrentFindOperationEndPosition.col > ll)
			_p->oCurrentFindOperationEndPosition.col = ll;
		rc = _p->oCursor;
	}

	// Sanitize
	if(rc.row < 0)
		rc.row = 0;

	if(rc.row >= iLineCount)
		rc.row = iLineCount - 1;

	C3TextEditorLine * pLine = _p->lLines.at(rc.row);
	if(rc.col < 0)
		rc.col = 0;
	else if(rc.col > pLine->szText.length())
		rc.col = pLine->szText.length();

	QRegularExpressionMatch oMatch;

	if(pOperation->direction() == C3FindOperation::Forward)
	{
		for(;;)
		{
			pLine = _p->lLines.at(rc.row);
			
			int idx = QString_indexOf(pLine->szText,pOperation->regularExpression(),rc.col,&oMatch);
			if(idx >= 0)
			{
				if(
					(rc.row == _p->oCurrentFindOperationEndPosition.row) &&
					(
						(_p->bCurrentFindOperationFinishedStartRow) ||
						(pOperation->scope() == C3FindOperation::Selection)
					) &&
					(idx >= _p->oCurrentFindOperationEndPosition.col)
				)
					break; // reached end

				if(!bVisualStateChangeStarted)
					beginVisualStateChange();
				_p->oSelection.start.row = rc.row;
				_p->oSelection.start.col = idx;
				_p->oSelection.end.row = rc.row;
				_p->oSelection.end.col = idx + oMatch.capturedLength(0);
				_p->oCursor.row = rc.row;
				_p->oCursor.col = _p->oSelection.end.col;
				endVisualStateChange();
				pOperation->setMatch(oMatch);
				return true;
			}
	
			if(
				(rc.row == _p->oCurrentFindOperationEndPosition.row) &&
				(
					(_p->bCurrentFindOperationFinishedStartRow) ||
					(pOperation->scope() == C3FindOperation::Selection)
				)
			)
				break; // reached end
	
			rc.row++;
			rc.col = 0;
			_p->bCurrentFindOperationFinishedStartRow = true;
			if(rc.row >= iLineCount)
				rc.row = 0;
		}
	} else {
		for(;;)
		{
			pLine = _p->lLines.at(rc.row);
			
			if(rc.col > 0)
			{
				int idx = QString_lastIndexOf(pLine->szText,pOperation->regularExpression(),rc.col-1,&oMatch);
				if(idx >= 0)
				{
					if(
						(rc.row == _p->oCurrentFindOperationEndPosition.row) &&
						(
							(_p->bCurrentFindOperationFinishedStartRow) ||
							(pOperation->scope() == C3FindOperation::Selection)
						) &&
						(idx <= _p->oCurrentFindOperationEndPosition.col)
					)
						break; // reached end
				
					if(!bVisualStateChangeStarted)
						beginVisualStateChange();
					_p->oSelection.start.row = rc.row;
					_p->oSelection.start.col = idx;
					_p->oSelection.end.row = rc.row;
					_p->oSelection.end.col = idx + oMatch.capturedLength(0);
					_p->oCursor.row = rc.row;
					_p->oCursor.col = _p->oSelection.start.col;
					endVisualStateChange();
					pOperation->setMatch(oMatch);
					return true;
				}
			}
	
			if(
				(rc.row == _p->oCurrentFindOperationEndPosition.row) &&
				(
					(_p->bCurrentFindOperationFinishedStartRow) ||
					(pOperation->scope() == C3FindOperation::Selection)
				)
			)
				break; // reached end
	
			rc.row--;
			_p->bCurrentFindOperationFinishedStartRow = true;
			if(rc.row < 0)
				rc.row = iLineCount - 1;

			pLine = _p->lLines.at(rc.row);
			rc.col = pLine->szText.length();
		}
	}
	
	if(bVisualStateChangeStarted)
		endVisualStateChange();

	return false;
}


bool C3TextEditor::findOperationReplace(C3FindOperation * pOperation,const QString &szText)
{
	if(_p->oSelection.isEmpty())
		return false;

	beginVisualStateChange();

	C3TextEditorRowColumn rc = _p->oSelection.start;
	C3TextEditorRange mark = _p->oMark;

	int iStartLineCount = (_p->oSelection.end.row - _p->oSelection.start.row) + 1;
	int iStartLastRowLength = (_p->oSelection.end.row == _p->oSelection.start.row) ?
			(_p->oSelection.end.col - _p->oSelection.start.col) :
			_p->oSelection.end.col;
	bool bSelectionEndsInMarkEndRow = mark.end.row == _p->oSelection.end.row;
	bool bSelectionEndsAtOperationEndRow = (_p->oCurrentFindOperationEndPosition.row == _p->oSelection.end.row);
	bool bNeedToAdjustOperationEnd = (_p->oCurrentFindOperationEndPosition >= rc);
	bool bNeedToAdjustMark = mark.contains(_p->oSelection);

	QString szToInsert;

	if(pOperation->captureReplacementsEnabled())
	{
		QString szReplaced = szText;
		int iLast = pOperation->match().lastCapturedIndex();
		
		static QString szBS("\\\\");
		static QString szB("\\");
		static QString szCrap("¹#è");
		szReplaced.replace(szBS,szCrap);

		for(int i=0;i<=iLast;i++)
			szReplaced.replace(__utf8("\\%1").arg(i),pOperation->match().captured(i));
		
		static QString szNewlineEscape("\\n");
		static QString szNewline("\n");
		static QString szTabEscape("\\t");
		static QString szTab("\t");

		szReplaced.replace(szNewlineEscape,szNewline);
		szReplaced.replace(szTabEscape,szTab);

		szReplaced.replace(szCrap,szB);

		szToInsert = szReplaced;
	} else {
		szToInsert = szText;
	}

	insertTextInternal(szToInsert);

	// Put the cursor at the beginning if the operation goes backwards
	if(pOperation->direction() == C3FindOperation::Backward)
		_p->oCursor = rc;

	if(bNeedToAdjustMark || bNeedToAdjustOperationEnd)
	{
		QStringList sl = szToInsert.split(QChar('\n'));
		
		Q_ASSERT(sl.count() >= 1);
	
		int iInsertedLines = sl.count();
		int iLastLineLength = sl.at(sl.count() - 1).length();

		int iLineDiff = iInsertedLines - iStartLineCount;

		if(bNeedToAdjustMark)
		{
			mark.end.row += iLineDiff;

			if(bSelectionEndsInMarkEndRow)
				mark.end.col += (iLastLineLength - iStartLastRowLength);
		}
		
		if(bNeedToAdjustOperationEnd)
		{
			_p->oCurrentFindOperationEndPosition.row += iLineDiff;
			if(bSelectionEndsAtOperationEndRow)
				_p->oCurrentFindOperationEndPosition.col += (iLastLineLength - iStartLastRowLength);
		}
	}

	_p->oMark = mark;

	endVisualStateChange();

	return true;
}

void C3TextEditor::findOperationReplaceAllFinished(C3FindOperation * pOperation)
{
	if(_p->iCurrentFindOperationId != pOperation->id())
		return; // hum.. a different op?

	if(pOperation->scope() != C3FindOperation::Selection)
		return; // nothing to do

	if(_p->oMark.isEmpty())
		return;

	beginVisualStateChange();
	_p->oSelection = _p->oMark;
	if(pOperation->direction() == C3FindOperation::Backward)
		_p->oCursor = _p->oSelection.start;
	else
		_p->oCursor = _p->oSelection.end;
	endVisualStateChange();
}
