#ifndef _C3TextEditorFormatWriter_h_
#define _C3TextEditorFormatWriter_h_
//=============================================================================
//
//   File : C3TextEditorFormatWriter.h
//   Creation Date : gio 22 ott 2015 22:51:50
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

#include "C3TextEditorLine.h"

#include <QList>

///
/// \class C3TextEditorFormatWriter
/// \brief The C3TextEditorFormatWriter class
///
/// This class...
///
class C3TextEditorFormatWriter
{
public:

	///
	/// Creates an instance of C3TextEditorFormatWriter
	///
	C3TextEditorFormatWriter();

	///
	/// Destroys the instance of C3TextEditorFormatWriter
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFormatWriter();

public:

	virtual bool write(const QString &szPath,QList<C3TextEditorLine *> &lLines,QString &szError) = 0;

}; // class C3TextEditorFormatWriter

#endif //!_C3TextEditorFormatWriter_h_
