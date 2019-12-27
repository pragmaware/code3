//=============================================================================
//
//   File : C3NewJavaClassDialog.cpp
//   Creation Date : mer 11 nov 2015 00:53:05
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

#include "C3NewJavaClassDialog.h"
#include "C3NewJavaClassSettings.h"
#include "C3NewJavaClassGenerator.h"
#include "C3OptionsWidget.h"
#include "C3OptionEditorString.h"
#include "C3OptionEditorBoolean.h"
#include "C3OptionEditorGroup.h"
#include "C3OptionEditorFile.h"
#include "C3OptionEditorText.h"
#include "C3OptionEditorEnumeration.h"
#include "C3DockFileBrowser.h"
#include "C3NewObjectUtils.h"

#include "C3Workspace.h"
#include "C3Editor.h"
#include "C3MainWindow.h"

#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QShowEvent>
#include <QFileInfo>

class C3NewJavaClassDialogPrivate
{
public:
	C3NewJavaClassSettings oSettings;

	C3OptionsWidget * pGeneral;

	C3OptionEditorString * pPackage;
	
	C3OptionEditorString * pClassName;
	C3OptionEditorEnumeration<C3NewJavaClassSettings::ClassType> * pClassType;

	C3OptionEditorString * pBaseClassName;

	C3OptionEditorFile * pJavaFilePath;

	C3OptionEditorGroup * pOptionsGroup;

	C3OptionsWidget * pJavaFileFormat;
	C3OptionEditorText * pJavaFileHeader;
	C3OptionEditorText * pJavaFilePreamble;
	C3OptionEditorText * pJavaFileTrailer;

	C3OptionsWidget * pJavaFilePreview;
	QString szJavaFilePreview;
	C3OptionEditorText * pJavaFilePreviewText;

};

#define _p m_pC3NJavaCD

C3NewJavaClassDialog::C3NewJavaClassDialog()
	: C3OptionsDialog(__tr("New Java Class"))
{
	C3Workspace * pWorkspace = C3Workspace::currentWorkspace();

	_p = new C3NewJavaClassDialogPrivate();

	// Set defaults and options that are stored.

	_p->oSettings.eClassType = C3NewJavaClassSettings::StandardClass;

	_p->oSettings.szJavaFileHeader = C3NewObjectUtils::buildFileHeader(
			QString(),
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);

	_p->oSettings.szJavaFilePreamble = pWorkspace->genericSetting("new.creator.java.szJavaFilePreamble").toString();
	_p->oSettings.szJavaFileTrailer = pWorkspace->genericSetting("new.creator.java.szJavaFileTrailer").toString();

	_p->oSettings.szPackage = guessPackage();

	_p->pGeneral = new C3OptionsWidget(__tr("General"),parentForWidgets());

	_p->pPackage = _p->pGeneral->addStringEditor(__tr("Package"),&(_p->oSettings.szPackage));
	QObject::connect(_p->pPackage,SIGNAL(edited()),this,SLOT(slotPackageEdited()));

	_p->pClassName = _p->pGeneral->addStringEditor(__tr("Class Name"),&(_p->oSettings.szClassName));
	QObject::connect(_p->pClassName,SIGNAL(edited()),this,SLOT(slotClassNameEdited()));


	QList< QPair<int,QString> > lOptions;
	lOptions.append(QPair<int,QString>(C3NewJavaClassSettings::Interface,__tr("Interface")));
	lOptions.append(QPair<int,QString>(C3NewJavaClassSettings::StandardClass,__tr("Standard Class")));
	lOptions.append(QPair<int,QString>(C3NewJavaClassSettings::SingletonWithInitDoneFunctions,__tr("Singleton with Public init() / done() Functions")));
	lOptions.append(QPair<int,QString>(C3NewJavaClassSettings::SingletonWithUserControlledInstantiation,__tr("Singleton with User Controlled Instantiation")));
	lOptions.append(QPair<int,QString>(C3NewJavaClassSettings::SingletonWithLazyInstantiation,__tr("Singleton with Lazy Instantiation")));

	_p->pClassType = _p->pGeneral->addEnumerationEditor<C3NewJavaClassSettings::ClassType>(
			__tr("Class Type"),
			&(_p->oSettings.eClassType),
			lOptions
		);
	QObject::connect(_p->pClassType,SIGNAL(edited()),this,SLOT(slotClassTypeEdited()));


	_p->pBaseClassName = _p->pGeneral->addStringEditor(__tr("Base Class"),&(_p->oSettings.szBaseClassName));
	QObject::connect(_p->pBaseClassName,SIGNAL(edited()),this,SLOT(slotBaseClassNameEdited()));

	_p->pJavaFilePath = _p->pGeneral->addFileEditor(__tr("Java File"),&(_p->oSettings.szJavaFilePath));
	QObject::connect(_p->pJavaFilePath,SIGNAL(edited()),this,SLOT(slotJavaFilePathEdited()));

	_p->pOptionsGroup = _p->pGeneral->addGroup(__tr("Options"));

	_p->pGeneral->addSpaceEater();

	addWidget(_p->pGeneral);


	_p->pJavaFileFormat = new C3OptionsWidget(__tr("Java File Format"),parentForWidgets());

	_p->pJavaFileHeader = _p->pJavaFileFormat->addTextEditor(__tr("Header (top of the file)"),&(_p->oSettings.szJavaFileHeader));
	_p->pJavaFilePreamble = _p->pJavaFileFormat->addTextEditor(__tr("Preamble (before implementation)"),&(_p->oSettings.szJavaFilePreamble));
	_p->pJavaFileTrailer = _p->pJavaFileFormat->addTextEditor(__tr("Trailer (after implementation)"),&(_p->oSettings.szJavaFileTrailer));

	addWidget(_p->pJavaFileFormat);


	_p->pJavaFilePreview = new C3OptionsWidget(__tr("Java File Preview"),parentForWidgets());
	QObject::connect(_p->pJavaFilePreview,SIGNAL(aboutToShow()),this,SLOT(slotJavaFilePreviewAboutToShow()));

	_p->pJavaFilePreviewText = _p->pJavaFilePreview->addTextEditor(__tr("Preview of Contents"),&(_p->szJavaFilePreview),C3OptionEditorText::ReadOnly);

	addWidget(_p->pJavaFilePreview);


	setMinimumSize(QSize(450,450));

	enableDisableButtons();
}

