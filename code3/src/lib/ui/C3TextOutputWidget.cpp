//=============================================================================
//
//   File : C3TextOutputWidget.cpp
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

#include "C3TextOutputWidget.h"
#include "C3Workspace.h"
#include "C3FileUtils.h"

#include <QFont>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QFileInfo>
#include <QContextMenuEvent>
#include <C3TextEditor.h>
#include <QScrollBar>
#include <QFontMetrics>

class C3TextOutputWidgetPrivate
{
public:
	int iLineHeight;
};

#define _p m_pC3TOW

C3TextOutputWidget::C3TextOutputWidget(QWidget * pParent)
	: QTextEdit(pParent)
{
	_p = new C3TextOutputWidgetPrivate();
	
	setFont(QFont("Noto Mono",11)); // FIXME: make this optional!
	
	QFontMetrics fm(font());
	
	_p->iLineHeight = fm.height();

	setLineWrapMode(QTextEdit::NoWrap);
	setTextInteractionFlags(Qt::TextSelectableByMouse); // | Qt::TextSelectableByKeyboard); <-- this one forces ClickFocus
	setReadOnly(true);
}

C3TextOutputWidget::~C3TextOutputWidget()
{
	delete _p;
}

void C3TextOutputWidget::contextMenuEvent(QContextMenuEvent * e)
{
	QMenu menu;
	QAction * a = menu.addAction(__tr("Copy"),this,SLOT(slotCopy()));
	//a->setEnabled(copyAvailable());

	menu.addAction(__tr("Select All"),this,SLOT(slotSelectAll()));
	menu.addAction(__tr("Move Text to New File"),this,SLOT(slotMoveTextToNewFile()));
	menu.addAction(__tr("Open All Files Found"),this,SLOT(slotOpenAllFilesFound()));
	menu.exec(mapToGlobal(e->pos()));
}


void C3TextOutputWidget::slotCopy()
{
	copy();
}

void C3TextOutputWidget::slotSelectAll()
{
	selectAll();
}

void C3TextOutputWidget::slotOpenAllFilesFound()
{
	QString txt = toPlainText();
	
	QStringList lst = txt.split(QChar('\n'));
	foreach(QString s,lst)
		tryToOpenFileInText(s);
}

void C3TextOutputWidget::slotMoveTextToNewFile()
{
	C3Link oLink;
	oLink.setEditorFactoryId(__ascii("text"));

	QString txt = toPlainText();

	C3Editor * pEd = C3Workspace::currentWorkspace()->newFile(oLink);
	if(!pEd)
		return;

	C3TextEditor * pTed = dynamic_cast<C3TextEditor *>(pEd);
	if(!pTed)
		return;

	pTed->insertText(txt);
}

void C3TextOutputWidget::appendHtml(const QString &szText)
{
	QScrollBar * sb = verticalScrollBar();
	int sbPos = -1;
	bool bAtEnd;
	if(sb)
	{
		sbPos = sb->value();
		bAtEnd = sb->value() >= (sb->maximum() - _p->iLineHeight);
	} else {
		bAtEnd = true;
	}
	QTextCursor cur = textCursor();
	int pos = cur.position(); // this is nasty: the cursor MOVES even when detached from the QTextEdit.
	if(!cur.atEnd())
		moveCursor(QTextCursor::End);
	insertHtml(szText);
	if(bAtEnd)
	{
		moveCursor(QTextCursor::End);
	} else {
		cur.setPosition(pos);
		setTextCursor(cur);
		if(sbPos >= 0)
			sb->setValue(sbPos);
	}
}

void C3TextOutputWidget::appendText(const QString &szText)
{
	appendHtml(__utf8("<span style=\"color: #c0c0c0; white-space: pre;\">%2</span>").arg(szText.toHtmlEscaped()));
}

void C3TextOutputWidget::appendText(const QString &szText,const QString &szStyle)
{
	appendHtml(__utf8("<span style=\"%1; white-space: pre;\">%2</span>").arg(szStyle).arg(szText.toHtmlEscaped()));
}

