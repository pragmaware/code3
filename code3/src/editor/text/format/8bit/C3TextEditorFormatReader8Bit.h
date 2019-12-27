#ifndef _C3TextEditorFormatReader8Bit_h_
#define _C3TextEditorFormatReader8Bit_h_
//=============================================================================
//
//   File : C3TextEditorFormatReader8Bit.h
//   Creation Date : gio 22 ott 2015 23:13:44
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

#include "C3TextEditorFormatReader.h"

class C3TextEditorFormatReader8BitPrivate;

class QTextCodec;

///
/// \class C3TextEditorFormatReader8Bit
/// \brief The C3TextEditorFormatReader8Bit class
///
/// This class...
///
class C3TextEditorFormatReader8Bit : public C3TextEditorFormatReader
{
public:

	///
	/// Creates an instance of C3TextEditorFormatReader8Bit
	///
	C3TextEditorFormatReader8Bit(QTextCodec * pCodec,bool bByteOrderMark);

	///
	/// Destroys the instance of C3TextEditorFormatReader8Bit
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFormatReader8Bit();

private:

	C3TextEditorFormatReader8BitPrivate * m_pC3TEFRB;

private:

	virtual void runInternal();

	void processData(bool bForceEmitLastLine,bool bIsLastCall);

}; // class C3TextEditorFormatReader8Bit

#endif //!_C3TextEditorFormatReader8Bit_h_
