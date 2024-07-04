//=============================================================================
//
//   File : C3TextEditor.cpp
//   Creation Date : gio 22 ott 2015 22:22:57
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

#include "C3TextEditor.h"
#include "C3TextEditor_private.h"

#include "C3TextEditorLine.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorFormat.h"
#include "C3TextEditorFormatReader.h"
#include "C3TextEditorFormatWriter.h"
#include "C3TextEditorMargin.h"
#include "C3TextEditorModePlainText.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorViewport.h"
#include "C3TextEditorUndo.h"
#include "C3TextEditorCompletionWidget.h"

#include "C3PixmapCache.h"
#include "C3GeneralActions.h"
#include "C3Workspace.h"

#include <QResizeEvent>
#include <QFileInfo>
#include <QScrollBar>
#include <QMutex>
#include <QThreadPool>
#include <QFontMetricsF>
#include <QTimer>
#include <QCursor>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QPaintEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QPainter>
#include <QMenu>
#include <QJsonObject>
#include <QFocusEvent>
#include <QFile>
#include <QTextCodec>

#include <qdrawutil.h>

#define CURSOR_BLINK_TIME 650

C3TextEditor::C3TextEditor(C3EditorFactory * pFactory,QWidget * pParent)
	: C3Editor(pFactory,pParent,__tr("Text Editor"))
{
	_p = new C3TextEditorPrivate();

	_p->pEditor = this;
	
	_p->pFactory = dynamic_cast<C3TextEditorFactory *>(pFactory);
	Q_ASSERT(_p->pFactory);
	
	_p->pOptions = _p->pFactory->options();
	
	resetInheritedSettings(); // copy stuff from options
	
	_p->oCursor.set(0,0);
	_p->fMaximumLineWidth = 0.0;
	_p->bIgnoreScrollBarChanges = false;
	_p->iVisualStateChangeStackCount = 0;
	_p->iCurrentFindOperationId = -1;
	_p->pMultiUndo = NULL;
	_p->iLeftMargin = 4;
	_p->bCursorMovedWhileHidden = false;

	_p->pCompletionWidget = NULL;

	_p->pViewport = new C3TextEditorViewport(this,_p);
	_p->pMargin = new C3TextEditorMargin(this,_p);

	_p->pVScrollBar = new QScrollBar(Qt::Vertical,this);
	_p->pHScrollBar = new QScrollBar(Qt::Horizontal,this);

	_p->iScrollBarSize = _p->pVScrollBar->sizeHint().width();

	QObject::connect(_p->pVScrollBar,SIGNAL(valueChanged(int)),this,SLOT(slotVerticalScrollBarValueChanged(int)));
	QObject::connect(_p->pHScrollBar,SIGNAL(valueChanged(int)),this,SLOT(slotHorizontalScrollBarValueChanged(int)));

	_p->pFormat = NULL;
	_p->pReader = NULL;

	_p->pFontMetrics = NULL;
	
	_p->bCursorOn = true;
	_p->fCursorUpDownX = -1.0;
	
	_p->pCursorTimer = new QTimer();
	QObject::connect(_p->pCursorTimer,SIGNAL(timeout()),this,SLOT(slotCursorTimerTimeout()));

	// FIXME: Mayge pre-guess it?
	_p->pMode = new C3TextEditorModePlainText(_p->pFactory);

	// Setup reader inter-thread communication
	QObject::connect(this,SIGNAL(signalReaderWakeUp()),this,SLOT(slotReaderWakeUp()));

	setFocusProxy(_p->pViewport);

	computeFontMetrics();
	layout();
}

C3TextEditor::~C3TextEditor()
{
	delete _p->pCursorTimer;

	killMouseScrollTimer();
	ensureNoRunningRead();

	{
		C3TextEditorLinesLocker oLocker(_p);
		qDeleteAll(_p->lLines);
		_p->lLines.clear();
	}

	qDeleteAll(_p->lUndoStack);
	_p->lUndoStack.clear();
	
	if(_p->pMultiUndo)
		delete _p->pMultiUndo;

	if(_p->pFontMetrics)
		delete _p->pFontMetrics;

	// child of viewport
	//if(_p->pCompletionWidget)
	//	delete _p->pCompletionWidget;

	delete _p->pMode;

	delete _p;
}

void C3TextEditor::workspacePropertiesChanged()
{
	// This call comes directly from C3Workspace *after* C3MainWindow::workspacePropertiesChanged() has been emitted.
	// This means that C3TextEditorModeCPPSharedData has already updated itself.
	
	// Reattach mode so colors will be updated.
	_p->pMode->attach(_p);
}

const QString & C3TextEditor::privateInfoText1() const
{
	if(_p->szCursorPositionText.isEmpty())
	{
		int iCol = _p->oCursor.col;

		if(_p->lLines.count() > 0)
		{
			C3TextEditorLine * pLine = _p->lLines.at(_p->oCursor.row);

			if(pLine && (iCol > pLine->szText.length()))
				iCol = pLine->szText.length();
		}
	
		const_cast<C3TextEditor *>(this)->_p->szCursorPositionText =
				__utf8("[row: %1 col: %2]")
						.arg(_p->oCursor.row + 1)
						.arg(iCol);
	}

	return _p->szCursorPositionText;
}

const QString & C3TextEditor::privateInfoText2() const
{
	if(_p->szModeNameText.isEmpty())
	{
		const_cast<C3TextEditor *>(this)->_p->szModeNameText =
				__utf8("[%1]")
						.arg(_p->pMode->name());
	}

	return _p->szModeNameText;
}


C3EditorFormat * C3TextEditor::format()
{
	return _p->pFormat;
}

QString C3TextEditor::guessBestModeByNameOnly()
{
	QString szPath = path();
	if(szPath.isEmpty())
		return QString();
	
	QFileInfo inf(szPath);
	
	QString szFileName = inf.fileName().toLower();
	
	static QString szCMakeLists("cmakelists.txt");
	
	// handle special cases first
	if(szFileName == szCMakeLists)
		return __utf8("cmake");
	
	QString szExt = inf.suffix().toLower();

	return _p->pFactory->guessBestModeByExtension(szExt);
}

QString C3TextEditor::guessBestMode()
{
	QString szModeId = guessBestModeByNameOnly();
	if(!szModeId.isEmpty())
		return szModeId;
	return __utf8("plain");
}

QString C3TextEditor::guessBestMode(const QByteArray &data)
{
	QString szModeId = guessBestModeByNameOnly();
	if(!szModeId.isEmpty())
		return szModeId;

	if(data.startsWith("#"))
	{
		if(data.startsWith("#!/bin/bash"))
			return __utf8("bash");
		if(data.startsWith("#!/bin/sh"))
			return __utf8("bash");
		if(data.startsWith("#if"))
			return __utf8("cpp");
		if(data.startsWith("#@")) // likely a .config file with bash style syntax
			return __utf8("bash");
		if(data.startsWith("# ")) // likely something with a comment
			return __utf8("bash");
	}
	
	if(data.startsWith("<"))
	{
		if(data.startsWith("<?xml"))
			return __utf8("xml");
		if(data.startsWith("<?php"))
			return __utf8("php");
		if(data.startsWith("<html"))
			return __utf8("html");
		if(data.startsWith("<!--"))
			return __utf8("html");
	}

	if(data.startsWith("{"))
	{
		// json?
		return __utf8("js");
	}
	
	if(data.startsWith("/"))
	{
		// emacs mode at the beginning of the file? (STL has this)
		if(data.contains("c++") || data.contains("C++"))
			return __utf8("cpp");
	}

	return __utf8("plain");

}


void C3TextEditor::setBestGuessMode()
{
	setMode(guessBestMode());
}

QString C3TextEditor::getNextMode()
{
	QString szId = _p->pMode->id();

	QList< QPair< QString,QString > > lIdAndName;
	_p->pFactory->getModeList(lIdAndName);
	
	bool bGotIt = false;
	
	typedef QPair<QString,QString> QPSS;
	
	foreach(QPSS p,lIdAndName)
	{
		if(bGotIt)
			return p.first;
		if(p.first == szId)
			bGotIt = true;
	}
	
	return lIdAndName.first().first;
}

void C3TextEditor::cycleMode()
{
	setMode(getNextMode());
}

