//=============================================================================
//
//   File : C3FileSnippetExtractorWorker.cpp
//   Creation Date : 2015/12/31 00:00:51
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

#include "C3FileSnippetExtractorWorker.h"

#include <QMutex>
#include <QFile>

class C3FileSnippetExtractorWorkerPrivate
{
public:
	QMutex oMutex;
	C3FileSnippetExtractorWorkerListener * pListener;
	QString szPath;
	quint32 uFromLine;
	quint32 uToLine;
	QStringList * pLines;
	QString szError;
};

#define _p m_pC3FSEW

C3FileSnippetExtractorWorker::C3FileSnippetExtractorWorker(C3FileSnippetExtractorWorkerListener * pListener,const QString &szPath,quint32 uFromLine,quint32 uToLine)
	: QRunnable()
{
	_p = new C3FileSnippetExtractorWorkerPrivate();
	_p->pLines = NULL;
	_p->pListener = pListener;
	_p->szPath = szPath;
	_p->uFromLine = uFromLine;
	_p->uToLine = uToLine;
}

C3FileSnippetExtractorWorker::~C3FileSnippetExtractorWorker()
{
	_p->oMutex.lock();
	if(_p->pLines)
		delete _p->pLines;
	_p->oMutex.unlock();
	delete _p;
}

bool C3FileSnippetExtractorWorker::abort()
{
	if(!_p->oMutex.tryLock())
		return false;
	_p->pListener = NULL;
	_p->oMutex.unlock();
	return true;
}

void C3FileSnippetExtractorWorker::runInternal()
{
	if(!_p->pListener)
	{
		_p->szError = __tr("Aborted");
		return;
	}

	QFile f(_p->szPath);
	
	if(!f.open(QFile::ReadOnly))
	{
		_p->szError = __tr("Can't open file: %1").arg(f.error());
		return;
	}

	quint32 uLine = 0;

	char buffer[1024]; // 1K lines are far too long

	while(uLine < _p->uFromLine)
	{
		if(!_p->pListener)
		{
			_p->szError = __tr("Aborted");
			return;
		}
	
		qint64 iRet = f.readLine(buffer,1024);
		if(iRet < 0)
		{
			_p->szError = __tr("File read error: %1").arg(f.error());
			return;
		}
		
		if(iRet < 1)
		{
			_p->szError = __tr("Start line not found");
			return;
		}
		
		if(iRet >= 1024)
		{
			_p->szError = __tr("File has lines that are too long to extract");
			return;
		}
		
		uLine++;
	}

	// got start line.
	while(uLine <= _p->uToLine)
	{
		if(!_p->pListener)
		{
			_p->szError = __tr("Aborted");
			return;
		}

		qint64 iRet = f.readLine(buffer,1024);
		if(iRet < 0)
		{
			_p->szError = __tr("File read error: %1").arg(f.error());
			return;
		}
		
		if(iRet < 1)
			return;
		
		if(iRet >= 1024)
		{
			_p->szError = __tr("File has lines that are too long to extract");
			return;
		}
		
		_p->pLines->append(QString::fromUtf8(buffer));

		uLine++;
	}
}

void C3FileSnippetExtractorWorker::run()
{
	_p->pLines = new QStringList();

	runInternal();

	_p->oMutex.lock();
	if(_p->pListener)
		_p->pListener->onFileSnippetExtractWorkerTerminated(this,_p->pLines,_p->szError);
	else
		delete _p->pLines;
	_p->pLines = NULL;
	_p->oMutex.unlock();
}
