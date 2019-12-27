//=============================================================================
//
//   File : C3NewCPPClassGenerator.cpp
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

#include "C3NewCPPClassGenerator.h"

#include "C3NewCPPClassSettings.h"
#include "C3NewObjectUtils.h"
#include "C3StringUtils.h"

#include <QStringList>
#include <QRegExp>

class C3NewCPPClassGeneratorPrivate
{
public:
	C3NewCPPClassSettings * pSettings;
};

#define _p m_pC3NCPPCG

C3NewCPPClassGenerator::C3NewCPPClassGenerator(C3NewCPPClassSettings * pSettings)
{
	_p = new C3NewCPPClassGeneratorPrivate();
	_p->pSettings = pSettings;

	fixSettings();
}

C3NewCPPClassGenerator::~C3NewCPPClassGenerator()
{
	delete _p;
}

void C3NewCPPClassGenerator::fixSettings()
{
	_p->pSettings->bGenerateCPP = true;
	_p->pSettings->bInlineConstructorDestructor = false;
	_p->pSettings->bGenerateConstructorDestructor = true;

	if(!_p->pSettings->szClassName.isEmpty())
	{
		int idx = _p->pSettings->szClassName.indexOf(QChar('<'));
		if(idx >= 0)
		{
			_p->pSettings->szTemplateParameter = _p->pSettings->szClassName.mid(idx+1);
			while(_p->pSettings->szTemplateParameter.endsWith(QChar('>')))
				_p->pSettings->szTemplateParameter.truncate(_p->pSettings->szTemplateParameter.length() - 1);
			_p->pSettings->szClassName.truncate(idx);
		} else {
			_p->pSettings->szTemplateParameter = "T";
		}
	}

	_p->pSettings->szFullClassName = _p->pSettings->szClassName;

	switch(_p->pSettings->eClassType)
	{
		case C3NewCPPClassSettings::SimpleStuctWithPublicMembers:
			_p->pSettings->bGenerateCPP = false;
			_p->pSettings->bBaseClassIsQObject = false;
			_p->pSettings->bInlineConstructorDestructor = false;
			_p->pSettings->bGenerateConstructorDestructor = false;
			_p->pSettings->bHideMembersViaPrivateClass = false;
		break;
		case C3NewCPPClassSettings::Namespace:
			_p->pSettings->szBaseClassNames = QString();
			_p->pSettings->bBaseClassIsQObject = false;
			_p->pSettings->bGenerateConstructorDestructor = false;
			_p->pSettings->bInlineConstructorDestructor = false;
			_p->pSettings->bHideMembersViaPrivateClass = false;
		break;
		case C3NewCPPClassSettings::TemplateClass:
			_p->pSettings->bGenerateCPP = false;
			_p->pSettings->bBaseClassIsQObject = false;
			_p->pSettings->bInlineConstructorDestructor = true;
			_p->pSettings->bGenerateConstructorDestructor = true;
			_p->pSettings->szFullClassName = __utf8("%1<%2>").arg(_p->pSettings->szClassName).arg(_p->pSettings->szTemplateParameter);
			_p->pSettings->bHideMembersViaPrivateClass = false;
		break;
		case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
			_p->pSettings->szConstructorArguments = QString();
		break;
		case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
		break;
		case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
			_p->pSettings->szConstructorArguments = QString();
		break;
		//case C3NewCPPClassSettings::StandardClass:
		default:
			
		break;
	}


	switch(_p->pSettings->eClassType)
	{
		case C3NewCPPClassSettings::StandardClass:
		case C3NewCPPClassSettings::TemplateClass:
			// nuthin
		break;
		default:
			_p->pSettings->bGenerateCopyConstructor = false;
			_p->pSettings->bGenerateMoveConstructor = false;
			_p->pSettings->bGenerateAssignmentOperator = false;
		break;
	}

	// Fixup some values so we don't need to check them later.
	if(_p->pSettings->szBaseClassNames.isEmpty())
	{
		_p->pSettings->bBaseClassIsQObject = false;
		_p->pSettings->szBaseClassIncludeFiles = QString();
	}
	
	if(_p->pSettings->bHideMembersViaPrivateClass)
	{
		QString szTmp = _p->pSettings->szClassName;
		szTmp.replace(QRegExp("[^A-Z]"),QString());
		if(szTmp.isEmpty())
			szTmp = "Private";
		_p->pSettings->szPrivateMemberName = __utf8("m_p%1").arg(szTmp);
	}
	
	_p->pSettings->lBaseClassNames = C3NewCPPClassSettings::splitClassNameList(_p->pSettings->szBaseClassNames);
}

