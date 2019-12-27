//=============================================================================
//
//   File : C3ExternalToolManager.cpp
//   Creation Date : dom 08 nov 2015 06:44:56
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

#include "C3ExternalToolManager.h"

#include <QHash>
#include <QFileInfo>
#include <QStandardPaths>

C3ExternalToolManager * C3ExternalToolManager::m_pInstance = NULL;

class C3ExternalToolManagerPrivate
{
public:
	QHash<QString,QString> hTools;
};

#define _p m_pC3ETM

C3ExternalToolManager::C3ExternalToolManager()
{
	m_pInstance = this;

	_p = new C3ExternalToolManagerPrivate();

}

C3ExternalToolManager::~C3ExternalToolManager()
{
	delete _p;
	
	m_pInstance = NULL;
}

void C3ExternalToolManager::searchForTool(const QString &szToolName)
{
	QString szPath = QStandardPaths::findExecutable(szToolName);
	if(!szPath.isEmpty())
	{
		_p->hTools.insert(szToolName,szPath);
		return;
	}

	QFileInfo inf(__utf8("/usr/bin/%1").arg(szToolName));
	if(inf.exists() && inf.isExecutable())
	{
		_p->hTools.insert(szToolName,inf.absoluteFilePath());
		return;
	}
	
	inf.setFile(__utf8("/usr/local/bin/%1").arg(szToolName));
	if(inf.exists() && inf.isExecutable())
	{
		_p->hTools.insert(szToolName,inf.absoluteFilePath());
		return;
	}

	inf.setFile(__utf8("/bin/%1").arg(szToolName));
	if(inf.exists() && inf.isExecutable())
	{
		_p->hTools.insert(szToolName,inf.absoluteFilePath());
		return;
	}

	inf.setFile(__utf8("/sbin/%1").arg(szToolName));
	if(inf.exists() && inf.isExecutable())
	{
		_p->hTools.insert(szToolName,inf.absoluteFilePath());
		return;
	}

	// not there, mark it
	_p->hTools.insert(szToolName,__utf8("#"));
}

QString C3ExternalToolManager::findTool(const QString &szToolName)
{
	QString szVal = _p->hTools.value(szToolName);
	if(szVal.isEmpty())
	{
		searchForTool(szToolName);
		szVal = _p->hTools.value(szToolName);
	}

	if(szVal.length() == 1)
		return QString(); // already tried to find it, not there

	return szVal;
}
