//=============================================================================
//
//   File : C3TextEditorModeCPPGenerateEnumSwitchDialog.cpp
//   Creation Date : 2016/10/15 04:03:54
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

#include "C3TextEditorModeCPPGenerateEnumSwitchDialog.h"
#include "C3StringUtils.h"
#include "C3SymbolScope.h"
#include "C3OptionsWidgetContainer.h"
#include "C3Symbol.h"
#include "C3RichTextItemDelegate.h"
#include "C3OptionsWidget.h"
#include <QList>
#include <QListWidget>
#include <QPushButton>
#include <QGridLayout>
#include "C3TextEditorSymbolInfo.h"
#include "C3TextEditorSymbolContext.h"
#include "C3TextEditor.h"
#include "C3SymbolManager.h"

class C3TextEditorModeCPPGenerateEnumSwitchDialogPrivate
{
public:
	bool bCanGenerate;
	
	C3OptionsWidget * pGeneral;
	QListWidget * pListWidget;
};

#define _p m_pC3TEMCPPGESD

C3TextEditorModeCPPGenerateEnumSwitchDialog::C3TextEditorModeCPPGenerateEnumSwitchDialog(C3TextEditor * pEditor)
	: C3TextEditorModeCPPCodeEditDialog(
			__tr("Generate Enum Switch"),
			pEditor,
			ScanEditor,
			C3OptionsWidgetContainer::HideLeftTreeWidget
		)
{
	_p = new C3TextEditorModeCPPGenerateEnumSwitchDialogPrivate();
	_p->bCanGenerate = false;

	_p->pGeneral = new C3OptionsWidget(__tr("General"),parentForWidgets());
	
	addWidget(_p->pGeneral);

	_p->pListWidget = _p->pGeneral->addListWidget(0,0,1,4);
	_p->pListWidget->setItemDelegate(new C3RichTextItemDelegate(_p->pListWidget));

	QObject::connect(_p->pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *)));
	
	_p->pGeneral->gridLayout()->setRowStretch(0,1000);
	
	setMinimumSize(560,400);


	enableDisableButtons();
}

C3TextEditorModeCPPGenerateEnumSwitchDialog::~C3TextEditorModeCPPGenerateEnumSwitchDialog()
{
	delete _p;
}

void C3TextEditorModeCPPGenerateEnumSwitchDialog::onAllTasksFinished()
{
	QList<C3TextEditorSymbolInfo * > lInfoList;

	C3TextEditorSymbolContext oContext;

	editor()->computeSymbolContext(
			editor()->cursorPosition(),
			oContext,
			C3TextEditor::ExtendSymbolToRight
		);

	C3SymbolManager::instance()->lockStore();

	C3SymbolManager::instance()->storeLockedIdentifySymbol(
			&oContext,
			&lInfoList,
			true,
			true,
			C3Symbol::Cpp
		);

	C3SymbolScope * pFileContext = C3SymbolManager::instance()->storeLockedResolveFileContext(
			oContext.szFilePath,
			oContext.oRange.end.row
		);

	if(!pFileContext)
	{
		qDeleteAll(lInfoList);
		C3SymbolManager::instance()->unlockStore();
		return;
	}


	foreach(C3TextEditorSymbolInfo * pInfo,lInfoList)
	{
		if(!pInfo->pSymbol)
			continue;
			
		if(!(pInfo->pSymbol->type() & (
				C3Symbol::MemberVariable | C3Symbol::LocalVariable | C3Symbol::GlobalVariable |
				C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition
			)))
			continue;

		C3SymbolScope * pScope = C3SymbolManager::instance()->storeLockedResolveTypeOfSymbol(
				pFileContext->file(),
				pFileContext,
				pInfo->pSymbol
			);
	
		if(!pScope)
			continue;

		if(pScope->type() != C3Symbol::Enumeration)
			continue;

		QListWidgetItem * it = new QListWidgetItem(_p->pListWidget);
		it->setText(pInfo->oLink.toHtml());
		it->setData(Qt::UserRole,QVariant((qlonglong)(pInfo->pSymbol)));

		_p->pListWidget->addItem(it);
	}

	if(_p->pListWidget->count() > 0)
	{
		_p->pListWidget->setCurrentRow(0);
		_p->pListWidget->setFocus();
	}

	qDeleteAll(lInfoList);

	C3SymbolManager::instance()->unlockStore();

	enableDisableButtons();
}

void C3TextEditorModeCPPGenerateEnumSwitchDialog::enableDisableButtons()
{
	OKButton()->setEnabled(_p->pListWidget->currentItem());
}

