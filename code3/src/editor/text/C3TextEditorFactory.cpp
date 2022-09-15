//=============================================================================
//
//   File : C3TextEditorFactory.cpp
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

#include "C3TextEditorFactory.h"
#include "C3TextEditor.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorFormat8Bit.h"
#include "C3TextEditorModePlainText.h"
#include "C3TextEditorModeJavascript.h"
#include "C3TextEditorModeHTML.h"
#include "C3TextEditorModeChatScript.h"
#include "C3TextEditorModeCSS.h"
#include "C3TextEditorModePHP.h"
#include "C3TextEditorModeCPP.h"
#include "C3TextEditorModeSQL.h"
#include "C3TextEditorModeJava.h"
#include "C3TextEditorModeCMake.h"
#include "C3TextEditorModeBash.h"
#include "C3TextEditorModePython.h"
#include "C3TextEditorModeDart.h"
#include "C3TextEditorModeXML.h"
#include "C3TextEditorModeVHDL.h"
#include "C3TextEditorModeCPPSharedData.h"
#include "C3TextEditorModePHPSharedData.h"
#include "C3TextEditorModeASMSharedData.h"
#include "C3TextEditorModeVHDLSharedData.h"
#include "C3TextEditorModeJavaSharedData.h"
#include "C3TextEditorOptionsWidgetColors.h"
#include "C3TextEditorOptionsWidgetFonts.h"
#include "C3TextEditorModeASM.h"

#include "C3SettingsDialog.h"

#include <QFileInfo>
#include <QHash>
#include <QMenu>

C3TextEditorFactory * C3TextEditorFactory::m_pInstance = NULL;

class C3TextEditorFactoryPrivate
{
public:
	C3TextEditorOptions * pOptions;
	QHash<QString,int> oExtensionMap;
	QHash<QString,int> oFileNameMap;
	QHash<QString,QString> oModeByExtensionMap;
	QHash<QString,C3TextEditorModeDescriptor *> oModeMap;
	QMenu * pContextMenu;
	C3TextEditorModeCPPSharedData * pModeCPPSharedData;
	C3TextEditorModePHPSharedData * pModePHPSharedData;
	C3TextEditorModeASMSharedData * pModeASMSharedData;
	C3TextEditorModeVHDLSharedData * pModeVHDLSharedData;
	C3TextEditorModeJavaSharedData * pModeJavaSharedData;

};

#define _p m_pC3TEF

C3TextEditorFactory::C3TextEditorFactory()
	: C3EditorFactory(__ascii("text"),__tr("Text Editor"))
{
	Q_ASSERT(!m_pInstance);
	
	m_pInstance = this;

	_p = new C3TextEditorFactoryPrivate();
	_p->pOptions = NULL;
	_p->pContextMenu = NULL;
	_p->pModeCPPSharedData = NULL;
	_p->pModePHPSharedData = NULL;
	_p->pModeASMSharedData = NULL;
	_p->pModeJavaSharedData = NULL;
	_p->pModeVHDLSharedData = NULL;

	fillExtensionMap();
	fillFormatMap();
	fillModeMap();
	fillModeByExtensionMap();
}

C3TextEditorFactory::~C3TextEditorFactory()
{
	qDeleteAll(_p->oModeMap);
	if(_p->pOptions)
		delete _p->pOptions;
	if(_p->pContextMenu)
		delete _p->pContextMenu;
	if(_p->pModeCPPSharedData)
		delete _p->pModeCPPSharedData;
	if(_p->pModePHPSharedData)
		delete _p->pModePHPSharedData;
	if(_p->pModeJavaSharedData)
		delete _p->pModeJavaSharedData;
	if(_p->pModeVHDLSharedData)
		delete _p->pModeVHDLSharedData;
	if(_p->pModeASMSharedData)
		delete _p->pModeASMSharedData;
	delete _p;
	
	m_pInstance = NULL;
}

C3TextEditorModeVHDLSharedData * C3TextEditorFactory::modeVHDLSharedData()
{
	if(_p->pModeVHDLSharedData)
		return _p->pModeVHDLSharedData;
	
	_p->pModeVHDLSharedData = new C3TextEditorModeVHDLSharedData(this);
	_p->pModeVHDLSharedData->rebuildColorHashes();
	
	return _p->pModeVHDLSharedData;
}

