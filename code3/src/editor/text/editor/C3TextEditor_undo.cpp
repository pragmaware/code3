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
#include "C3TextEditorUndo.h"

#define MAX_UNDO_ENTRIES 1024

bool C3TextEditor::undoAvailable()
{
	return !_p->lUndoStack.isEmpty();
}

void C3TextEditor::pushUndo(C3TextEditorUndo * pUndo)
{
	if(_p->pMultiUndo)
	{
		_p->pMultiUndo->lChildren.append(pUndo);
		return;
	}

	_p->lUndoStack.append(pUndo);
	if(_p->lUndoStack.count() > MAX_UNDO_ENTRIES)
		delete _p->lUndoStack.takeFirst();
}

C3TextEditorUndo * C3TextEditor::popUndo()
{
	Q_ASSERT(!_p->pMultiUndo);

	if(_p->lUndoStack.isEmpty())
		return NULL;
	return _p->lUndoStack.takeLast();
}

void C3TextEditor::beginMultiUndo()
{
	// FIXME: This should be recursive! It should stash any pending multiundo in a stack, do this multiundo and 
	//        at the end either unstash a multiundo or push directly
	//        See how CcTextEdit does it!

	Q_ASSERT(!_p->pMultiUndo);
	_p->pMultiUndo = new C3TextEditorUndo(C3TextEditorUndo::Container);
}

void C3TextEditor::endMultiUndo()
{
	Q_ASSERT(_p->pMultiUndo);
	Q_ASSERT(_p->pMultiUndo->eType == C3TextEditorUndo::Container);

	int cnt = _p->pMultiUndo->lChildren.count();

	if(cnt < 1)
	{
		// empty multi undo
		delete _p->pMultiUndo;
		_p->pMultiUndo = NULL;
		return;
	}

	if(cnt == 1)
	{
		_p->lUndoStack.append(_p->pMultiUndo->lChildren.takeFirst());
		delete _p->pMultiUndo;
	} else {
		_p->lUndoStack.append(_p->pMultiUndo);
	}

	_p->pMultiUndo = NULL;

	if(_p->lUndoStack.count() > MAX_UNDO_ENTRIES)
		delete _p->lUndoStack.takeFirst();
}

void C3TextEditor::undoInternal(C3TextEditorUndo * pUndo)
{
	switch(pUndo->eType)
	{
		case C3TextEditorUndo::Insert:
			deleteRangeInternal(pUndo->oRange,false);
		break;
		case C3TextEditorUndo::Delete:
			cursorMoveTo(pUndo->oRange.start,false);
			insertTextInternal(pUndo->szText,false);
		break;
		case C3TextEditorUndo::Container:
		{
			while(!pUndo->lChildren.isEmpty())
			{
				C3TextEditorUndo * pChildUndo = pUndo->lChildren.takeLast();
				undoInternal(pChildUndo);
				delete pChildUndo;
			}
		}
		break;
		default:
			Q_ASSERT(false);
		break;
	}
}


void C3TextEditor::undo()
{
	if(_p->lUndoStack.isEmpty())
		return;
	
	C3TextEditorUndo * pUndo = popUndo();
	Q_ASSERT(pUndo);

	beginVisualStateChange();

	undoInternal(pUndo);

	endVisualStateChange();

	delete pUndo;
}