void C3TextEditor::setMode(const QString &szModeId)
{
	if(_p->pMode->id() == szModeId)
		return;

	
	C3TextEditorMode * pMode = _p->pFactory->createMode(szModeId);
	Q_ASSERT(pMode);
	
	if(!pMode)
		return; // bug

	if(_p->pMode)
		delete _p->pMode;
	_p->pMode = pMode;
	_p->pMode->attach(_p);
	
	_p->bCheckedRelatedFile = false;
	_p->szCachedRelatedFilePath = QString();
	
	_p->szModeNameText = QString();
	emit privateInfoText2Changed(this);
	
	layout(); // do a full layout
	_p->pViewport->update(); // and a viewport update
}

void C3TextEditor::fillViewMenu(QMenu *pMenu)
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	QMenu * pModeMenu = pMenu->addMenu(__tr("Mode"));

	QAction * a = pModeMenu->addAction(__tr("Guess"));
	a->setData("mode.guess");
	a->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_M);
	a->setShortcutContext(Qt::WidgetShortcut);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotViewMenuModeSubmenuActionActivated()));
	
	a = pModeMenu->addAction(__tr("Cycle"));
	a->setData("mode.cycle");
	a->setShortcut(Qt::CTRL + Qt::Key_M);
	a->setShortcutContext(Qt::WidgetShortcut);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotViewMenuModeSubmenuActionActivated()));

	pModeMenu->addSeparator();

	QList< QPair<QString,QString> > lIdAndNames;

	_p->pFactory->getModeList(lIdAndNames);
	
	typedef QPair<QString,QString> DaStringPair ;
	
	foreach(const DaStringPair & p,lIdAndNames)
	{
		a = pModeMenu->addAction(p.second);
		a->setData(p.first);
		a->setCheckable(true);
		if(_p->pMode->id() == p.first)
			a->setChecked(true);
		QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotViewMenuModeSubmenuActionActivated()));
	}

	QMenu * pGoToMenu = pMenu->addMenu(__tr("Go To"));

	pGoToMenu->addAction(__tr("Beginning of File"),this,SLOT(goToStart()));
	pGoToMenu->addAction(__tr("Line..."),this,SLOT(goToLine()));
	pGoToMenu->addAction(__tr("End of File"),this,SLOT(goToEnd()));

	QMenu * pTabSizeMenu = pMenu->addMenu(__tr("Tab Stop Size"));

	a = pTabSizeMenu->addAction(__tr("Increase"),this,SLOT(increaseTabStopSize()));
	a->setShortcut(Qt::CTRL + Qt::Key_T);
	a->setShortcutContext(Qt::WidgetShortcut);

	a = pTabSizeMenu->addAction(__tr("Decrease"),this,SLOT(decreaseTabStopSize()));
	a->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_T);
	a->setShortcutContext(Qt::WidgetShortcut);
	if(_p->iTabStopSize < 2)
		a->setEnabled(false);

	QMenu * pAdditionalLineSpacingMenu = pMenu->addMenu(__tr("Line Spacing"));

	a = pAdditionalLineSpacingMenu->addAction(__tr("Increase"),this,SLOT(increaseAdditionalLineSpacing()));
	a->setShortcut(Qt::CTRL + Qt::Key_L);
	a->setShortcutContext(Qt::WidgetShortcut);

	a = pAdditionalLineSpacingMenu->addAction(__tr("Decrease"),this,SLOT(decreaseAdditionalLineSpacing()));
	a->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_L);
	a->setShortcutContext(Qt::WidgetShortcut);
	if(_p->iAdditionalLineSpacing < 1)
		a->setEnabled(false);

	// always perform a full search
	_p->bCheckedRelatedFile = false;
	_p->szCachedRelatedFilePath = QString();

	QString szRelatedFilePath = relatedFilePath();
	if(!szRelatedFilePath.isEmpty())
	{
		QFileInfo inf(szRelatedFilePath);
		szRelatedFilePath = inf.fileName();
	}

	if(szRelatedFilePath.isEmpty())
	{
		a = pMenu->addAction(
				pPixmapCache->pixmap(__utf8("general/20px/open-related-file.png")),
				__tr("Related File")
			);
		a->setEnabled(false);
	} else {
		a = pMenu->addAction(
				pPixmapCache->pixmap(__utf8("general/20px/open-related-file.png")),
				__tr("Related File (%1)").arg(szRelatedFilePath)
			);
	}

	a->setShortcut(Qt::CTRL + Qt::Key_R);
	a->setShortcutContext(Qt::WidgetShortcut);


	QObject::connect(a,SIGNAL(triggered()),this,SLOT(openRelatedFile()));
}

void C3TextEditor::fillEditMenu(QMenu *pMenu)
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	fillEditMenuAddUndo(pMenu);

	pMenu->addSeparator();

	fillEditMenuAddCopyCutAndPaste(pMenu);

	pMenu->addSeparator();

	QAction * a = pMenu->addAction(
			pPixmapCache->pixmap(__utf8("general/20px/edit-select-all.png")),
			__tr("Select All")
		);
	a->setShortcut(Qt::CTRL + Qt::Key_A);
	a->setShortcutContext(Qt::WidgetShortcut);

	QObject::connect(a,SIGNAL(triggered()),this,SLOT(selectAll()));
}

void C3TextEditor::fillEditMenuAddUndo(QMenu * pMenu)
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	QAction * a = pMenu->addAction(
			pPixmapCache->pixmap(__utf8("general/20px/edit-undo.png")),
			__tr("Undo")
		);
	if(!undoAvailable())
		a->setEnabled(false);
	a->setShortcut(Qt::CTRL + Qt::Key_Z);
	a->setShortcutContext(Qt::WidgetShortcut);

	QObject::connect(a,SIGNAL(triggered()),this,SLOT(undo()));
}

void C3TextEditor::fillEditMenuAddCopyCutAndPaste(QMenu * pMenu)
{
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	QAction * a = pMenu->addAction(
			pPixmapCache->pixmap(__utf8("general/20px/edit-copy.png")),
			__tr("Copy")
		);
	if(_p->oSelection.isEmpty())
		a->setEnabled(false);
	a->setShortcut(Qt::CTRL + Qt::Key_C);
	a->setShortcutContext(Qt::WidgetShortcut);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(copy()));

	a = pMenu->addAction(
			pPixmapCache->pixmap(__utf8("general/20px/edit-cut.png")),
			__tr("Cut")
		);
	if(_p->oSelection.isEmpty() || readOnly())
		a->setEnabled(false);
	a->setShortcut(Qt::CTRL + Qt::Key_X);
	a->setShortcutContext(Qt::WidgetShortcut);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(cut()));

	a = pMenu->addAction(
			pPixmapCache->pixmap(__utf8("general/20px/edit-paste.png")),
			__tr("Paste")
		);
	if(readOnly())
		a->setEnabled(false);
	a->setShortcut(Qt::CTRL + Qt::Key_V);
	a->setShortcutContext(Qt::WidgetShortcut);
	QObject::connect(a,SIGNAL(triggered()),this,SLOT(paste()));
}


void C3TextEditor::slotViewMenuModeSubmenuActionActivated()
{
	QAction * a = dynamic_cast<QAction *>(sender());
	if(!a)
		return;

	QString szId = a->data().toString();

	if(szId == __utf8("mode.guess"))
	{
		szId = guessBestMode();
	} else if(szId == __utf8("mode.cycle"))
	{
		szId = getNextMode();
	}
	
	setMode(szId);
}

bool C3TextEditor::saveInternal(
		const QString &szPath,
		C3EditorFormat * pFormat,
		QString &szError
	)
{
	C3TextEditorFormat * pTextFormat = _p->pFormat;
	
	if(pFormat)
	{
		pTextFormat = dynamic_cast<C3TextEditorFormat *>(pFormat);
		Q_ASSERT(pTextFormat);
	}

	C3TextEditorFormatWriter * pWriter = pTextFormat->createWriter();
	if(!pWriter)
	{
		szError = __tr("Could not create writer for format '%1'").arg(pTextFormat->name());
		return false;
	}

	bool bRet = pWriter->write(szPath,_p->lLines,szError);

	delete pWriter;

	if(!bRet)
		return false;

	// Succeeded.

	QFileInfo inf(szPath);

	_p->pFormat = pTextFormat;

	_p->szCachedRelatedFilePath = QString();
	_p->bCheckedRelatedFile = false;

	setTitle(inf.fileName());
	setPath(inf.canonicalFilePath());

	_p->pMode->fileSaved(modified());

	setModified(false);
	setReadOnly(false);

	return true;
}