C3TextEditorModeASMSharedData * C3TextEditorFactory::modeASMSharedData()
{
	if(_p->pModeASMSharedData)
		return _p->pModeASMSharedData;
	
	_p->pModeASMSharedData = new C3TextEditorModeASMSharedData(this);
	_p->pModeASMSharedData->rebuildColorHashes();
	
	return _p->pModeASMSharedData;
}

C3TextEditorModePHPSharedData * C3TextEditorFactory::modePHPSharedData()
{
	if(_p->pModePHPSharedData)
		return _p->pModePHPSharedData;
	
	_p->pModePHPSharedData = new C3TextEditorModePHPSharedData(this);
	_p->pModePHPSharedData->rebuildColorHashes();
	
	return _p->pModePHPSharedData;
}

C3TextEditorModeCPPSharedData * C3TextEditorFactory::modeCPPSharedData()
{
	if(_p->pModeCPPSharedData)
		return _p->pModeCPPSharedData;
	
	_p->pModeCPPSharedData = new C3TextEditorModeCPPSharedData(this);
	_p->pModeCPPSharedData->rebuildColorHashes();

	return _p->pModeCPPSharedData;
}

C3TextEditorModeJavaSharedData * C3TextEditorFactory::modeJavaSharedData()
{
	if(_p->pModeJavaSharedData)
		return _p->pModeJavaSharedData;
	
	_p->pModeJavaSharedData = new C3TextEditorModeJavaSharedData(this);
	_p->pModeJavaSharedData->rebuildColorHashes();

	return _p->pModeJavaSharedData;
}

C3TextEditorOptions * C3TextEditorFactory::options()
{
	if(!_p->pOptions)
	{
		_p->pOptions = new C3TextEditorOptions();
		_p->pOptions->load();
	}
	return _p->pOptions;
}

C3Editor * C3TextEditorFactory::createEditor(
		const C3Link &oLink,
		QWidget * pParent
	)
{
	C3TextEditor * pEditor = new C3TextEditor(this,pParent);

	if(oLink.path().isEmpty())
	{
		if(!pEditor->newFile())
		{
			delete pEditor;
			return NULL;
		}
	} else {
		if(!pEditor->open(oLink))
		{
			delete pEditor;
			return NULL;
		}
	}

	return pEditor;
}

void C3TextEditorFactory::getModeList(QList< QPair<QString,QString> > &lIdAndName)
{
	QHash<QString,C3TextEditorModeDescriptor *>::iterator i;

	lIdAndName.clear();

	for(i = _p->oModeMap.begin();i != _p->oModeMap.end();++i)
		lIdAndName.append(QPair<QString,QString>(i.key(),i.value()->szName));

	std::sort(
			lIdAndName.begin(),
			lIdAndName.end(),
			[](const QPair<QString,QString> & a, const QPair<QString,QString> & b) -> bool
			{ 
			    return a.first > b.first; 
			}
		);
}

C3TextEditorMode * C3TextEditorFactory::createMode(const QString &szId)
{
	C3TextEditorModeDescriptor * d = _p->oModeMap.value(szId,NULL);
	if(!d)
	{
		qDebug("WARNING: Could not create mode with id %s",szId.toUtf8().data());
		return new C3TextEditorModePlainText(this);
	}
	
	return d->fnCreate();
}


void C3TextEditorFactory::fillModeMap()
{
#define ADD_MODE(_szId,_szName,_szClassName) \
	do { \
		C3TextEditorModeDescriptor * d = new C3TextEditorModeDescriptor(); \
		d->szName = _szName; \
		d->fnCreate = [this](){ return new _szClassName(this); }; \
		_p->oModeMap.insert(_szId,d); \
	} while(0)
	
	ADD_MODE("plain","Plain Text",C3TextEditorModePlainText);
	ADD_MODE("asm","ASM",C3TextEditorModeASM);
	ADD_MODE("chatscript","ChatScript",C3TextEditorModeChatScript);
	ADD_MODE("cmake","CMake",C3TextEditorModeCMake);
	ADD_MODE("cpp","C/C++",C3TextEditorModeCPP);
	ADD_MODE("css","CSS",C3TextEditorModeCSS);
	ADD_MODE("dart","Dart",C3TextEditorModeDart);
	ADD_MODE("html","HTML",C3TextEditorModeHTML);
	ADD_MODE("java","Java",C3TextEditorModeJava);
	ADD_MODE("js","Javascript",C3TextEditorModeJavascript);
	ADD_MODE("php","PHP",C3TextEditorModePHP);
	ADD_MODE("sql","SQL",C3TextEditorModeSQL);
	ADD_MODE("bash","Bash",C3TextEditorModeBash);
	ADD_MODE("xml","XML",C3TextEditorModeXML);
	ADD_MODE("python","Python",C3TextEditorModePython);
	ADD_MODE("vhdl","VHDL",C3TextEditorModeVHDL);
}

