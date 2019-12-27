#ifndef _C3TextEditorOptions_h_
#define _C3TextEditorOptions_h_
//=============================================================================
//
//   File : C3TextEditorOptions.h
//   Creation Date : sab 24 ott 2015 04:54:22
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

#include "C3TextEditorOptionsColors.h"

#include <QFont>
#include <QColor>

///
/// \class C3TextEditorOptions
/// \brief The C3TextEditorOptions class
///
/// This class...
///
class C3TextEditorOptions : public C3TextEditorOptionsColors
{
public:

	///
	/// Creates an instance of C3TextEditorOptions
	///
	C3TextEditorOptions();

	///
	/// Destroys the instance of C3TextEditorOptions
	/// and frees all the relevant resources
	///
	~C3TextEditorOptions();

public:


	// the general text font
	QFont oTextFont;

	// the number of spaces per tab we use
	int iTabStopSize;
	
	// the additional line spacing we use
	int iAdditionalLineSpacing;

public:

	void load();
	void save();

}; // class C3TextEditorOptions

#endif //!_C3TextEditorOptions_h_
