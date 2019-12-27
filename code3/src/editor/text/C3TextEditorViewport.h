#ifndef _C3TextEditorViewport_h_
#define _C3TextEditorViewport_h_
//=============================================================================
//
//   File : C3TextEditorViewport.h
//   Creation Date : sab 24 ott 2015 04:02:14
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

///
/// \class C3TextEditorViewport
/// \brief The C3TextEditorViewport class
///
/// This class...
///
class C3TextEditorViewport : public QWidget
{
public:

	///
	/// Creates an instance of C3TextEditorViewport
	///
	C3TextEditorViewport(
			C3TextEditor * pParent,
			C3TextEditorPrivate * pPrivate
		);

	///
	/// Destroys the instance of C3TextEditorViewport
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorViewport();

private:

	virtual void paintEvent(QPaintEvent * e);
	virtual void keyPressEvent(QKeyEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * e);
	virtual void mouseMoveEvent(QMouseEvent * e);
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual void wheelEvent(QWheelEvent * e);

	// We need this to catch Tab and Shift+Tab
	bool event(QEvent * e);

private:
	void leftMousePressEvent(QMouseEvent * e);
	void rightMousePressEvent(QMouseEvent * e);
	void middleMousePressEvent(QMouseEvent * e);

private:

	C3TextEditorPrivate * m_pC3TE;

}; // class C3TextEditorViewport

#endif //!_C3TextEditorViewport_h_
