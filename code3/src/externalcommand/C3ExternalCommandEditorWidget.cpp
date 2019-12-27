//=============================================================================
//
//   File : C3ExternalCommandEditorWidget.cpp
//   Creation Date : 2015/11/19 23:05:53
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

#include "C3ExternalCommandEditorWidget.h"

#include "C3OptionEditorString.h"
#include "C3OptionEditorBoolean.h"
#include "C3OptionEditorEnumeration.h"

#include <QStyleFactory>
#include <QCheckBox>
#include <QFileDialog>
#include <QPushButton>

class C3ExternalCommandEditorWidgetPrivate
{
public:
	C3ExternalCommand * pCommand;
	
	C3OptionEditorString * pNameEditor;
	C3OptionEditorString * pIconEditor;
	C3OptionEditorString * pWorkingDirectoryEditor;
	C3OptionEditorText * pCommandEditor;
	C3OptionEditorString * pShortcutEditor;
	C3OptionEditorBoolean * pSaveAllFilesEditor;
	C3OptionEditorBoolean * pClearOutputOnStartEditor;
	C3OptionEditorBoolean * pNeedsPasswordEditor;
	C3OptionEditorBoolean * pHighlightPathsEditor;
	C3OptionEditorEnumeration<C3ExternalCommand::Type> * pTypeEditor;
};

#define _p m_pC3ECEW

C3ExternalCommandEditorWidget::C3ExternalCommandEditorWidget(QWidget * pParent,const C3ExternalCommand &cmd)
	: C3OptionsWidget(cmd.name(),pParent)
{
	_p = new C3ExternalCommandEditorWidgetPrivate();
	_p->pCommand = new C3ExternalCommand(cmd);

	_p->pNameEditor = addStringEditor(__tr("Name"),&(_p->pCommand->m_szName));
	QObject::connect(_p->pNameEditor,SIGNAL(edited()),this,SLOT(slotNameEdited()));
	
	QList< QPair<int,QString> > lOptions;
	lOptions.append(QPair<int,QString>(C3ExternalCommand::DetachedProcess,__tr("Detached Process")));
	lOptions.append(QPair<int,QString>(C3ExternalCommand::DetachedShellScript,__tr("Detached Shell Script")));
	lOptions.append(QPair<int,QString>(C3ExternalCommand::ControlledProcess,__tr("Controlled Process")));
	lOptions.append(QPair<int,QString>(C3ExternalCommand::ControlledShellScript,__tr("Controlled Shell Script")));
	// ControlledProcessWithArguments is not used here
	
	_p->pTypeEditor = addEnumerationEditor(
			__tr("Type"),
			&(_p->pCommand->m_eType),
			lOptions
		);

	QObject::connect(_p->pTypeEditor,SIGNAL(edited()),this,SLOT(slotTypeEdited()));

	_p->pIconEditor = addStringEditor(__tr("Icon"),&(_p->pCommand->m_szIcon),C3OptionEditorString::AddSelectorButton);
	_p->pIconEditor->selectorButton()->setText(QString("..."));
	QObject::connect(_p->pIconEditor->selectorButton(),SIGNAL(clicked()),this,SLOT(slotSelectIcon()));
	_p->pShortcutEditor = addStringEditor(__tr("Shortcut"),&(_p->pCommand->m_szShortcut));
	_p->pWorkingDirectoryEditor = addStringEditor(__tr("Working Directory"),&(_p->pCommand->m_szWorkingDirectory));
	_p->pCommandEditor = addTextEditor(__tr("Command"),&(_p->pCommand->m_szCommand));
	
	_p->pSaveAllFilesEditor = addBooleanEditor(__tr("Save All Files Before Launch"),&(_p->pCommand->m_bSaveAllFiles));
	_p->pClearOutputOnStartEditor = addBooleanEditor(__tr("Clear Output Before Launch"),&(_p->pCommand->m_bClearOutputOnStart));
	_p->pNeedsPasswordEditor = addBooleanEditor(__tr("Needs Password"),&(_p->pCommand->m_bNeedsPassword));
	
	_p->pNeedsPasswordEditor->checkBox()->setToolTip(
			__tr("Before running the command you will be asked for a password. It will be cached on subsequent runs and substituted in place of %%password%%. Use 'echo %%password%% | sudo -S <command>' in shellscripts.")
		);

	_p->pHighlightPathsEditor = addBooleanEditor(__tr("Highlight Paths in Output"),&(_p->pCommand->m_bHighlightPaths));

	enableDisableClearOutputOnStart();
}

C3ExternalCommandEditorWidget::~C3ExternalCommandEditorWidget()
{
	delete _p->pCommand;
	delete _p;
}

const C3ExternalCommand & C3ExternalCommandEditorWidget::command() const
{
	return *(_p->pCommand);
}

void C3ExternalCommandEditorWidget::slotNameEdited()
{
	setTitle(_p->pNameEditor->currentValue());
}

void C3ExternalCommandEditorWidget::enableDisableClearOutputOnStart()
{
	C3ExternalCommand::Type t = _p->pTypeEditor->currentValue();
	switch(t)
	{
		case C3ExternalCommand::ControlledProcess:
		case C3ExternalCommand::ControlledProcessWithArguments:
		case C3ExternalCommand::ControlledShellScript:
			_p->pClearOutputOnStartEditor->setEnabled(true);
		break;
		default:
			_p->pClearOutputOnStartEditor->setEnabled(false);
		break;
	}
}

void C3ExternalCommandEditorWidget::slotTypeEdited()
{
	enableDisableClearOutputOnStart();
}

void C3ExternalCommandEditorWidget::slotSelectIcon()
{
	QString sz = QFileDialog::getOpenFileName(
			this,
			__tr("Select Icon"),
			QString("/usr/local/share/code3/1.0/images/")
		);
		
	if(sz.isEmpty())
		return;
		
	_p->pIconEditor->setCurrentValue(sz);
}

bool C3ExternalCommandEditorWidget::commit()
{
	bool bRet = C3OptionsWidget::commit();
	if(!bRet)
		return false;
	return true;
}
