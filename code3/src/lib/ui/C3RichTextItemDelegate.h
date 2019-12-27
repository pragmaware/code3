#ifndef _C3RichTextItemDelegate_h_
#define _C3RichTextItemDelegate_h_
//=============================================================================
//
//   File : C3RichTextItemDelegate.h
//   Creation Date : 2015/11/25 18:35:35
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

#include <QStyledItemDelegate>

class C3RichTextItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	C3RichTextItemDelegate(QObject * pParent = NULL);
	virtual ~C3RichTextItemDelegate();

public:
	virtual void paint(QPainter * painter,const QStyleOptionViewItem & option,const QModelIndex & index) const;
	QSize sizeHint(const QStyleOptionViewItem & option,const QModelIndex & index) const;

}; // class C3RichTextItemDelegate

#endif //!_C3RichTextItemDelegate_h_