void C3TextEditorFactory::fillModeByExtensionMap()
{
	_p->oModeByExtensionMap.insert("js","js");
	_p->oModeByExtensionMap.insert("json","js"); // FIXME?
	_p->oModeByExtensionMap.insert("dart","dart");
	_p->oModeByExtensionMap.insert("html","html");
	_p->oModeByExtensionMap.insert("xhtml","html");
	_p->oModeByExtensionMap.insert("htm","html");
	_p->oModeByExtensionMap.insert("txt","plain");
	_p->oModeByExtensionMap.insert("css","css");
	_p->oModeByExtensionMap.insert("less","css");
	_p->oModeByExtensionMap.insert("sass","css");
	_p->oModeByExtensionMap.insert("qss","css");
	_p->oModeByExtensionMap.insert("php","php");
	_p->oModeByExtensionMap.insert("php5","php");
	_p->oModeByExtensionMap.insert("c","cpp");
	_p->oModeByExtensionMap.insert("cc","cpp");
	_p->oModeByExtensionMap.insert("cu","cpp"); // CUDA
	_p->oModeByExtensionMap.insert("cpp","cpp");
	_p->oModeByExtensionMap.insert("cxx","cpp");
	_p->oModeByExtensionMap.insert("lxx","cpp"); // opencascade
	_p->oModeByExtensionMap.insert("h","cpp");
	_p->oModeByExtensionMap.insert("hh","cpp");
	_p->oModeByExtensionMap.insert("hpp","cpp");
	_p->oModeByExtensionMap.insert("hxx","cpp");
	_p->oModeByExtensionMap.insert("m","cpp"); // FIXME
	_p->oModeByExtensionMap.insert("mm","cpp"); // FIXME
	_p->oModeByExtensionMap.insert("cmake","cmake");
	_p->oModeByExtensionMap.insert("java","java");
	_p->oModeByExtensionMap.insert("sh","bash");
	_p->oModeByExtensionMap.insert("xml","xml");
	_p->oModeByExtensionMap.insert("xaml","xml");
	_p->oModeByExtensionMap.insert("sql","sql");
	_p->oModeByExtensionMap.insert("tsql","sql");
	_p->oModeByExtensionMap.insert("py","python");
	_p->oModeByExtensionMap.insert("py3","python");
	_p->oModeByExtensionMap.insert("pyw","python");
	_p->oModeByExtensionMap.insert("top","chatscript");
	_p->oModeByExtensionMap.insert("vhdl","vhdl");
	_p->oModeByExtensionMap.insert("vhd","vhdl");
	_p->oModeByExtensionMap.insert("po","bash"); // FIXME?
	_p->oModeByExtensionMap.insert("sh","bash");
	_p->oModeByExtensionMap.insert("asm","asm");
	_p->oModeByExtensionMap.insert("s","asm"); // FIXME! Not true! this is GAS syntax, which is different
}

QString C3TextEditorFactory::guessBestModeByExtension(const QString &szLowerCaseExt)
{
	return _p->oModeByExtensionMap.value(szLowerCaseExt);
}


