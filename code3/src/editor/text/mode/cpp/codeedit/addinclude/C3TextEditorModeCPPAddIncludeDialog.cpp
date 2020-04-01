//=============================================================================
//
//   File : C3TextEditorModeCPPAddIncludeDialog.cpp
//   Creation Date : 2016/09/24 02:27:13
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

#include "C3TextEditorModeCPPAddIncludeDialog.h"
#include "C3Link.h"
#include "C3Workspace.h"
#include "C3TextEditorModeCPPCodeGenerationUtils.h"
#include "C3TextEditor.h"
#include "C3OptionsWidgetContainer.h"
#include "C3OptionsWidget.h"
#include "C3RichTextItemDelegate.h"
#include "C3FindOperation.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QCheckBox>
#include <QMap>
#include <QDir>
#include <C3StringUtils.h>

class C3TextEditorModeCPPAddIncludeDialogPrivate
{
public:
	C3TextEditor * pEditor;
	
	QList<C3TextEditorSymbolInfo *> * pInfoList;
	
	C3OptionsWidget * pGeneral;
	
	QListWidget * pListWidget;
	QCheckBox * pBUseAngularParenthesis;
};

#define _p m_pC3TEMCPPAID

C3TextEditorModeCPPAddIncludeDialog::C3TextEditorModeCPPAddIncludeDialog(C3TextEditor * pEditor)
	: C3TextEditorModeCPPCodeEditDialog(
			__tr("Add Include File"),
			pEditor,
			IdentifySymbolAtCursor,
			C3OptionsWidgetContainer::HideLeftTreeWidget
		)
{
	_p = new C3TextEditorModeCPPAddIncludeDialogPrivate();
	
	_p->pEditor = pEditor;
	
	_p->pGeneral = new C3OptionsWidget(__tr("General"),parentForWidgets());
	
	addWidget(_p->pGeneral);

	_p->pListWidget = _p->pGeneral->addListWidget(0,0,1,4);
	_p->pListWidget->setItemDelegate(new C3RichTextItemDelegate(_p->pListWidget));

	
	QObject::connect(_p->pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *)));

	_p->pBUseAngularParenthesis = _p->pGeneral->addCheckBox(__tr("Use angular quotes (<*>)"),1,0,1,4);
	
	_p->pGeneral->gridLayout()->setRowStretch(0,1000);
	
	setMinimumSize(560,400);
	
	enableDisableButtons();
}

C3TextEditorModeCPPAddIncludeDialog::~C3TextEditorModeCPPAddIncludeDialog()
{
	if(_p->pInfoList)
		delete _p->pInfoList;
	delete _p;
}

void C3TextEditorModeCPPAddIncludeDialog::showEvent(QShowEvent * e)
{
	C3TextEditorModeCPPCodeEditDialog::showEvent(e);
	_p->pListWidget->setFocus();
}

void C3TextEditorModeCPPAddIncludeDialog::onSymbolIdentified(QList<C3TextEditorSymbolInfo *> * pInfoList)
{
	_p->pInfoList = pInfoList;
	
	_p->pListWidget->clear();
	
	if(!_p->pInfoList)
	{
		return;
	}
	
	QMap<QString,C3Link *> hFiles;
	
	foreach(C3TextEditorSymbolInfo * pInfo,*pInfoList)
	{	
		QString szPath = pInfo->oLink.path();
	
		if(szPath.endsWith(__ascii(".c"),Qt::CaseInsensitive))
			continue;
		if(szPath.endsWith(__ascii(".cpp"),Qt::CaseInsensitive))
			continue;
		if(szPath.endsWith(__ascii(".cxx"),Qt::CaseInsensitive))
			continue;
		if(szPath.endsWith(__ascii(".lxx"),Qt::CaseInsensitive))
			continue; // opencascade
		if(szPath.endsWith(__ascii(".cc"),Qt::CaseInsensitive))
			continue;
	
		int idx = szPath.lastIndexOf(QChar('/'));
		if(idx < 0)
			szPath.lastIndexOf(QChar('\\'));
		
		if(idx >= 0)
		{
			hFiles.insert(szPath.mid(idx+1),&(pInfo->oLink));

			if(idx > 0)
			{
				int idx2 = szPath.lastIndexOf(QChar('/'),idx-1);
				if(idx2 < 0)
					szPath.lastIndexOf(QChar('\\'),idx-1);

				if(idx2 >= 0)
					hFiles.insert(szPath.mid(idx2+1),&(pInfo->oLink));
			}
		} else {
			// doh
			hFiles.insert(szPath,&(pInfo->oLink));
		}
		
		// for Qt objects also use the <className> form!!!
		QString szSymbol = pInfo->oLink.value("symbol").toString();
		
		if(
				szSymbol.startsWith(QChar('Q')) &&
				szPath.endsWith(__ascii("%1%2.h").arg(QDir::separator()).arg(szSymbol.toLower()))
			)
		{
			hFiles.insert(szSymbol,&(pInfo->oLink)); // direct symbol include for Qt
		}
	}
	
	
	for(QMap<QString,C3Link *>::Iterator it = hFiles.begin();it != hFiles.end();++it)
	{
		QString szFile = it.key();
		
		QListWidgetItem * pItem = new QListWidgetItem(_p->pListWidget);

		static QString szStyle1 = __utf8(
				"font-family: Monospace;"
				"font-weight: bold;"
				"font-size: normal;"
				"color: #eeffa0;"
				"margin-bottom: 4px;"
			);
	
		static QString szStyle2 = __utf8(
				"font-family: Monospace;"
				"font-weight: 300;"
				"font-size: normal;"
				"color: #a0a0a0;"
				"margin-bottom: 4px;"
				"margin-left: 16px;"
			);

		static QString szStyle3 = __utf8(
				"font-family: Monospace;"
				"font-weight: 300;"
				"font-size: small;"
				"color: #808080;"
				"margin-left: 16px;"
			);
	
		QString sz = __utf8(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
					"<div style='%5'>%6</div>"
				)
					.arg(szStyle1).arg(szFile)
					.arg(szStyle2).arg(it.value()->description())
					.arg(szStyle3).arg(it.value()->text());

		// description contains "function definition"

		pItem->setText(sz);
		pItem->setData(Qt::UserRole,szFile);
		pItem->setData(Qt::UserRole+1,it.value()->path());
	}
	
	if(_p->pListWidget->count() > 0)
	{
		_p->pListWidget->setCurrentRow(0);
		_p->pListWidget->setFocus();
	}

	enableDisableButtons();
}

