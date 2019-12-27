#ifndef _C3NewEmptyTextFileCreator_h_
#define _C3NewEmptyTextFileCreator_h_
//=============================================================================
//
//   File : C3NewEmptyTextFileCreator.h
//   Creation Date : sab 07 nov 2015 06:00:52
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

#include "C3NewObjectCreator.h"

///
/// \class C3NewEmptyTextFileCreator
/// \brief The C3NewEmptyTextFileCreator class
///
/// This class...
///
class C3NewEmptyTextFileCreator : public C3NewObjectCreator
{
public:

	///
	/// Creates an instance of C3NewEmptyTextFileCreator
	///
	C3NewEmptyTextFileCreator(
			const QString &szName,
			const QString &szIcon,
			const QString &szMode
		);

	///
	/// Destroys the instance of C3NewEmptyTextFileCreator
	/// and frees all the relevant resources
	///
	~C3NewEmptyTextFileCreator();

private:
	QString m_szName;
	QString m_szIcon;
	QString m_szMode;

public:

	virtual QString name();
	virtual QString icon();

	virtual void run();

}; // class C3NewEmptyTextFileCreator

#endif //!_C3NewEmptyTextFileCreator_h_
