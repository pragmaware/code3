#ifndef _C3NewCPPClassSettings_h_
#define _C3NewCPPClassSettings_h_
//=============================================================================
//
//   File : C3NewCPPClassSettings.h
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
/// \class C3NewCPPClassSettings
/// \brief The C3NewCPPClassSettings class
///
/// This class...
///
class C3NewCPPClassSettings
{
public:
	QString szClassName;

	QString szBaseClassNames; // Separated by ,
	QString szBaseClassIncludeFiles; // Separated by ,

	bool bBaseClassIsQObject;
	
	QString szConstructorArguments;

	QString szAPIDeclarator;
	
	QString szCPPFileHeader;
	QString szCPPFilePreamble;
	QString szCPPFileTrailer;

	QString szHFileHeader;
	QString szHFilePreamble;
	QString szHFileTrailer;

	bool bHasVirtualFunctions;
	bool bHideMembersViaPrivateClass;
	
	bool bGenerateCopyConstructor;
	bool bGenerateMoveConstructor;
	bool bGenerateAssignmentOperator;

	QString szCPPFilePath;
	QString szHFilePath;

	enum ClassType
	{
		SimpleStuctWithPublicMembers,
		Namespace,
		TemplateClass,
		StandardClass,
		SingletonWithInitDoneFunctions,
		SingletonWithUserControlledInstantiation,
		SingletonWithLazyInstantiation
	};

	ClassType eClassType;

	// Internal stuff, used by the generator
	bool bGenerateCPP;
	bool bGenerateConstructorDestructor;
	bool bInlineConstructorDestructor;
	QList<QString> lBaseClassNames; // including template and stuff
	QString szFullClassName; // including template parameter
	QString szTemplateParameter;
	QString szPrivateMemberName;

	static QList<QString> splitClassNameList(const QString &nl);

}; // class C3NewCPPClassSettings

#endif //!_C3NewCPPClassSettings_h_