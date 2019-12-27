#ifndef _C3DockWidgetTitleBar_h_
#define _C3DockWidgetTitleBar_h_
//=============================================================================
//
//   File : C3DockWidgetTitleBar.h
//   Creation Date : 2015/11/15 00:56:59
//   Project : 
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

#include <QFrame>

class C3DockWidgetTitleBarPrivate;
class QMenu;
class QToolButton;

class C3DockWidgetTitleBar : public QFrame
{
	Q_OBJECT
public:
	C3DockWidgetTitleBar(QWidget * pParent,const QString &szTitle);
	virtual ~C3DockWidgetTitleBar();

private:
	C3DockWidgetTitleBarPrivate * m_pC3DWTB;

public:

	void setTitle(const QString &szTitle);
	QToolButton * addButton(const QString &szIcon,const QString &szToolTip);
	QMenu * addMenuButton(const QString &szIcon,const QString &szToolTip);
	void addSeparator();

}; // class C3DockWidgetTitleBar

#endif //!_C3DockWidgetTitleBar_h_