bool C3TextEditor::newFile()
{
	C3EditorFormat * pFormat = factory()->format(__utf8("utf8"));

	if(!pFormat)
	{
		error(__tr("Could not find file format '%1'").arg(__utf8("utf8")));
		return false;
	}

	C3TextEditorFormat * pTextEditorFormat = dynamic_cast<C3TextEditorFormat *>(pFormat);
	Q_ASSERT(pTextEditorFormat);

	ensureNoRunningRead();
	killMouseScrollTimer();

	{
		C3TextEditorLinesLocker oLocker(_p);
		qDeleteAll(_p->lLines);
		_p->lLines.clear();
	}

	qDeleteAll(_p->lUndoStack);
	_p->lUndoStack.clear();

	if(_p->pMultiUndo)
	{
		Q_ASSERT(false); // should NOT happen
		delete _p->pMultiUndo;
		_p->pMultiUndo = NULL;
	}

	// No running readers/writers here: no need to lock.

	_p->szReadingError = QString();
	_p->bContentWasAlteredDuringRead = false;
	_p->szReadAlterationReason = QString();
	_p->bReadingFinished = false;
	_p->szCachedRelatedFilePath = QString();
	_p->bCheckedRelatedFile = false;
	
	C3TextEditorLine * pLine = new C3TextEditorLine();
	pLine->fWidth = 0.0;
	
	{
		C3TextEditorLinesLocker oLocker(_p);
		_p->lLines.append(pLine);
	}

	_p->iTopLine = 0;
	_p->pFormat = pTextEditorFormat;
	_p->fMaximumLineWidth = 0.0;
	_p->iCurrentFindOperationId = -1;

	_p->szCursorPositionText = QString();
	_p->oCursor.set(0,0);
	_p->oSelection.clear();
	_p->oMark.clear();
	_p->fCursorUpDownX = -1.0;

	if(_p->pMode->id() != "plain")
	{
		delete _p->pMode;
		_p->pMode = new C3TextEditorModePlainText(_p->pFactory);
		_p->szModeNameText = QString();
	}
	_p->pMode->attach(_p);

	// FIXME: Far too many signals emitted here, the main window is probably able to update itself anyway
	setModified(false);
	setPath(QString());
	setTitle(__tr("< unnamed >"));
	setReadOnly(false);
	emit privateInfoText2Changed(this);

	layout();

	return true;
}

void C3TextEditor::repeatedOpen(const C3Link &oLink)
{
	// reapply mode id, if needed
	QString szModeId = oLink.textEditorModeId();

	C3TextEditorMode * pMode = NULL;
	if((!szModeId.isEmpty()) && (_p->pMode->id() != szModeId))
		pMode = _p->pFactory->createMode(szModeId);

	if(pMode)
	{
		delete _p->pMode;
		_p->pMode = pMode;
		_p->szModeNameText = QString();
		_p->pMode->attach(_p);
	}

	// reapply location
	QString szLocation = oLink.location();

	if(!szLocation.isEmpty())
	{
		if(!_p->bReadingFinished) // still loading
			_p->oLink.setLocation(szLocation);
		else
			cursorMoveTo(szLocation);
	}

	emit privateInfoText2Changed(this);
}

QString C3TextEditor::guessBestFormat(const QByteArray &data)
{
	if(data.size() < 3)
		return __utf8("utf8");

	if(
			(((unsigned char)data.at(0)) == 0xef) &&
			(((unsigned char)data.at(1)) == 0xbb) &&
			(((unsigned char)data.at(2)) == 0xbf)
		)
	{
		return __utf8("utf8bom");
	}

	// look for encoding=<something>

	if(data.size() > 16)
	{
		int idx = data.indexOf("encoding=");
		if(idx >= 0)
		{
			QString ba = QString::fromUtf8(data.mid(idx+9));
			idx = ba.indexOf(QRegExp("[ 	,;\\r\\n]"));
			if(idx >= 0)
				ba = ba.left(idx);
				
			if(_p->pFactory->format(ba))
			{
				qDebug("[Got stored encoding %s]",ba.toUtf8().data());
				return ba;
			}
		}
	}

	return __utf8("utf8");
}


bool C3TextEditor::open(const C3Link &oLink)
{
	QString szPath = oLink.path();
	QString szName = oLink.value("name").toString();
	QByteArray oData = oLink.value("data").toByteArray();

	
	//qDebug("Open path(%s) location(%s)",szPath.toUtf8().data(),hParams.value("location").toString().toUtf8().data());

	QString szFormatId = oLink.textEditorFormatId();
	if(szFormatId.isEmpty())
	{
		// Guess it from "data"
		//qDebug("Need to guess format for path(%s) data size is %d[%x,%x,%x]",szPath.toUtf8().data(),v.size(),(char)v[0],(char)v[1],(char)v[2]);
		if(oData.size() > 0)
			szFormatId = guessBestFormat(oData);
		else
			szFormatId = __utf8("utf8");
		//qDebug("Guessed format is %s",szFormatId.toUtf8().data());
	} else {
		//qDebug("Got format id %s",szFormatId.toUtf8().data());
	}

	C3EditorFormat * pFormat = factory()->format(szFormatId);
	
	if(!pFormat)
	{
		error(__tr("Could not find file format '%1'").arg(szFormatId));
		return false;
	}

	C3TextEditorFormat * pTextEditorFormat = dynamic_cast<C3TextEditorFormat *>(pFormat);
	Q_ASSERT(pTextEditorFormat);

	C3TextEditorFormatReader * pReader = pTextEditorFormat->createReader();
	if(!pReader)
	{
		error(__tr("Could not create reader for format '%1'").arg(szFormatId));
		return false;
	}

	// OK, we're ready: shouldn't be able to fail anymore

	ensureNoRunningRead();
	killMouseScrollTimer();

	{
		C3TextEditorLinesLocker oLocker(_p);
		qDeleteAll(_p->lLines);
		_p->lLines.clear();
	}

	qDeleteAll(_p->lUndoStack);
	_p->lUndoStack.clear();

	if(_p->pMultiUndo)
	{
		Q_ASSERT(false); // should NOT happen
		delete _p->pMultiUndo;
		_p->pMultiUndo = NULL;
	}

	// No running readers/writers here: no need to lock.

	_p->szReadingError = QString();
	_p->bContentWasAlteredDuringRead = false;
	_p->szReadAlterationReason = QString();
	_p->bReadingFinished = false;
	_p->oLink = oLink;

	_p->iTopLine = 0;
	_p->pFormat = pTextEditorFormat;
	_p->pReader = pReader;
	_p->pReader->setup(szPath,this,*(_p->pFontMetrics),_p->fFontTabStopWidth);
	_p->fMaximumLineWidth = 0.0;
	_p->iCurrentFindOperationId = -1;
	_p->fCursorUpDownX = -1.0;

	_p->oCursor.set(0,0);
	_p->oSelection.clear();
	_p->oMark.clear();

	// FIXME: Far too many signals triggered here!
	setModified(false);
	setTitle(szName);
	setPath(szPath);
	setReadOnly(true);

	QString szModeId = oLink.textEditorModeId();

	C3TextEditorMode * pMode = NULL;
	if((!szModeId.isEmpty()) && (_p->pMode->id() != szModeId))
		pMode = _p->pFactory->createMode(szModeId);

	if(pMode)
	{
		delete _p->pMode;
		_p->pMode = pMode;
		_p->szModeNameText = QString();
	} else {
		QString szBestModeId = guessBestMode(oData);
	
		if(_p->pMode->id() != szBestModeId)
		{
			delete _p->pMode;
	
			pMode = _p->pFactory->createMode(szBestModeId);
			Q_ASSERT(pMode);
		
			if(!pMode)
				_p->pMode = new C3TextEditorModePlainText(_p->pFactory);
			else
				_p->pMode = pMode;
			_p->szModeNameText = QString();
		}
	}

	_p->pMode->attach(_p);

	emit privateInfoText2Changed(this);

	QThreadPool::globalInstance()->start(_p->pReader);

	layout();
	
	return true;
}

bool C3TextEditor::saveState(QJsonObject &oObject)
{
	oObject.insert(__utf8("path"),path());
	oObject.insert(__utf8("formatId"),format()->id());
	oObject.insert(__utf8("location"),__utf8("%1,%2").arg(_p->oCursor.row+1).arg(_p->oCursor.col));
	//qDebug("Editor %x saveState location(%s)",this,oObject.value("location").toString().toUtf8().data());
	return true;
}

