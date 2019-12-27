#ifndef _C3NewJavaClassCreator_h_
#define _C3NewJavaClassCreator_h_
//=============================================================================
//
//   File : C3NewJavaClassCreator.h
//   Creation Date : sab 07 nov 2015 06:46:09
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

#include "C3NewObjectCreator.h"

///
/// \class C3NewJavaClassCreator
/// \brief The C3NewJavaClassCreator class
///
/// This class...
///
class C3NewJavaClassCreator : public C3NewObjectCreator
{
public:

	///
	/// Creates an instance of C3NewJavaClassCreator
	///
	C3NewJavaClassCreator();

	///
	/// Destroys the instance of C3NewJavaClassCreator
	/// and frees all the relevant resources
	///
	virtual ~C3NewJavaClassCreator();

public:

	virtual QString name();
	virtual QString icon();

	virtual void run();
	
}; // class C3NewJavaClassCreator

#endif //!_C3NewJavaClassCreator_h_
