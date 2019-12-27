#ifndef _C3DockTextConsole_h_
#define _C3DockTextConsole_h_
//=============================================================================
//
//   File : C3DockTextConsole.h
//   Creation Date : 2015/11/19 19:51:48
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

#include <QDockWidget>

class C3DockTextConsolePrivate;
class C3TextOutputWidget;
class C3DockTextConsoleOutputWidgetInfo;

class C3DockConsoleChannel
{
public:
	QString szId;
	QString szTitle;
	bool bCanBeTerminated; // this is set only by external commands

public:
	C3DockConsoleChannel()
		: bCanBeTerminated(false)
	{
	}

	C3DockConsoleChannel(const C3DockConsoleChannel &s)
		: szId(s.szId),
		szTitle(s.szTitle),
		bCanBeTerminated(s.bCanBeTerminated)
	{
	}
	
	void operator = (const C3DockConsoleChannel &s)
	{
		szId = s.szId;
		szTitle = s.szTitle;
		bCanBeTerminated = s.bCanBeTerminated;
	}
};

class C3DockTextConsole : public QDockWidget
{
	Q_OBJECT
public:
	C3DockTextConsole(QWidget * pParent);
	virtual ~C3DockTextConsole();

private:
	C3DockTextConsolePrivate * m_pC3DCO;

public:

	void appendHtml(const C3DockConsoleChannel &oChannel,const QString &szHtml,bool bIsError = false);
	void appendText(const C3DockConsoleChannel &oChannel,const QString &szText,bool bIsError = false);
	void appendText(const C3DockConsoleChannel &oChannel,const QString &szText,const QString &szStyle,bool bIsError = false);
	void clear(const C3DockConsoleChannel &oChannel);
	void updateChannel(const C3DockConsoleChannel &oChannel);
	void activateChannel(const C3DockConsoleChannel &oChannel);

private:

	C3DockTextConsoleOutputWidgetInfo * findInfo(const C3DockConsoleChannel &oChannel);
	C3DockTextConsoleOutputWidgetInfo * getInfo(const C3DockConsoleChannel &oChannel);
	C3DockTextConsoleOutputWidgetInfo * findInfo(C3TextOutputWidget * pWidget);
	C3DockTextConsoleOutputWidgetInfo * currentInfo();
	C3TextOutputWidget * findOutputWidget(const C3DockConsoleChannel &oChannel);
	C3TextOutputWidget * getOutputWidget(const C3DockConsoleChannel &oChannel);
	C3TextOutputWidget * currentOutputWidget();
	void enableDisableButtons();
	void updateTitle();
	void refillTextForAllChannels();

protected slots:

	void slotTerminateButtonClicked();
	void slotCloseButtonClicked();
	void slotClearButtonClicked();
	void slotCurrentTabChanged(int index);
	void slotTabCloseRequested(int index);
	void slotViewOptionsMenuAboutToShow();
	void slotGoToFirstErrorButtonClicked();
	
	void slotSetMessageFilterShowAllMessages();
	void slotSetMessageFilterShowOnlyErrors();

}; // class C3DockTextConsole

#endif //!_C3DockTextConsole_h_
