#ifndef _C3TextEditorFormatWriter8Bit_h_
#define _C3TextEditorFormatWriter8Bit_h_
//=============================================================================
//
//   File : C3TextEditorFormatWriter8Bit.h
//   Creation Date : gio 22 ott 2015 23:14:06
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

#include "C3TextEditorFormatWriter.h"

class C3TextEditorFormatWriter8BitPrivate;

class QTextCodec;

///
/// \class C3TextEditorFormatWriter8Bit
/// \brief The C3TextEditorFormatWriter8Bit class
///
/// This class...
///
class C3TextEditorFormatWriter8Bit : public C3TextEditorFormatWriter
{
public:

	///
	/// Creates an instance of C3TextEditorFormatWriter8Bit
	///
	C3TextEditorFormatWriter8Bit(QTextCodec * pCodec,bool bByteOrderMark);

	///
	/// Destroys the instance of C3TextEditorFormatWriter8Bit
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFormatWriter8Bit();

private:

	C3TextEditorFormatWriter8BitPrivate * m_pC3TEFWB;

private:

	virtual bool write(const QString &szPath,QList<C3TextEditorLine *> &lLines,QString &szError);

}; // class C3TextEditorFormatWriter8Bit

#endif //!_C3TextEditorFormatWriter8Bit_h_
