#ifndef _C3TextEditorUndo_h_
#define _C3TextEditorUndo_h_
//=============================================================================
//
//   File : C3TextEditorUndo.h
//   Creation Date : dom 01 nov 2015 05:28:30
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

#include "C3TextEditorRange.h"

#include <QString>
#include <QLinkedList>

#include <qalgorithms.h>

///
/// \class C3TextEditorUndo
/// \brief The C3TextEditorUndo class
///
/// This class...
///
class C3TextEditorUndo
{
public:
	enum Type
	{
		///
		/// Insert type undo.
		/// szText is empty/ignored
		/// oRange is the range at that the text was inserted.
		///
		Insert,
		///
		/// Delete type undo.
		/// szText is the text that was deleted.
		/// oRange is the range that was deleted.
		///
		Delete,
		///
		/// Container type undo.
		/// Only lChildren members are valid.
		///
		Container
	};

	C3TextEditorRange oRange;
	QString szText;
	Type eType;
	QLinkedList<C3TextEditorUndo *> lChildren;
public:

	C3TextEditorUndo(Type eUndoType,const C3TextEditorRange &oUndoRange,const QString &szUndoText)
		: oRange(oUndoRange), szText(szUndoText), eType(eUndoType)
	{
	}

	C3TextEditorUndo(Type eUndoType)
		: eType(eUndoType)
	{
	}
	
	~C3TextEditorUndo()
	{
		if(!lChildren.isEmpty())
			qDeleteAll(lChildren);
	}

}; // class C3TextEditorUndo

#endif //!_C3TextEditorUndo_h_
