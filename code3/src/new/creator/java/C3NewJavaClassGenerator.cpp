//=============================================================================
//
//   File : C3NewJavaClassGenerator.cpp
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

#include "C3NewJavaClassGenerator.h"

#include "C3NewJavaClassSettings.h"
#include "C3NewObjectUtils.h"
#include "C3StringUtils.h"

#include <QStringList>
#include <QRegExp>

class C3NewJavaClassGeneratorPrivate
{
public:
	C3NewJavaClassSettings * pSettings;
};

#define _p m_pC3NJavaCG

C3NewJavaClassGenerator::C3NewJavaClassGenerator(C3NewJavaClassSettings * pSettings)
{
	_p = new C3NewJavaClassGeneratorPrivate();
	_p->pSettings = pSettings;

	fixSettings();
}

C3NewJavaClassGenerator::~C3NewJavaClassGenerator()
{
	delete _p;
}

void C3NewJavaClassGenerator::fixSettings()
{
	if(!_p->pSettings->szClassName.isEmpty())
	{
		int idx = _p->pSettings->szClassName.indexOf(QChar('<'));
		if(idx >= 0)
		{
			_p->pSettings->szGenericParameter = _p->pSettings->szClassName.mid(idx+1);
			while(_p->pSettings->szGenericParameter.endsWith(QChar('>')))
				_p->pSettings->szGenericParameter.truncate(_p->pSettings->szGenericParameter.length() - 1);
			_p->pSettings->szClassName.truncate(idx);
		} else {
			_p->pSettings->szGenericParameter = "T";
		}
	}

	_p->pSettings->szFullClassName = _p->pSettings->szClassName;

	switch(_p->pSettings->eClassType)
	{
		case C3NewJavaClassSettings::StandardClass:
			if(_p->pSettings->szGenericParameter.isEmpty())
				_p->pSettings->szFullClassName = QString();
			else
				_p->pSettings->szFullClassName = __utf8("%1<%2>").arg(_p->pSettings->szClassName).arg(_p->pSettings->szGenericParameter);
		break;
		case C3NewJavaClassSettings::Interface:
			_p->pSettings->szBaseClassName = QString();
		break;
		case C3NewJavaClassSettings::SingletonWithInitDoneFunctions:
		break;
		case C3NewJavaClassSettings::SingletonWithUserControlledInstantiation:
		break;
		case C3NewJavaClassSettings::SingletonWithLazyInstantiation:
		break;
		//case C3NewJavaClassSettings::StandardClass:
		default:

		break;
	}
}

QString C3NewJavaClassGenerator::generateDeclaration()
{
	QString s = __utf8("package %1;\n\n").arg(_p->pSettings->szPackage);
	
	switch(_p->pSettings->eClassType)
	{
		case C3NewJavaClassSettings::Interface:
			s += __utf8("public interface %1").arg(_p->pSettings->szFullClassName);
		break;
		default:
			s += __utf8("public class %1").arg(_p->pSettings->szFullClassName);
		break;
	}
	
	if(!_p->pSettings->szBaseClassName.isEmpty())
		s += __utf8(" extends %1").arg(_p->pSettings->szBaseClassName);
	
	s += __utf8(
			"\n" \
			"{\n" \
			"\n" \
			"}\n\n"
		);

	return s;
}

QString C3NewJavaClassGenerator::addTrailingNewlines(const QString &szText)
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

QString C3NewJavaClassGenerator::generateJava()
{
	QString szHeader = addTrailingNewlines(_p->pSettings->szJavaFileHeader.trimmed());
	QString szPreamble = addTrailingNewlines(_p->pSettings->szJavaFilePreamble.trimmed());
	QString szData = addTrailingNewlines(generateDeclaration());
	QString szTrailer = addTrailingNewlines(_p->pSettings->szJavaFileTrailer.trimmed());
	
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
