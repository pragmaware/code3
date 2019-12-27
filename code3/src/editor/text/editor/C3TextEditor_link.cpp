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

#include "C3TextEditorViewport.h"
#include "C3TextEditorLine.h"
#include "C3TextEditorMode.h"
#include "C3LinkSelectionDialog.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorFormat.h"
#include "C3Workspace.h"

#include <QHash>

bool C3TextEditor::jumpToLinkAtCursor(unsigned int uFlags)
{
	return triggerSymbolIdentificationInternal(_p->oCursor,true,uFlags);
}

bool C3TextEditor::createLink(C3Link &link)
{
	link.setPath(path());
	link.setEditorFactoryId(factory()->id());
	link.setTextEditorFormatId(_p->pFormat->id());
	link.setTextEditorModeId(_p->pMode->id());
	if(_p->oCursor.col > 0)
		link.setLocation(__utf8("%1:%2").arg(_p->oCursor.row+1).arg(_p->oCursor.col));
	else
		link.setLocation(__utf8("%1").arg(_p->oCursor.row+1));
	return true;
}