void C3TextEditorFactory::fillExtensionMap()
{
	_p->oExtensionMap.clear();
	
	// Known binary files
	_p->oExtensionMap.insert("exe",CantOpen);
	_p->oExtensionMap.insert("bin",CantOpen);
	_p->oExtensionMap.insert("zip",CantOpen);
	_p->oExtensionMap.insert("gz",CantOpen);
	_p->oExtensionMap.insert("tgz",CantOpen);
	_p->oExtensionMap.insert("gz",CantOpen);
	_p->oExtensionMap.insert("bz2",CantOpen);
	_p->oExtensionMap.insert("xz",CantOpen);
	_p->oExtensionMap.insert("png",CantOpen);
	_p->oExtensionMap.insert("jpg",CantOpen);
	_p->oExtensionMap.insert("jpeg",CantOpen);
	_p->oExtensionMap.insert("png",CantOpen);
	_p->oExtensionMap.insert("gif",CantOpen);
	_p->oExtensionMap.insert("mp3",CantOpen);
	_p->oExtensionMap.insert("mp4",CantOpen);
	_p->oExtensionMap.insert("avi",CantOpen);
	_p->oExtensionMap.insert("mpeg",CantOpen);
	_p->oExtensionMap.insert("class",CantOpen);
	_p->oExtensionMap.insert("apk",CantOpen);
	_p->oExtensionMap.insert("rar",CantOpen);
	_p->oExtensionMap.insert("iso",CantOpen);
	_p->oExtensionMap.insert("ttf",CantOpen);
	_p->oExtensionMap.insert("pdf",CantOpen);
	_p->oExtensionMap.insert("odt",CantOpen);
	_p->oExtensionMap.insert("odp",CantOpen);
	_p->oExtensionMap.insert("ods",CantOpen);
	_p->oExtensionMap.insert("doc",CantOpen);
	_p->oExtensionMap.insert("xls",CantOpen);
	_p->oExtensionMap.insert("docx",CantOpen);
	_p->oExtensionMap.insert("xlsx",CantOpen);
	_p->oExtensionMap.insert("webm",CantOpen);
	_p->oExtensionMap.insert("mkv",CantOpen);
	_p->oExtensionMap.insert("ps",CantOpen);
	
	// Known text files
	_p->oExtensionMap.insert("txt",PerfectMatch);
	_p->oExtensionMap.insert("c",PerfectMatch);
	_p->oExtensionMap.insert("cc",PerfectMatch);
	_p->oExtensionMap.insert("cu",PerfectMatch); // cuda
	_p->oExtensionMap.insert("h",PerfectMatch);
	_p->oExtensionMap.insert("hh",PerfectMatch);
	_p->oExtensionMap.insert("m",PerfectMatch);
	_p->oExtensionMap.insert("mm",PerfectMatch);
	_p->oExtensionMap.insert("cpp",PerfectMatch);
	_p->oExtensionMap.insert("cxx",PerfectMatch);
	_p->oExtensionMap.insert("qss",PerfectMatch);
	_p->oExtensionMap.insert("hpp",PerfectMatch);
	_p->oExtensionMap.insert("hxx",PerfectMatch);
	_p->oExtensionMap.insert("js",PerfectMatch);
	_p->oExtensionMap.insert("json",PerfectMatch);
	_p->oExtensionMap.insert("java",PerfectMatch);
	_p->oExtensionMap.insert("html",PerfectMatch);
	_p->oExtensionMap.insert("xhtml",PerfectMatch);
	_p->oExtensionMap.insert("htm",PerfectMatch);
	_p->oExtensionMap.insert("css",PerfectMatch);
	_p->oExtensionMap.insert("less",PerfectMatch);
	_p->oExtensionMap.insert("sass",PerfectMatch);
	_p->oExtensionMap.insert("pl",PerfectMatch);
	_p->oExtensionMap.insert("php",PerfectMatch);
	_p->oExtensionMap.insert("php5",PerfectMatch);
	_p->oExtensionMap.insert("py",PerfectMatch);
	_p->oExtensionMap.insert("py3",PerfectMatch);
	_p->oExtensionMap.insert("pyw",PerfectMatch);
	_p->oExtensionMap.insert("conf",PerfectMatch);
	_p->oExtensionMap.insert("rc",PerfectMatch);
	_p->oExtensionMap.insert("ini",PerfectMatch);
	_p->oExtensionMap.insert("diff",PerfectMatch);
	_p->oExtensionMap.insert("patch",PerfectMatch);
	_p->oExtensionMap.insert("cmake",PerfectMatch);
	_p->oExtensionMap.insert("sh",PerfectMatch);
	_p->oExtensionMap.insert("xml",PerfectMatch);
	_p->oExtensionMap.insert("xaml",PerfectMatch);
	_p->oExtensionMap.insert("sql",PerfectMatch);
	_p->oExtensionMap.insert("top",PerfectMatch);
	_p->oExtensionMap.insert("vhdl",PerfectMatch);
	_p->oExtensionMap.insert("vhd",PerfectMatch);
	_p->oExtensionMap.insert("po",PerfectMatch);
	_p->oExtensionMap.insert("asm",PerfectMatch);
	_p->oExtensionMap.insert("s",PerfectMatch);
	_p->oExtensionMap.insert("S",PerfectMatch);
	
	_p->oFileNameMap.insert("readme",PerfectMatch);
	_p->oFileNameMap.insert("license",PerfectMatch);
	_p->oFileNameMap.insert("todo",PerfectMatch);
}

