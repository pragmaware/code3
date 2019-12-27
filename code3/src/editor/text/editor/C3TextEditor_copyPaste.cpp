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
#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorMargin.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorViewport.h"

#include <QClipboard>
#include <QApplication>

void C3TextEditor::copy()
{
	QString szSelection = getSelectionText();

	//qDebug("SELECTION (%s)",szSelection.toUtf8().data());

	QClipboard * pClipboard = QApplication::clipboard();

	pClipboard->setText(szSelection,QClipboard::Clipboard);
	if(pClipboard->supportsSelection())
		pClipboard->setText(szSelection,QClipboard::Selection);
	if(pClipboard->supportsFindBuffer())
		pClipboard->setText(szSelection,QClipboard::FindBuffer);
}

void C3TextEditor::cut()
{
	copy();
	deleteRange(_p->oSelection);
}

void C3TextEditor::paste()
{
	QClipboard * pClipboard = QApplication::clipboard();

	QString szData = pClipboard->text(QClipboard::Clipboard);
	if(szData.isEmpty() && pClipboard->supportsSelection())
	{
		szData = pClipboard->text(QClipboard::Selection);
		if(szData.isEmpty() && pClipboard->supportsFindBuffer())
			szData = pClipboard->text(QClipboard::FindBuffer);
	}
	
	insertText(szData);
}



