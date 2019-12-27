#ifndef _C3WorkspaceOpenFileWorker_h_
#define _C3WorkspaceOpenFileWorker_h_
//=============================================================================
//
//   File : C3WorkspaceOpenFileWorker.h
//   Creation Date : ven 23 ott 2015 01:51:39
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

#include "C3Link.h"

#include <QRunnable>
#include <QHash>
#include <QVariant>

class C3WorkspaceOpenFileWorkerPrivate;
class C3Workspace;

///
/// \class C3WorkspaceOpenFileWorker
/// \brief The C3WorkspaceOpenFileWorker class
///
/// This class...
///
class C3WorkspaceOpenFileWorker : public QRunnable
{
public:

	///
	/// Creates an instance of C3WorkspaceOpenFileWorker
	///
	C3WorkspaceOpenFileWorker(C3Workspace * pWorkspace,const C3Link &oLink);

	///
	/// Destroys the instance of C3WorkspaceOpenFileWorker
	/// and frees all the relevant resources
	///
	virtual ~C3WorkspaceOpenFileWorker();

private:

	C3WorkspaceOpenFileWorkerPrivate * m_pC3WOFW;

public:

	int id() const;
	
	C3Link & link();
	
	// Returns true if the abort operation succeeded.
	// Returns false if the abort operation did not succeed
	// because the worker is already inside it's termination
	// call.
	bool abort();

protected:

	virtual void run();

private:

	QString runInternal();

}; // class C3WorkspaceOpenFileWorker

#endif //!_C3WorkspaceOpenFileWorker_h_
