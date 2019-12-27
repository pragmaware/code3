//=============================================================================
//
//   File : C3DockTextConsole.cpp
//   Creation Date : 2015/11/19 19:51:48
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

#include "C3DockTextConsole.h"

#include "C3TextOutputWidget.h"
#include "C3DockWidgetTitleBar.h"
#include "C3MainWindow.h"
#include "C3ExternalCommandManager.h"

#include <QTabWidget>
#include <QToolButton>
#include <QHash>
#include <QMessageBox>
#include <QMenu>
#include <QWidgetAction>
#include <QLabel>

enum MessageFilter
{
	ShowAllMessages = 1,
	ShowOnlyErrors = 2
};

class C3DockTextConsoleOutputWidgetInfo
{
public:
	C3DockConsoleChannel oChannel;
	C3TextOutputWidget * pWidget;
	QString szAllText; // all the messages
	QString szErrorText; // only errors
};

class C3DockTextConsolePrivate
{
public:
	QTabWidget * pTabWidget;
	QHash<QString,C3DockTextConsoleOutputWidgetInfo *> lOutputWidgets;
	C3DockWidgetTitleBar * pTitleBar;
	QToolButton * pClearButton;
	QToolButton * pGoToFirstErrorButton;
	QToolButton * pTerminateButton;
	MessageFilter eMessageFilter;
};

#define _p m_pC3DCO

C3DockTextConsole::C3DockTextConsole(QWidget * pParent)
	: QDockWidget(pParent)
{
	_p = new C3DockTextConsolePrivate();

	_p->eMessageFilter = ShowAllMessages;

	_p->pTitleBar = new C3DockWidgetTitleBar(this,__utf8(" %1").arg(__tr("Output")));

	_p->pTerminateButton = _p->pTitleBar->addButton(__utf8("dock/12px/kill.png"),__tr("Terminate Running Process"));
	QObject::connect(_p->pTerminateButton,SIGNAL(clicked()),this,SLOT(slotTerminateButtonClicked()));

	_p->pTitleBar->addSeparator();

	_p->pClearButton = _p->pTitleBar->addButton(__utf8("dock/12px/clear.png"),__tr("Clear Current Console"));
	QObject::connect(_p->pClearButton,SIGNAL(clicked()),this,SLOT(slotClearButtonClicked()));

	QMenu * pMenu = _p->pTitleBar->addMenuButton(__utf8("dock/12px/gear.png"),__tr("View Options"));
	QObject::connect(pMenu,SIGNAL(aboutToShow()),this,SLOT(slotViewOptionsMenuAboutToShow()));

	_p->pGoToFirstErrorButton = _p->pTitleBar->addButton(__utf8("dock/12px/go-to-first-error.png"),__tr("Go to First Error"));
	QObject::connect(_p->pGoToFirstErrorButton,SIGNAL(clicked()),this,SLOT(slotGoToFirstErrorButtonClicked()));

	_p->pTitleBar->addSeparator();

	QToolButton * pButton = _p->pTitleBar->addButton(__utf8("dock/12px/x.png"),__tr("Close Console"));
	QObject::connect(pButton,SIGNAL(clicked()),this,SLOT(slotCloseButtonClicked()));

	setTitleBarWidget(_p->pTitleBar);

	setWindowTitle(__tr("Command Output"));
	setObjectName(__utf8("C3DockTextConsole"));
	//setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetMovable);


	_p->pTabWidget = new QTabWidget(this);
	_p->pTabWidget->setTabShape(QTabWidget::Triangular);
	_p->pTabWidget->setTabPosition(QTabWidget::South);
	_p->pTabWidget->setTabBarAutoHide(true);
	_p->pTabWidget->setMovable(true);
	_p->pTabWidget->setTabsClosable(true);
	_p->pTabWidget->setDocumentMode(true);
	
	QObject::connect(_p->pTabWidget,SIGNAL(currentChanged(int)),this,SLOT(slotCurrentTabChanged(int)));
	QObject::connect(_p->pTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(slotTabCloseRequested(int)));

	setWidget(_p->pTabWidget);
}

C3DockTextConsole::~C3DockTextConsole()
{
	qDeleteAll(_p->lOutputWidgets);
	delete _p;
}

void C3DockTextConsole::slotViewOptionsMenuAboutToShow()
{
	QObject * ob = sender();
	if(!ob)
		return;
	QMenu * pMenu = dynamic_cast<QMenu *>(ob);
	if(!pMenu)
		return;

	pMenu->clear();

	QWidgetAction * act = new QWidgetAction(pMenu);
	
	QLabel * l = new QLabel(pMenu);
	l->setText(__tr("Message Filter"));

	// FIXME: Does not seem to work?
	l->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	l->setLineWidth(1);
	l->setMidLineWidth(1);

	// FIXME: Does not seem to work?
	l->setAutoFillBackground(true);
	l->setBackgroundRole(QPalette::Window);

	// FIXME: Does not seem to work?
	QPalette pal = l->palette();
	pal.setColor(QPalette::Window,QColor(70,70,70));
	l->setPalette(pal);

	l->setAlignment(Qt::AlignCenter);
	
	QFont fnt = l->font();
	fnt.setBold(true);
	l->setFont(fnt);
	
	//l->setStyleSheet("QLabel { background-color: #404040; color: #ffffff; font-weight: bold; }"); <-- does not work, problems with style?
	
	act->setDefaultWidget(l);
	
	pMenu->addAction(act);
	
	QAction * a = pMenu->addAction(__tr("Show All Messages"));
	a->setCheckable(true);
	a->setChecked(_p->eMessageFilter == ShowAllMessages);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotSetMessageFilterShowAllMessages()));
	
	a = pMenu->addAction(__tr("Show Only Errors"));
	a->setCheckable(true);
	a->setChecked(_p->eMessageFilter == ShowOnlyErrors);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotSetMessageFilterShowOnlyErrors()));
}

