#ifndef _C3CentralWidget_h_
#define _C3CentralWidget_h_
//=============================================================================
//
//   File : C3CentralWidget.h
//   Creation Date : dom 25 ott 2015 02:28:43
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

class C3CentralWidgetPrivate;
class C3MainWindow;
class C3FindWidget;
class QStackedWidget;

///
/// \class C3CentralWidget
/// \brief The C3CentralWidget class
///
/// This class...
///
class C3CentralWidget : public QWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3CentralWidget
	///
	C3CentralWidget(C3MainWindow * pParent);

	///
	/// Destroys the instance of C3CentralWidget
	/// and frees all the relevant resources
	///
	virtual ~C3CentralWidget();

private:

	C3CentralWidgetPrivate * m_pC3CW;

public:

	// This one never changes!
	QStackedWidget * stackedWidget();

	void message(
			const QString &szMessage,
			const QString &szIcon = QString(),
			const QString &szStyleSheet = QString()
		);
		
	void closeMessageContainer();
	
	void showFindWidget();

	void hideFindWidget();
	
	bool findWidgetVisible();

	C3FindWidget * findWidget();

protected:

	virtual void resizeEvent(QResizeEvent * e);

private:

	void layout();

}; // class C3CentralWidget

#endif //!_C3CentralWidget_h_
