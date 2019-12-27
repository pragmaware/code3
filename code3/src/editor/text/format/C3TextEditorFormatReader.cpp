//=============================================================================
//
//   File : C3TextEditorFormatReader.cpp
//   Creation Date : gio 22 ott 2015 22:50:40
//   Project : Code 3
//   Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This file is part of the Code 3 Editor distribution
//   Copyright (C) 2015-2019 Szymon Tomasz Stefanek <sts at pragmaware dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 3
//   of the License, or (at your option) any later version
//
//=============================================================================

#include "C3TextEditorFormatReader.h"
#include "C3TextEditor.h"
#include "C3TextEditorLine.h"
#include "C3TextEditorFontMetricsUtils.h"

#include <QFontMetricsF>
#include <QMutex>

class C3TextEditorFormatReaderPrivate
{
public:
	// Shared stuff
	QMutex oMutex;
	C3TextEditor * pEditor;
	
	// Internal stuff
	QFontMetricsF * pFontMetrics;
	int fFontTabStopWidth;
	QString szPath;
	bool bContentAltered;
	QString szAlterationReason;
	QString szError;
};




#define _p m_pTEFRPP


C3TextEditorFormatReader::C3TextEditorFormatReader()
	: QRunnable()
{
	_p = new C3TextEditorFormatReaderPrivate();
	_p->pEditor = NULL;
	_p->bContentAltered = false;
	_p->pFontMetrics = NULL;
}

C3TextEditorFormatReader::~C3TextEditorFormatReader()
{
	if(_p->pFontMetrics)
		delete _p->pFontMetrics;
	delete _p;
}

void C3TextEditorFormatReader::setup(const QString &szPath,C3TextEditor * pEditor,const QFontMetricsF &oFontMetrics,int fFontTabStopWidth)
{
	Q_ASSERT(!_p->pEditor);
	_p->szPath = szPath;
	_p->pEditor = pEditor;
	_p->pFontMetrics = new QFontMetricsF(oFontMetrics);
	_p->fFontTabStopWidth = fFontTabStopWidth;
}

void C3TextEditorFormatReader::abort()
{
	_p->oMutex.lock();
	_p->pEditor = NULL;
	_p->oMutex.unlock();
}

bool C3TextEditorFormatReader::aborted()
{
	// Does not make sense to lock here as even with locking the returned value may be outdated
	return _p->pEditor == NULL;
}

const QString & C3TextEditorFormatReader::path()
{
	return _p->szPath;
}

void C3TextEditorFormatReader::run()
{
	_p->oMutex.lock();
	bool bAborted = _p->pEditor == NULL;
	_p->oMutex.unlock();

	if(bAborted)
		return;

	runInternal();

	_p->oMutex.lock();
	if(_p->pEditor)
		_p->pEditor->readerTerminated(this,_p->szError,_p->bContentAltered,_p->szAlterationReason);
	_p->oMutex.unlock();
}

void C3TextEditorFormatReader::error(const QString &szError)
{
	_p->szError = szError;
}

void C3TextEditorFormatReader::emitLines(QLinkedList<C3TextEditorLine *> &lLines)
{
	_p->oMutex.lock();
	if(_p->pEditor)
	{
		foreach(C3TextEditorLine * l,lLines)
			l->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(_p->pFontMetrics,l->szText,_p->fFontTabStopWidth);
		_p->pEditor->readerAppendLines(this,lLines);
	} else {
		qDeleteAll(lLines);
		lLines.clear();
	}
	_p->oMutex.unlock();
}

void C3TextEditorFormatReader::markContentAsAltered(const QString &szReason)
{
	_p->bContentAltered = true;
	_p->szAlterationReason = szReason;
}

