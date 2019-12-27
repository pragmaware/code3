//=============================================================================
//
//   File : C3NewCPPClassDialog.cpp
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

#include "C3NewCPPClassDialog.h"
#include "C3NewCPPClassSettings.h"
#include "C3NewCPPClassGenerator.h"
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

class C3NewCPPClassDialogPrivate
{
public:
	C3NewCPPClassSettings oSettings;

	C3OptionsWidget * pGeneral;
	
	C3OptionEditorString * pClassName;
	C3OptionEditorEnumeration<C3NewCPPClassSettings::ClassType> * pClassType;

	C3OptionEditorGroup * pBaseClassGroup;
	C3OptionEditorString * pBaseClassNames;
	C3OptionEditorString * pBaseClassIncludeFile;
	C3OptionEditorString * pConstructorArguments;

	C3OptionEditorGroup * pFilesGroup;
	C3OptionEditorFile * pCPPFilePath;
	C3OptionEditorFile * pHFilePath;

	C3OptionEditorGroup * pOptionsGroup;
	C3OptionEditorBoolean * pBaseClassIsQObject;
	C3OptionEditorBoolean * pHasVirtualFunctions;
	C3OptionEditorBoolean * pHideMembersViaPrivateClass;

	C3OptionEditorBoolean * pGenerateCopyConstructor;
	C3OptionEditorBoolean * pGenerateMoveConstructor;
	C3OptionEditorBoolean * pGenerateAssignmentOperator;
	
	C3OptionEditorString * pAPIDeclarator;

	C3OptionsWidget * pCPPFileFormat;
	C3OptionEditorText * pCPPFileHeader;
	C3OptionEditorText * pCPPFilePreamble;
	C3OptionEditorText * pCPPFileTrailer;

	C3OptionsWidget * pHFileFormat;
	C3OptionEditorText * pHFileHeader;
	C3OptionEditorText * pHFilePreamble;
	C3OptionEditorText * pHFileTrailer;

	C3OptionsWidget * pCPPFilePreview;
	QString szCPPFilePreview;
	C3OptionEditorText * pCPPFilePreviewText;

	C3OptionsWidget * pHFilePreview;
	QString szHFilePreview;
	C3OptionEditorText * pHFilePreviewText;

};

#define _p m_pC3NCPPCD

