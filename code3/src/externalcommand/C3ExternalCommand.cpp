//=============================================================================
//
//   File : C3ExternalCommand.cpp
//   Creation Date : 2015/11/19 22:42:22
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

#include "C3ExternalCommand.h"
#include "QDir"
#include "C3Application.h"
#include "C3JsonUtils.h"

C3ExternalCommand::C3ExternalCommand()
	: m_eType(ControlledShellScript),
	m_bSaveAllFiles(true),
	m_bClearOutputOnStart(true),
	m_bNeedsPassword(false),
	m_bHighlightPaths(false)
{
}

C3ExternalCommand::C3ExternalCommand(const C3ExternalCommand &src)
	: m_szId(src.m_szId),
	m_eType(src.m_eType),
	m_szName(src.m_szName),
	m_szIcon(src.m_szIcon),
	m_szWorkingDirectory(src.m_szWorkingDirectory),
	m_szCommand(src.m_szCommand),
	m_szShortcut(src.m_szShortcut),
	m_bSaveAllFiles(src.m_bSaveAllFiles),
	m_lArguments(src.m_lArguments),
	m_bClearOutputOnStart(src.m_bClearOutputOnStart),
	m_bNeedsPassword(src.m_bNeedsPassword),
	m_bHighlightPaths(src.m_bHighlightPaths)
{
	
}

C3ExternalCommand::~C3ExternalCommand()
{
}

void C3ExternalCommand::copyFrom(C3ExternalCommand &src)
{
	m_szId = src.m_szId;
	m_eType = src.m_eType;
	m_szName = src.m_szName;
	m_szIcon = src.m_szIcon;
	m_szShortcut = src.m_szShortcut;
	m_szWorkingDirectory = src.m_szWorkingDirectory;
	m_szCommand = src.m_szCommand;
	m_lArguments = src.m_lArguments;
	m_bSaveAllFiles = src.m_bSaveAllFiles;
	m_bClearOutputOnStart = src.m_bClearOutputOnStart;
	m_bNeedsPassword = src.m_bNeedsPassword;
	m_bHighlightPaths = src.m_bHighlightPaths;
}

static QString g_szDetachedProcess("detachedProcess");
static QString g_szDetachedShellScript("detachedShellScript");
static QString g_szControlledProcess("controlledProcess");
static QString g_szControlledProcessWithArguments("controlledProcessWithArguments");
static QString g_szControlledShellScript("controlledShellScript");

QJsonObject C3ExternalCommand::toJsonObject() const
{
	QJsonObject ob;
	
	ob.insert("id",m_szId);
	switch(m_eType)
	{
		case DetachedProcess:
			ob.insert("type",g_szDetachedProcess);
		break;
		case DetachedShellScript:
			ob.insert("type",g_szDetachedShellScript);
		break;
		case ControlledProcess:
			ob.insert("type",g_szControlledProcess);
		break;
		case ControlledProcessWithArguments:
			ob.insert("type",g_szControlledProcessWithArguments);
		break;
		default:
		//case ControlledShellScript:
			ob.insert("type",g_szControlledShellScript);
		break;
	}
	ob.insert("name",m_szName);
	if(!m_szIcon.isEmpty())
	{
		QString szBasePath = C3Application::applicationDirPath();
		if(!szBasePath.endsWith(QDir::separator()))
			szBasePath.append(QDir::separator());
		szBasePath += "images";

		if(m_szIcon.startsWith(szBasePath))
			ob.insert("icon",m_szIcon.mid(szBasePath.length()));
		else {
			szBasePath = C3_RESOURCES_DIR;
			if(!szBasePath.endsWith(QDir::separator()))
				szBasePath.append(QDir::separator());
			szBasePath += "images";
			if(m_szIcon.startsWith(szBasePath))
				ob.insert("icon",m_szIcon.mid(szBasePath.length()));
			else
				ob.insert("icon",m_szIcon);
		}
	}
	if(!m_szShortcut.isEmpty())
		ob.insert("shortctut",m_szShortcut);
	if(!m_szWorkingDirectory.isEmpty())
		ob.insert("workingDirectory",m_szWorkingDirectory);
	ob.insert("command",m_szCommand);
	if(m_bSaveAllFiles)
		ob.insert("saveAllFiles",m_bSaveAllFiles);
	if(m_bNeedsPassword)
		ob.insert("needsPassword",m_bNeedsPassword);
	if(m_bClearOutputOnStart)
		ob.insert("clearOutputOnStart",m_bClearOutputOnStart);
	if(!m_lArguments.isEmpty())
		ob.insert("arguments",C3JsonUtils::stringListToJsonArray(m_lArguments));
	if(m_bHighlightPaths)
		ob.insert("highlightPaths",m_bHighlightPaths);
	return ob;
}

bool C3ExternalCommand::decodeJsonObject(const QJsonObject &ob)
{
	m_szId = ob.value("id").toString();
	if(m_szId.isEmpty())
		return false;

	m_szName = ob.value("name").toString();
	if(m_szName.isEmpty())
		return false;

	QString szType = ob.value("type").toString();
	if(szType == g_szDetachedProcess)
		m_eType = DetachedProcess;
	else if(szType == g_szDetachedShellScript)
		m_eType = DetachedShellScript;
	else if(szType == g_szControlledProcess)
		m_eType = ControlledProcess;
	else if(szType == g_szControlledProcessWithArguments)
		m_eType = ControlledProcessWithArguments;
	else if(szType == g_szControlledShellScript)
		m_eType = ControlledShellScript;
	else
		return false;

	m_lArguments = C3JsonUtils::jsonArrayToStringList(ob.value("arguments").toArray());

	m_szIcon = ob.value("icon").toString();
	m_szWorkingDirectory = ob.value("workingDirectory").toString();

	m_szShortcut = ob.value("shortctut").toString();

	m_szCommand = ob.value("command").toString();
	if(m_szCommand.isEmpty())
		return false;

	m_bSaveAllFiles = ob.value("saveAllFiles").toBool();
	m_bClearOutputOnStart = ob.value("clearOutputOnStart").toBool();
	m_bNeedsPassword = ob.value("needsPassword").toBool();
	m_bHighlightPaths = ob.value("highlightPaths").toBool();

	return true;
}
