//=============================================================================
//
//   File : C3TextEditorModeCPPMemberVariableGeneratorDialog.cpp
//   Creation Date : 2016/09/13 00:58:42
//   Project : ssex
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

#include "C3TextEditorModeCPPMemberVariableGeneratorDialog.h"

#include "C3OptionsWidget.h"

#include "C3OptionEditorString.h"
#include "C3OptionEditorBoolean.h"
#include "C3TextEditorModeCPPMemberVariableGenerator.h"
#include "C3OptionEditorGroup.h"
#include "C3SymbolManagerRequestScanEditorFile.h"
#include "C3SymbolManagerRequestIdentifyContext.h"
#include "C3SymbolManager.h"
#include "C3OptionsWidgetContainer.h"
#include "C3Workspace.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class C3TextEditorModeCPPMemberVariableGeneratorDialogPrivate
{
public:
	C3TextEditor * pEditor;

	C3TextEditorModeCPPMemberVariableGenerator::Settings oSettings;

	C3OptionsWidget * pGeneral;

	C3OptionEditorString * pTargetClass;

	C3OptionEditorString * pTypeName;
	C3OptionEditorString * pMemberName;
	C3OptionEditorEnumeration<C3Symbol::AccessLevel> * pAccessLevel;
	C3OptionEditorString * pComment;

	
	C3OptionEditorGroup * pSetterGroup;
	C3OptionEditorString * pSetterName;
	C3OptionEditorBoolean * pSetterAcceptsReference;
	C3OptionEditorBoolean * pSetterAcceptsConst;
	C3OptionEditorEnumeration<C3Symbol::AccessLevel> * pSetterAccessLevel;
	
	C3OptionEditorGroup * pGetterGroup;
	C3OptionEditorString * pGetterName;
	C3OptionEditorBoolean * pGetterReturnsReference;
	C3OptionEditorBoolean * pGetterReturnsConst;
	C3OptionEditorBoolean * pGetterIsConst;
	C3OptionEditorEnumeration<C3Symbol::AccessLevel> * pGetterAccessLevel;
	
	C3OptionEditorEnumeration<C3TextEditorModeCPPMemberVariableGenerator::Location> * pLocation;
	
	C3SymbolManagerClassDescription * pClassDescription;
	
	QString szDummy;
};

#define _p m_pC3TEMCPPMGD