void C3DockTextConsole::slotSetMessageFilterShowAllMessages()
{
	if(_p->eMessageFilter == ShowAllMessages)
		return;
	_p->eMessageFilter = ShowAllMessages;
	refillTextForAllChannels();
}


void C3DockTextConsole::slotSetMessageFilterShowOnlyErrors()
{
	if(_p->eMessageFilter == ShowOnlyErrors)
		return;
	_p->eMessageFilter = ShowOnlyErrors;
	refillTextForAllChannels();
}

void C3DockTextConsole::refillTextForAllChannels()
{
	foreach(C3DockTextConsoleOutputWidgetInfo * pInfo,_p->lOutputWidgets)
	{
		pInfo->pWidget->clear();
		pInfo->pWidget->appendHtml((_p->eMessageFilter == ShowAllMessages) ? pInfo->szAllText : pInfo->szErrorText);
	}
}

void C3DockTextConsole::updateChannel(const C3DockConsoleChannel &oChannel)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = _p->lOutputWidgets.value(oChannel.szId);
	if(!pInfo)
		return;
	pInfo->oChannel = oChannel;
	
	int idx = _p->pTabWidget->indexOf(pInfo->pWidget);
	if(idx < 0)
		return;

	_p->pTabWidget->setTabText(idx,pInfo->oChannel.szTitle);
	enableDisableButtons();
}

void C3DockTextConsole::activateChannel(const C3DockConsoleChannel &oChannel)
{
	C3TextOutputWidget * pWidget = getOutputWidget(oChannel);

	updateChannel(oChannel);

	_p->pTabWidget->setCurrentWidget(pWidget);

	updateTitle();
	enableDisableButtons();
}


C3DockTextConsoleOutputWidgetInfo * C3DockTextConsole::findInfo(const C3DockConsoleChannel &oChannel)
{
	return _p->lOutputWidgets.value(oChannel.szId);
}

C3DockTextConsoleOutputWidgetInfo * C3DockTextConsole::findInfo(C3TextOutputWidget * pWidget)
{
	QHash<QString,C3DockTextConsoleOutputWidgetInfo *>::Iterator it;
	for(it = _p->lOutputWidgets.begin();it != _p->lOutputWidgets.end();++it)
	{
		C3DockTextConsoleOutputWidgetInfo * pInfo = *it;
		if(pInfo->pWidget == pWidget)
			return pInfo;
	}
	return NULL;
}

C3DockTextConsoleOutputWidgetInfo * C3DockTextConsole::getInfo(const C3DockConsoleChannel &oChannel)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = _p->lOutputWidgets.value(oChannel.szId);
	if(pInfo)
		return pInfo;

	pInfo = new C3DockTextConsoleOutputWidgetInfo();

	pInfo->pWidget = new C3TextOutputWidget(_p->pTabWidget);
	pInfo->oChannel = oChannel;

	_p->pTabWidget->addTab(pInfo->pWidget,oChannel.szTitle);
	_p->pTabWidget->setCurrentWidget(pInfo->pWidget);
	
	_p->lOutputWidgets.insert(oChannel.szId,pInfo);
	updateTitle();
	enableDisableButtons();

	return pInfo;
}

C3TextOutputWidget * C3DockTextConsole::findOutputWidget(const C3DockConsoleChannel &oChannel)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = _p->lOutputWidgets.value(oChannel.szId);
	if(!pInfo)
		return NULL;
	return pInfo->pWidget;
}

C3TextOutputWidget * C3DockTextConsole::getOutputWidget(const C3DockConsoleChannel &oChannel)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = getInfo(oChannel);
	if(pInfo)
		return pInfo->pWidget;
	return NULL;
}

C3TextOutputWidget * C3DockTextConsole::currentOutputWidget()
{
	QWidget * w = _p->pTabWidget->currentWidget();
	if(!w)
		return NULL;
	return dynamic_cast<C3TextOutputWidget *>(w);
}

C3DockTextConsoleOutputWidgetInfo * C3DockTextConsole::currentInfo()
{
	return findInfo(currentOutputWidget());
}

void C3DockTextConsole::appendHtml(const C3DockConsoleChannel &oChannel,const QString &szText,bool bIsError)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = getInfo(oChannel);
	if(!pInfo)
		return; // bug

	pInfo->szAllText += szText;

	if(bIsError)
		pInfo->szErrorText += szText;

	if((_p->eMessageFilter == ShowAllMessages) || bIsError)
		pInfo->pWidget->appendHtml(szText);
}

