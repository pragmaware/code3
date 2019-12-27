//=============================================================================
//
//   File : C3OptionsWidget.cpp
//   Creation Date : ven 06 nov 2015 03:31:45
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

#include "C3OptionsWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QList>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>

#include "C3OptionEditor.h"
#include "C3OptionEditorBoolean.h"
#include "C3OptionEditorColor.h"
#include "C3OptionEditorString.h"
#include "C3OptionEditorGroup.h"
#include "C3OptionEditorInteger.h"
#include "C3OptionEditorFile.h"
#include "C3OptionEditorFont.h"
#include "C3OptionEditorText.h"
#include "C3OptionEditorScrollArea.h"
#include "C3OptionEditorStringList.h"

#include "C3OptionsWidgetDescriptor.h"

// Standard layout is 4 columns
// The simple editors will use 2 columns for label and 2 columns for editor

class C3OptionsWidgetPrivate
{
public:
	QGridLayout * pLayout;
	int iNextRow;
	
	QList<C3OptionEditor *> lEditors;
	
	QString szTitle;
	
	C3OptionsWidgetDescriptor * pOptionsWidgetDescriptor;
};

#define _p m_pC3OW

C3OptionsWidget::C3OptionsWidget(const QString &szTitle,QWidget * pParent)
	: QWidget(pParent)
{
	_p = new C3OptionsWidgetPrivate();

	_p->pOptionsWidgetDescriptor = NULL;

	_p->szTitle = szTitle;

	_p->iNextRow = 0;

	_p->pLayout = new QGridLayout(this);

	_p->pLayout->setMargin(0);
	_p->pLayout->setSpacing(3);
	
	_p->pLayout->setColumnStretch(2,200);
	_p->pLayout->setColumnStretch(1,100);
}

C3OptionsWidget::~C3OptionsWidget()
{
	qDeleteAll(_p->lEditors);
	delete _p;
}

const QString & C3OptionsWidget::title() const
{
	return _p->szTitle;
}

void C3OptionsWidget::setTitle(const QString &szTitle)
{
	if(_p->szTitle == szTitle)
		return;
	_p->szTitle = szTitle;
	emit titleChanged(this);
}


QGridLayout * C3OptionsWidget::gridLayout()
{
	return _p->pLayout;
}

int C3OptionsWidget::allocateRows(int iCount)
{
	int iRow = _p->iNextRow;
	_p->iNextRow += iCount;
	return iRow;
}

QScrollArea * C3OptionsWidget::addScrollArea(int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QScrollArea * l = new QScrollArea(this);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QLabel * C3OptionsWidget::addLabel(const QString &szText,int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QLabel * l = new QLabel(this);
	l->setText(szText);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QPushButton * C3OptionsWidget::addPushButton(const QString &szText,int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QPushButton * l = new QPushButton(this);
	l->setText(szText);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QLineEdit * C3OptionsWidget::addLineEdit(int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QLineEdit * l = new QLineEdit(this);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QTextEdit * C3OptionsWidget::addTextEdit(int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QTextEdit * l = new QTextEdit(this);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QGroupBox * C3OptionsWidget::addGroupBox(const QString &szLabel,int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QGroupBox * l = new QGroupBox(szLabel,this);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QCheckBox * C3OptionsWidget::addCheckBox(const QString &szLabel,int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QCheckBox * l = new QCheckBox(szLabel,this);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QComboBox * C3OptionsWidget::addComboBox(int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QComboBox * l = new QComboBox(this);
	l->setInsertPolicy(QComboBox::NoInsert);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

QListWidget * C3OptionsWidget::addListWidget(int iRow,int iCol,int iRowSpan,int iColSpan)
{
	QListWidget * l = new QListWidget(this);
	_p->pLayout->addWidget(l,iRow,iCol,iRowSpan,iColSpan);
	return l;
}

void C3OptionsWidget::addSpaceEater()
{
	_p->pLayout->setRowStretch(_p->iNextRow,1000);
	_p->iNextRow++;
}

void C3OptionsWidget::addOptionEditor(C3OptionEditor * pEditor)
{
	_p->lEditors.append(pEditor);
}

QLabel * C3OptionsWidget::addLabel(
		const QString &szText,
		unsigned int uOptions
	)
{
	int iRow = allocateRows(1);
	return addLabel(szText,iRow,(uOptions & AddLabelFullWidth) ? 0 : 1,1,(uOptions & AddLabelFullWidth) ? 2 : 1);
}

C3OptionEditorGroup * C3OptionsWidget::addGroup(
		const QString &szLabel,
		unsigned int uOptions
	)
{
	C3OptionEditorGroup * pGroup = new C3OptionEditorGroup(this);
	pGroup->create(szLabel,uOptions);
	addOptionEditor(pGroup);
	return pGroup;
}

C3OptionEditorScrollArea * C3OptionsWidget::addScrollArea(
		unsigned int uOptions
	)
{
	C3OptionEditorScrollArea * pGroup = new C3OptionEditorScrollArea(this);
	pGroup->create(uOptions);
	addOptionEditor(pGroup);
	return pGroup;
}

C3OptionEditorColor * C3OptionsWidget::addColorEditor(
		const QString &szLabel,
		QColor * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorColor * pEditor = new C3OptionEditorColor(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorFont * C3OptionsWidget::addFontEditor(
		const QString &szLabel,
		QFont * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorFont * pEditor = new C3OptionEditorFont(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorString * C3OptionsWidget::addStringEditor(
		const QString &szLabel,
		QString * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorString * pEditor = new C3OptionEditorString(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorInteger * C3OptionsWidget::addIntegerEditor(
		const QString &szLabel,
		int * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorInteger * pEditor = new C3OptionEditorInteger(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorStringList * C3OptionsWidget::addStringListEditor(
		const QString &szLabel,
		QStringList * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorStringList * pEditor = new C3OptionEditorStringList(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorText * C3OptionsWidget::addTextEditor(
		const QString &szLabel,
		QString * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorText * pEditor = new C3OptionEditorText(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorFile * C3OptionsWidget::addFileEditor(
		const QString &szLabel,
		QString * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorFile * pEditor = new C3OptionEditorFile(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

C3OptionEditorBoolean * C3OptionsWidget::addBooleanEditor(
		const QString &szLabel,
		bool * pOption,
		unsigned int uOptions
	)
{
	C3OptionEditorBoolean * pEditor = new C3OptionEditorBoolean(this);
	pEditor->create(szLabel,pOption,uOptions);
	addOptionEditor(pEditor);
	return pEditor;
}

bool C3OptionsWidget::commit()
{
	foreach(C3OptionEditor * pEditor,_p->lEditors)
	{
		if(!pEditor->commit())
			return false;
	}
	
	return true;
}

void C3OptionsWidget::setOptionsWidgetDescriptor(C3OptionsWidgetDescriptor * pDescriptor)
{
	_p->pOptionsWidgetDescriptor = pDescriptor;
}

C3OptionsWidgetDescriptor * C3OptionsWidget::optionsWidgetDescriptor()
{
	return _p->pOptionsWidgetDescriptor;
}

void C3OptionsWidget::showEvent(QShowEvent * e)
{
	emit aboutToShow();
	QWidget::showEvent(e);
}