bool C3NewCPPClassGenerator::needsCPPFile()
{
	return _p->pSettings->bGenerateCPP;
}

QString C3NewCPPClassGenerator::generateDeclaration()
{
	// Includes first.
	QStringList lRawIncludes = _p->pSettings->szBaseClassIncludeFiles.split(QChar(','),QString::SkipEmptyParts);

	QStringList lIncludes;

	Q_FOREACH(QString szRawInclude,lRawIncludes)
	{
		QString szRI = szRawInclude.trimmed();
	
		if(szRI.startsWith(QChar('<')) || szRI.startsWith(QChar('"')))
			lIncludes.append(__utf8("#include %1").arg(szRI));
		else
			lIncludes.append(__utf8("#include \"%1\"").arg(szRI));
	}

	QStringList lPreamble;

	if(_p->pSettings->bHideMembersViaPrivateClass)
		lPreamble.append(__utf8("class %1Private;").arg(_p->pSettings->szClassName));

	QString szBody;

	if(_p->pSettings->eClassType == C3NewCPPClassSettings::Namespace)
	{
		szBody = __utf8(
				"namespace %1\n"
				"{\n"
				"\n"
				"} // namespace %1\n"
			).arg(_p->pSettings->szClassName);
	} else {
		// not namespace, needs constructor, destructor and stuff

		QStringList lConstructDestruct;

		if(_p->pSettings->bGenerateConstructorDestructor)
		{
			if(_p->pSettings->bInlineConstructorDestructor)
			{
				if(!_p->pSettings->szConstructorArguments.isEmpty())
					lConstructDestruct.append(__utf8("	%1(%2)").arg(_p->pSettings->szClassName).arg(_p->pSettings->szConstructorArguments));
				else
					lConstructDestruct.append(__utf8("	%1()").arg(_p->pSettings->szClassName));
	
				if(_p->pSettings->lBaseClassNames.count() > 0)
				{
					QString szCD = _p->pSettings->lBaseClassNames.join(__ascii("(), "));
					lConstructDestruct.append(__utf8("		: %1()").arg(szCD));
				}
	
				lConstructDestruct.append(__utf8("	{"));

				switch(_p->pSettings->eClassType)
				{
					case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
						lConstructDestruct.append(__utf8("		m_pInstance = this;"));
					break;
					default:
						// nuthin
					break;
				}

				lConstructDestruct.append(__utf8("	}"));
				lConstructDestruct.append(QString());
	
				if(_p->pSettings->bGenerateCopyConstructor)
				{
					lConstructDestruct.append(__utf8("	%1(const %1 &src);").arg(_p->pSettings->szClassName));

					if(_p->pSettings->lBaseClassNames.count() > 0)
					{
						QString szCD = _p->pSettings->lBaseClassNames.join(__ascii("(), "));
						lConstructDestruct.append(__utf8("		: %1()").arg(szCD));
					}
	
					lConstructDestruct.append(__utf8("	{"));
					lConstructDestruct.append(__utf8("		// TODO: Implement copy constructor"));
					lConstructDestruct.append(__utf8("	}"));
					lConstructDestruct.append(QString());
				}
				if(_p->pSettings->bGenerateMoveConstructor)
				{
					lConstructDestruct.append(__utf8("	%1(const %1 &&src);").arg(_p->pSettings->szClassName));

					if(_p->pSettings->lBaseClassNames.count() > 0)
					{
						QString szCD = _p->pSettings->lBaseClassNames.join(__ascii("(), "));
						lConstructDestruct.append(__utf8("		: %1()").arg(szCD));
					}
	
					lConstructDestruct.append(__utf8("	{"));
					lConstructDestruct.append(__utf8("		// TODO: Implement move constructor"));
					lConstructDestruct.append(__utf8("	}"));
					lConstructDestruct.append(QString());
				}
	
				if(_p->pSettings->bHasVirtualFunctions)
					lConstructDestruct.append(__utf8("	virtual ~%1()").arg(_p->pSettings->szClassName));
				else
					lConstructDestruct.append(__utf8("	~%1()").arg(_p->pSettings->szClassName));
	
				lConstructDestruct.append(__utf8("	{"));

				switch(_p->pSettings->eClassType)
				{
					case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
						lConstructDestruct.append(__utf8("		m_pInstance = NULL;"));
					break;
					default:
						// nuthin
					break;
				}

				lConstructDestruct.append(__utf8("	}"));
				lConstructDestruct.append(QString());
	
			} else {
	
				if(!_p->pSettings->szConstructorArguments.isEmpty())
					lConstructDestruct.append(__utf8("	%1(%2);").arg(_p->pSettings->szClassName).arg(_p->pSettings->szConstructorArguments));
				else
					lConstructDestruct.append(__utf8("	%1();").arg(_p->pSettings->szClassName));

				if(_p->pSettings->bGenerateCopyConstructor)
					lConstructDestruct.append(__utf8("	%1(const %1 &src);").arg(_p->pSettings->szClassName));
				if(_p->pSettings->bGenerateMoveConstructor)
					lConstructDestruct.append(__utf8("	%1(const %1 &&src);").arg(_p->pSettings->szClassName));

				if(_p->pSettings->bHasVirtualFunctions)
					lConstructDestruct.append(__utf8("	virtual ~%1();").arg(_p->pSettings->szClassName));
				else
					lConstructDestruct.append(__utf8("	~%1();").arg(_p->pSettings->szClassName));
		
			}
		}
		
		QStringList lPublicFunctions;
		QStringList lPrivateVariables;
		
		switch(_p->pSettings->eClassType)
		{
			case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
			case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
			case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
				lPrivateVariables.append(__utf8("	static %1 * m_pInstance;").arg(_p->pSettings->szFullClassName));
			break;
			default:
				// nuthin
			break;
		}

		if(_p->pSettings->bHideMembersViaPrivateClass)
			lPrivateVariables.append(__utf8("	%1Private * %2;").arg(_p->pSettings->szClassName).arg(_p->pSettings->szPrivateMemberName));

		switch(_p->pSettings->eClassType)
		{
			case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
			case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
				lPublicFunctions.append(__utf8("	static %1 * instance()").arg(_p->pSettings->szFullClassName));
				lPublicFunctions.append(__utf8("	{"));
				lPublicFunctions.append(__utf8("		return m_pInstance;"));
				lPublicFunctions.append(__utf8("	}"));
				lPublicFunctions.append(QString());
			break;
			case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
				lPublicFunctions.append(__utf8("	static %1 * instance()").arg(_p->pSettings->szFullClassName));
				lPublicFunctions.append(__utf8("	{"));
				lPublicFunctions.append(__utf8("		if(!m_pInstance)"));
				// FIXME: If got constructor arguments this does not compile!
				lPublicFunctions.append(__utf8("			m_pInstance = new %1()").arg(_p->pSettings->szFullClassName));
				lPublicFunctions.append(__utf8("		return m_pInstance;"));
				lPublicFunctions.append(__utf8("	}"));
				lPublicFunctions.append(QString());
			break;
			default:
				// nuthin
			break;
		}

		if(_p->pSettings->bGenerateAssignmentOperator)
		{
			if(_p->pSettings->bInlineConstructorDestructor)
			{
				lPublicFunctions.append(__utf8("	%1 & operator = (const %1 &src)").arg(_p->pSettings->szClassName));
				lPublicFunctions.append(__utf8("	{"));
				lPublicFunctions.append(__utf8("		if(&src == this)"));
				lPublicFunctions.append(__utf8("			return *this;"));
				lPublicFunctions.append(__utf8("		// TODO: Implement assignment operator"));
				lPublicFunctions.append(__utf8("		return *this;"));
				lPublicFunctions.append(__utf8("	}"));
				lPublicFunctions.append(QString());
			} else {
				lPublicFunctions.append(__utf8("	%1 & operator = (const %1 &src);").arg(_p->pSettings->szClassName));
			}
		}

		switch(_p->pSettings->eClassType)
		{
			case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
				lPublicFunctions.append(__utf8("	static void init();"));
				lPublicFunctions.append(__utf8("	static void done();"));
			break;
			default:
				// nuthin
			break;
		}
		
		if(_p->pSettings->eClassType == C3NewCPPClassSettings::TemplateClass)
			szBody = __utf8("template<typename %1> class ").arg(_p->pSettings->szTemplateParameter);
		else {
			szBody = "class ";
			if(!_p->pSettings->szAPIDeclarator.isEmpty())
				szBody += __utf8("%1 ").arg(_p->pSettings->szAPIDeclarator);
		}
		
		szBody += _p->pSettings->szClassName;


		if(_p->pSettings->lBaseClassNames.count() > 0)
		{
			QString szCD = _p->pSettings->lBaseClassNames.join(__ascii(", public "));
			szBody += " : public " + szCD;
		}

		szBody += "\n";
		szBody += "{\n";

		if(_p->pSettings->bBaseClassIsQObject)
			szBody += "	Q_OBJECT\n";

		if(!lConstructDestruct.isEmpty())
		{
			switch(_p->pSettings->eClassType)
			{
				case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
				case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
					szBody += "private:\n";
				break;
				default:
					szBody += "public:\n";
				break;
			}
		
			szBody += lConstructDestruct.join("\n");
			if(!lConstructDestruct.last().isEmpty())
				szBody += "\n";
			szBody += "\n";
		}

		if(!lPrivateVariables.isEmpty())
		{
			szBody += "private:\n";
			szBody += lPrivateVariables.join("\n");
			if(!lPrivateVariables.last().isEmpty())
				szBody += "\n";
			szBody += "\n";
		}

		if(!lPublicFunctions.isEmpty())
		{
			szBody += "public:\n";
			szBody += lPublicFunctions.join("\n");
			if(!lPublicFunctions.last().isEmpty())
				szBody += "\n";
			szBody += "\n";
		}

		szBody += __utf8("}; // class %1\n").arg(_p->pSettings->szClassName);
	
	}

	QString s;

	if(!lIncludes.isEmpty())
	{
		s += lIncludes.join("\n");
		if(!lIncludes.last().isEmpty())
			s += "\n";
		s += "\n";
	}

	if(!lPreamble.isEmpty())
	{
		s += lPreamble.join("\n");
		if(!lPreamble.last().isEmpty())
			s += "\n";
		s += "\n";
	}

	s += szBody;

	return s;
}

