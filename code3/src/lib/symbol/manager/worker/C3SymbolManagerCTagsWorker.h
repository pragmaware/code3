#ifndef _C3SymbolManagerCTagsWorker_h_
#define _C3SymbolManagerCTagsWorker_h_
//=============================================================================
//
//   File : C3SymbolManagerCTagsWorker.h
//   Creation Date : dom 08 nov 2015 06:55:19
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
#include <QStringList>

class C3SymbolManagerCTagsWorkerPrivate;
class C3SymbolManager;

///
/// \class C3SymbolManagerCTagsWorker
/// \brief The C3SymbolManagerCTagsWorker class
///
/// This class...
///
class C3SymbolManagerCTagsWorker : public QRunnable
{
public:

	enum JobFlags
	{
		JobLimitRecursionDepthTo1 = 1
	};

	///
	/// Creates an instance of C3SymbolManagerCTagsWorker.
	///
	/// You should either pass lPaths and lExclusionPatterns OR the id of an editor.
	/// These two modes are exclusive!
	///
	C3SymbolManagerCTagsWorker(
			int iId,
			C3SymbolManager * pSymbolManager,
			const QString &szCTagsToolPath,
			const QStringList &lPaths,
			const QStringList &lExclusionPatterns,
			int iEditorId = -1,
			unsigned int uFlags = 0
		);

	///
	/// Destroys the instance of C3SymbolManagerCTagsWorker
	/// and frees all the relevant resources
	///
	virtual ~C3SymbolManagerCTagsWorker();

private:

	C3SymbolManagerCTagsWorkerPrivate * m_pC3WCTW;

public:

	int id();

	// Returns true if the abort operation succeeded.
	// Returns false if the abort operation did not succeed
	// because the worker is already inside it's termination
	// call.
	bool abort();

protected:

	virtual void run();

private:

	QString runInternal();
	
	QString saveEditorContentsToTemporaryFile();

}; // class C3SymbolManagerCTagsWorker

#endif //!_C3SymbolManagerCTagsWorker_h_
