#ifndef _C3TextEditorModeHTML_h_
#define _C3TextEditorModeHTML_h_
//=============================================================================
//
//   File : C3TextEditorModeHTML.h
//   Creation Date : gio 29 ott 2015 02:50:45
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

#include "C3TextEditorModeWithInterLineState.h"

class C3TextEditorModeParserXML;

///
/// \class C3TextEditorModeHTML
/// \brief The C3TextEditorModeHTML class
///
/// This class...
///
class C3TextEditorModeHTML : public C3TextEditorModeWithInterLineState
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModeHTML
	///
	C3TextEditorModeHTML(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModeHTML
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeHTML();

private:

	C3TextEditorModeParserXML * m_pParser;

private:

	virtual void computeMetadata();

}; // class C3TextEditorModeHTML

#endif //!_C3TextEditorModeHTML_h_