void C3TextEditor::resetInheritedSettings()
{
	_p->iAdditionalLineSpacing = _p->pOptions->iAdditionalLineSpacing;
	_p->iTabStopSize = _p->pOptions->iTabStopSize;
}

void C3TextEditor::generalSettingsChanged()
{
	resetInheritedSettings();

	computeFontMetrics();

	_p->pMode->attach(_p);

	layout();

	_p->pViewport->update();
}


void C3TextEditor::computeFontMetrics()
{
	if(_p->pFontMetrics)
		delete _p->pFontMetrics;

	_p->pFontMetrics = new QFontMetricsF(_p->pOptions->oTextFont);

	//
	// Font metrics as they should be.
	//
	// --*----------------------------+---------+-----------+
	//   *                            |         |           |
	//   *                            |         |           |
	// --****----****---+             | ascent  |           |
	//   *   *  *   *   | x-height    |         | height    |
	//   *   *  *   *   |             |         |           | line spacing
	// --****----****---+--baseline---+         |           |
	//              *                 | descent |           |
	// ----------***----+-------------+---------+           |
	//                                          | leading   |
	// -----------------------------------------+           |
	//                                          | additionalInterLineSpacing
	// -----------------------------------------+-----------+
	//
	// In practice it's a mess. Descent is sometimes negative (because it's expressed in font coordinates)
	// but then Qt computes height() as ascent() + descent() and gets it wrong.
	// Line spacing isn't correct either.
	//

	_p->iFontAscent = _p->pFontMetrics->ascent();
	if(_p->iFontAscent < 0)
		_p->iFontAscent = -_p->iFontAscent;
	_p->iFontDescent = _p->pFontMetrics->descent(); // qt reports it as negative (and maybe it's right in some stupid world)
	if(_p->iFontDescent < 0)
		_p->iFontDescent = -_p->iFontDescent;
	_p->iFontHeight = _p->iFontAscent + _p->iFontDescent; // _p->pFontMetrics->height(); <-- Qt gets it wrong since it sums ascent and negative descent.
	_p->iFontLeading = _p->pFontMetrics->leading();
	if(_p->iFontLeading < 0)
		_p->iFontLeading = -_p->iFontLeading;
	_p->iFontLineSpacing = _p->pFontMetrics->lineSpacing();
	int iAuxLineSpacing = _p->iFontAscent + _p->iFontDescent + _p->iFontLeading;
	if(_p->iFontLineSpacing < iAuxLineSpacing)
		_p->iFontLineSpacing = iAuxLineSpacing;
	_p->iFontLineSpacing += _p->iAdditionalLineSpacing;
	_p->fFontTabStopWidth = _p->pFontMetrics->width(QChar(' ')) * _p->iTabStopSize;
	_p->fFontElisionWidth = _p->pFontMetrics->width(__utf8("…"));

	static QString szZeroes = __ascii("0000000000");

	for(int i=0;i<10;i++)
		_p->fFontZeroSequenceWidths[i] = _p->pFontMetrics->width(szZeroes.left(i));
}

QString C3TextEditor::relatedFilePathWithExtension(const QString &szExt,bool bTryHarder,void * pFindFileOptions)
{
	C3Workspace::FindFileOptions * pOptions = (C3Workspace::FindFileOptions *)pFindFileOptions;

	QFileInfo inf(path());
	
	QChar sep = QDir::separator();
	
	QString szName = inf.completeBaseName();
	if(bTryHarder)
	{
		int idx = szName.indexOf(QChar('_'));
		if(idx > 0)
			szName = szName.mid(0,idx);
	}
	szName.append(QChar('.'));
	szName.append(szExt);

	pOptions->sPathHint = inf.absolutePath();

	QString szRet = C3Workspace::currentWorkspace()->findFile(szName,pOptions);
	if(!szRet.isEmpty())
		return szRet;

	szName = inf.completeBaseName();
	if(bTryHarder)
	{
		int idx = szName.indexOf(QChar('_'));
		if(idx > 0)
			szName = szName.mid(0,idx);
	}
	szName.append(QChar('.'));
	szName.append(szExt.toUpper());

	szRet = C3Workspace::currentWorkspace()->findFile(szName,pOptions);
	if(!szRet.isEmpty())
		return szRet;

	return QString();
}

QString C3TextEditor::relatedFilePath(bool bTryHarder)
{
	if(_p->bCheckedRelatedFile)
		return _p->szCachedRelatedFilePath;

	_p->bCheckedRelatedFile = false;
	_p->szCachedRelatedFilePath = QString();

	QString szPath = path();

	if(szPath.isEmpty())
		return _p->szCachedRelatedFilePath;

	QFileInfo inf(path());

	QString szSuffix = inf.suffix().toLower();
	
	if(szSuffix.isEmpty())
		return _p->szCachedRelatedFilePath;

	static QString szCpp("cpp");
	static QString szC("c");
	static QString szCc("cc");
	static QString szCxx("cxx");
	static QString szM("m"); // ObjC
	static QString szH("h");
	static QString szHh("hh");
	static QString szHpp("hpp");
	static QString szHxx("hxx");
	
	C3Workspace::FindFileOptions opt;
	opt.pDeadline = new QDeadlineTimer(6000); // 6 sec max

	if((szSuffix == szCpp) || (szSuffix == szC) || (szSuffix == szCc) || (szSuffix == szCxx) || (szSuffix == szM))
	{
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szH,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szHh,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szHxx,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szHpp,bTryHarder,&opt);
		return _p->szCachedRelatedFilePath;
	}

	if((szSuffix == szH) || (szSuffix == szHh) || (szSuffix == szHxx) || (szSuffix == szHpp))
	{
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szC,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szCpp,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szCc,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szCxx,bTryHarder,&opt);
		if(!_p->szCachedRelatedFilePath.isEmpty())
			return _p->szCachedRelatedFilePath;
		_p->szCachedRelatedFilePath = relatedFilePathWithExtension(szM,bTryHarder,&opt);
		return _p->szCachedRelatedFilePath;
	}

	return _p->szCachedRelatedFilePath;
}

bool C3TextEditor::canOpenRelatedFile()
{
	if(_p->bCheckedRelatedFile)
		return !_p->szCachedRelatedFilePath.isEmpty();
	return !relatedFilePath().isEmpty();
}

void C3TextEditor::openRelatedFile()
{
	QString szRelatedFilePath = relatedFilePath(false);

	if(szRelatedFilePath.isEmpty())
	{
		// try harder, strip _
		szRelatedFilePath = relatedFilePath(true);

		if(szRelatedFilePath.isEmpty())
			return;
	}
	
	C3Workspace::currentWorkspace()->openFileAsync(szRelatedFilePath);
}

void C3TextEditor::recomputeLineWidths()
{
	_p->fMaximumLineWidth = 0.0;

	foreach(C3TextEditorLine * l,_p->lLines)
	{
		l->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(_p->pFontMetrics,l->szText,_p->fFontTabStopWidth);
		if(_p->fMaximumLineWidth < l->fWidth)
			_p->fMaximumLineWidth = l->fWidth;
	}
}

void C3TextEditor::slotVerticalScrollBarValueChanged(int iVal)
{
	if(_p->bIgnoreScrollBarChanges)
		return;

	_p->iTopLine = iVal;

	_p->pViewport->update();
	_p->pMargin->update();
}

void C3TextEditor::slotHorizontalScrollBarValueChanged(int iVal)
{
	if(_p->bIgnoreScrollBarChanges)
		return;

	_p->iXOffset = iVal;
	_p->pViewport->update();
	_p->pMargin->update();
}

