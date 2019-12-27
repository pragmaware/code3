//=============================================================================
//
//   File : C3TextEditor_io.cpp
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
#include "C3TextEditorFormatReader.h"
#include "C3TextEditorFormatWriter.h"

#include <QThreadPool>

#define _p m_pC3TE

bool C3TextEditor::isLoading()
{
	return _p->pReader != NULL;
}


void C3TextEditor::ensureNoRunningRead()
{
	_p->oReaderMutex.lock();
	if(_p->pReader)
	{
		_p->oReaderMutex.unlock();
		_p->pReader->abort();
		_p->oReaderMutex.lock();
		
		if(_p->pReader)
		{
#if QT_VERSION >= 0x050500
			QThreadPool::globalInstance()->cancel(_p->pReader);
#endif
			_p->pReader = NULL;
		}
	}

	qDeleteAll(_p->lReaderLines);
	_p->lReaderLines.clear();

	_p->oReaderMutex.unlock();
}

void C3TextEditor::slotReaderWakeUp()
{
	_p->oReaderMutex.lock();

	if(_p->bReadingFinished)
	{
		// Redundant call. We have handled the finish condition in a previous call (probably NOT triggered by readerTerminated() though).
		_p->oReaderMutex.unlock();
		return;
	}

	bool bFinished = _p->pReader == NULL;

	bool bLinesAdded = false;

	if(_p->lReaderLines.count() > 0)
	{
		C3TextEditorLinesLocker oLocker(_p);

		foreach(C3TextEditorLine * l,_p->lReaderLines)
		{
			// width is already computed!
			if(_p->fMaximumLineWidth < l->fWidth)
				_p->fMaximumLineWidth = l->fWidth;
			_p->lLines.append(l);
		}
		_p->lReaderLines.clear();
		bLinesAdded = true;
	}

	_p->oReaderMutex.unlock();
	
	if(bFinished)
	{
		// no reader running
		_p->iCurrentFindOperationId = -1;

		if(!_p->szReadingError.isEmpty())
		{
			error(__tr("Failed to read the file %1. %2").arg(path()).arg(_p->szReadingError));
			warning(__tr("The file %1 wasn't fully read: it was opened in read-only mode.").arg(path()));
			if(_p->lLines.count() < 1)
			{
				// add an empty line
				C3TextEditorLine * pLine = new C3TextEditorLine();
				_p->lLines.append(pLine);
			}
			setReadOnly(true);
		} else {
			if(_p->bContentWasAlteredDuringRead)
			{
				warning(__tr("The contents of the file %1 were altering in the read process. %2. The file was opened in read-only mode.").arg(path()).arg(_p->szReadAlterationReason));
				setReadOnly(true);
			} else {
				setReadOnly(false);
			}
			
			if(isVisible())
			{
				QString szLocation = _p->oLink.location();
				//qDebug("[%s] Applying location after read (%s)",path().toUtf8().data(),szLocation.toUtf8().data());
				if(!szLocation.isEmpty())
				{
					cursorMoveTo(szLocation);
					_p->oLink.clearLocation();
				}
			}
		}

		_p->bReadingFinished = true;
	}

	if(bLinesAdded)
		layout();
}

void C3TextEditor::readerAppendLines(C3TextEditorFormatReader * pReader,QLinkedList<C3TextEditorLine *> &lLines)
{
	// This is called on a slave thread!
	_p->oReaderMutex.lock();

	if(_p->pReader != pReader)
	{
		// wrong reader, it's probably a reader that has been canceled
		qDeleteAll(lLines);
		lLines.clear();
		_p->oReaderMutex.unlock();
		return;
	}

	if(lLines.count() > 0)
	{
		if(_p->lReaderLines.isEmpty())
		{
			_p->lReaderLines.swap(lLines);
		} else {
			_p->lReaderLines += lLines;
			lLines.clear();
		}
	}

	_p->oReaderMutex.unlock();

	emit signalReaderWakeUp();
}

void C3TextEditor::readerTerminated(C3TextEditorFormatReader * pReader,const QString &szError,bool bContentWasAltered,const QString &szAlterationReason)
{
	// This is called on a slave thread!
	_p->oReaderMutex.lock();
	
	if(_p->pReader != pReader)
	{
		// wrong reader, it's probably a reader that has been canceled
		_p->oReaderMutex.unlock();
		return;
	}
	
	_p->pReader = NULL;
	
	_p->szReadingError = szError;
	_p->bContentWasAlteredDuringRead = bContentWasAltered;
	_p->szReadAlterationReason = szAlterationReason;

	_p->oReaderMutex.unlock();

	emit signalReaderWakeUp();
}