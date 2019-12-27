#ifndef _C3Settings_h_
#define _C3Settings_h_
//=============================================================================
//
//   File : C3Settings.h
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


#include "c3_config.h"

#include "C3SettingsUserHighlightingSet.h"

#include <QStringList>

///
/// \class C3Settings
/// \brief The C3Settings class
///
/// This class...
///
class C3Settings
{
public:

	///
	/// Creates an instance of C3Settings
	///
	C3Settings();

	///
	/// Destroys the instance of C3Settings
	/// and frees all the relevant resources
	///
	virtual ~C3Settings();

protected:

	///
	/// The one and only instance pointer for the class C3Settings
	///
	static C3Settings * m_pInstance;

private:

	///
	/// the list of recent workspaces
	///
	QStringList m_lRecentWorkspaces;

	///
	/// the list of recent files
	///
	QStringList m_lRecentFiles;

	///
	/// the user's first name
	///
	QString m_szUserFirstName;

	///
	/// the user's last name
	///
	QString m_szUserLastName;

	///
	/// the user's email
	///
	QString m_szUserEMail;

	///
	/// the default file header
	///
	QString m_szFileHeader;
	
	///
	/// the default user highlighting set
	///
	C3SettingsUserHighlightingSet m_oUserHighlightingSet;
	
	///
	/// the exclusion patterns
	///
	QStringList m_lFileExclusionPatterns;

public:

	///
	/// Returns the one and only instance pointer for the class C3Settings
	/// The returned pointer is valid only after a succesfull call to init()
	///
	/// \sa init()
	/// \sa done()
	///
	static C3Settings * instance()
	{
		return m_pInstance;
	};

	///
	/// Returns the list of recent workspaces
	///
	const QStringList & recentWorkspaces() const
	{
		return m_lRecentWorkspaces;
	}

	void addRecentWorkspace(const QString &szPath);

	///
	/// Returns the list of recent files
	///
	const QStringList & recentFiles() const
	{
		return m_lRecentFiles;
	}

	void addRecentFile(const QString &szPath);

	///
	/// Sets the user's first name
	///
	void setUserFirstName(const QString &szUserFirstName)
	{
		m_szUserFirstName = szUserFirstName;
	}

	///
	/// Returns the user's first name
	///
	const QString & userFirstName() const
	{
		return m_szUserFirstName;
	}

	///
	/// Sets the user's last name
	///
	void setUserLastName(const QString &szUserLastName)
	{
		m_szUserLastName = szUserLastName;
	}

	QString * userFirstNameOption()
	{
		return &m_szUserFirstName;
	}

	///
	/// Returns the user's last name
	///
	const QString & userLastName() const
	{
		return m_szUserLastName;
	}

	QString * userLastNameOption()
	{
		return &m_szUserLastName;
	}

	///
	/// Sets the user's email
	///
	void setUserEMail(const QString &szUserEMail)
	{
		m_szUserEMail = szUserEMail;
	}

	///
	/// Returns the user's email
	///
	const QString & userEMail() const
	{
		return m_szUserEMail;
	}

	QString * userEMailOption()
	{
		return &m_szUserEMail;
	}

	QString userFullName() const
	{
		QString szRet = m_szUserFirstName;
		if(!m_szUserLastName.isEmpty())
		{
			if(!szRet.isEmpty())
				szRet.append(QChar(' '));
			szRet.append(m_szUserLastName);
		}
		return szRet;
	}

	///
	/// Sets the default file header
	///
	void setFileHeader(const QString &szFileHeader)
	{
		m_szFileHeader = szFileHeader;
	}

	///
	/// Returns the default file header
	///
	const QString & fileHeader() const
	{
		return m_szFileHeader;
	}

	QString * fileHeaderOption()
	{
		return &m_szFileHeader;
	}

	C3SettingsUserHighlightingSet * userHighlightingSet()
	{
		return &m_oUserHighlightingSet;
	}
	
	const QStringList & fileExclusionPatterns() const
	{
		return m_lFileExclusionPatterns;
	}
	
	void setFileExclusionPatterns(const QStringList &lPatterns)
	{
		m_lFileExclusionPatterns = lPatterns;
	}
	
	QStringList * fileExclusionPatternsOption()
	{
		return &m_lFileExclusionPatterns;
	}

public:

	void loadRecent();
	void saveRecent();

	void load();
	void save();
	
	static QStringList defaultFileExclusionPatterns();

}; // class C3Settings

#endif //!_C3Settings_h_