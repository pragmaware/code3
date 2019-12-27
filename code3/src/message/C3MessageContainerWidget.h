#ifndef _C3MessageContainerWidget_h_
#define _C3MessageContainerWidget_h_
//=============================================================================
//
//   File : C3MessageContainerWidget.h
//   Creation Date : dom 25 ott 2015 02:33:28
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

class C3MessageContainerWidgetPrivate;
class C3CentralWidget;

///
/// \class C3MessageContainerWidget
/// \brief The C3MessageContainerWidget class
///
/// This class...
///
class C3MessageContainerWidget : public QWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3MessageContainerWidget
	///
	C3MessageContainerWidget(C3CentralWidget * pParent);

	///
	/// Destroys the instance of C3MessageContainerWidget
	/// and frees all the relevant resources
	///
	virtual ~C3MessageContainerWidget();

private:

	C3MessageContainerWidgetPrivate * m_pC3ACW;

public:

	void message(
			const QString &szMessage,
			const QString &szIcon = QString(),
			const QString &szStyleSheet = QString()
		);

	int heightForWidth(int iWidth);

protected:


	virtual void resizeEvent(QResizeEvent * e);

protected slots:

	void slotCloseClicked();
	void slotScrollBarValueChanged(int iVal);

private:

	int computeBaseHeight(int iWidth);
	void doLayout();

}; // class C3MessageContainerWidget

#endif //!_C3MessageContainerWidget_h_
