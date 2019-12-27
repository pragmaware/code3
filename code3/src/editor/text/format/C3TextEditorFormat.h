#ifndef _C3TextEditorFormat_h_
#define _C3TextEditorFormat_h_
//=============================================================================
//
//   File : C3TextEditorFormat.h
//   Creation Date : gio 22 ott 2015 22:44:21
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

#include "C3EditorFormat.h"

#include <QString>

class C3TextEditorFormatReader;
class C3TextEditorFormatWriter;

///
/// \class C3TextEditorFormat
/// \brief The C3TextEditorFormat class
///
/// This class...
///
class C3TextEditorFormat : public C3EditorFormat
{
public:

	///
	/// Creates an instance of C3TextEditorFormat
	///
	C3TextEditorFormat(const QString &szId,const QString &szName);

	///
	/// Destroys the instance of C3TextEditorFormat
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFormat();

public:

	virtual C3TextEditorFormatReader * createReader() = 0;
	virtual C3TextEditorFormatWriter * createWriter() = 0;

}; // class C3TextEditorFormat

#endif //!_C3TextEditorFormat_h_