C3TextEditorModeCPPMemberVariableGeneratorDialog::C3TextEditorModeCPPMemberVariableGeneratorDialog(C3TextEditor * pEditor)
	: C3TextEditorModeCPPCodeEditDialog(
			__tr("New C++ Member Variable"),
			pEditor,
			IdentifyContextAtCursor,
			C3OptionsWidgetContainer::HideLeftTreeWidget
		)
{
	_p = new C3TextEditorModeCPPMemberVariableGeneratorDialogPrivate();
	
	_p->pEditor = pEditor;
	_p->oSettings.pClassDescription = NULL;
	_p->pClassDescription = NULL;

	_p->oSettings.eLocation = C3TextEditorModeCPPMemberVariableGenerator::AtCursorPosition;

	_p->pGeneral = new C3OptionsWidget(__tr("General"),parentForWidgets());

	_p->pTargetClass = _p->pGeneral->addStringEditor(__tr("Target Class"),&(_p->szDummy));
	_p->pTargetClass->lineEdit()->setEnabled(false);
	_p->pTargetClass->setCurrentValue(__tr("Identifying..."));

	_p->pTypeName = _p->pGeneral->addStringEditor(__tr("Type Name"),&(_p->oSettings.szTypeName));
	QObject::connect(_p->pTypeName,SIGNAL(edited()),this,SLOT(slotTypeNameEdited()));

	_p->pMemberName = _p->pGeneral->addStringEditor(__tr("Member Name"),&(_p->oSettings.szMemberName));
	QObject::connect(_p->pMemberName,SIGNAL(edited()),this,SLOT(slotMemberNameEdited()));


	QList<QPair<int,QString> > lAccessOptions;
	lAccessOptions.append(QPair<int,QString>(C3Symbol::AccessLevelPublic,__tr("Public")));
	lAccessOptions.append(QPair<int,QString>(C3Symbol::AccessLevelProtected,__tr("Protected")));
	lAccessOptions.append(QPair<int,QString>(C3Symbol::AccessLevelPrivate,__tr("Private")));

	_p->pAccessLevel = _p->pGeneral->addEnumerationEditor<C3Symbol::AccessLevel>(
			__tr("Access Level"),
			&(_p->oSettings.eAccessLevel),
			lAccessOptions,
			0
		);
	_p->pAccessLevel->setCurrentValue(C3Symbol::AccessLevelPrivate);
	_p->pAccessLevel->setEnabled(false);

	_p->pComment = _p->pGeneral->addStringEditor(__tr("Comment"),&(_p->oSettings.szComment));

	_p->pSetterGroup = _p->pGeneral->addGroup(__tr("Generate Setter"),C3OptionEditorGroup::Checkable);
	_p->pSetterName = _p->pSetterGroup->childOptionsWidget()->addStringEditor(__tr("Name"),&(_p->oSettings.szSetterName));
	_p->pSetterAcceptsReference = _p->pSetterGroup->childOptionsWidget()->addBooleanEditor(__tr("Takes Reference"),&(_p->oSettings.bSetterAcceptsReference));
	_p->pSetterAcceptsConst = _p->pSetterGroup->childOptionsWidget()->addBooleanEditor(__tr("Takes Const"),&(_p->oSettings.bSetterAcceptsConst));
	_p->pSetterAccessLevel = _p->pSetterGroup->childOptionsWidget()->addEnumerationEditor<C3Symbol::AccessLevel>(
			__tr("Access Level"),
			&(_p->oSettings.eSetterAccessLevel),
			lAccessOptions,
			0
		);
	_p->pSetterAccessLevel->setCurrentValue(C3Symbol::AccessLevelPublic);
	_p->pSetterAccessLevel->setEnabled(false);


	_p->pSetterGroup->childOptionsWidget()->addSpaceEater();

	_p->pGetterGroup = _p->pGeneral->addGroup(__tr("Generate Getter"),C3OptionEditorGroup::Checkable);

	_p->pGetterName = _p->pGetterGroup->childOptionsWidget()->addStringEditor(__tr("Name"),&(_p->oSettings.szGetterName));
	_p->pGetterReturnsReference = _p->pGetterGroup->childOptionsWidget()->addBooleanEditor(__tr("Returns Reference"),&(_p->oSettings.bGetterReturnsReference));
	_p->pGetterReturnsConst = _p->pGetterGroup->childOptionsWidget()->addBooleanEditor(__tr("Returns Const"),&(_p->oSettings.bGetterReturnsConst));
	_p->pGetterIsConst = _p->pGetterGroup->childOptionsWidget()->addBooleanEditor(__tr("Is Const"),&(_p->oSettings.bGetterIsConst));
	_p->pGetterAccessLevel = _p->pGetterGroup->childOptionsWidget()->addEnumerationEditor<C3Symbol::AccessLevel>(
			__tr("Access Level"),
			&(_p->oSettings.eGetterAccessLevel),
			lAccessOptions,
			0
		);
	_p->pGetterAccessLevel->setCurrentValue(C3Symbol::AccessLevelPublic);
	_p->pGetterAccessLevel->setEnabled(false);


	_p->pGetterGroup->childOptionsWidget()->addSpaceEater();

	QList<QPair<int,QString> > lOptions;
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberVariableGenerator::AtCursorPosition,__tr("At Cursor Position")));
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberVariableGenerator::InContextContainerDeclaration,__tr("In Class Declaration (Setter/Getter Inlineable)")));
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberVariableGenerator::InContextContainerDeclarationAndDefinition,__tr("In Class Declaration and Definition")));

	_p->pLocation = _p->pGeneral->addEnumerationEditor<C3TextEditorModeCPPMemberVariableGenerator::Location>(
			__tr("Insert Location"),
			&(_p->oSettings.eLocation),
			lOptions,
			0
		);
	_p->pLocation->setEnabled(false);

	_p->pGeneral->addSpaceEater();
	
	addWidget(_p->pGeneral);

	setMinimumSize(QSize(450,450));
	
	enableDisableButtons();
}

C3TextEditorModeCPPMemberVariableGeneratorDialog::~C3TextEditorModeCPPMemberVariableGeneratorDialog()
{
	if(_p->pClassDescription)
		delete _p->pClassDescription;
	delete _p;
}


