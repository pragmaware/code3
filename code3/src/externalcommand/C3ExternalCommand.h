#ifndef _C3ExternalCommand_h_
#define _C3ExternalCommand_h_
//=============================================================================
//
//   File : C3ExternalCommand.h
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

#include "c3_config.h"

#include <QString>
#include <QStringList>
#include <QJsonObject>

class C3ExternalCommand
{
	friend class C3ExternalCommandEditorWidget;
public:
	C3ExternalCommand();
	C3ExternalCommand(const C3ExternalCommand &src);
	~C3ExternalCommand();

public:

	enum Type
	{
		// m_szCommands contains command and arguments.
		// The process is launched and detached immediately.
		// No output is captured.
		DetachedProcess,
		// m_szCommands contains command and arguments.
		// The shell is launched and detached immediately.
		// No output is captured.
		DetachedShellScript,
		// m_szCommands contains command and arguments.
		// The process is launched and output is captured. Can be killed.
		ControlledProcess,
		// m_szCommands contains command and arguments.
		// The shell is launched and output is captured. Can be killed.
		ControlledShellScript,
		// m_szCommands contains command, m_lArguments contain the arguments.
		// The process is launched and output is captured. Can be killed.
		ControlledProcessWithArguments
	};

protected:

	QString m_szId;
	Type m_eType;
	QString m_szName;
	QString m_szIcon;
	QString m_szWorkingDirectory;
	QString m_szCommand;
	QString m_szShortcut;
	bool m_bSaveAllFiles;
	// meaningful only for Controlled* commands as the other ones don't have any output
	bool m_bClearOutputOnStart;
	bool m_bNeedsPassword; // ask for password at beginning (and cache it), replace with %%password%%
	// These are used only in ControlledProcessWithArguments
	QStringList m_lArguments;
	bool m_bHighlightPaths;

public:

	bool needsPassword() const
	{
		return m_bNeedsPassword;
	}
	
	void setNeedsPassword(bool bNeedsPassword)
	{
		m_bNeedsPassword = bNeedsPassword;
	}

	bool clearOutputOnStart() const
	{
		return m_bClearOutputOnStart;
	}
	
	void setClearOutputOnStart(bool b)
	{
		m_bClearOutputOnStart = b;
	}

	const QStringList & arguments() const
	{
		return m_lArguments;
	}
	
	void setArguments(const QStringList &lArgs)
	{
		m_lArguments = lArgs;
	}

	const QString & shortcut() const
	{
		return m_szShortcut;
	}
	
	void setShortcut(const QString &szShortcut)
	{
		m_szShortcut = szShortcut;
	}

	Type type() const
	{
		return m_eType;
	}
	
	void setType(Type eType)
	{
		m_eType = eType;
	}

	const QString & id() const
	{
		return m_szId;
	}
	
	void setId(const QString &szId)
	{
		m_szId = szId;
	}

	const QString & icon() const
	{
		return m_szIcon;
	}
	
	void setIcon(const QString &szIcon)
	{
		m_szIcon = szIcon;
	}

	const QString & name() const
	{
		return m_szName;
	}
	
	void setName(const QString &szName)
	{
		m_szName = szName;
	}

	const QString & workingDirectory() const
	{
		return m_szWorkingDirectory;
	}
	
	void setWorkingDirectory(const QString &szDir)
	{
		m_szWorkingDirectory = szDir;
	}
	
	const QString & command() const
	{
		return m_szCommand;
	}
	
	void setCommand(const QString &szPath)
	{
		m_szCommand = szPath;
	}
	
	bool saveAllFiles() const
	{
		return m_bSaveAllFiles;
	}
	
	void setSaveAllFiles(bool bSaveAllFiles)
	{
		m_bSaveAllFiles = bSaveAllFiles;
	}
	
	bool highlightPaths() const
	{
		return m_bHighlightPaths;
	}
	
	void setHighlightPaths(bool bDoIt)
	{
		m_bHighlightPaths = bDoIt;
	}
	
	QJsonObject toJsonObject() const;
	bool decodeJsonObject(const QJsonObject &ob);

	void copyFrom(C3ExternalCommand &src);

}; // class C3ExternalCommand

#endif //!_C3ExternalCommand_h_
