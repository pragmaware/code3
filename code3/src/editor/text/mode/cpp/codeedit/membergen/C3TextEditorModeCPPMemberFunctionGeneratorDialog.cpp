//=============================================================================
//
//   File : C3TextEditorModeCPPMemberFunctionGeneratorDialog.cpp
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

#include "C3TextEditorModeCPPMemberFunctionGeneratorDialog.h"

#include "C3OptionsWidget.h"

#include "C3OptionEditorString.h"
#include "C3OptionEditorBoolean.h"
#include "C3TextEditorModeCPPMemberFunctionGenerator.h"
#include "C3OptionEditorGroup.h"
#include "C3SymbolManagerRequestScanEditorFile.h"
#include "C3SymbolManagerRequestIdentifyContext.h"
#include "C3OptionsWidgetContainer.h"
#include "C3SymbolManager.h"
#include "C3Workspace.h"
#include "C3Symbol.h"

#include <QLineEdit>
#include <QRegExp>
#include <QPushButton>
#include <QLabel>

class C3TextEditorModeCPPMemberFunctionGeneratorDialogPrivate
{
public:
	C3TextEditor * pEditor;

	C3TextEditorModeCPPMemberFunctionGenerator::Settings oSettings;

	C3OptionsWidget * pGeneral;

	C3OptionEditorString * pTargetClass;

	C3OptionEditorString * pReturnType;
	C3OptionEditorString * pFunctionName;
	C3OptionEditorString * pParameters;
	C3OptionEditorString * pComment;
	
	C3OptionEditorBoolean * pIsConst;
	C3OptionEditorBoolean * pIsVirtual;
	C3OptionEditorBoolean * pIsStatic;
	
	C3OptionEditorEnumeration<C3TextEditorModeCPPMemberFunctionGenerator::Location> * pLocation;
	C3OptionEditorEnumeration<C3Symbol::AccessLevel> * pAccessLevel;
	
	C3SymbolManagerClassDescription * pClassDescription;
	
	QString szDummy;
};

#define _p m_pC3TEMCPPMGD