QString C3NewCPPClassGenerator::generateImplementation()
{
	// Includes first.
	QStringList lIncludes;

	lIncludes.append(__utf8("#include \"%1\"").arg(C3StringUtils::fileNameFromPath(_p->pSettings->szHFilePath)));

	QString szBody;

	if(_p->pSettings->eClassType == C3NewCPPClassSettings::Namespace)
	{
		szBody = __utf8(
					"namespace %1\n"
					"{\n"
					"\n"
					"} // namespace %1\n"
				).arg(_p->pSettings->szClassName);
	} else {
		switch(_p->pSettings->eClassType)
		{
			case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
			case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
			case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
				szBody += __utf8("%1 * %1::m_pInstance = NULL;\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("\n");
			break;
			default:
				// nuthin
			break;
		}

		switch(_p->pSettings->eClassType)
		{
			case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
				szBody += __utf8("class %1DestructorHelper\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("{\n");
				szBody += __utf8("public:");
				szBody += __utf8("	~%1DestructorHelper()\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("	{\n");
				szBody += __utf8("		if(m_pInstance)\n");
				szBody += __utf8("			delete m_pInstance;\n");
				szBody += __utf8("	}\n");
				szBody += __utf8("};\n");
				szBody += __utf8("\n");
				szBody += __utf8("static %1DestructorHelper g_o%1DestructorHelper;\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("\n");
			break;
			default:
				// nuthin
			break;
		}

		if(_p->pSettings->bHideMembersViaPrivateClass)
		{
			szBody += __utf8("class %1Private\n").arg(_p->pSettings->szClassName);
			szBody += __utf8("{\n");
			szBody += __utf8("public:\n");
			szBody += __utf8("\n");
			szBody += __utf8("};\n");
			szBody += __utf8("\n");
			szBody += __utf8("#define _p %1\n").arg(_p->pSettings->szPrivateMemberName);
			szBody += __utf8("\n");
		}

		if(_p->pSettings->bGenerateConstructorDestructor && !_p->pSettings->bInlineConstructorDestructor)
		{
			if(!_p->pSettings->szConstructorArguments.isEmpty())
				szBody += __utf8("%1::%1(%2)\n").arg(_p->pSettings->szClassName).arg(_p->pSettings->szConstructorArguments);
			else
				szBody += __utf8("%1::%1()\n").arg(_p->pSettings->szClassName);


			if(_p->pSettings->lBaseClassNames.count() > 0)
			{
				QString szCD = _p->pSettings->lBaseClassNames.join(__ascii("(), "));
				szBody += __utf8("	: %1()\n").arg(szCD);
			}

			szBody += "{\n";

			switch(_p->pSettings->eClassType)
			{
				case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
					szBody += __utf8("	m_pInstance = this;\n");
				break;
				default:
					// nuthin
				break;
			}

			if(_p->pSettings->bHideMembersViaPrivateClass)
				szBody += __utf8("	_p = new %1Private();\n").arg(_p->pSettings->szClassName);


			szBody += "}\n";
			szBody += "\n";


			if(_p->pSettings->bGenerateCopyConstructor)
			{
				szBody += __utf8("%1::%1(const %1 &src)\n").arg(_p->pSettings->szClassName);

				if(_p->pSettings->lBaseClassNames.count() > 0)
				{
					QString szCD = _p->pSettings->lBaseClassNames.join(__ascii("(src), "));
					szBody += __utf8("	: %1(src)\n").arg(szCD);
				}
	
				szBody += "{\n";
				szBody += "	// TODO: Implement copy constructor\n";
				szBody += "}\n";
				szBody += "\n";
			}
			
			if(_p->pSettings->bGenerateMoveConstructor)
			{
				szBody += __utf8("%1::%1(const %1 &&src)\n").arg(_p->pSettings->szClassName);

				if(_p->pSettings->lBaseClassNames.count() > 0)
				{
					QString szCD = _p->pSettings->lBaseClassNames.join(__ascii("(src), "));
					szBody += __utf8("	: %1(src)\n").arg(szCD);
				}
	
				szBody += "{\n";
				szBody += "	// TODO: Implement move constructor\n";
				szBody += "}\n";
				szBody += "\n";
			}

			szBody += __utf8("%1::~%1()\n").arg(_p->pSettings->szClassName);
			szBody += "{\n";

			if(_p->pSettings->bHideMembersViaPrivateClass)
				szBody += __utf8("	delete _p;\n");

			switch(_p->pSettings->eClassType)
			{
				case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
					szBody += __utf8("	m_pInstance = NULL;\n");
				break;
				default:
					// nuthin
				break;
			}

			szBody += "}\n";
			szBody += "\n";

			if(_p->pSettings->bGenerateAssignmentOperator)
			{
					szBody += __utf8("%1 & %1::operator = (const %1 &src)\n").arg(_p->pSettings->szClassName);
					szBody += __utf8("{\n");
					szBody += __utf8("	if(&src == this)\n");
					szBody += __utf8("		return *this;\n");
					szBody += __utf8("	// TODO: Implement assignment operator\n");
					szBody += __utf8("	return *this;\n");
					szBody += __utf8("}\n");
					szBody += "\n";
			}
		}

		switch(_p->pSettings->eClassType)
		{
			case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
				szBody += __utf8("void %1::init()\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("{\n");
				szBody += __utf8("	if(!m_pInstance)\n");
				// FIXME: if have constructor arguments this does not compile!
				szBody += __utf8("		m_pInstance = new %1();\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("}\n");
				szBody += __utf8("\n");
				szBody += __utf8("void %1::done()\n").arg(_p->pSettings->szClassName);
				szBody += __utf8("{\n");
				szBody += __utf8("	if(!m_pInstance)\n");
				szBody += __utf8("		return;\n");
				szBody += __utf8("	delete m_pInstance;\n");
				szBody += __utf8("	m_pInstance = NULL;\n");
				szBody += __utf8("}\n");
				szBody += __utf8("\n");
			break;
			default:
				// nuthin
			break;
		}

	}

	QString s;

	if(!lIncludes.isEmpty())
	{
		s += lIncludes.join("\n");
		if(!lIncludes.last().isEmpty())
			s += "\n";
		s += "\n";
	}
	
	s += szBody;

	return s;
}

QString C3NewCPPClassGenerator::addTrailingNewlines(const QString &szText)
{
	if(szText.isEmpty())
		return szText;

	QString szFixed = szText;

	// last line
	if(!szFixed.endsWith(QChar('\n')))
		szFixed.append(QChar('\n'));
	// space
	szFixed.append(QChar('\n'));

	return szFixed;
}


QString C3NewCPPClassGenerator::generateH()
{
	QString szHeader = addTrailingNewlines(_p->pSettings->szHFileHeader.trimmed());
	QString szPreamble = addTrailingNewlines(_p->pSettings->szHFilePreamble.trimmed());
	QString szData = addTrailingNewlines(generateDeclaration());
	QString szTrailer = addTrailingNewlines(_p->pSettings->szHFileTrailer.trimmed());

	QString szFileName = C3StringUtils::fileNameFromPath(_p->pSettings->szHFilePath);

	int idx = szFileName.indexOf(QChar('<'));
	if(idx >= 0)
		szFileName = szFileName.left(idx);

	szFileName.replace(QRegExp("[^A-Za-z0-9_]"),"_");

	QString szGuard = __utf8("_%1_").arg(szFileName);
	
	return QString(
			"#ifndef %1\n"
			"#define %1\n"
			"%2"
			"%3"
			"%4"
			"%5"
			"#endif //!%1"
		)
			.arg(szGuard)
			.arg(szHeader)
			.arg(szPreamble)
			.arg(szData)
			.arg(szTrailer);
}

QString C3NewCPPClassGenerator::generateCPP()
{
	QString szHeader = addTrailingNewlines(_p->pSettings->szCPPFileHeader.trimmed());
	QString szPreamble = addTrailingNewlines(_p->pSettings->szCPPFilePreamble.trimmed());
	QString szData = addTrailingNewlines(generateImplementation());
	QString szTrailer = addTrailingNewlines(_p->pSettings->szCPPFileTrailer.trimmed());
	
	return QString(
			"%1"
			"%2"
			"%3"
			"%4"
		)
			.arg(szHeader)
			.arg(szPreamble)
			.arg(szData)
			.arg(szTrailer);
}
