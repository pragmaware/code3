#ifndef _C3NewJavaClassSettings_h_
#define _C3NewJavaClassSettings_h_
//=============================================================================
//
//   File : C3NewJavaClassSettings.h
//   Creation Date : mer 11 nov 2015 03:41:24
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
/// \class C3NewJavaClassSettings
/// \brief The C3NewJavaClassSettings class
///
/// This class...
///
class C3NewJavaClassSettings
{
public:
	QString szPackage;
	QString szClassName;
	QString szBaseClassName;
	
	QString szJavaFileHeader;
	QString szJavaFilePreamble;
	QString szJavaFileTrailer;

	QString szJavaFilePath;

	enum ClassType
	{
		Interface,
		StandardClass,
		SingletonWithInitDoneFunctions,
		SingletonWithUserControlledInstantiation,
		SingletonWithLazyInstantiation
	};

	ClassType eClassType;

	// Internal stuff, used by the generator
	QString szFullClassName; // including template parameter
	QString szGenericParameter;

}; // class C3NewJavaClassSettings

#endif //!_C3NewJavaClassSettings_h_