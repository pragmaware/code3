#ifndef _C3ExternalToolManager_h_
#define _C3ExternalToolManager_h_
//=============================================================================
//
//   File : C3ExternalToolManager.h
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


#include "c3_config.h"

#include <QString>

class C3ExternalToolManagerPrivate;

///
/// \class C3ExternalToolManager
/// \brief The C3ExternalToolManager class
///
/// This class...
///
class C3ExternalToolManager
{
public:

	///
	/// Creates an instance of C3ExternalToolManager
	///
	C3ExternalToolManager();

	///
	/// Destroys the instance of C3ExternalToolManager
	/// and frees all the relevant resources
	///
	~C3ExternalToolManager();

private:

	C3ExternalToolManagerPrivate * m_pC3ETM;

	static C3ExternalToolManager * m_pInstance;

public:

	static C3ExternalToolManager * instance()
	{
		return m_pInstance;
	}

	QString findTool(const QString &szToolName);

private:

	void searchForTool(const QString &szToolName);

}; // class C3ExternalToolManager

#endif //!_C3ExternalToolManager_h_
