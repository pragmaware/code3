#ifndef _C3OptionEditor_h_
#define _C3OptionEditor_h_
//=============================================================================
//
//   File : C3OptionEditor.h
//   Creation Date : mer 11 nov 2015 01:51:16
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

#include <QObject>

class C3OptionsWidget;

///
/// \class C3OptionEditor
/// \brief The C3OptionEditor class
///
/// This class...
///
class C3OptionEditor : public QObject
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditor
	///
	C3OptionEditor(C3OptionsWidget * pOptionsWidget);

	///
	/// Destroys the instance of C3OptionEditor
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditor();

private:

	C3OptionsWidget * m_pOptionsWidget;

public:

	C3OptionsWidget * optionsWidget()
	{
		return m_pOptionsWidget;
	}


public:

	virtual bool commit() = 0;

}; // class C3OptionEditor

#endif //!_C3OptionEditor_h_
