//=============================================================================
//
//   File : C3ImageEditor.cpp
//   Creation Date : mar 20 ott 2015 04:24:03
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

#include "C3ImageEditor.h"

#include <QGridLayout>
#include <QToolBar>
#include <QFileInfo>
#include <QVariant>
#include <QJsonObject>

#include "C3ImageEditorView.h"


class C3ImageEditorPrivate
{
public:
	QGridLayout * pLayout;

	QToolBar * pToolBar;
	
	C3ImageEditorView * pView;
	
	QString szInfoText;
};

#define _p m_pC3IE

C3ImageEditor::C3ImageEditor(C3EditorFactory * pFactory,QWidget * pParent)
	: C3Editor(pFactory,pParent,__tr("Image Editor"))
{
	_p = new C3ImageEditorPrivate();

	_p->pLayout = new QGridLayout(this);
	
	_p->pToolBar = new QToolBar(this);
	
	_p->pLayout->addWidget(_p->pToolBar,0,0);
	
	_p->pView = new C3ImageEditorView(this);
	
	_p->pLayout->addWidget(_p->pView,1,0);
	
	_p->pLayout->setMargin(0);
	_p->pLayout->setSpacing(0);
	_p->pLayout->setRowStretch(1,100);
}

C3ImageEditor::~C3ImageEditor()
{
	delete _p;
}

bool C3ImageEditor::saveState(QJsonObject &oObject)
{
	oObject.insert(__utf8("path"),path());
	//oObject.insert(__utf8("formatId"),format()->id());
	return true;
}

C3EditorFormat * C3ImageEditor::format()
{
	// FIXME
	return NULL;
}

const QString & C3ImageEditor::privateInfoText1() const
{
	return _p->szInfoText;
}

const QString & C3ImageEditor::privateInfoText2() const
{
	return _p->szInfoText;
}


bool C3ImageEditor::closeFile(bool bCanAbort)
{
	// FIXME
	return true;
}

bool C3ImageEditor::newFile()
{
	return true;
}

bool C3ImageEditor::saveInternal(
		const QString &szPath,
		C3EditorFormat * pFormat,
		QString &szError
	)
{
	szError = __tr("Not implemented");
	return false;
}

bool C3ImageEditor::open(const C3Link &oLink)
{
	if(!_p->pView->open(oLink))
		return false;

	setPath(oLink.path());
	setTitle(oLink.value("name").toString());

	return true;
}

