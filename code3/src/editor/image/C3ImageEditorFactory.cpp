//=============================================================================
//
//   File : C3ImageEditorFactory.cpp
//   Creation Date : mar 20 ott 2015 03:45:33
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

#include "C3ImageEditorFactory.h"

#include "C3ImageEditor.h"

#include <QFileInfo>
#include <QHash>

class C3ImageEditorFactoryPrivate
{
public:
	QHash<QString,int> oExtensionMap;
};

#define _p m_pC3TEF

C3ImageEditorFactory::C3ImageEditorFactory()
	: C3EditorFactory(__utf8("image"),__tr("Image Editor"))
{
	_p = new C3ImageEditorFactoryPrivate();

	fillExtensionMap();
}

C3ImageEditorFactory::~C3ImageEditorFactory()
{
	delete _p;
}

C3Editor * C3ImageEditorFactory::createEditor(
		const C3Link &oLink,
		QWidget * pParent
	)
{
	C3ImageEditor * pEditor = new C3ImageEditor(this,pParent);

	if(!pEditor->open(oLink))
	{
		delete pEditor;
		return NULL;
	}

	return pEditor;
}

void C3ImageEditorFactory::fillExtensionMap()
{
	// Known image files
	_p->oExtensionMap.insert("png",PerfectMatch);
	_p->oExtensionMap.insert("gif",PerfectMatch);
	_p->oExtensionMap.insert("jpeg",PerfectMatch);
	_p->oExtensionMap.insert("jpg",PerfectMatch);
	_p->oExtensionMap.insert("tif",PerfectMatch);
	_p->oExtensionMap.insert("tiff",PerfectMatch);
	_p->oExtensionMap.insert("bmp",PerfectMatch);
	_p->oExtensionMap.insert("dds",PerfectMatch);
	_p->oExtensionMap.insert("tga",PerfectMatch);
	_p->oExtensionMap.insert("webp",PerfectMatch);
	_p->oExtensionMap.insert("icns",PerfectMatch);
	_p->oExtensionMap.insert("jp2",PerfectMatch);
	_p->oExtensionMap.insert("mng",PerfectMatch);
}

int C3ImageEditorFactory::score(
		const C3Link &oLink,
		const QFileInfo &inf
	)
{
	if(inf.isDir())
		return CantOpen;

	int iVal;

	QString szExt = inf.suffix().toLower();
	if(!szExt.isEmpty())
	{
		iVal = _p->oExtensionMap.value(szExt,-1);
		if(iVal >= 0)
			return iVal;
	}

	// Fixme, could look at contents too!
	return CantOpen;
}
