#ifndef _C3WorkspaceFileScanWorker_h_
#define _C3WorkspaceFileScanWorker_h_
//=============================================================================
//
//   File : C3WorkspaceFileScanWorker.h
//   Creation Date : 2015/11/14 03:28:41
//   Project : 
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

#include <QRunnable>

#include <QStringList>

class C3WorkspaceFileScanWorkerPrivate;
class C3Workspace;

class C3WorkspaceFileScanWorker : public QRunnable
{
public:
	C3WorkspaceFileScanWorker(
			C3Workspace * pWorkspace,
			const QStringList &lPaths,
			const QStringList &lExclusionPatterns,
			int iMaxDepth
		);
	virtual ~C3WorkspaceFileScanWorker();

private:
	C3WorkspaceFileScanWorkerPrivate * m_pC3WFSW;

public:

	// Returns true if the abort operation succeeded.
	// Returns false if the abort operation did not succeed
	// because the worker is already inside it's termination
	// call.
	bool abort();

protected:

	virtual void run();

private:

	QString runInternal();

	void scanPath(const QString &szPath);
	void scanPathInternal(const QString &szPath);

}; // class C3WorkspaceFileScanWorker

#endif //!_C3WorkspaceFileScanWorker_h_
