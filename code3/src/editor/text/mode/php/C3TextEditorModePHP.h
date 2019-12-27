#ifndef _C3TextEditorModePHP_h_
#define _C3TextEditorModePHP_h_
//=============================================================================
//
//   File : C3TextEditorModePHP.h
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

#include "C3TextEditorModeWithSymbolAnalysis.h"

class C3TextEditorModeParserXML;

///
/// \class C3TextEditorModePHP
/// \brief The C3TextEditorModePHP class
///
/// This class...
///
class C3TextEditorModePHP : public C3TextEditorModeWithSymbolAnalysis
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModePHP
	///
	C3TextEditorModePHP(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModePHP
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModePHP();

private:

	// PHP is actually HTML with PHP substack enabled
	C3TextEditorModeParserXML * m_pParser;

private:

	virtual void computeMetadata();

}; // class C3TextEditorModePHP

#endif //!_C3TextEditorModePHP_h_
