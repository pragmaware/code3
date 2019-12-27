#ifndef _C3TextEditorModeJava_h_
#define _C3TextEditorModeJava_h_
//=============================================================================
//
//   File : C3TextEditorModeJava.h
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

class C3TextEditorModeParserJava;
class C3TextEditorRowColumn;

#include "C3TextEditorLine.h"
#include "C3TextEditorRange.h"

///
/// \class C3TextEditorModeJava
/// \brief The C3TextEditorModeJava class
///
/// This class...
///
class C3TextEditorModeJava : public C3TextEditorModeWithSymbolAnalysis
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModeJava
	///
	C3TextEditorModeJava(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModeJava
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeJava();

private:

	C3TextEditorModeParserJava * m_pParser;

protected:

	virtual void computeMetadata();

	virtual void maybeCompletion(const C3TextEditorRowColumn &rc);

}; // class C3TextEditorModeJava

#endif //!_C3TextEditorModeJava_h_