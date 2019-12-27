#ifndef _C3TextEditorFormat8Bit_h_
#define _C3TextEditorFormat8Bit_h_
//=============================================================================
//
//   File : C3TextEditorFormat8Bit.h
//   Creation Date : gio 22 ott 2015 23:06:22
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

#include "C3TextEditorFormat.h"

#include <QString>

class C3TextEditorFormat8BitPrivate;

///
/// \class C3TextEditorFormat8Bit
/// \brief The C3TextEditorFormat8Bit class
///
/// This class...
///
class C3TextEditorFormat8Bit : public C3TextEditorFormat
{
public:

	///
	/// Creates an instance of C3TextEditorFormat8Bit
	///
	C3TextEditorFormat8Bit(const QString &szId,const QString &szName,const QString &szTextCodecName,bool bByteOrderMark);

	///
	/// Destroys the instance of C3TextEditorFormat8Bit
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFormat8Bit();

private:

	C3TextEditorFormat8BitPrivate * m_pTEF8P;

public:

	virtual C3TextEditorFormatReader * createReader();
	virtual C3TextEditorFormatWriter * createWriter();

}; // class C3TextEditorFormat8Bit

#endif //!_C3TextEditorFormat8Bit_h_