int C3TextEditorFactory::score(
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

	iVal = _p->oFileNameMap.value(inf.fileName().toLower(),-1);
	if(iVal >= 0)
		return iVal;

	QByteArray d = oLink.value("data").toByteArray();
	if(d.size() > 0)
	{
		if(d.startsWith("#!/"))
			return PerfectMatch; // shell script or something similar
		if(d.startsWith("<?xml"))
			return Appropriate; // likely to be xml
		if(d.startsWith("<?php"))
			return Appropriate; // likely to be php
		//if(d.startsWith("#if"))
		//	return PerfectMatch; // C/C++ probably
		//if(d.startsWith("//"))
		//	return PerfectMatch; // C/C++ probably
	}

	return CanOpen;
}

void C3TextEditorFactory::fillFormatMap()
{
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("utf8"),
					__tr("Plain text with UTF-8 encoding"),
					__utf8("UTF-8"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("utf8bom"),
					__tr("Plain text with UTF-8 encoding and byte order mark"),
					__utf8("UTF-8"),
					true
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-1"),
					__tr("Plain text with ISO8859-1 (Latin 1, Western Europe) encoding"),
					__utf8("ISO8859-1"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-2"),
					__tr("Plain text with ISO8859-2 (Latin 2, Eastern Europe) encoding"),
					__utf8("ISO8859-2"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-3"),
					__tr("Plain text with ISO8859-3 (Latin 3, Old Turkish, Maltese, Esperanto) encoding"),
					__utf8("ISO8859-3"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-4"),
					__tr("Plain text with ISO8859-4 (Latin 4, Estonian, Lettonan...) encoding"),
					__utf8("ISO8859-4"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-5"),
					__tr("Plain text with ISO8859-5 (Cyrillic) encoding"),
					__utf8("ISO8859-5"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-6"),
					__tr("Plain text with ISO8859-6 (Arabic) encoding"),
					__utf8("ISO8859-6"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-7"),
					__tr("Plain text with ISO8859-7 (Greek) encoding"),
					__utf8("ISO8859-7"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-8"),
					__tr("Plain text with ISO8859-8 (Hebrew) encoding"),
					__utf8("ISO8859-8"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-9"),
					__tr("Plain text with ISO8859-9 (New Turkish) encoding"),
					__utf8("ISO8859-9"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-10"),
					__tr("Plain text with ISO8859-10 (Nordic) encoding"),
					__utf8("ISO8859-10"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-13"),
					__tr("Plain text with ISO8859-13 (Baltic) encoding"),
					__utf8("ISO8859-13"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-14"),
					__tr("Plain text with ISO8859-14 (Celtic) encoding"),
					__utf8("ISO8859-14"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-15"),
					__tr("Plain text with ISO8859-15 (Latin 1 with € sign) encoding"),
					__utf8("ISO8859-15"),
					false
				)
		);
	/*
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("iso8859-16"),
					__tr("Plain text with ISO8859-16 (Nordic with € sign) encoding"),
					__utf8("ISO8859-16"),
					false
				)
		);
	*/
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("koi8r"),
					__tr("Plain text with KOI8-R (Russian) encoding"),
					__utf8("KOI8-R"),
					false
				)
		);
	registerFormat(
			new C3TextEditorFormat8Bit(
					__utf8("koi8u"),
					__tr("Plain text with KOI8-U (Ukrainian) encoding"),
					__utf8("KOI8-U"),
					false
				)
		);
}

QMenu * C3TextEditorFactory::sharedContextMenu()
{
	if(!_p->pContextMenu)
		_p->pContextMenu = new QMenu();
	return _p->pContextMenu;
}

void C3TextEditorFactory::saveSettings()
{
	if(_p->pOptions)
		_p->pOptions->save();
}

void C3TextEditorFactory::addWidgetsToSettingsDialog(C3SettingsDialog * pDialog)
{
	C3TextEditorOptions * pOptions = _p->pOptions;

	C3OptionsWidgetDescriptor * pParent = pDialog->findDescriptor(__ascii("TextEditorOptions"));

	pDialog->addWidget(
			pParent,
			__tr("Colors"),
			[pOptions](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3TextEditorOptionsWidgetColors(szLabel,pParent,pOptions);
			}
		);

	pDialog->addWidget(
			pParent,
			__tr("Fonts"),
			[pOptions](const QString &szLabel,QWidget * pParent) -> C3OptionsWidget *
			{
				return new C3TextEditorOptionsWidgetFonts(szLabel,pParent,pOptions);
			}
		);
}

