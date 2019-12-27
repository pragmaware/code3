#ifndef _C3TextEditorMargin_h_
#define _C3TextEditorMargin_h_
//=============================================================================
//
//   File : C3TextEditorMargin.h
//   Creation Date : mer 28 ott 2015 01:21:08
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

#include <QWidget>

#include "C3TextEditor.h"

class C3TextEditorMarginPrivate;
class C3TextEditorPrivate;


///
/// \class C3TextEditorMargin
/// \brief The C3TextEditorMargin class
///
/// This class...
///
class C3TextEditorMargin : public QWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorMargin
	///
	C3TextEditorMargin(
			C3TextEditor * pParent,
			C3TextEditorPrivate * pPrivate
		);

	///
	/// Destroys the instance of C3TextEditorMargin
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorMargin();


private:

	C3TextEditorPrivate * m_pC3TE;

public:

	int widthHint();

private:

	virtual void paintEvent(QPaintEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);


}; // class C3TextEditorMargin

#endif //!_C3TextEditorMargin_h_