void C3TextEditorModeCPPMemberVariableGeneratorDialog::onContextIdentified(C3SymbolManagerSymbolDescription * pContextSymbol)
{
	if(!pContextSymbol->pContainingClass)
	{
		_p->pTargetClass->setCurrentValue(__tr("Not found - inserting at cursor"));
		qDebug("GOT NO CONTAINER CLASS");
		return;
	}
	
	_p->pClassDescription = pContextSymbol->takeContainingClass();
	delete pContextSymbol;

	_p->pTargetClass->setCurrentValue(_p->pClassDescription->szClassName);

	C3TextEditor * pDeclarationEditor = dynamic_cast<C3TextEditor *>(
			C3Workspace::currentWorkspace()->findEditor(__ascii("text"),_p->pClassDescription->szDeclaratonFile)
		);
	C3TextEditor * pDefinitionEditor = dynamic_cast<C3TextEditor *>(
			C3Workspace::currentWorkspace()->findEditor(__ascii("text"),_p->pClassDescription->szDefinitionFile)
		);

	QList<QPair<int,QString> > lOptions;
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberVariableGenerator::AtCursorPosition,__tr("At Cursor Position")));

	if(pDeclarationEditor && ((pDeclarationEditor == _p->pEditor) || (pDefinitionEditor == _p->pEditor)))
	{
		lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberVariableGenerator::InContextContainerDeclaration,__tr("In Class Declaration (Setter/Getter Inlineable)")));
		if(pDefinitionEditor)
			lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberVariableGenerator::InContextContainerDeclarationAndDefinition,__tr("In Class Declaration and Definition")));
		// FIXME: Else try to open the file?

		_p->pLocation->setCurrentValue(C3TextEditorModeCPPMemberVariableGenerator::InContextContainerDeclaration);
	} else {
		_p->pLocation->setCurrentValue(C3TextEditorModeCPPMemberVariableGenerator::AtCursorPosition);
	}

	_p->pLocation->setEnabled(true);
	_p->pAccessLevel->setEnabled(true);
	_p->pGetterAccessLevel->setEnabled(true);
	_p->pSetterAccessLevel->setEnabled(true);
}

bool C3TextEditorModeCPPMemberVariableGeneratorDialog::commit()
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

	_p->oSettings.bGenerateGetter = _p->pGetterGroup->isChecked();
	_p->oSettings.bGenerateSetter = _p->pSetterGroup->isChecked();
	_p->oSettings.pClassDescription = _p->pClassDescription;
	
	C3TextEditorModeCPPMemberVariableGenerator oGenerator(
			_p->pEditor,
			&(_p->oSettings)
		);

	if(!oGenerator.run())
		return false;

	return true;
}

void C3TextEditorModeCPPMemberVariableGeneratorDialog::showEvent(QShowEvent * e)
{
	C3OptionsDialog::showEvent(e);

	_p->pTypeName->lineEdit()->setFocus();
}

void C3TextEditorModeCPPMemberVariableGeneratorDialog::slotTypeNameEdited()
{
	QString szTypeName = _p->pTypeName->currentValue();

	if(szTypeName.contains(QChar('*')) || szTypeName.contains(QChar('&')))
	{
		_p->pGetterReturnsReference->setCurrentValue(false);
		_p->pSetterAcceptsReference->setCurrentValue(false);
	} else if((szTypeName.length() > 1) && szTypeName[0].isUpper())
	{
		_p->pGetterReturnsReference->setCurrentValue(true);
		_p->pGetterReturnsConst->setCurrentValue(true);
		_p->pGetterIsConst->setCurrentValue(true);
		_p->pSetterAcceptsReference->setCurrentValue(true);
		_p->pSetterAcceptsConst->setCurrentValue(true);
	} else {
		_p->pGetterReturnsReference->setCurrentValue(false);
		_p->pGetterReturnsConst->setCurrentValue(false);
		_p->pSetterAcceptsReference->setCurrentValue(false);
		_p->pSetterAcceptsConst->setCurrentValue(false);
	}

	enableDisableButtons();
}

void C3TextEditorModeCPPMemberVariableGeneratorDialog::slotMemberNameEdited()
{
	QString szMemberName = _p->pMemberName->currentValue();

	if(szMemberName.startsWith(__ascii("m_")))
		szMemberName.remove(0,2);
	else if(szMemberName.startsWith(__ascii("_")))
		szMemberName.remove(0,1);

	int idx = -1;
	for(int i=0;i<szMemberName.length();i++)
	{
		if(szMemberName[i].isUpper())
		{
			idx = i;
			break;
		}
	}
	
	if(idx > 0)
		szMemberName.remove(0,idx);

	QString szGetterName = szMemberName;
	if(szGetterName.length() > 0)
		szGetterName[0] = szGetterName[0].toLower();
	_p->pGetterName->setCurrentValue(szGetterName);

	QString szSetterName = szMemberName;
	if(szSetterName.length() > 0)
		szSetterName[0] = szSetterName[0].toUpper();
	szSetterName = __ascii("set") + szSetterName;
	_p->pSetterName->setCurrentValue(szSetterName);

	enableDisableButtons();
}


bool C3TextEditorModeCPPMemberVariableGeneratorDialog::isValid()
{
	if(_p->pTypeName->currentValue().isEmpty())
		return false;
	if(_p->pMemberName->currentValue().isEmpty())
		return false;

	if(_p->pGetterGroup->isChecked() && _p->pGetterName->currentValue().isEmpty())
		return false;
	if(_p->pSetterGroup->isChecked() && _p->pSetterName->currentValue().isEmpty())
		return false;

	return true;
}

void C3TextEditorModeCPPMemberVariableGeneratorDialog::enableDisableButtons()
{
	OKButton()->setEnabled(isValid());
}

