#ifndef _C3NewObjectCreator_h_
#define _C3NewObjectCreator_h_
//=============================================================================
//
//   File : C3NewObjectCreator.h
//   Creation Date : sab 07 nov 2015 05:57:13
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

///
/// \class C3NewObjectCreator
/// \brief The C3NewObjectCreator class
///
/// This class...
///
class C3NewObjectCreator
{
public:

	///
	/// Creates an instance of C3NewObjectCreator
	///
	C3NewObjectCreator();

	///
	/// Destroys the instance of C3NewObjectCreator
	/// and frees all the relevant resources
	///
	virtual ~C3NewObjectCreator();

public:

	virtual QString name() = 0;

	virtual QString icon() = 0;

	virtual void run() = 0;

}; // class C3NewObjectCreator

#endif //!_C3NewObjectCreator_h_