void C3TextEditor::layout()
{
	int iLineCount = _p->lLines.count();

	if(MAXIMUM_LINE_WIDTH_IS_INVALID(_p))
	{
		qreal fLocalMaximumLineWidth = 0;
		foreach(C3TextEditorLine * pLine,_p->lLines)
		{
			if(pLine->fWidth > fLocalMaximumLineWidth)
				fLocalMaximumLineWidth = pLine->fWidth;
		}
		_p->fMaximumLineWidth = fLocalMaximumLineWidth;
	}

	_p->iContentsHeight = _p->iFontLineSpacing * iLineCount;
	_p->iContentsWidth = _p->iLeftMargin + ::ceil(_p->fMaximumLineWidth);

	int iMarginWidth = _p->pMargin->widthHint();

	int iInitialViewportWidth = _p->iViewportWidth;
	int iInitialViewportHeight = _p->iViewportHeight;

	_p->iViewportWidth = width() - iMarginWidth; // - 2;
	_p->iViewportHeight = height(); // - 2;

	bool bNeedVScroll = false;
	bool bNeedHScroll = false;
	
	if(_p->iContentsHeight > _p->iViewportHeight)
	{
		bNeedVScroll = true;
		_p->iViewportWidth -= _p->iScrollBarSize;
		if(_p->iContentsWidth > _p->iViewportWidth)
		{
			bNeedHScroll = true;
			_p->iViewportHeight -= _p->iScrollBarSize;
		}
	} else if(_p->iContentsWidth > _p->iViewportWidth)
	{
		bNeedHScroll = true;
		_p->iViewportHeight -= _p->iScrollBarSize;
		if(_p->iContentsHeight > _p->iViewportHeight)
		{
			bNeedVScroll = true;
			_p->iViewportWidth -= _p->iScrollBarSize;
		}
	}

	_p->iVisibleLineCount = _p->iViewportHeight / _p->iFontLineSpacing;
	if(_p->iVisibleLineCount > iLineCount)
		_p->iVisibleLineCount = iLineCount;

	//qDebug("Layout: visible %d, vh %d, flsp %d",_p->iVisibleLineCount,_p->iViewportHeight,_p->iFontLineSpacing);

	// Manage scrollbars
	if(bNeedHScroll)
	{
		_p->pHScrollBar->setGeometry(
				0,
				height() - _p->iScrollBarSize,
				bNeedVScroll ? (width() - _p->iScrollBarSize) : width(),
				_p->iScrollBarSize
			);

		int iMax = _p->iContentsWidth - _p->iViewportWidth;
		Q_ASSERT(iMax > 0);

		if(_p->iXOffset > iMax)
			_p->iXOffset = iMax;

		_p->bIgnoreScrollBarChanges = true;
		_p->pHScrollBar->setMaximum(iMax);
		_p->pHScrollBar->setPageStep(_p->iViewportWidth);
		_p->pHScrollBar->setValue(_p->iXOffset);
		_p->bIgnoreScrollBarChanges = false;

		if(!_p->pHScrollBar->isVisible())
			_p->pHScrollBar->show();
	} else {
		if(_p->pHScrollBar->isVisible())
			_p->pHScrollBar->hide();
	}

	if(bNeedVScroll)
	{
		_p->pVScrollBar->setGeometry(
				width() - _p->iScrollBarSize,
				0,
				_p->iScrollBarSize,
				bNeedHScroll ? (height() - _p->iScrollBarSize) : height()
			);

		int iMax = iLineCount - _p->iVisibleLineCount;
		Q_ASSERT(iMax > 0);

		if(_p->iTopLine > iMax)
			_p->iTopLine = iMax;

		_p->bIgnoreScrollBarChanges = true;
		_p->pVScrollBar->setMaximum(iMax);
		_p->pVScrollBar->setPageStep(_p->iVisibleLineCount);
		_p->pVScrollBar->setValue(_p->iTopLine);
		_p->bIgnoreScrollBarChanges = false;

		if(!_p->pVScrollBar->isVisible())
			_p->pVScrollBar->show();
	} else {
		if(_p->pVScrollBar->isVisible())
			_p->pVScrollBar->hide();
	}

	// *MAY* trigger an update
#if 0
	_p->pMargin->setGeometry(1,1,iMarginWidth,_p->iViewportHeight);
	_p->pViewport->setGeometry(1+iMarginWidth,1,_p->iViewportWidth,_p->iViewportHeight);
#else
	_p->pMargin->setGeometry(0,0,iMarginWidth,_p->iViewportHeight);
	_p->pViewport->setGeometry(iMarginWidth,0,_p->iViewportWidth,_p->iViewportHeight);
#endif

	if(
		(iInitialViewportWidth != _p->iViewportWidth) ||
		(iInitialViewportHeight != _p->iViewportHeight)
	)
		update(); // shaded rect
}

#if 0
void C3TextEditor::paintEvent(QPaintEvent * e)
{
	QPainter p(this);

	int iMarginWidth = _p->pMargin->width();

	QRect r(0,0,_p->iViewportWidth+iMarginWidth+2,_p->iViewportHeight+2);

	qDrawShadePanel(&p,r,palette(),true);
}
#endif

void C3TextEditor::resizeEvent(QResizeEvent * e)
{
	layout();
}

void C3TextEditor::showEvent(QShowEvent * e)
{
	C3Editor::showEvent(e);

	layout();

	if(_p->bCursorMovedWhileHidden)
	{
		beginVisualStateChange();
		_p->oVisualStateChangeInitialCursor.col++; // fake a cursor movement
		endVisualStateChange();
	}
	
	_p->bCursorOn = true;
	_p->pCursorTimer->start(CURSOR_BLINK_TIME);

	// go to location after read finished
	if(_p->bReadingFinished)
	{
		QString szLocation = _p->oLink.location();
		//qDebug("[%s] Applying location in show event (%s)",path().toUtf8().data(),szLocation.toUtf8().data());
		if(!szLocation.isEmpty())
		{
			cursorMoveTo(szLocation);
			_p->oLink.clearLocation();
		}
	}
}

void C3TextEditor::hideEvent(QHideEvent * e)
{
	C3Editor::hideEvent(e);
	//layout();

	_p->pCursorTimer->stop();
}

void C3TextEditor::focusInEvent(QFocusEvent * e)
{
	_p->bCursorOn = true;
	_p->pCursorTimer->start(CURSOR_BLINK_TIME);

	QWidget::focusInEvent(e);
}

void C3TextEditor::focusOutEvent(QFocusEvent * e)
{
	_p->pCursorTimer->stop();
	QWidget::focusOutEvent(e);
}

void C3TextEditor::escape()
{
	if(!_p->pCompletionWidget)
		return;
	if(!_p->pCompletionWidget->isVisible())
		return;
	_p->pCompletionWidget->hide();
}

void C3TextEditor::beginVisualStateChange()
{
	Q_ASSERT(_p->iVisualStateChangeStackCount >= 0);

	if(_p->iVisualStateChangeStackCount == 0)
	{
		_p->oVisualStateChangeInitialCursor = _p->oCursor;
		_p->oVisualStateChangeInitialSelection = _p->oSelection;
		_p->oVisualStateChangeInitialMark = _p->oMark;
		_p->iVisualStateChangeInitialLineCount = _p->lLines.count();
		_p->fVisualStateChangeInitialMaximumLineWidth = ::ceil(_p->fMaximumLineWidth);
		_p->bVisualStateChangeForceFullRepaint = false;
	}

	_p->iVisualStateChangeStackCount++;

	Q_ASSERT(_p->iVisualStateChangeStackCount < 10); // stack too deep: something is probably wrong.

	//qDebug("START %d",_p->iVisualStateChangeStackCount);
}

