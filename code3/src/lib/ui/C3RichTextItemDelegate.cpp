//=============================================================================
//
//   File : C3RichTextItemDelegate.cpp
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

#include "C3RichTextItemDelegate.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QTextDocument>
#include <QApplication>
#include <QAbstractTextDocumentLayout>

#define MARGIN 4

C3RichTextItemDelegate::C3RichTextItemDelegate(QObject * pParent)
	: QStyledItemDelegate(pParent)
{
}

C3RichTextItemDelegate::~C3RichTextItemDelegate()
{
}

void C3RichTextItemDelegate::paint(QPainter* painter,const QStyleOptionViewItem & option,const QModelIndex &index) const
{
	QStyleOptionViewItemV4 optionV4 = option;
	initStyleOption(&optionV4, index);

	QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

	QTextDocument doc;
	doc.setHtml(optionV4.text);
	doc.setTextWidth(optionV4.rect.width());

	/// Painting item without text
	optionV4.text = QString();

	// Force a color that doesn't crash with styled items, close to the background
	optionV4.palette.setColor(QPalette::Highlight,QColor(50,0,0));
	//optionV4.palette.setColor(QPalette::Background,QColor(80,0,0));
	//optionV4.palette.setColor(QPalette::Base,QColor(80,0,0));

	style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette = optionV4.palette;


	// Highlighting text if item is selected
	if (optionV4.state & QStyle::State_Selected)
		ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

	//QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);

	//qDebug("RECT(%d,%d,%d,%d) SUB(%d,%d,%d,%d)",optionV4.rect.x(),optionV4.rect.y(),optionV4.rect.width(),optionV4.rect.height(),
	//	textRect.x(),textRect.y(),textRect.width(),textRect.height());

	painter->save();
	painter->translate(optionV4.rect.x() + MARGIN,optionV4.rect.y() + MARGIN);
	painter->setClipRect(0,0,optionV4.rect.width(),optionV4.rect.height());
	doc.documentLayout()->draw(painter, ctx);
	painter->restore();
}

QSize C3RichTextItemDelegate::sizeHint(const QStyleOptionViewItem & option,const QModelIndex & index) const
{
	QStyleOptionViewItemV4 optionV4 = option;
	initStyleOption(&optionV4, index);

	QTextDocument doc;
	doc.setHtml(optionV4.text);
	doc.setTextWidth(optionV4.rect.width());
	return QSize(doc.idealWidth() + MARGIN + MARGIN, doc.size().height() + MARGIN + MARGIN);
}