C3NewCPPClassDialog::C3NewCPPClassDialog()
	: C3OptionsDialog(__tr("New C++ Class"))
{
	C3Workspace * pWorkspace = C3Workspace::currentWorkspace();

	_p = new C3NewCPPClassDialogPrivate();

	// Set defaults and options that are stored.

	_p->oSettings.eClassType = C3NewCPPClassSettings::StandardClass;

	_p->oSettings.szAPIDeclarator = pWorkspace->genericSetting("new.creator.cpp.szAPIDeclarator").toString();

	_p->oSettings.szCPPFileHeader = C3NewObjectUtils::buildFileHeader(
			QString(),
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);

	_p->oSettings.szCPPFilePreamble = pWorkspace->genericSetting("new.creator.cpp.szCPPFilePreamble").toString();
	_p->oSettings.szCPPFileTrailer = pWorkspace->genericSetting("new.creator.cpp.szCPPFileTrailer").toString();

	_p->oSettings.szHFileHeader = C3NewObjectUtils::buildFileHeader(
			QString(),
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);

	_p->oSettings.szHFilePreamble = pWorkspace->genericSetting("new.creator.cpp.szHFilePreamble").toString();
	_p->oSettings.szHFileTrailer = pWorkspace->genericSetting("new.creator.cpp.szHFileTrailer").toString();

	_p->pGeneral = new C3OptionsWidget(__tr("General"),parentForWidgets());

	_p->pClassName = _p->pGeneral->addStringEditor(__tr("Class Name"),&(_p->oSettings.szClassName));
	QObject::connect(_p->pClassName,SIGNAL(edited()),this,SLOT(slotClassNameEdited()));


	QList< QPair<int,QString> > lOptions;
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::SimpleStuctWithPublicMembers,__tr("Simple Struct with Public Members (no CPP file, no constructor)")));
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::StandardClass,__tr("Standard Class")));
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::TemplateClass,__tr("Template Class (no CPP file)")));
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::SingletonWithInitDoneFunctions,__tr("Singleton with Public init() / done() Functions")));
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::SingletonWithUserControlledInstantiation,__tr("Singleton with User Controlled Instantiation")));
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::SingletonWithLazyInstantiation,__tr("Singleton with Lazy Instantiation")));
	lOptions.append(QPair<int,QString>(C3NewCPPClassSettings::Namespace,__tr("Namespace (technically not a class but well...)")));

	_p->pClassType = _p->pGeneral->addEnumerationEditor<C3NewCPPClassSettings::ClassType>(
			__tr("Class Type"),
			&(_p->oSettings.eClassType),
			lOptions
		);
	QObject::connect(_p->pClassType,SIGNAL(edited()),this,SLOT(slotClassTypeEdited()));

	_p->pConstructorArguments = _p->pGeneral->addStringEditor(__tr("Constructor Arguments"),&(_p->oSettings.szConstructorArguments));

	_p->pBaseClassGroup = _p->pGeneral->addGroup(__tr("Inherits"));

	_p->pBaseClassNames = _p->pBaseClassGroup->childOptionsWidget()->addStringEditor(__tr("Base Classes"),&(_p->oSettings.szBaseClassNames));
	QObject::connect(_p->pBaseClassNames,SIGNAL(edited()),this,SLOT(slotBaseClassNameEdited()));

	_p->pBaseClassIncludeFile = _p->pBaseClassGroup->childOptionsWidget()->addStringEditor(__tr("Include File"),&(_p->oSettings.szBaseClassIncludeFiles));


	_p->pFilesGroup = _p->pGeneral->addGroup(__tr("Files"));

	_p->pCPPFilePath = _p->pFilesGroup->childOptionsWidget()->addFileEditor(__tr("CPP File"),&(_p->oSettings.szCPPFilePath));
	QObject::connect(_p->pCPPFilePath,SIGNAL(edited()),this,SLOT(slotCPPFilePathEdited()));
	_p->pHFilePath = _p->pFilesGroup->childOptionsWidget()->addFileEditor(__tr("Header File"),&(_p->oSettings.szHFilePath));
	QObject::connect(_p->pHFilePath,SIGNAL(edited()),this,SLOT(slotHFilePathEdited()));

	_p->pOptionsGroup = _p->pGeneral->addGroup(__tr("Options"));

	_p->pHasVirtualFunctions = _p->pOptionsGroup->childOptionsWidget()->addBooleanEditor(__tr("Has virtual functions (virtual destructor)"),&(_p->oSettings.bHasVirtualFunctions));


	_p->pBaseClassIsQObject = _p->pOptionsGroup->childOptionsWidget()->addBooleanEditor(__tr("Derived from Qt's QObject (add Q_OBJECT macro)"),&(_p->oSettings.bBaseClassIsQObject));
	_p->pHideMembersViaPrivateClass = _p->pOptionsGroup->childOptionsWidget()->addBooleanEditor(__tr("Hide members in <Class>Private class"),&(_p->oSettings.bHideMembersViaPrivateClass));

	_p->pGenerateCopyConstructor = _p->pOptionsGroup->childOptionsWidget()->addBooleanEditor(__tr("Generate copy constructor"),&(_p->oSettings.bGenerateCopyConstructor));
	_p->pGenerateMoveConstructor = _p->pOptionsGroup->childOptionsWidget()->addBooleanEditor(__tr("Generate move constructor"),&(_p->oSettings.bGenerateMoveConstructor));
	_p->pGenerateAssignmentOperator = _p->pOptionsGroup->childOptionsWidget()->addBooleanEditor(__tr("Generate assignment operator"),&(_p->oSettings.bGenerateAssignmentOperator));

	_p->pAPIDeclarator = _p->pOptionsGroup->childOptionsWidget()->addStringEditor(__tr("API Declarator"),&(_p->oSettings.szAPIDeclarator));

	_p->pGeneral->addSpaceEater();

	addWidget(_p->pGeneral);


	_p->pHFileFormat = new C3OptionsWidget(__tr("H File Format"),parentForWidgets());

	_p->pHFileHeader = _p->pHFileFormat->addTextEditor(__tr("Header (top of the file)"),&(_p->oSettings.szHFileHeader));
	_p->pHFilePreamble = _p->pHFileFormat->addTextEditor(__tr("Preamble (before declaration)"),&(_p->oSettings.szHFilePreamble));
	_p->pHFileTrailer = _p->pHFileFormat->addTextEditor(__tr("Trailer (after declaration)"),&(_p->oSettings.szHFileTrailer));

	addWidget(_p->pHFileFormat);


	_p->pCPPFileFormat = new C3OptionsWidget(__tr("CPP File Format"),parentForWidgets());

	_p->pCPPFileHeader = _p->pCPPFileFormat->addTextEditor(__tr("Header (top of the file)"),&(_p->oSettings.szCPPFileHeader));
	_p->pCPPFilePreamble = _p->pCPPFileFormat->addTextEditor(__tr("Preamble (before implementation)"),&(_p->oSettings.szCPPFilePreamble));
	_p->pCPPFileTrailer = _p->pCPPFileFormat->addTextEditor(__tr("Trailer (after implementation)"),&(_p->oSettings.szCPPFileTrailer));

	addWidget(_p->pCPPFileFormat);


	_p->pHFilePreview = new C3OptionsWidget(__tr("H File Preview"),parentForWidgets());
	QObject::connect(_p->pHFilePreview,SIGNAL(aboutToShow()),this,SLOT(slotHFilePreviewAboutToShow()));

	_p->pHFilePreviewText = _p->pHFilePreview->addTextEditor(__tr("Preview of Contents"),&(_p->szHFilePreview),C3OptionEditorText::ReadOnly);

	addWidget(_p->pHFilePreview);



	_p->pCPPFilePreview = new C3OptionsWidget(__tr("CPP File Preview"),parentForWidgets());
	QObject::connect(_p->pCPPFilePreview,SIGNAL(aboutToShow()),this,SLOT(slotCPPFilePreviewAboutToShow()));

	_p->pCPPFilePreviewText = _p->pCPPFilePreview->addTextEditor(__tr("Preview of Contents"),&(_p->szCPPFilePreview),C3OptionEditorText::ReadOnly);

	addWidget(_p->pCPPFilePreview);


	setMinimumSize(QSize(450,450));

	enableDisableButtons();
}

