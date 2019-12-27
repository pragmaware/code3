#ifndef _C3MessageWidget_h_
#define _C3MessageWidget_h_
//=============================================================================
//
//   File : C3MessageWidget.h
//   Creation Date : dom 25 ott 2015 22:15:50
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

class C3MessageWidgetPrivate;

///
/// \class C3MessageWidget
/// \brief The C3MessageWidget class
///
/// This class...
///
class C3MessageWidget : public QWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3MessageWidget
	///
	C3MessageWidget(QWidget * pParent);

	///
	/// Destroys the instance of C3MessageWidget
	/// and frees all the relevant resources
	///
	virtual ~C3MessageWidget();

private:

	C3MessageWidgetPrivate * m_pC3AW;

public:

	void showMessage(
			const QString &szMessage,
			const QString &szIcon = QString(),
			const QString &szStyleSheet = QString()
		);

	int heightForWidth(int iWidth);

protected:

	virtual void paintEvent(QPaintEvent *);


}; // class C3MessageWidget

#endif //!_C3MessageWidget_h_