void C3TextEditor::endVisualStateChange(unsigned int uFlags)
{
	// FIXME: If not visible, return, but then perform a full layout when shown?

	Q_ASSERT(_p->iVisualStateChangeStackCount > 0);

	//qDebug("END %d",_p->iVisualStateChangeStackCount);

	if(uFlags & ForceFullRepaint)
		_p->bVisualStateChangeForceFullRepaint = true;

	_p->iVisualStateChangeStackCount--;
	
	if(_p->iVisualStateChangeStackCount > 0)
		return;

	//qDebug("PROCESSED");

	bool bFullRepaint = false;

	bool bIsVisible = isVisible();
	bool bCursorMoved = (_p->oVisualStateChangeInitialCursor != _p->oCursor) ||
						(bIsVisible && _p->bCursorMovedWhileHidden) ||
						(uFlags & PlaceCursorRowInMiddleOfScreen);
	bool bLineCountChanged = _p->iVisualStateChangeInitialLineCount != _p->lLines.count();
	bool bLineWidthChanged = MAXIMUM_LINE_WIDTH_IS_INVALID(_p) || (_p->fVisualStateChangeInitialMaximumLineWidth != ::ceil(_p->fMaximumLineWidth));
	bool bSelectionChanged = _p->oVisualStateChangeInitialSelection != _p->oSelection;
	bool bMarkChanged = _p->oVisualStateChangeInitialMark != _p->oMark;

	bool bAreaChanged = bLineCountChanged || bLineWidthChanged;

	if(
		bAreaChanged ||
		(bIsVisible && (_p->iVisibleLineCount < 1))  // didn't properly call layout() yet
	)
	{
		layout();
		bFullRepaint = true;
	}

	if(_p->bVisualStateChangeForceFullRepaint || bSelectionChanged || bMarkChanged)
		bFullRepaint = true;

	if(bCursorMoved)
	{
		//qDebug("Cursor moved: row was %d, top line was %d, visible count was %d",_p->oCursor.row,_p->iTopLine,_p->iVisibleLineCount);

		_p->bCursorMovedWhileHidden = !bIsVisible;
	
		if(!(uFlags & DontResetCursorUpDownX))
			_p->fCursorUpDownX = -1.0; // reset it (so next cursor up/down operations will use a new position)

		_p->bIgnoreScrollBarChanges = true;
		
		if(uFlags & PlaceCursorRowInMiddleOfScreen)
		{
			// place the cursor in the middle of the screen
			_p->iTopLine = _p->oCursor.row - (_p->iVisibleLineCount / 2);
			if(_p->iTopLine < 0)
				_p->iTopLine = 0;
			else {
				int iMax = _p->lLines.count() - _p->iVisibleLineCount;
				if(iMax < 0)
					iMax = 0;
				if(_p->iTopLine > iMax)
					_p->iTopLine = iMax;
			}

			_p->pVScrollBar->setValue(_p->iTopLine);
			bFullRepaint = true;
		} else {
			// just make sure it's visible
			if(_p->oCursor.row < _p->iTopLine)
			{
				_p->iTopLine = _p->oCursor.row;
				_p->pVScrollBar->setValue(_p->iTopLine);
				bFullRepaint = true;
			} else if(_p->oCursor.row > (_p->iTopLine + _p->iVisibleLineCount - 1))
			{
				_p->iTopLine = _p->oCursor.row - _p->iVisibleLineCount + 1;
				if(_p->iTopLine < 0)
					_p->iTopLine = 0;
				_p->pVScrollBar->setValue(_p->iTopLine);
				bFullRepaint = true;
			}
		}

		int x = qRound(columnToAbsoluteXPosition(_p->oCursor.row,_p->oCursor.col));

#define X_MARGIN 7
	
		if(x < (_p->iXOffset + _p->iLeftMargin))
		{
			_p->iXOffset = x - _p->iLeftMargin;
			_p->pHScrollBar->setValue(_p->iXOffset);
			bFullRepaint = true;
		} else {
			int iRight = _p->iXOffset + _p->iViewportWidth - X_MARGIN;
			if(x >= iRight)
			{
				_p->iXOffset = x - _p->iViewportWidth + X_MARGIN;
				_p->pHScrollBar->setValue(_p->iXOffset);
				bFullRepaint = true;
			}
		}
	
		_p->bIgnoreScrollBarChanges = false;
		
		_p->szCursorPositionText = QString();
		emit privateInfoText1Changed(this);
	}

	// restart cursor blinking
	if(bIsVisible && (!(uFlags & DontRestartCursorTimer)))
	{
		_p->bCursorOn = true;
		_p->pCursorTimer->start(CURSOR_BLINK_TIME);
	}

	if((!(uFlags & DontHideCompletionWidget)) && (_p->pCompletionWidget) && (_p->pCompletionWidget->isVisible()))
	{
		_p->pCompletionWidget->hide();
		bFullRepaint = true;
	}

	if(bFullRepaint)
	{
		_p->pViewport->update();
		_p->pMargin->update();
	} else {
		repaintCursorRow();
		if(_p->oVisualStateChangeInitialCursor.row != _p->oCursor.row)
			repaintRow(_p->oVisualStateChangeInitialCursor.row);
	}
}

void C3TextEditor::repaintRow(int iRow)
{
	if(iRow < _p->iTopLine)
		return;
	if(iRow >= (_p->iTopLine + _p->iVisibleLineCount))
		return;

	int iY = (iRow - _p->iTopLine) * _p->iFontLineSpacing;

	_p->pViewport->update(0,iY,_p->iViewportWidth,_p->iFontLineSpacing);
	_p->pMargin->update(0,iY,_p->pMargin->width(),_p->iFontLineSpacing);
}

void C3TextEditor::killMouseScrollTimer()
{
	if(!_p->pMouseScrollTimer)
		return;

	_p->pMouseScrollTimer->stop();
	delete _p->pMouseScrollTimer;
	_p->pMouseScrollTimer = NULL;
}

void C3TextEditor::startMouseScrollTimer()
{
	if(_p->pMouseScrollTimer)
		return;

	_p->pMouseScrollTimer = new QTimer(this);
	QObject::connect(_p->pMouseScrollTimer,SIGNAL(timeout()),this,SLOT(slotMouseScrollTimerTimeout()));
	_p->pMouseScrollTimer->start(80);
}

void C3TextEditor::slotMouseScrollTimerTimeout()
{
	QPoint pnt = _p->pViewport->mapFromGlobal(QCursor::pos());

	C3TextEditorRowColumn rc;
	
	if(!viewportPointToRowColumn(pnt,rc,true))
		return; // should never happen because we pass true.

	cursorMoveTo(rc,true);
}

QString C3TextEditor::getSelectionText()
{
	return getRangeText(_p->oSelection);
}

QString C3TextEditor::getLineText(int iLine)
{
	C3TextEditorLinesLocker oLocker(_p);

	if(iLine < 0)
		return QString();
	if(iLine >= _p->lLines.count())
		return QString();
	
	C3TextEditorLine * pLine = _p->lLines.at(iLine);
	if(!pLine)
		return QString();
	
	return pLine->szText;
}

QString C3TextEditor::getRangeText(const C3TextEditorRange &oRange)
{
	QString szText;

	C3TextEditorRange oSafeRange = oRange;

	if(oSafeRange.isEmpty())
		return szText;

	int iLineCount = _p->lLines.count();
	if(iLineCount < 1)
		return QString();

	if(oSafeRange.end.row >= iLineCount)
	{
		oSafeRange.end.row = iLineCount - 1;
		oSafeRange.end.col = _p->lLines.at(oSafeRange.end.row)->szText.length();
	} else {
		int iLength = _p->lLines.at(oSafeRange.end.row)->szText.length();
		if(oSafeRange.end.col >= iLength)
			oSafeRange.end.col = iLength;
	}

	if(oSafeRange.start.row < 0)
		oSafeRange.start.row = 0;

	if(oSafeRange.isEmpty())
		return szText;

	int iRow = oSafeRange.start.row;
	int iEndRow = oSafeRange.end.row;

	if(iRow == iEndRow)
	{
		C3TextEditorLine * pLine = _p->lLines.at(iRow);
		int iLen = oSafeRange.end.col - oSafeRange.start.col;
		szText = pLine->szText.mid(oSafeRange.start.col,iLen);
	} else {

		C3TextEditorLine * pFirstLine = _p->lLines.at(iRow);
		szText.append(pFirstLine->szText.mid(oSafeRange.start.col));
	
		iRow++;
	
		while(iRow < iEndRow)
		{
			C3TextEditorLine * pLine = _p->lLines.at(iRow);
			szText.append(QChar('\n'));
			szText.append(pLine->szText);

			iRow++;
		}

		C3TextEditorLine * pLastLine = _p->lLines.at(iRow);
		szText.append(QChar('\n'));
		szText.append(pLastLine->szText.left(oSafeRange.end.col));
	}

	return szText;
}

void C3TextEditor::selectRange(const C3TextEditorRange &oRange)
{
	beginVisualStateChange();

	_p->oSelection = oRange;

	if(!_p->oSelection.isEmpty())
	{
		int iLineCount = _p->lLines.count();
	
		if(_p->oSelection.end.row >= iLineCount)
		{
			_p->oSelection.end.row = iLineCount - 1;
			_p->oSelection.end.col = _p->lLines.at(_p->oSelection.end.row)->szText.length();
		} else {
			int iLength = _p->lLines.at(_p->oSelection.end.row)->szText.length();
			if(_p->oSelection.end.col >= iLength)
				_p->oSelection.end.col = iLength;
		}
	}
	
	if(!_p->oSelection.isEmpty())
		_p->oCursor = _p->oSelection.end;

	endVisualStateChange();
}

const C3TextEditorRange & C3TextEditor::selectedRange()
{
	return _p->oSelection;
}