void C3DockTextConsole::appendText(const C3DockConsoleChannel &oChannel,const QString &szText,bool bIsError)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = getInfo(oChannel);
	if(!pInfo)
		return; // bug

	QString szFormatted = __utf8("<span style=\"color: #c0c0c0; white-space: pre;\">%1</span>").arg(szText.toHtmlEscaped());

	pInfo->szAllText += szFormatted;

	if(bIsError)
		pInfo->szErrorText += szFormatted;

	if((_p->eMessageFilter == ShowAllMessages) || bIsError)
		pInfo->pWidget->appendHtml(szFormatted);
}

void C3DockTextConsole::appendText(const C3DockConsoleChannel &oChannel,const QString &szText,const QString &szStyle,bool bIsError)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = getInfo(oChannel);
	if(!pInfo)
		return; // bug

	QString szFormatted = __utf8("<span style=\"%1; white-space: pre;\">%2</span>").arg(szStyle).arg(szText.toHtmlEscaped());

	pInfo->szAllText += szFormatted;

	if(bIsError)
		pInfo->szErrorText += szFormatted;

	if((_p->eMessageFilter == ShowAllMessages) || bIsError)
		pInfo->pWidget->appendHtml(szFormatted);
}

void C3DockTextConsole::clear(const C3DockConsoleChannel &oChannel)
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = getInfo(oChannel);
	if(!pInfo)
		return; // bug

	pInfo->szAllText = QString();
	pInfo->szErrorText = QString();

	pInfo->pWidget->clear();
}

void C3DockTextConsole::slotTerminateButtonClicked()
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = currentInfo();
	if(!pInfo)
		return;
	
	if(!pInfo->oChannel.bCanBeTerminated)
		return;
	
	if(C3ExternalCommandManager::instance()->terminateCommandById(pInfo->oChannel.szId,false))
		pInfo->oChannel.bCanBeTerminated = false;

	enableDisableButtons();
	updateTitle();
}

void C3DockTextConsole::slotCloseButtonClicked()
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = currentInfo();
	if(pInfo && pInfo->oChannel.bCanBeTerminated)
	{
		switch(
			QMessageBox::question(
					this,
					__tr("Terminate Process"),
					__tr("A process is currently attached to the console you're attempting to close. Do you want to terminate it?"),
					QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
				)
			)
		{
			case QMessageBox::Yes:
				if(C3ExternalCommandManager::instance()->terminateCommandById(pInfo->oChannel.szId,true))
					pInfo->oChannel.bCanBeTerminated = false;
			break;
			case QMessageBox::No:
				// nuthin
			break;
			//case QMessageBox::Cancel:
			default:
				// doh?
				return;
			break;
		}
	}

	C3MainWindow::instance()->closeTextConsole();
}

void C3DockTextConsole::slotClearButtonClicked()
{
	C3TextOutputWidget * w = currentOutputWidget();
	if(!w)
		return;
	w->clear();
}

void C3DockTextConsole::slotGoToFirstErrorButtonClicked()
{
	C3TextOutputWidget * w = currentOutputWidget();
	if(!w)
		return;
	
	if(w->goToFirstOccurenceOf("error:"))
		return;
	if(w->goToFirstOccurenceOf("warning:"))
		return;
	if(w->goToFirstOccurenceOf("error"))
		return;
}


void C3DockTextConsole::slotCurrentTabChanged(int index)
{
	updateTitle();
	enableDisableButtons();
}

void C3DockTextConsole::slotTabCloseRequested(int index)
{
	QWidget * w = _p->pTabWidget->widget(index);
	if(!w)
		return;
	C3TextOutputWidget * to = dynamic_cast<C3TextOutputWidget *>(w);
	if(!to)
		return;

	C3DockTextConsoleOutputWidgetInfo * pInfo = findInfo(to);
	if(!pInfo)
		return;

	QString szKey = _p->lOutputWidgets.key(pInfo);
	if(szKey.isEmpty())
		return;
	
	_p->lOutputWidgets.remove(szKey);
	delete pInfo->pWidget;
	delete pInfo;

	updateTitle();
	enableDisableButtons();
	
	if(_p->lOutputWidgets.isEmpty())
		C3MainWindow::instance()->closeTextConsole();
}

void C3DockTextConsole::updateTitle()
{
	switch(_p->lOutputWidgets.count())
	{
		case 1:
			_p->pTitleBar->setTitle(__tr("Output - %1").arg(_p->lOutputWidgets.begin().value()->oChannel.szTitle));
		break;
		default:
			_p->pTitleBar->setTitle(__tr("Output"));
		break;
	}
}

void C3DockTextConsole::enableDisableButtons()
{
	C3DockTextConsoleOutputWidgetInfo * pInfo = C3DockTextConsole::currentInfo();
	if(!pInfo)
	{
		_p->pTerminateButton->setEnabled(false);
		return;
	}

	_p->pTerminateButton->setEnabled(pInfo->oChannel.bCanBeTerminated);
}
