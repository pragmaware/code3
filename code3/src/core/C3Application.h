#ifndef _C3Application_h_
#define _C3Application_h_
//=============================================================================
//
//   File : C3Application.h
//   Creation Date : mar 20 ott 2015 00:01:32
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

#include <QApplication>
#include <QList>

#include "C3EditorFactory.h"
#include "C3AlertSource.h"

class C3ApplicationPrivate;


///
/// \class C3Application
/// \brief The C3Application class
///
/// This class...
///
class C3Application : public QApplication, public C3AlertSource
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3Application
	///
	C3Application(int & argc,char ** argv);

	///
	/// Destroys the instance of C3Application
	/// and frees all the relevant resources
	///
	virtual ~C3Application();

private:

	static C3Application * m_pInstance;

	C3ApplicationPrivate * m_pC3A;

public:

	bool init(int argc,char ** argv);
	
	static C3Application * instance()
	{
		return m_pInstance;
	}

	QList<C3EditorFactory *> & editorFactories() const;

private:

	void loadStyleSheet();

}; // class C3Application

#endif //!_C3Application_h_