bool C3TextEditor::computeNearestTokenRange(int iRow,int iCol,C3TextEditorRange &oRange)
{
	if(iRow < 0)
		return false;
	if(iRow >= _p->lLines.count())
		return false;
	
	C3TextEditorLine * pLine = _p->lLines.at(iRow);
	Q_ASSERT(pLine);

	if(pLine->szText.length() < 1)
		return false;

	oRange.start.row = iRow;
	oRange.end.row = iRow;

	int iLength = pLine->szText.length();

	if(iCol >= iLength)
		iCol = iLength - 1;

	const QChar * b = pLine->szText.unicode();
	const QChar * e = b + iLength;
	const QChar * p = b + iCol;

	if(p->isLetterOrNumber() || (p->unicode() == '_'))
	{
		// go left
		const QChar * px = p;
		while(px > b)
		{
			px--;
			if(!(px->isLetterOrNumber() || (px->unicode() == '_')))
			{
				px++;
				break;
			}
		}
		
		oRange.start.col = px - b;
		
		// go right
		px = p;
		while(px < e)
		{
			px++;
			if(!(px->isLetterOrNumber() || (px->unicode() == '_')))
				break;
		}

		oRange.end.col = px - b;
	} else if(p->isSpace())
	{
		// go left
		const QChar * px = p;
		while(px > b)
		{
			px--;
			if(!px->isSpace())
			{
				px++;
				break;
			}
		}
		
		oRange.start.col = px - b;
		
		// go right
		px = p;
		while(px < e)
		{
			px++;
			if(!px->isSpace())
				break;
		}

		oRange.end.col = px - b;
	} else {
		// go left
		const QChar * px = p;
		while(px > b)
		{
			px--;
			if(px->isSpace() || px->isLetterOrNumber() || (px->unicode() == '_'))
			{
				px++;
				break;
			}
		}
		
		oRange.start.col = px - b;
		
		// go right
		px = p;
		while(px < e)
		{
			px++;
			if(px->isSpace() || px->isLetterOrNumber() || (px->unicode() == '_'))
				break;
		}

		oRange.end.col = px - b;
	}
	
	return oRange.end.col != oRange.start.col;
}

bool C3TextEditor::computeRangeOfWordBeforePosition(
		const C3TextEditorRowColumn &oPos,
		C3TextEditorRange &oRange,
		bool bIncludeDots,
		bool bIncludeSlashes
	)
{
	int iRow = oPos.row;
	int iCol = oPos.col;

	if(iRow < 0)
		return false;
	if(iRow >= _p->lLines.count())
		return false;

	C3TextEditorLine * pLine = _p->lLines.at(iRow);
	Q_ASSERT(pLine);

	if(pLine->szText.length() < 1)
		return false;

	oRange.start.row = iRow;
	oRange.end.row = iRow;

	int iLength = pLine->szText.length();

	if(iCol > iLength)
		iCol = iLength;

	if(iCol == 0)
		return false; 

	iCol--;

	const QChar * b = pLine->szText.unicode();
	const QChar * e = b + iLength;
	const QChar * p = b + iCol;

	if(!(
			p->isLetterOrNumber() ||
			(p->unicode() == '_') ||
			((p->unicode() == '.') && bIncludeDots) ||
			(((p->unicode() == '/') || (p->unicode() == '\\')) && bIncludeSlashes)
	))
		return false;

	// go left
	const QChar * px = p;
	while(px > b)
	{
		px--;
		if(!(
				px->isLetterOrNumber() ||
				(px->unicode() == '_') ||
				((px->unicode() == '.') && bIncludeDots) ||
				(((px->unicode() == '/') || (px->unicode() == '\\')) && bIncludeSlashes)
		))
		{
			px++;
			break;
		}
	}

	oRange.start.col = px - b;
	
	// go right
	px = p;
	while(px < e)
	{
		px++;
		if(!(
				px->isLetterOrNumber() ||
				(px->unicode() == '_') ||
				((px->unicode() == '.') && bIncludeDots) ||
				(((px->unicode() == '/') || (px->unicode() == '\\')) && bIncludeSlashes)
		))
			break;
	}

	oRange.end.col = px - b;

	return oRange.end.col != oRange.start.col;
}

void C3TextEditor::selectAll()
{
	beginVisualStateChange();

	_p->oMark.clear();

	_p->oSelection.start.row = 0;
	_p->oSelection.start.col = 0;
	
	int iLineCount = _p->lLines.count() - 1;
	Q_ASSERT(iLineCount >= 0);

	C3TextEditorLine * pLine = _p->lLines.at(iLineCount);

	_p->oSelection.end.row = iLineCount;
	_p->oSelection.end.col = pLine->szText.length();

	_p->oCursor = _p->oSelection.end;

	endVisualStateChange();
}

void C3TextEditor::clearSelection()
{
	beginVisualStateChange();

	_p->oMark.clear();

	_p->oSelection.clear();

	endVisualStateChange();
}

void C3TextEditor::updateOrInvalidateMaximumLineWidthAfterChange(qreal fMaximumPreChangeLineWidth,qreal fMaximumPostChangeLineWidth)
{
	Q_ASSERT(_p->iVisualStateChangeStackCount > 0); // must be called in a beginVisualStateChange()/endVisualStateChange() pair

	// Take care of maximum line width changes
	if(!MAXIMUM_LINE_WIDTH_IS_VALID(_p))
	{
		// it's already invalid, will be taken care of in endVisualStateChange() -> layout()
		return;
	}

	if(fMaximumPostChangeLineWidth >= _p->fMaximumLineWidth)
	{
		// new lines contain the maximum line
		_p->fMaximumLineWidth = fMaximumPostChangeLineWidth;
	} else {
		// new lines are shorter
		if(fMaximumPreChangeLineWidth >= _p->fMaximumLineWidth)
		{
			// oops... the initial line width was the maximum
			// the maximum probably decreased now, but we can't know unless
			// we rescan the whole buffer. Setting it to MAXIMUM_LINE_WIDTH_INVALID
			// will take care of it in endVisualStateChange() -> layout().
			_p->fMaximumLineWidth = MAXIMUM_LINE_WIDTH_INVALID;
		} else {
			// we didn't manipulate the maximum width line at all.
		}
	}
}

void C3TextEditor::showContextMenu(const QPoint &oGlobalPos)
{
	QMenu * pMenu = _p->pFactory->sharedContextMenu();
	Q_ASSERT(pMenu);

	pMenu->clear();

	QMenu * pSymbolMenu = pMenu->addMenu(__tr("Symbol"));
	fillSymbolMenu(pSymbolMenu);

	pMenu->addSeparator();

	fillEditMenuAddCopyCutAndPaste(pMenu);

	pMenu->addSeparator();

	QMenu * pEditMenu = pMenu->addMenu(__tr("Edit"));
	fillEditMenu(pEditMenu);
	
	QMenu * pViewMenu = pMenu->addMenu(__tr("View"));
	fillViewMenu(pViewMenu);
	
	QMenu * pCodeMenu = pMenu->addMenu(__tr("Code"));
	fillCodeMenu(pCodeMenu);
	
	QMenu * pFXMenu = pMenu->addMenu(__tr("FX"));
	fillFXMenu(pFXMenu);

	pMenu->addSeparator();

	pMenu->addAction(C3GeneralActions::instance()->documentSaveAction());
	pMenu->addAction(C3GeneralActions::instance()->documentSaveAsAction());
	pMenu->addAction(C3GeneralActions::instance()->documentSaveAllAction());
	pMenu->addAction(C3GeneralActions::instance()->documentCloseAction());

	pMenu->popup(oGlobalPos);
}

void C3TextEditor::fillCodeMenu(QMenu * pMenu)
{
	_p->pMode->fillCodeMenu(pMenu);
}

void C3TextEditor::slotCursorTimerTimeout()
{
	beginVisualStateChange();
	_p->bCursorOn = !_p->bCursorOn;
	endVisualStateChange(DontRestartCursorTimer | DontHideCompletionWidget);
}

int C3TextEditor::scrollRowOffset()
{
	if(!_p->pVScrollBar->isVisible())
		return 0;
	return _p->pVScrollBar->value();
}

void C3TextEditor::setScrollRowOffset(int iOffset)
{
	if(!_p->pVScrollBar->isVisible())
		return;
	if(iOffset > _p->pVScrollBar->maximum())
		iOffset = _p->pVScrollBar->maximum();
	else if(iOffset < _p->pVScrollBar->minimum())
		iOffset = _p->pVScrollBar->minimum();
	
	_p->pVScrollBar->setValue(iOffset);
	slotVerticalScrollBarValueChanged(iOffset);
}