C3NewJavaClassDialog::~C3NewJavaClassDialog()
{
	delete _p;
}

void C3NewJavaClassDialog::showEvent(QShowEvent * e)
{
	C3OptionsDialog::showEvent(e);
	
	_p->pClassName->lineEdit()->setFocus();
}

QString C3NewJavaClassDialog::guessPackage()
{
	C3MainWindow * pMainWindow = C3MainWindow::instance();

	QString szDir = pMainWindow->currentDirectory();

	if(!C3Workspace::currentWorkspace()->isDefault())
	{
		QFileInfo p1(szDir);
		szDir = p1.canonicalFilePath();
		
		QFileInfo p2(C3Workspace::currentWorkspace()->path());
		QString szDir2 = p2.canonicalFilePath();
	
		if(szDir.startsWith(szDir2))
			szDir = szDir.mid(szDir2.length());
	}

	szDir = szDir.replace("/",".");
	szDir = szDir.replace("\\",".");
	szDir = szDir.replace("..",".");

	if(szDir.startsWith("."))
		szDir = szDir.mid(1);
	if(szDir.endsWith("."))
		szDir = szDir.left(szDir.length() - 1);

	if(szDir.startsWith("src."))
		szDir = szDir.mid(4);

	int i1 = szDir.indexOf(".com.");
	int i2 = szDir.indexOf(".net.");
	int i3 = szDir.indexOf(".it.");
	int i4 = szDir.indexOf(".org.");
	int i5 = szDir.indexOf(".pl.");

	int idx = 99999;
	if((i1 >= 0) && (i1 < idx))
		idx = i1;
	if((i2 >= 0) && (i2 < idx))
		idx = i2;
	if((i3 >= 0) && (i3 < idx))
		idx = i3;
	if((i4 >= 0) && (i4 < idx))
		idx = i4;
	if((i5 >= 0) && (i5 < idx))
		idx = i5;

	if(idx < 99999)
		szDir = szDir.mid(idx+1);

	return szDir;
}

bool C3NewJavaClassDialog::isValid()
{
	if(_p->pClassName->currentValue().isEmpty())
		return false;
	QString szJava = _p->pJavaFilePath->currentValue();
	if(szJava.isEmpty())
		return false;

	return true;
}

void C3NewJavaClassDialog::updateJavaFileHeader()
{
	QString szFilePath = _p->pJavaFilePath->currentValue();

	QString szHeader = C3NewObjectUtils::buildFileHeader(
			szFilePath,
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);

	_p->pJavaFileHeader->setCurrentValue(szHeader);
}

void C3NewJavaClassDialog::enableDisableButtons()
{
	OKButton()->setEnabled(isValid());
}

void C3NewJavaClassDialog::slotJavaFilePathEdited()
{
	updateJavaFileHeader();
}

