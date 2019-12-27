#ifndef _C3NewJavaClassGenerator_h_
#define _C3NewJavaClassGenerator_h_
//=============================================================================
//
//   File : C3NewJavaClassGenerator.h
//   Creation Date : mer 11 nov 2015 05:17:24
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

class C3NewJavaClassSettings;
class C3NewJavaClassGeneratorPrivate;

#include <QString>

///
/// \class C3NewJavaClassGenerator
/// \brief The C3NewJavaClassGenerator class
///
/// This class...
///
class C3NewJavaClassGenerator
{
public:

	///
	/// Creates an instance of C3NewJavaClassGenerator
	///
	C3NewJavaClassGenerator(C3NewJavaClassSettings * pSettings);

	///
	/// Destroys the instance of C3NewJavaClassGenerator
	/// and frees all the relevant resources
	///
	~C3NewJavaClassGenerator();

private:

	C3NewJavaClassGeneratorPrivate * m_pC3NJavaCG;

public:

	QString generateJava();
	QString generateDeclaration();

	QString addTrailingNewlines(const QString &szText);

private:

	void fixSettings();

}; // class C3NewJavaClassGenerator

#endif //!_C3NewJavaClassGenerator_h_