#ifndef _C3TextEditorModePlainText_h_
#define _C3TextEditorModePlainText_h_
//=============================================================================
//
//   File : C3TextEditorModePlainText.h
//   Creation Date : sab 24 ott 2015 23:41:10
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

#include "C3TextEditorMode.h"

class C3TextEditorModePlainTextPrivate;


///
/// \class C3TextEditorModePlainText
/// \brief The C3TextEditorModePlainText class
///
/// This class...
///
class C3TextEditorModePlainText : public C3TextEditorMode
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModePlainText
	///
	C3TextEditorModePlainText(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModePlainText
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModePlainText();

private:

	C3TextEditorModePlainTextPrivate * m_pC3TEMPT;

public:

	virtual void attach(
			C3TextEditorCoreData * pData
		);

	virtual void ensureMetadataUpToDate(
			int iFirstLine,
			int iLastLine
		);

	virtual bool textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		);

	virtual bool textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		);

	virtual bool textModified(
			int iFirstLine,
			int iLastLine
		);

private:

	void computeBlocks(C3TextEditorLine * pLine);

}; // class C3TextEditorModePlainText

#endif //!_C3TextEditorModePlainText_h_