#ifndef _C3TextEditorModeRust_h_
#define _C3TextEditorModeRust_h_
//=============================================================================
//
//   File : C3TextEditorModeRust.h
//   Creation Date : 18/05/2026 21:49
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

class C3TextEditorModeParserRust;
class C3TextEditorRowColumn;

#include "C3TextEditorLine.h"
#include "C3TextEditorRange.h"

///
/// \class C3TextEditorModeRust
/// \brief The C3TextEditorModeRust class
///
/// This class...
///
class C3TextEditorModeRust : public C3TextEditorModeWithSymbolAnalysis
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModeRust
	///
	C3TextEditorModeRust(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModeRust
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeRust();

private:

	C3TextEditorModeParserRust * m_pParser;

protected:

	virtual void computeMetadata();

//	virtual void maybeCompletion(const C3TextEditorRowColumn &rc);

}; // class C3TextEditorModeRust

#endif //!_C3TextEditorModeRust_h_