C3TextEditorModeCPPMemberFunctionGeneratorDialog::C3TextEditorModeCPPMemberFunctionGeneratorDialog(C3TextEditor * pEditor)
	: C3TextEditorModeCPPCodeEditDialog(
			__tr("New C++ Member Function"),
			pEditor,
			IdentifyContextAtCursor,
			C3OptionsWidgetContainer::HideLeftTreeWidget
		)
{
	_p = new C3TextEditorModeCPPMemberFunctionGeneratorDialogPrivate();
	
	_p->pEditor = pEditor;
	_p->oSettings.pClassDescription = NULL;
	_p->pClassDescription = NULL;

	_p->oSettings.eLocation = C3TextEditorModeCPPMemberFunctionGenerator::AtCursorPosition;

	_p->pGeneral = new C3OptionsWidget(__tr("General"),parentForWidgets());

	_p->pTargetClass = _p->pGeneral->addStringEditor(__tr("Target Class"),&(_p->szDummy));
	_p->pTargetClass->lineEdit()->setEnabled(false);
	_p->pTargetClass->setCurrentValue(__tr("Identifying..."));

	_p->pReturnType = _p->pGeneral->addStringEditor(__tr("Return Type"),&(_p->oSettings.szReturnType));
	QObject::connect(_p->pReturnType,SIGNAL(edited()),this,SLOT(slotReturnTypeEdited()));

	_p->pFunctionName = _p->pGeneral->addStringEditor(__tr("Function Name"),&(_p->oSettings.szFunctionName));

	QString szText = pEditor->getSelectionText();
	QRegExp re(__ascii("[a-zA-Z_0-9]+"));
	if(re.exactMatch(szText))
		_p->pFunctionName->setCurrentValue(szText);

	QObject::connect(_p->pFunctionName,SIGNAL(edited()),this,SLOT(slotFunctionNameEdited()));

	_p->pParameters = _p->pGeneral->addStringEditor(__tr("Parameters"),&(_p->oSettings.szParameters));

	_p->pComment = _p->pGeneral->addStringEditor(__tr("Comment"),&(_p->oSettings.szComment));

	QList<QPair<int,QString> > lOptions2;
	lOptions2.append(QPair<int,QString>(C3Symbol::AccessLevelPublic,__tr("Public")));
	lOptions2.append(QPair<int,QString>(C3Symbol::AccessLevelProtected,__tr("Protected")));
	lOptions2.append(QPair<int,QString>(C3Symbol::AccessLevelPrivate,__tr("Private")));

	_p->pAccessLevel = _p->pGeneral->addEnumerationEditor<C3Symbol::AccessLevel>(
			__tr("Access Level"),
			&(_p->oSettings.eAccessLevel),
			lOptions2,
			0
		);
	_p->pAccessLevel->setEnabled(false);

	_p->pIsConst = _p->pGeneral->addBooleanEditor(__tr("Is Const"),&(_p->oSettings.bIsConst));
	_p->pIsVirtual = _p->pGeneral->addBooleanEditor(__tr("Is Virtual"),&(_p->oSettings.bIsVirtual));
	_p->pIsStatic = _p->pGeneral->addBooleanEditor(__tr("Is Static"),&(_p->oSettings.bIsStatic));

	QList<QPair<int,QString> > lOptions;
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberFunctionGenerator::AtCursorPosition,__tr("At Cursor Position")));
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberFunctionGenerator::InContextContainerDeclaration,__tr("In Class Declaration (Inlineable)")));
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberFunctionGenerator::InContextContainerDeclarationAndDefinition,__tr("In Class Declaration and Definition")));

	_p->pLocation = _p->pGeneral->addEnumerationEditor<C3TextEditorModeCPPMemberFunctionGenerator::Location>(
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

C3TextEditorModeCPPMemberFunctionGeneratorDialog::~C3TextEditorModeCPPMemberFunctionGeneratorDialog()
{
	if(_p->pClassDescription)
		delete _p->pClassDescription;
	delete _p;
}


void C3TextEditorModeCPPMemberFunctionGeneratorDialog::onContextIdentified(C3SymbolManagerSymbolDescription * pContextSymbol)
{
	if(!pContextSymbol->pContainingClass)
	{
		_p->pTargetClass->setCurrentValue(__tr("Not found - inserting at cursor"));
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
	lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberFunctionGenerator::AtCursorPosition,__tr("At Cursor Position")));

	if(pDeclarationEditor && ((pDeclarationEditor == _p->pEditor) || (pDefinitionEditor == _p->pEditor)))
	{
		lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberFunctionGenerator::InContextContainerDeclaration,__tr("In Class Declaration (Setter/Getter Inlineable)")));
		if(pDefinitionEditor)
		{
			lOptions.append(QPair<int,QString>(C3TextEditorModeCPPMemberFunctionGenerator::InContextContainerDeclarationAndDefinition,__tr("In Class Declaration and Definition")));
			_p->pLocation->setCurrentValue(C3TextEditorModeCPPMemberFunctionGenerator::InContextContainerDeclarationAndDefinition);
		} else {
			_p->pLocation->setCurrentValue(C3TextEditorModeCPPMemberFunctionGenerator::InContextContainerDeclaration);
		}

	} else {
		_p->pLocation->setCurrentValue(C3TextEditorModeCPPMemberFunctionGenerator::AtCursorPosition);
	}

	_p->pLocation->setEnabled(true);
	_p->pAccessLevel->setEnabled(true);

	_p->pAccessLevel->setCurrentValue(C3Symbol::AccessLevelPrivate);
}

bool C3TextEditorModeCPPMemberFunctionGeneratorDialog::commit()
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

	_p->oSettings.pClassDescription = _p->pClassDescription;
	
	C3TextEditorModeCPPMemberFunctionGenerator oGenerator(
			_p->pEditor,
			&(_p->oSettings)
		);

	if(!oGenerator.run())
		return false;

	return true;
}

void C3TextEditorModeCPPMemberFunctionGeneratorDialog::showEvent(QShowEvent * e)
{
	C3OptionsDialog::showEvent(e);

	_p->pReturnType->lineEdit()->setFocus();
}

void C3TextEditorModeCPPMemberFunctionGeneratorDialog::slotReturnTypeEdited()
{
	enableDisableButtons();
}

void C3TextEditorModeCPPMemberFunctionGeneratorDialog::slotFunctionNameEdited()
{
	enableDisableButtons();
}

bool C3TextEditorModeCPPMemberFunctionGeneratorDialog::isValid()
{
	if(_p->pReturnType->currentValue().isEmpty())
		return false;
	if(_p->pFunctionName->currentValue().isEmpty())
		return false;

	return true;
}

void C3TextEditorModeCPPMemberFunctionGeneratorDialog::enableDisableButtons()
{
	OKButton()->setEnabled(isValid());
}