void C3TextEditorModeCPPAddIncludeDialog::enableDisableButtons()
{
	OKButton()->setEnabled(_p->pListWidget->currentRow() >= 0);
}

bool C3TextEditorModeCPPAddIncludeDialog::commit()
{
	QListWidgetItem * pItem = _p->pListWidget->currentItem();
	if(!pItem)
		return false;

	if(!C3OptionsDialog::commit())
	{
		qDebug("Commit failed!");
		return false;
	}

	QString szFile = pItem->data(Qt::UserRole).toString();
	if(szFile.isEmpty())
		return false;
		
	QString szFullPath = pItem->data(Qt::UserRole+1).toString();

	C3TextEditor::CursorAndScrollBarState oSave;

	_p->pEditor->saveCursorAndScrollBarState(oSave);

	QString szFileReplaced = szFile;
	szFileReplaced.replace(__ascii("."),__ascii("\\."));

	C3FindOperation oOp;

	_p->pEditor->cursorMoveTo(C3TextEditorRowColumn(0,0),false);

	oOp.setId(C3FindOperation::nextOperationId());
	oOp.setRegularExpression(QRegularExpression(
			__ascii("#include[ 	]+[\"<]%1[\">]").arg(szFileReplaced)
		));

	bool bRet = _p->pEditor->findOperationRun(&oOp);

	if(bRet)
	{
		_p->pEditor->restoreCursorAndScrollBarState(oSave);

		return QMessageBox::information(
				this,
				__tr("Include Present"),
				__tr("The include file %1 is already present").arg(szFile),
				__tr("Close"),__tr("Choose Other")
			) == 0;
	}
	
	C3Workspace::currentWorkspace()->path();

	_p->pEditor->restoreCursorAndScrollBarState(oSave);
	
	if(!C3TextEditorModeCPPCodeGenerationUtils::insertIncludeLine(
			_p->pEditor,
			_p->pBUseAngularParenthesis->isChecked() ? 
				__ascii("#include <%1>").arg(szFile) :
				__ascii("#include \"%1\"").arg(szFile)
		))
	{

		QMessageBox::information(
				this,
				__tr("Insertion Failed"),
				__tr("Failed to insert the #include line")
			);
		return false;
	}

	return true;
}

void C3TextEditorModeCPPAddIncludeDialog::slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *)
{
	enableDisableButtons();

	QListWidgetItem * pItem = _p->pListWidget->currentItem();
	if(!pItem)
		return;

	QString szFile = pItem->data(Qt::UserRole).toString();
	if(szFile.isEmpty())
		return;

	QString szFullPath = pItem->data(Qt::UserRole+1).toString();
	if(szFullPath.isEmpty())
		return;
		
	QString szWorkspacePath = C3Workspace::currentWorkspace()->path();
	if(szWorkspacePath.isEmpty())
		return;

	QFileInfo inf(szWorkspacePath);
	QString szShort = inf.absolutePath();
	
	bool bAngular;
	
	if(!szFullPath.startsWith(szShort))
	{
		bAngular = true; // out of workspace
	} else {
		QFileInfo fi1(szFullPath);
		QString sz1 = fi1.canonicalFilePath();
		QFileInfo fi2(_p->pEditor->path());
		QString sz2 = fi2.canonicalFilePath();

		int iCC = C3StringUtils::commonCharacterCount(sz1,sz2);
		QString szCommon = sz1.left(iCC);

		if(
				szCommon.contains(__literal("/src/"),Qt::CaseInsensitive) ||
				szCommon.contains(__literal("/source/"),Qt::CaseInsensitive) ||
				szCommon.contains(__literal("/sources/"),Qt::CaseInsensitive)
			)
		{
			// have common "src" root. assume same project
			bAngular = false;
		} else {
			
			sz1.remove(0,iCC);
			sz2.remove(0,iCC);
			
			if(sz1.contains(__ascii("src/")) || sz2.contains(__ascii("src/")))
			{
				bAngular = true;
			} else {
				// if there are more than 4 levels of difference, assume it's a different project.

				int iSeps1 = sz1.count(QDir::separator());
				int iSeps2 = sz2.count(QDir::separator());
	
				bAngular = (iSeps1 + iSeps2) > 4;
			}
		}
	}

	_p->pBUseAngularParenthesis->setChecked(bAngular);
}
