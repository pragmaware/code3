#ifndef _C3AlertSource_h_
#define _C3AlertSource_h_
//=============================================================================
//
//   File : C3AlertSource.h
//   Creation date: mar 20 ott 2015 01:30:54
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
/// \class C3AlertSource
/// \brief The C3AlertSource class
///
/// This class...
///
class C3AlertSource
{
public:

	///
	/// Creates an instance of C3AlertSource
	///
	C3AlertSource(const QString &szName);

	///
	/// Destroys the instance of C3AlertSource
	/// and frees all the relevant resources
	///
	~C3AlertSource();

private:

	///
	/// the source name
	///
	QString m_szName;

public:

	///
	/// Returns the source name
	///
	const QString & name() const
	{
		return m_szName;
	}

	enum AlertLevel
	{
		Paranoia,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	QString alertLevelDescription(AlertLevel eLevel);

	void alert(
			AlertLevel eLevel,
			const QString &szMessage
		);

	void paranoia(const QString &szMessage)
	{
		alert(Paranoia,szMessage);
	}

	void debug(const QString &szMessage)
	{
		alert(Debug,szMessage);
	}

	void info(const QString &szMessage)
	{
		alert(Info,szMessage);
	}

	void error(const QString &szMessage)
	{
		alert(Error,szMessage);
	}
	
	void warning(const QString &szMessage)
	{
		alert(Warning,szMessage);
	}

	void fatal(const QString &szMessage)
	{
		alert(Fatal,szMessage);
	}

}; // class C3AlertSource

#endif //!_C3AlertSource_h_