C3NewCPPClassDialog::~C3NewCPPClassDialog()
{
	delete _p;
}

void C3NewCPPClassDialog::showEvent(QShowEvent * e)
{
	C3OptionsDialog::showEvent(e);
	
	_p->pClassName->lineEdit()->setFocus();
}

bool C3NewCPPClassDialog::isValid()
{
	if(_p->pClassName->currentValue().isEmpty())
		return false;
	QString szCPP = _p->pCPPFilePath->currentValue();
	if(szCPP.isEmpty())
		return false;
	QString szH = _p->pHFilePath->currentValue();
	if(szH.isEmpty())
		return false;
	if(szCPP == szH)
		return false;

	return true;
}

void C3NewCPPClassDialog::updateCPPFileHeader()
{
	QString szFilePath = _p->pCPPFilePath->currentValue();

	QString szHeader = C3NewObjectUtils::buildFileHeader(
			szFilePath,
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);

	_p->pCPPFileHeader->setCurrentValue(szHeader);
}

void C3NewCPPClassDialog::updateHFileHeader()
{
	QString szFilePath = _p->pHFilePath->currentValue();

	QString szHeader = C3NewObjectUtils::buildFileHeader(
			szFilePath,
			C3NewObjectUtils::BuildHeaderCommentStyleCSingleLine
		);
	
	_p->pHFileHeader->setCurrentValue(szHeader);
}

void C3NewCPPClassDialog::enableDisableButtons()
{
	OKButton()->setEnabled(isValid());
}

void C3NewCPPClassDialog::slotCPPFilePathEdited()
{
	updateCPPFileHeader();
}

void C3NewCPPClassDialog::slotHFilePathEdited()
{
	updateHFileHeader();
}

