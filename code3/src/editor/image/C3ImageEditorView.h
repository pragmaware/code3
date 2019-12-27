#ifndef _C3ImageEditorView_h_
#define _C3ImageEditorView_h_
//=============================================================================
//
//   File : C3ImageEditorView.h
//   Creation Date : mar 20 ott 2015 04:50:04
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

#include "C3ImageEditor.h"
#include "C3Link.h"

class C3ImageEditorViewPrivate;

///
/// \class C3ImageEditorView
/// \brief The C3ImageEditorView class
///
/// This class...
///
class C3ImageEditorView : public QWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3ImageEditorView
	///
	C3ImageEditorView(C3ImageEditor * pParent);

	///
	/// Destroys the instance of C3ImageEditorView
	/// and frees all the relevant resources
	///
	~C3ImageEditorView();

private:

	C3ImageEditorViewPrivate * m_pC3IEV;

public:

	bool open(const C3Link &oLink);

	void layout();

protected slots:
	void slotVerticalScrollBarValueChanged(int iVal);
	void slotHorizontalScrollBarValueChanged(int iVal);

private:

	virtual void paintEvent(QPaintEvent * e);
	virtual void resizeEvent(QResizeEvent * e);
	virtual void showEvent(QShowEvent * e);

}; // class C3ImageEditorView

#endif //!_C3ImageEditorView_h_