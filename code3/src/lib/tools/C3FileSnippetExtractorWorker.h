#ifndef _C3FileSnippetExtractorWorker_h_
#define _C3FileSnippetExtractorWorker_h_
//=============================================================================
//
//   File : C3FileSnippetExtractorWorker.h
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

#include "c3_config.h"

#include <QRunnable>
#include <QString>
#include <QStringList>

class C3FileSnippetExtractorWorkerPrivate;
class C3FileSnippetExtractorWorker;

class C3FileSnippetExtractorWorkerListener
{
public:
	virtual void onFileSnippetExtractWorkerTerminated(
			C3FileSnippetExtractorWorker * pWorker,
			QStringList * pLines,
			const QString &szError
		) = 0;
};

class C3FileSnippetExtractorWorker : public QRunnable
{
public:
	C3FileSnippetExtractorWorker(C3FileSnippetExtractorWorkerListener * pListener,const QString &szPath,quint32 uFromLine,quint32 uToLine);
	virtual ~C3FileSnippetExtractorWorker();

private:
	C3FileSnippetExtractorWorkerPrivate * m_pC3FSEW;

public:

	// Returns true if the abort operation succeeded.
	// Returns false if the abort operation did not succeed
	// because the worker is already inside it's termination
	// call.
	bool abort();
	
protected:

	virtual void run();

private:

	void runInternal();

}; // class C3FileSnippetExtractorWorker

#endif //!_C3FileSnippetExtractorWorker_h_