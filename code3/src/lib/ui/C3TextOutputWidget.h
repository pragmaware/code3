#ifndef _C3TextOutputWidget_h_
#define _C3TextOutputWidget_h_
//=============================================================================
//
//   File : C3TextOutputWidget.h
//   Creation Date : 2015/11/19 21:17:41
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

#include <QTextEdit>

class C3TextOutputWidgetPrivate;

class C3TextOutputWidget : public QTextEdit
{
	Q_OBJECT
public:
	C3TextOutputWidget(QWidget * pParent);
	virtual ~C3TextOutputWidget();

public:

	void appendText(const QString &szText);
	void appendText(const QString &szText,const QString &szStyle);
	void appendHtml(const QString &szText);

	bool goToFirstOccurenceOf(const QString &szText);

private:
	C3TextOutputWidgetPrivate * m_pC3TOW;

protected:

	virtual void contextMenuEvent(QContextMenuEvent * e);
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	bool tryToOpenFile(const QString &szPath);
	bool tryToOpenFileInText(const QString &szText);

private slots:
	void slotCopy();
	void slotSelectAll();
	void slotOpenAllFilesFound();
	void slotMoveTextToNewFile();

}; // class C3TextOutputWidget

#endif //!_C3TextOutputWidget_h_