bool C3TextEditorModeCPPGenerateEnumSwitchDialog::commit()
{
	QListWidgetItem * pItem = _p->pListWidget->currentItem();
	if(!pItem)
		return false;
	
	bool ok;
	
	qlonglong llSymbolPointer = pItem->data(Qt::UserRole).toULongLong(&ok);
	if(!ok)
		return false;

	QList<C3TextEditorSymbolInfo * > lInfoList;

	C3TextEditorSymbolContext oContext;

	editor()->computeSymbolContext(
			editor()->cursorPosition(),
			oContext,
			C3TextEditor::ExtendSymbolToRight
		);

	C3SymbolManager::instance()->lockStore();

	C3SymbolManager::instance()->storeLockedIdentifySymbol(
			&oContext,
			&lInfoList,
			true,
			true,
			C3Symbol::Cpp
		);

	C3SymbolScope * pFileContext = C3SymbolManager::instance()->storeLockedResolveFileContext(
			oContext.szFilePath,
			oContext.oRange.end.row
		);

	if(!pFileContext)
	{
		qDeleteAll(lInfoList);
		C3SymbolManager::instance()->unlockStore();
		return false;
	}

	C3SymbolScope * pEnum = NULL;

	foreach(C3TextEditorSymbolInfo * pInfo,lInfoList)
	{
		if(!pInfo->pSymbol)
			continue;
			
		if(!(pInfo->pSymbol->type() & (
				C3Symbol::MemberVariable | C3Symbol::LocalVariable | C3Symbol::GlobalVariable |
				C3Symbol::FunctionPrototype | C3Symbol::FunctionDefinition
			)))
			continue;

		C3SymbolScope * pScope = C3SymbolManager::instance()->storeLockedResolveTypeOfSymbol(
				pFileContext->file(),
				pFileContext,
				pInfo->pSymbol
			);
	
		if(!pScope)
			continue;

		if(pScope->type() != C3Symbol::Enumeration)
			continue;

		if(((qlonglong)(pInfo->pSymbol)) == llSymbolPointer)
		{
			pEnum = pScope;
			break;
		}
	}

	if(!pEnum)
	{
		qDebug("Ops... symbol store probably changed in the meantime");
		qDeleteAll(lInfoList);
		C3SymbolManager::instance()->unlockStore();
		return false;
	}
	
	QStringList lContainingScope;
	if(pFileContext->resolvedContainingScope())
		lContainingScope = pFileContext->resolvedContainingScope()->buildFullyQualifiedNameAsList();
	
	QStringList lEnumScope;
	if(pEnum->resolvedContainingScope())
		lEnumScope = pEnum->resolvedContainingScope()->buildFullyQualifiedNameAsList();
	
	// remove the common part
	while((lContainingScope.count() > 0) && (lEnumScope.count() > 0))
	{
		if(lContainingScope.first() != lEnumScope.first())
			break;
		lContainingScope.removeFirst();
		lEnumScope.removeFirst();
	}
	
	QString szEnumScope;
	foreach(QString szPart,lEnumScope)
	{
		if(szEnumScope.isEmpty())
			szEnumScope = szPart;
		else
			szEnumScope = __ascii("%1::%2").arg(szEnumScope,szPart);
	}

	if(!szEnumScope.isEmpty())
		szEnumScope.append(__ascii("::"));


	QString szLines;
	
	if(pEnum->symbols())
	{
		QMultiMap<QString,C3Symbol *>::Iterator it;;
	
		for(it = pEnum->symbols()->begin();it != pEnum->symbols()->end();++it)
		{
			szLines.append(
				__ascii(
						"\tcase %1%2:\n"
						"\tbreak;\n"
					).arg(szEnumScope,it.value()->identifier())
			);
		}
	}

	QString szLine = editor()->getLineText(oContext.oRange.start.row);
	
	int tabs = 0;
	while(szLine.startsWith(QChar('\t')))
	{
		tabs++;
		szLine = szLine.mid(1);
	}

	szLine = szLine.trimmed();

	if(szLine.startsWith(__ascii("switch(")))
	{
		szLine = szLine.mid(7);
		if(szLine.endsWith(__ascii(")")))
			szLine = szLine.mid(0,szLine.length()-1);
	}

	// FIXME: Want scope names until a visible one!

	QString szCode = __ascii(
			"switch(%1)\n"
			"{\n"
				"%2"
			"\tdefault:\n"
			"\tbreak;\n"
			"}\n"
		).arg(szLine,szLines);

	while(tabs > 0)
	{
		szCode = C3StringUtils::indentLines(szCode);
		tabs--;
	}

	oContext.oRange.start.col = 0;
	oContext.oRange.end.col = 999999; // end of line

	editor()->selectRange(oContext.oRange);
	editor()->insertText(szCode);

	qDeleteAll(lInfoList);
	C3SymbolManager::instance()->unlockStore();

	return true;
}

void C3TextEditorModeCPPGenerateEnumSwitchDialog::slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *)
{
}