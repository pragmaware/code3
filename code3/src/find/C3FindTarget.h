#ifndef _C3FindTarget_h_
#define _C3FindTarget_h_
//=============================================================================
//
//   File : C3FindTarget.h
//   Creation Date : dom 01 nov 2015 19:55:29
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

#include <QString>

#include "C3FindOperation.h"

///
/// \class C3FindTarget
/// \brief The C3FindTarget class
///
/// This class...
///
class C3FindTarget
{
public:
	enum Type
	{
		Editor
	};

public:

	///
	/// Creates an instance of C3FindTarget
	///
	C3FindTarget(Type eType);

	///
	/// Destroys the instance of C3FindTarget
	/// and frees all the relevant resources
	///
	virtual ~C3FindTarget();

private:

	///
	/// the target type
	///
	Type m_eType;

public:

	///
	/// Returns the target type
	///
	Type type() const
	{
		return m_eType;
	}

	virtual bool supportsFindOperations() = 0;

	virtual QString findOperationGetInitialSearchString() = 0;

	// The target must remember the id of the last find operation.
	// If the current id is not equal to the last then it should reset
	// the find operation start position.
	// The target must call pOperation->setMatch() and return true
	// if a match was found and return false if no match was found
	// and the start position has been reached.
	virtual bool findOperationRun(C3FindOperation * pOperation) = 0;

	// Replace the current selection that was found by the specified operation.
	virtual bool findOperationReplace(C3FindOperation * pOperation,const QString &szText) = 0;
	
	// A Replace All operation is finished. You might want to restore the full selection.
	virtual void findOperationReplaceAllFinished(C3FindOperation * pOperation);

}; // class C3FindTarget

#endif //!_C3FindTarget_h_