void C3NewCPPClassDialog::slotClassNameEdited()
{
	C3MainWindow * pMainWindow = C3MainWindow::instance();
	
	QString szClass = _p->pClassName->currentValue();

	int idx = szClass.indexOf(QChar('<'));
	if(idx >= 0)
		szClass = szClass.left(idx);

	szClass.replace(QRegExp("[^A-Za-z0-9_]"),"_");

	if(szClass.isEmpty())
	{
		_p->pCPPFilePath->setCurrentValue(QString());
		_p->pHFilePath->setCurrentValue(QString());

		updateCPPFileHeader();
		updateHFileHeader();
		enableDisableButtons();
		return;
	}

	QString szDir = pMainWindow->currentDirectory();

	QString szCPP = szDir;
	if(!szCPP.endsWith(QDir::separator()))
		szCPP.append(QDir::separator());
	szCPP.append(szClass);
	szCPP.append(__utf8(".cpp"));

	QString szH = szDir;
	if(!szH.endsWith(QDir::separator()))
		szH.append(QDir::separator());
	szH.append(szClass);
	szH.append(__utf8(".h"));

	_p->pCPPFilePath->setCurrentValue(szCPP);
	_p->pHFilePath->setCurrentValue(szH);
	
	if(idx >= 0)
	{
		_p->pClassType->setCurrentValue(C3NewCPPClassSettings::TemplateClass);
		enableDisableControlsRelatedToClassType();
		_p->pClassType->setEnabled(false);
	} else {
		_p->pClassType->setEnabled(true);
	}

	updateCPPFileHeader();
	updateHFileHeader();
	enableDisableButtons();
}

void C3NewCPPClassDialog::slotBaseClassNameEdited()
{
	QString szBaseClasses = _p->pBaseClassNames->currentValue();

	QList<QString> lBaseClasses = C3NewCPPClassSettings::splitClassNameList(szBaseClasses);

	QString szIncludes;

	bool bGotQObject = false;

	Q_FOREACH(QString cl,lBaseClasses)
	{
		int idx = cl.indexOf(QChar('<'));
		if(idx > 0)
			cl.truncate(idx);

		if(!szIncludes.isEmpty())
			szIncludes.append(__ascii(", "));
		
		if(cl.startsWith(QChar('Q')))
		{
			bGotQObject = true;
			szIncludes.append(__ascii("<%1>").arg(cl));
		} else {
			szIncludes.append(__ascii("\"%1.h\"").arg(cl));
		}
	}

	_p->pBaseClassIncludeFile->setCurrentValue(szIncludes);
	_p->pBaseClassIsQObject->setCurrentValue(bGotQObject);

	if(bGotQObject)
		_p->pHasVirtualFunctions->setCurrentValue(true);

	enableDisableButtons();
}

void C3NewCPPClassDialog::enableDisableControlsRelatedToClassType()
{
	C3NewCPPClassSettings::ClassType eType = _p->pClassType->currentValue();

	switch(eType)
	{
		case C3NewCPPClassSettings::Namespace:
			_p->pBaseClassNames->setEnabled(false);
			_p->pBaseClassIncludeFile->setEnabled(false);
		break;
		default:
			_p->pBaseClassNames->setEnabled(true);
			_p->pBaseClassIncludeFile->setEnabled(true);
		break;
	}

	switch(eType)
	{
		case C3NewCPPClassSettings::SimpleStuctWithPublicMembers:
			_p->pHasVirtualFunctions->setEnabled(false);
			_p->pBaseClassIsQObject->setEnabled(false);
			_p->pConstructorArguments->setEnabled(false);
		break;
		case C3NewCPPClassSettings::Namespace:
			_p->pHasVirtualFunctions->setEnabled(false);
			_p->pBaseClassIsQObject->setEnabled(false);
			_p->pConstructorArguments->setEnabled(false);
		break;
		case C3NewCPPClassSettings::TemplateClass:
			_p->pHasVirtualFunctions->setEnabled(true);
			_p->pBaseClassIsQObject->setEnabled(false);
			_p->pConstructorArguments->setEnabled(true);
		break;
		case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
			_p->pHasVirtualFunctions->setEnabled(true);
			_p->pBaseClassIsQObject->setEnabled(true);
			_p->pConstructorArguments->setEnabled(false);
		break;
		case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
			_p->pHasVirtualFunctions->setEnabled(true);
			_p->pBaseClassIsQObject->setEnabled(true);
			_p->pConstructorArguments->setEnabled(true);
		break;
		case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
			_p->pHasVirtualFunctions->setEnabled(true);
			_p->pBaseClassIsQObject->setEnabled(true);
			_p->pConstructorArguments->setEnabled(false);
		break;
		//case C3NewCPPClassSettings::StandardClass:
		default:
			_p->pHasVirtualFunctions->setEnabled(true);
			_p->pBaseClassIsQObject->setEnabled(true);
			_p->pConstructorArguments->setEnabled(true);
		break;
	}

	switch(eType)
	{
		case C3NewCPPClassSettings::SimpleStuctWithPublicMembers:
		case C3NewCPPClassSettings::TemplateClass:
			_p->pCPPFilePath->setEnabled(false);
			_p->pCPPFileHeader->setEnabled(false);
			_p->pCPPFilePreamble->setEnabled(false);
			_p->pCPPFileTrailer->setEnabled(false);
			_p->pCPPFilePreviewText->setEnabled(false);
		break;
		default:
			_p->pCPPFilePath->setEnabled(true);
			_p->pCPPFileHeader->setEnabled(true);
			_p->pCPPFilePreamble->setEnabled(true);
			_p->pCPPFileTrailer->setEnabled(true);
			_p->pCPPFilePreviewText->setEnabled(true);
		break;
	}

	switch(eType)
	{
		case C3NewCPPClassSettings::StandardClass:
		case C3NewCPPClassSettings::SingletonWithLazyInstantiation:
		case C3NewCPPClassSettings::SingletonWithUserControlledInstantiation:
		case C3NewCPPClassSettings::SingletonWithInitDoneFunctions:
			_p->pHideMembersViaPrivateClass->setEnabled(true);
		break;
		default:
			_p->pHideMembersViaPrivateClass->setEnabled(false);
		break;
	}


	switch(eType)
	{
		case C3NewCPPClassSettings::StandardClass:
		case C3NewCPPClassSettings::TemplateClass:
			_p->pGenerateCopyConstructor->setEnabled(true);
			_p->pGenerateMoveConstructor->setEnabled(true);
			_p->pGenerateAssignmentOperator->setEnabled(true);
		break;
		default:
			_p->pGenerateCopyConstructor->setEnabled(false);
			_p->pGenerateMoveConstructor->setEnabled(false);
			_p->pGenerateAssignmentOperator->setEnabled(false);
		break;
	}


}


