#ifndef _C3SymbolManagerRequest_h_
#define _C3SymbolManagerRequest_h_
//=============================================================================
//
//   File : C3SymbolManagerRequest.h
//   Creation Date : 2015/12/06 05:01:22
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

#include <QObject>

class C3SymbolManagerRequestPrivate;

class C3SymbolManagerRequest : public QObject
{
	friend class C3SymbolManagerBase;
	Q_OBJECT
private:

	C3SymbolManagerRequestPrivate * m_pSMRP;

public:
	enum Type
	{
		///
		/// Reset everything until now.
		/// Called when workspace changes.
		///
		ResetSymbolStore,
		///
		/// Perform a completion
		///
		PerformCompletion,
		///
		/// Identify a symbol
		///
		IdentifySymbol,
		///
		/// (Re)scan a single file
		///
		ScanEditorFile,
		///
		/// Identify context in a single file
		///
		IdentifyContext,
		///
		/// Identify a class and locate its major components across files
		///
		DescribeClass
	};

public:
	C3SymbolManagerRequest(Type eType);

protected:
	// can be called only by C3SymbolManager
	virtual ~C3SymbolManagerRequest();

public:

	Type type() const;

	// This will add the request to the global symbol manager. You will get the terminated() signal when it finishes.
	// From this moment the request is owned by the symbol manager.
	void start();

	// This will make sure that you will never get the terminated() signal (if you have connected it at all).
	void abort();

	// Returns true if the request was aborted.
	bool aborted();

signals:

	// Warning. NEVER call abort() from the slot connected to this.
	// Do NOT delete the request. It will be deleted by the manager.
	void terminated(C3SymbolManagerRequest * pRequest);

protected:
	// called by C3SymbolManager
	void emitTerminationSignal();
	// called by C3SymbolManager
	void markAsAborted();

}; // class C3SymbolManagerRequest

#endif //!_C3SymbolManagerRequest_h_