int C3TextEditor::scrollColumnOffset()
{
	if(!_p->pHScrollBar->isVisible())
		return 0;
	return _p->pHScrollBar->value();
}

void C3TextEditor::setScrollColumnOffset(int iOffset)
{
	if(!_p->pHScrollBar->isVisible())
		return;
	if(iOffset > _p->pHScrollBar->maximum())
		iOffset = _p->pHScrollBar->maximum();
	else if(iOffset < _p->pHScrollBar->minimum())
		iOffset = _p->pHScrollBar->minimum();
	
	_p->pHScrollBar->setValue(iOffset);
	slotHorizontalScrollBarValueChanged(iOffset);
}

void C3TextEditor::saveCursorAndScrollBarState(CursorAndScrollBarState &oState)
{
	oState.oCursorPos = _p->oCursor;
	oState.iYOffset = scrollRowOffset();
	oState.iXOffset = scrollColumnOffset();
}

void C3TextEditor::restoreCursorAndScrollBarState(const CursorAndScrollBarState &oState)
{
	//qDebug("RESTORE %d,%d %d,%d",oState.oCursorPos.row,oState.oCursorPos.col,oState.iYOffset,oState.iXOffset);
	cursorMoveTo(oState.oCursorPos,false);
	setScrollRowOffset(oState.iYOffset);
	setScrollColumnOffset(oState.iXOffset);
}


bool C3TextEditor::saveTemporaryFile(const QString &szFilePath,QString &szError,bool * pbAborted,const QString &szTrailer)
{
	// This can be called concurrently!!!
	if(pbAborted && *pbAborted)
	{
		szError = __tr("Aborted");
		return false;
	}

	QFile f(szFilePath);
	
	if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		szError = f.errorString();
		return false;
	}

	QTextCodec::ConverterState oState;

	C3TextEditorLinesLocker oLocker(_p);

	int c = _p->lLines.count();
	
	int i=0;
	
	QByteArray ba;

	C3TextEditorLine * pLine;

	QTextCodec * pTextCodec = QTextCodec::codecForName("UTF-8");
	Q_ASSERT(pTextCodec);

	while(i < c-1)
	{
		if(pbAborted && *pbAborted)
		{
			szError = __tr("Aborted");
			return false;
		}

		pLine = _p->lLines.at(i);
	
		ba = pTextCodec->fromUnicode(pLine->szText.unicode(),pLine->szText.length(),&oState);
		ba.append('\n');
		if(f.write(ba) != ba.size())
		{
			QString szDetail = f.errorString();
			szError = szDetail.isEmpty() ? __tr("File I/O error") : szDetail;
			return false;
		}
		i++;
	}

	if(pbAborted && *pbAborted)
	{
		szError = __tr("Aborted");
		return false;
	}

	pLine = _p->lLines.at(i);

	// last line without newline
	if(!pLine->szText.isEmpty())
	{
		ba = pTextCodec->fromUnicode(pLine->szText.unicode(),pLine->szText.length(),&oState);
		if(f.write(ba) != ba.size())
		{
			QString szDetail = f.errorString();
			szError = szDetail.isEmpty() ? __tr("File I/O error") : szDetail;
			return false;
		}
	}
	
	if(!szTrailer.isEmpty())
	{
		ba = pTextCodec->fromUnicode(szTrailer.unicode(),szTrailer.length(),&oState);
		if(f.write(ba) != ba.size())
		{
			QString szDetail = f.errorString();
			szError = szDetail.isEmpty() ? __tr("File I/O error") : szDetail;
			return false;
		}
	}

	//qDebug("[C3TextEditor][%s] Saved temporary file to %s",path().toUtf8().data(),szFilePath.toUtf8().data());

	return true;
}

void C3TextEditor::setTabStopSize(int iTabSize)
{
	_p->iTabStopSize = iTabSize;

	computeFontMetrics();
	recomputeLineWidths();
	_p->pMode->textModified(0,_p->lLines.count()-1);
	layout();
	update();
}

void C3TextEditor::increaseTabStopSize()
{
	setTabStopSize(_p->iTabStopSize + 1);
}

void C3TextEditor::decreaseTabStopSize()
{
	if(_p->iTabStopSize > 1)
		setTabStopSize(_p->iTabStopSize - 1);
}


void C3TextEditor::setAdditionalLineSpacing(int iSpacing)
{
	_p->iAdditionalLineSpacing = iSpacing;

	computeFontMetrics();
	recomputeLineWidths();
	_p->pMode->textModified(0,_p->lLines.count()-1);
	layout();
	update();
}

void C3TextEditor::increaseAdditionalLineSpacing()
{
	setAdditionalLineSpacing(_p->iAdditionalLineSpacing + 1);
}

void C3TextEditor::decreaseAdditionalLineSpacing()
{
	if(_p->iAdditionalLineSpacing > 0)
		setAdditionalLineSpacing(_p->iAdditionalLineSpacing - 1);
}

bool C3TextEditor::handleKeyEvent(QKeyEvent * e)
{
	if(_p->pMode->handleKeyEvent(e))
		return true;

	if(e->modifiers() & Qt::ControlModifier)
	{
		switch(e->key())
		{
			case Qt::Key_A:
				selectAll();
				return true;
			break;
			case Qt::Key_C:
				copy();
				return true;
			break;
			case Qt::Key_S:
				save();
				return true;
			break;
			case Qt::Key_X:
				cut();
				return true;
			break;
			case Qt::Key_V:
				paste();
				return true;
			break;
			case Qt::Key_Z:
				undo();
				return true;
			break;
			case Qt::Key_R:
				openRelatedFile();
				return true;
			break;
			case Qt::Key_M:
				if(e->modifiers() & Qt::ShiftModifier)
					setBestGuessMode();
				else
					cycleMode();
				return true;
			break;
			case Qt::Key_I:
				if(e->modifiers() & Qt::AltModifier)
					triggerSymbolIdentificationNoContextFilter();
				else
					triggerSymbolIdentification();
				return true;
			break;
			case Qt::Key_F:
				if(e->modifiers() & Qt::AltModifier)
				{
					grepCurrentSymbolInWorkspace();
					return true;
				}
				if(e->modifiers() & Qt::ShiftModifier)
				{
					grepCurrentSymbolInCurrentDirectory();
					return true;
				}
			break;
			case Qt::Key_T:
				if(e->modifiers() & Qt::ShiftModifier)
				{
					decreaseTabStopSize();
				} else {
					increaseTabStopSize();
				}
				return true;
			break;
			case Qt::Key_L:
				if(e->modifiers() & Qt::ShiftModifier)
				{
					decreaseAdditionalLineSpacing();
				} else {
					increaseAdditionalLineSpacing();
				}
				return true;
			break;
			case Qt::Key_Space:
				if(e->modifiers() & Qt::AltModifier)
					triggerSymbolCompletionNoContextFilter();
				else
					triggerSymbolCompletion();
				return true;
			break;
		}
	}

	if(e->modifiers() & Qt::ShiftModifier)
	{
		switch(e->key())
		{
			case Qt::Key_Tab:
				unindent();
				return true;
			break;
			case Qt::Key_Backtab:
				unindent();
				return true;
			break;
		}
	}

	switch(e->key())
	{
		case Qt::Key_Left:
			cursorLeft(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_Right:
			cursorRight(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_Up:
			if(!completionUp())
				cursorUp(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_Down:
			if(!completionDown())
				cursorDown(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_PageUp:
			cursorPageUp(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_PageDown:
			cursorPageDown(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_Home:
			cursorHome(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_End:
			cursorEnd(e->modifiers() & Qt::ShiftModifier);
			return true;
		break;
		case Qt::Key_Backspace:
			backspace();
			return true;
		break;
		case Qt::Key_Delete:
			del();
			return true;
		break;
		case Qt::Key_Escape:
			escape();
			return true;
		break;
		case Qt::Key_Enter:
		case Qt::Key_Return:
			if(!enterCompletion())
				newline();
			return true;
		break;
		case Qt::Key_Tab:
			if(!tabCompletion())
				tab();
			return true;
		break;
		case Qt::Key_Backtab:
			unindent();
			return true;
		break;
		case Qt::Key_Control:
		case Qt::Key_Shift:
		case Qt::Key_Meta:
		case Qt::Key_Alt:
			return false;
		break;
		default:
			insertTypedText(e->text());
			return true;
		break;
	}
	
	return false;
}