void C3NewCPPClassDialog::slotClassTypeEdited()
{
	enableDisableControlsRelatedToClassType();
}

void C3NewCPPClassDialog::slotHFilePreviewAboutToShow()
{
	if((!isValid()) || (!C3OptionsDialog::commit()))
	{
		_p->pHFilePreviewText->setCurrentValue(__tr("Invalid Settings"));
		return;
	}

	C3NewCPPClassGenerator g(&(_p->oSettings));

	_p->pHFilePreviewText->setCurrentValue(g.generateH());
}

void C3NewCPPClassDialog::slotCPPFilePreviewAboutToShow()
{
	if((!isValid()) || (!C3OptionsDialog::commit()))
	{
		_p->pCPPFilePreviewText->setCurrentValue(__tr("Invalid Settings"));
		return;
	}

	C3NewCPPClassGenerator g(&(_p->oSettings));
	
	if(!g.needsCPPFile())
	{
		_p->pCPPFilePreviewText->setCurrentValue(__tr("CPP File Not Generated"));
		return;
	}

	_p->pCPPFilePreviewText->setCurrentValue(g.generateCPP());
}


bool C3NewCPPClassDialog::commit()
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

	C3NewCPPClassGenerator g(&(_p->oSettings));

	bool bDoCPP = g.needsCPPFile();

	if(bDoCPP && QFile::exists(_p->oSettings.szCPPFilePath))
	{
		QMessageBox::StandardButton eButton = QMessageBox::question(
				C3MainWindow::instance(),
				__tr("File Exists"),
				__tr("The file %1 exists: do you want to overwrite it?").arg(_p->oSettings.szCPPFilePath),
				QMessageBox::Yes | QMessageBox::No
			);
	
		if(eButton != QMessageBox::Yes)
			return false;
	}

	if(QFile::exists(_p->oSettings.szHFilePath))
	{
		QMessageBox::StandardButton eButton = QMessageBox::question(
				C3MainWindow::instance(),
				__tr("File Exists"),
				__tr("The file %1 exists: do you want to overwrite it?").arg(_p->oSettings.szHFilePath),
				QMessageBox::Yes | QMessageBox::No
			);
	
		if(eButton != QMessageBox::Yes)
			return false;
	}

	C3Editor * pCPPFileEditor = NULL;

	if(bDoCPP)
	{
		pCPPFileEditor = C3Workspace::currentWorkspace()->findEditor("text",_p->oSettings.szCPPFilePath);
		if(pCPPFileEditor)
		{
			QMessageBox::StandardButton eButton = QMessageBox::question(
					C3MainWindow::instance(),
					__tr("File Being Edited"),
					__tr("The file %1 is being edited. The editor will be closed and file contents overwritten. Do you want to continue?").arg(_p->oSettings.szCPPFilePath),
					QMessageBox::Yes | QMessageBox::No
				);
		
			if(eButton != QMessageBox::Yes)
				return false;
		}
	}
	
	
	C3Editor * pHFileEditor = C3Workspace::currentWorkspace()->findEditor("text",_p->oSettings.szHFilePath);
	if(pHFileEditor)
	{
		QMessageBox::StandardButton eButton = QMessageBox::question(
				C3MainWindow::instance(),
				__tr("File Being Edited"),
				__tr("The file %1 is being edited. The editor will be closed and file contents overwritten. Do you want to continue?").arg(_p->oSettings.szHFilePath),
				QMessageBox::Yes | QMessageBox::No
			);
	
		if(eButton != QMessageBox::Yes)
			return false;
	}

	if(pCPPFileEditor)
		pWorkspace->closeEditor(pCPPFileEditor,false);
	if(pHFileEditor)
		pWorkspace->closeEditor(pHFileEditor,false);

	
	QByteArray oData;
	
	if(bDoCPP)
	{
		QString szCPPContents = g.generateCPP();
	
	
		QFile f1(_p->oSettings.szCPPFilePath);
		if(!f1.open(QFile::WriteOnly))
		{
			QMessageBox::StandardButton eButton = QMessageBox::critical(
					C3MainWindow::instance(),
					__tr("Can't Open File"),
					__tr("Failed to create the file %1: %2").arg(_p->oSettings.szCPPFilePath).arg(f1.error()),
					QMessageBox::Ok
				);
			
			return false;
		}

		oData = szCPPContents.toUtf8();
	
		if(f1.write(oData) != oData.size())
		{
			QMessageBox::StandardButton eButton = QMessageBox::critical(
					C3MainWindow::instance(),
					__tr("Error Writing File"),
					__tr("Failed to write to the file %1: %2").arg(_p->oSettings.szCPPFilePath).arg(f1.error()),
					QMessageBox::Ok
				);
			
			return false;
		}
	
		f1.close();
	
		pWorkspace->openFileAsync(_p->oSettings.szCPPFilePath);
	}
	
	QString szHContents = g.generateH();

	QFile f2(_p->oSettings.szHFilePath);
	if(!f2.open(QFile::WriteOnly))
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				C3MainWindow::instance(),
				__tr("Can't Open File"),
				__tr("Failed to create the file %1: %2").arg(_p->oSettings.szHFilePath).arg(f2.error()),
				QMessageBox::Ok
			);
		
		return false;
	}

	oData = szHContents.toUtf8();

	if(f2.write(oData) != oData.size())
	{
		QMessageBox::StandardButton eButton = QMessageBox::critical(
				C3MainWindow::instance(),
				__tr("Error Writing File"),
				__tr("Failed to write to the file %1: %2").arg(_p->oSettings.szHFilePath).arg(f2.error()),
				QMessageBox::Ok
			);
		
		return false;
	}

	f2.close();

	pWorkspace->openFileAsync(_p->oSettings.szHFilePath);

	pWorkspace->setGenericSetting("new.creator.cpp.szAPIDeclarator",QVariant(_p->oSettings.szAPIDeclarator));

	pWorkspace->setGenericSetting("new.creator.cpp.szCPPFilePreamble",QVariant(_p->oSettings.szCPPFilePreamble));
	pWorkspace->setGenericSetting("new.creator.cpp.szCPPFileTrailer",QVariant(_p->oSettings.szCPPFileTrailer));
	pWorkspace->setGenericSetting("new.creator.cpp.szHFilePreamble",QVariant(_p->oSettings.szHFilePreamble));
	pWorkspace->setGenericSetting("new.creator.cpp.szHFileTrailer",QVariant(_p->oSettings.szHFileTrailer));

	if(C3DockFileBrowser::instance())
		C3DockFileBrowser::instance()->reload();

	pWorkspace->notifyRescanNeeded();

	return true;
}