void C3NewJavaClassDialog::slotClassNameEdited()
{
	C3MainWindow * pMainWindow = C3MainWindow::instance();
	
	QString szClass = _p->pClassName->currentValue();

	int idx = szClass.indexOf(QChar('<'));
	if(idx >= 0)
		szClass = szClass.left(idx);

	szClass.replace(QRegExp("[^A-Za-z0-9_]"),"_");

	if(szClass.isEmpty())
	{
		_p->pJavaFilePath->setCurrentValue(QString());

		updateJavaFileHeader();
		enableDisableButtons();
		return;
	}

	QString szDir = pMainWindow->currentDirectory();

	QString szJava = szDir;
	if(!szJava.endsWith(QDir::separator()))
		szJava.append(QDir::separator());
	szJava.append(szClass);
	szJava.append(__utf8(".java"));

	_p->pJavaFilePath->setCurrentValue(szJava);
	
	if(idx >= 0)
	{
		_p->pClassType->setCurrentValue(C3NewJavaClassSettings::StandardClass);
		enableDisableControlsRelatedToClassType();
		_p->pClassType->setEnabled(false);
	} else {
		_p->pClassType->setEnabled(true);
	}

	updateJavaFileHeader();
	enableDisableButtons();
}

void C3NewJavaClassDialog::slotPackageEdited()
{
}


void C3NewJavaClassDialog::slotBaseClassNameEdited()
{
	enableDisableButtons();
}

void C3NewJavaClassDialog::enableDisableControlsRelatedToClassType()
{

}


void C3NewJavaClassDialog::slotClassTypeEdited()
{
	enableDisableControlsRelatedToClassType();
}

void C3NewJavaClassDialog::slotJavaFilePreviewAboutToShow()
{
	if((!isValid()) || (!C3OptionsDialog::commit()))
	{
		_p->pJavaFilePreviewText->setCurrentValue(__tr("Invalid Settings"));
		return;
	}

	C3NewJavaClassGenerator g(&(_p->oSettings));

	_p->pJavaFilePreviewText->setCurrentValue(g.generateJava());
}


bool C3NewJavaClassDialog::commit()
{
	if(!isValid())
	{
		qDebug("Commit failed: invalid settings!");
		return false;
	}

	if(!C3OptionsDialog::commit())
	{
		qDebug("Commit failed!");
		return false;
	}

	C3Workspace * pWorkspace = C3Workspace::currentWorkspace();

	C3NewJavaClassGenerator g(&(_p->oSettings));

	if(QFile::exists(_p->oSettings.szJavaFilePath))
	{
		QMessageBox::StandardButton eButton = QMessageBox::question(
				C3MainWindow::instance(),
				__tr("File Exists"),
				__tr("The file %1 exists: do you want to overwrite it?").arg(_p->oSettings.szJavaFilePath),
				QMessageBox::Yes | QMessageBox::No
			);
	
		if(eButton != QMessageBox::Yes)
			return false;
	}

	C3Editor * pJavaFileEditor = NULL;

	pJavaFileEditor = C3Workspace::currentWorkspace()->findEditor("text",_p->oSettings.szJavaFilePath);
	if(pJavaFileEditor)
	{
		QMessageBox::StandardButton eButton = QMessageBox::question(
				C3MainWindow::instance(),
				__tr("File Being Edited"),
				__tr("The file %1 is being edited. The editor will be closed and file contents overwritten. Do you want to continue?").arg(_p->oSettings.szJavaFilePath),
				QMessageBox::Yes | QMessageBox::No
			);
	
		if(eButton != QMessageBox::Yes)
			return false;
	}
	
	if(pJavaFileEditor)
		pWorkspace->closeEditor(pJavaFileEditor,false);

	QByteArray oData;
	
	QString szJavaContents = g.generateJava();

	QFile f1(_p->oSettings.szJavaFilePath);
	if(!f1.open(QFile::WriteOnly))
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				C3MainWindow::instance(),
				__tr("Can't Open File"),
				__tr("Failed to create the file %1: %2").arg(_p->oSettings.szJavaFilePath).arg(f1.error()),
				QMessageBox::Ok
			);
		
		return false;
	}

	oData = szJavaContents.toUtf8();

	if(f1.write(oData) != oData.size())
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				C3MainWindow::instance(),
				__tr("Error Writing File"),
				__tr("Failed to write to the file %1: %2").arg(_p->oSettings.szJavaFilePath).arg(f1.error()),
				QMessageBox::Ok
			);
		
		return false;
	}

	f1.close();

	pWorkspace->openFileAsync(_p->oSettings.szJavaFilePath);

	pWorkspace->setGenericSetting("new.creator.java.szJavaFilePreamble",QVariant(_p->oSettings.szJavaFilePreamble));
	pWorkspace->setGenericSetting("new.creator.java.szJavaFileTrailer",QVariant(_p->oSettings.szJavaFileTrailer));

	if(C3DockFileBrowser::instance())
		C3DockFileBrowser::instance()->reload();

	pWorkspace->notifyRescanNeeded();

	return true;
}
