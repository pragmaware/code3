#ifndef _C3TextEditorModePython_h_
#define _C3TextEditorModePython_h_
//=============================================================================
//
//   File : C3TextEditorModePython.h
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

class C3TextEditorModeParserPython;

///
/// \class C3TextEditorModePython
/// \brief The C3TextEditorModePython class
///
/// This class...
///
class C3TextEditorModePython : public C3TextEditorModeWithSymbolAnalysis
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModePython
	///
	C3TextEditorModePython(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModePython
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModePython();

private:

	C3TextEditorModeParserPython * m_pParser;

private:

	virtual void computeMetadata();
	
	virtual void maybeCompletion(const C3TextEditorRowColumn &rc);


}; // class C3TextEditorModePython

#endif //!_C3TextEditorModePython_h_