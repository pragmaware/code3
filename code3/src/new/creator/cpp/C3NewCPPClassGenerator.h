#ifndef _C3NewCPPClassGenerator_h_
#define _C3NewCPPClassGenerator_h_
//=============================================================================
//
//   File : C3NewCPPClassGenerator.h
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

class C3NewCPPClassSettings;
class C3NewCPPClassGeneratorPrivate;

#include <QString>

///
/// \class C3NewCPPClassGenerator
/// \brief The C3NewCPPClassGenerator class
///
/// This class...
///
class C3NewCPPClassGenerator
{
public:

	///
	/// Creates an instance of C3NewCPPClassGenerator
	///
	C3NewCPPClassGenerator(C3NewCPPClassSettings * pSettings);

	///
	/// Destroys the instance of C3NewCPPClassGenerator
	/// and frees all the relevant resources
	///
	~C3NewCPPClassGenerator();

private:

	C3NewCPPClassGeneratorPrivate * m_pC3NCPPCG;

public:

	bool needsCPPFile();
	bool needsHFile();

	QString generateDeclaration();
	QString generateImplementation();

	QString generateH();
	QString generateCPP();

	QString addTrailingNewlines(const QString &szText);

private:

	void fixSettings();

}; // class C3NewCPPClassGenerator

#endif //!_C3NewCPPClassGenerator_h_