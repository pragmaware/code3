//=============================================================================
//
//   File : C3Settings.cpp
//   Creation Date : ven 06 nov 2015 06:29:17
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

#include "C3Settings.h"

#include <QSettings>
#include <QJsonObject>
#include <QJsonDocument>

// The one and only C3Settingsobject
C3Settings * C3Settings::m_pInstance = NULL;

C3Settings::C3Settings()
{
	m_pInstance = this;
}

C3Settings::~C3Settings()
{
	m_pInstance = NULL;
}

void C3Settings::addRecentWorkspace(const QString &szPath)
{
	if(!m_lRecentWorkspaces.contains(szPath))
		m_lRecentWorkspaces.append(szPath);

	while(m_lRecentWorkspaces.count() > 10)
		m_lRecentWorkspaces.takeFirst();
}

void C3Settings::addRecentFile(const QString &szPath)
{
	if(!m_lRecentFiles.contains(szPath))
		m_lRecentFiles.append(szPath);

	while(m_lRecentFiles.count() > 10)
		m_lRecentFiles.takeFirst();
}

void C3Settings::loadRecent()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.recent");

	m_lRecentWorkspaces = s.value("recentWorkspaces").toStringList();
	m_lRecentFiles = s.value("recentFiles").toStringList();
}

void C3Settings::saveRecent()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.recent");
	
	s.setValue("recentWorkspaces",m_lRecentWorkspaces);
	s.setValue("recentFiles",m_lRecentFiles);
}

QStringList C3Settings::defaultFileExclusionPatterns()
{
	QStringList lRet;
	
	lRet.append("*.class");
	lRet.append("*.exe");
	lRet.append("*.bin");
	lRet.append("*.dll");
	lRet.append("*.so");
	lRet.append("*.obj");
	lRet.append("Build/");
	lRet.append("build/");
	lRet.append("bin/");
	lRet.append("obj/");
	lRet.append("build/");
	lRet.append("debug/");
	lRet.append("Release/");
	lRet.append("release/");
	lRet.append(".git/");
	lRet.append(".svn/");
	lRet.append("CMakeFiles/");
	
	return lRet;
}


void C3Settings::load()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.settings");

	m_szUserFirstName = s.value("userFirstName").toString();
	m_szUserLastName = s.value("userLastName").toString();
	m_szUserEMail = s.value("userEMail").toString();
	m_szFileHeader = s.value("fileHeader").toString();
	
	if(m_szFileHeader.isEmpty())
	{
		m_szFileHeader = "" \
			"File : %%filename%%\n" \
			"Creation date : %%date%%\n" \
			"Project : %%project%%\n" \
			"Author : %%author%% <%%email%%>\n";
	}
	
	QVariant v = s.value("userHighlightingSet");
	if(v.isNull())
	{
		// set defaults once
		m_oUserHighlightingSet.setDefaults();
	} else {
		QByteArray arry = v.toByteArray();
		QJsonParseError err;
		QJsonDocument oDoc = QJsonDocument::fromJson(arry,&err);
		if((!oDoc.isNull()) && (oDoc.isObject()))
		{
			QJsonObject oObject = oDoc.object();
			m_oUserHighlightingSet.loadFromJsonObject(oObject);
		} else {
			m_oUserHighlightingSet.clear();
		}
	}

	v = s.value("fileExclusionPatterns");
	if(v.isNull())
	{
		m_lFileExclusionPatterns = defaultFileExclusionPatterns();
	} else {
		m_lFileExclusionPatterns = v.toStringList();
	}
}

void C3Settings::save()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.settings");
	
	s.setValue("userFirstName",m_szUserFirstName);
	s.setValue("userLastName",m_szUserLastName);
	s.setValue("userEMail",m_szUserEMail);
	s.setValue("fileHeader",m_szFileHeader);
	
	QJsonObject ob;
	m_oUserHighlightingSet.saveToJsonObject(ob);
	
	QJsonDocument oDoc;
	oDoc.setObject(ob);

	QByteArray arry = oDoc.toJson(QJsonDocument::Compact);

	s.setValue("userHighlightingSet",arry);

	s.setValue("fileExclusionPatterns",m_lFileExclusionPatterns);
}