bool C3TextOutputWidget::tryToOpenFile(const QString &szPath)
{
	int idx = szPath.indexOf(QChar(':'),3);
	QString szFixedPath = (idx > 3) ? szPath.left(idx) : szPath;
	QString szRightPart = (idx > 3) ? szPath.mid(idx) : QString();

	if(C3FileUtils::isAbsolutePath(szFixedPath))
	{
		QFileInfo inf(szFixedPath);
		if(inf.exists() && inf.isFile())
		{
			C3Workspace::currentWorkspace()->openFileAsync(szPath); // with the :<n> part
			return true;
		}
		
		//qDebug("Absolute path %s not found",szFixedPath.toUtf8().data());
		return false;
	}
	
	QString szFound = C3Workspace::currentWorkspace()->findFile(szFixedPath,QString(),false);

	if(szFound.isEmpty())
	{
		//qDebug("File %s not found in workspace",szFixedPath.toUtf8().data());
		return false;
	}

	C3Workspace::currentWorkspace()->openFileAsync(szFound + szRightPart); // with the :<n> part
	return true;
}

bool C3TextOutputWidget::tryToOpenFileInText(const QString &szText)
{
#define TRY_REGEXP(_szRegexp) \
	do { \
		QRegularExpression rx(_szRegexp); \
		QRegularExpressionMatch match = rx.match(szText); \
		if(match.hasMatch()) \
		{ \
			QString szCaptured = match.captured(0).trimmed(); \
			/*qDebug("Trying file (%s)",szCaptured.toUtf8().data());*/ \
			if(tryToOpenFile(szCaptured)) \
				return true; \
		} \
	} while(0);

	// absolute, without spaces, at beginning
	TRY_REGEXP("^/[A-Za-z0-9\\-\\+_\\./]+(:[0-9]+([:,][0-9]+)?)?");

	// relative, without spaces, at beginning
	TRY_REGEXP("^[A-Za-z0-9\\-\\+_\\./]+(:[0-9]+([:,][0-9]+)?)?");

	// absolute, with spaces, with extension, at beginning
	TRY_REGEXP("^/[A-Za-z0-9\\-\\+_\\./ ]+\\.[A-Za-z]+(:[0-9]+([:,][0-9]+)?)?");

	// absolute, without spaces, not at beginning, with a leading space
	TRY_REGEXP(" /[A-Za-z0-9\\-\\+_\\./]+(:[0-9]+([:,][0-9]+)?)?");

	// absolute, with spaces, not at beginning, with a leading space
	TRY_REGEXP(" /[A-Za-z0-9\\-\\+_\\./ ]+(:[0-9]+([:,][0-9]+)?)?");

	// relative, with spaces, with extension, not at beginning, with a leading space
	TRY_REGEXP(" [A-Za-z0-9\\-\\+_\\./ ]+\\.[A-Za-z]+(:[0-9]+([:,][0-9]+)?)?");
	
	// python: File "build_data_sets.py", line 754,
	{
		QRegularExpression rx("File \"([A-Za-z0-9\\-\\+_\\./ ]+)\", line ([0-9]+),");
		QRegularExpressionMatch match = rx.match(szText);
		if(match.hasMatch())
		{
			QString szFile = match.captured(0).trimmed();
			QString szLine = match.captured(1).trimmed();
			/*qDebug("Trying file (%s)",szCaptured.toUtf8().data());*/
			if(tryToOpenFile(__literal("%1:%2").arg(szFile).arg(szLine)))
				return true;
		}
	}
	
	return false;
}


void C3TextOutputWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	QTextCursor cur = cursorForPosition(viewport()->mapFromGlobal(mapToGlobal(e->pos())));
	cur.select(QTextCursor::LineUnderCursor);
	
	QString szTxt = cur.selectedText();
	
	tryToOpenFileInText(szTxt);
}

bool C3TextOutputWidget::goToFirstOccurenceOf(const QString &szText)
{
	QTextCursor c = document()->find(szText);
	if(c.isNull())
		return false;
	setTextCursor(c);
	ensureCursorVisible();
	return true;
}
