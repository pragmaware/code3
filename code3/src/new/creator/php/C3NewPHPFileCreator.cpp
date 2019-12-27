//=============================================================================
//
//   File : C3NewPHPFileCreator.cpp
//   Creation Date : 2016/02/08 03:27:42
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

#include "C3NewPHPFileCreator.h"
#include "C3DockFileBrowser.h"

#include "C3Workspace.h"
#include "C3TextEditor.h"
#include "C3NewObjectUtils.h"

C3NewPHPFileCreator::C3NewPHPFileCreator()
	: C3NewObjectCreator()
{
}

C3NewPHPFileCreator::~C3NewPHPFileCreator()
{
}


QString C3NewPHPFileCreator::name()
{
	return __tr("PHP File");
}

QString C3NewPHPFileCreator::icon()
{
	return __utf8("new-php.png");
}

void C3NewPHPFileCreator::run()
{
	C3Link oLink;
	oLink.setEditorFactoryId("text");

	C3Editor * pEditor = C3Workspace::currentWorkspace()->newFile(oLink);
	if(!pEditor)
		return; // error already reported
	
	C3TextEditor * pEd = dynamic_cast<C3TextEditor *>(pEditor);
	if(!pEd)
		return; // aargh
	
	pEd->setMode(__ascii("php"));

	QString szHeader = C3NewObjectUtils::buildFileHeader(
			QString(),
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);

	pEd->insertText(
			__ascii(
					"<?php\n"
					"%1"
					"\n"
					"\n"
					"?>"
				).arg(szHeader)
		);

	pEd->saveAs();

	if(C3DockFileBrowser::instance())
		C3DockFileBrowser::instance()->reload();
}


