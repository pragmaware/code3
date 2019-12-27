//=============================================================================
//
//   File : C3FindWidget.cpp
//   Creation Date : dom 01 nov 2015 20:21:28
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

#include "C3FindWidget.h"
#include "C3MainWindow.h"
#include "C3Editor.h"
#include "C3FindOperation.h"
#include "C3FindTarget.h"
#include "C3Workspace.h"
#include "C3ExternalCommand.h"
#include "C3ExternalCommandManager.h"
#include "C3ExternalToolManager.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSettings>
#include <QStringList>
#include <QShowEvent>
#include <QHideEvent>
#include <QMessageBox>
#include <QCompleter>

#define MAX_RECENT_ENTRIES 20

class C3FindWidgetPrivate
{
public:
	QGridLayout * pLayout;

	QComboBox * pSearchStringComboBox;
	QComboBox * pReplaceStringComboBox;
	QComboBox * pSearchTypeComboBox;
	QComboBox * pSearchDirectionComboBox;
	QComboBox * pSearchCaseSensitivityComboBox;
	QComboBox * pSearchScopeComboBox;
	
	QPushButton * pFindButton;
	QPushButton * pReplaceButton;
	QPushButton * pReplaceAndFindButton;
	QPushButton * pReplaceAllButton;
	
	QStringList lRecentSearchStrings;
	QStringList lRecentReplaceStrings;
	
	C3FindOperation * pOperation;
	QList<C3FindTarget *> lFindTargets;
};

#define _p m_pC3FW

C3FindWidget::C3FindWidget(QWidget * pParent)
	: QWidget(pParent)
{
	_p = new C3FindWidgetPrivate();

	_p->pLayout = new QGridLayout(this);
	_p->pOperation = NULL;

	QLabel * l;
	
	l = new QLabel(this);
	l->setText(__tr("Find:"));
	_p->pLayout->addWidget(l,0,0);

	_p->pSearchStringComboBox = new QComboBox(this);
	_p->pSearchStringComboBox->setEditable(true);
	_p->pSearchStringComboBox->completer()->setCaseSensitivity(Qt::CaseSensitive);
	_p->pSearchStringComboBox->setInsertPolicy(QComboBox::NoInsert);
	QObject::connect(_p->pSearchStringComboBox,SIGNAL(editTextChanged(const QString &)),this,SLOT(slotSearchStringComboBoxEditTextChanged(const QString &)));
	QObject::connect(_p->pSearchStringComboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(slotSearchStringComboBoxLineEditReturnPressed()));
	_p->pLayout->addWidget(_p->pSearchStringComboBox,0,1,1,3);

	_p->pFindButton = new QPushButton(this);
	_p->pFindButton->setText(__tr("Find"));
	_p->pLayout->addWidget(_p->pFindButton,0,4);
	QObject::connect(_p->pFindButton,SIGNAL(clicked()),this,SLOT(slotFindButtonClicked()));


	l = new QLabel(this);
	l->setText(__tr("Replace:"));
	_p->pLayout->addWidget(l,1,0);

	_p->pReplaceStringComboBox = new QComboBox(this);
	_p->pReplaceStringComboBox->setEditable(true);
	_p->pReplaceStringComboBox->completer()->setCaseSensitivity(Qt::CaseSensitive);
	_p->pReplaceStringComboBox->setInsertPolicy(QComboBox::NoInsert);
	_p->pReplaceStringComboBox->setToolTip(
			__utf8(
				"In 'Regular Expression Match' mode you can use parenthesized subexpression captures.<br>"
				"\\0 maps to the whole match, "
				"\\1 maps to the first subexpression, etc.<br>"
				"Also \\n maps to a newline and \\t maps to a tab character."
			)
		);
	QObject::connect(_p->pReplaceStringComboBox,SIGNAL(editTextChanged(const QString &)),this,SLOT(slotReplaceStringComboBoxEditTextChanged(const QString &)));
	_p->pLayout->addWidget(_p->pReplaceStringComboBox,1,1,1,3);

	_p->pReplaceButton = new QPushButton(this);
	_p->pReplaceButton->setText(__tr("Replace"));
	_p->pLayout->addWidget(_p->pReplaceButton,1,4);
	QObject::connect(_p->pReplaceButton,SIGNAL(clicked()),this,SLOT(slotReplaceButtonClicked()));




	l = new QLabel(this);
	l->setText(__tr("Mode:"));
	_p->pLayout->addWidget(l,2,0);

	_p->pSearchTypeComboBox = new QComboBox(this);
	QObject::connect(_p->pSearchTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSearchTypeComboBoxCurrentIndexChanged(int)));
	_p->pLayout->addWidget(_p->pSearchTypeComboBox,2,1);


	l = new QLabel(this);
	l->setText(__utf8(" %1").arg(__tr("Case:")));
	_p->pLayout->addWidget(l,2,2);

	_p->pSearchCaseSensitivityComboBox = new QComboBox(this);
	QObject::connect(_p->pSearchCaseSensitivityComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSearchCaseSensitivityComboBoxCurrentIndexChanged(int)));
	_p->pLayout->addWidget(_p->pSearchCaseSensitivityComboBox,2,3);


	_p->pReplaceAndFindButton = new QPushButton(this);
	_p->pReplaceAndFindButton->setText(__tr("Replace and Find"));
	_p->pLayout->addWidget(_p->pReplaceAndFindButton,2,4);
	QObject::connect(_p->pReplaceAndFindButton,SIGNAL(clicked()),this,SLOT(slotReplaceAndFindButtonClicked()));





	l = new QLabel(this);
	l->setText(__tr("Direction:"));
	_p->pLayout->addWidget(l,3,0);

	_p->pSearchDirectionComboBox = new QComboBox(this);
	QObject::connect(_p->pSearchDirectionComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSearchDirectionComboBoxCurrentIndexChanged(int)));
	_p->pLayout->addWidget(_p->pSearchDirectionComboBox,3,1);


	l = new QLabel(this);
	l->setText(__utf8(" %1").arg(__tr("Scope:")));
	_p->pLayout->addWidget(l,3,2);

	_p->pSearchScopeComboBox = new QComboBox(this);
	QObject::connect(_p->pSearchScopeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSearchScopeComboBoxCurrentIndexChanged(int)));
	_p->pLayout->addWidget(_p->pSearchScopeComboBox,3,3);


	_p->pReplaceAllButton = new QPushButton(this);
	_p->pReplaceAllButton->setText(__tr("Replace All"));
	_p->pLayout->addWidget(_p->pReplaceAllButton,3,4);
	QObject::connect(_p->pReplaceAllButton,SIGNAL(clicked()),this,SLOT(slotReplaceAllButtonClicked()));


	_p->pLayout->setSpacing(1);
	_p->pLayout->setColumnStretch(1,100);

	_p->pLayout->setColumnStretch(3,100);

	loadSettings();

	_p->pSearchTypeComboBox->addItem(__tr("String Search"),__utf8("string"));
	_p->pSearchTypeComboBox->addItem(__tr("Wildcard Search"),__utf8("wildcard"));
	_p->pSearchTypeComboBox->addItem(__tr("Regular Expression Match"),__utf8("regexp"));

	_p->pSearchDirectionComboBox->addItem(__tr("Forward"),__utf8("forward"));
	_p->pSearchDirectionComboBox->addItem(__tr("Backward"),__utf8("backward"));

	_p->pSearchCaseSensitivityComboBox->addItem(__tr("Case Sensitive"),__utf8("casesensitive"));
	_p->pSearchCaseSensitivityComboBox->addItem(__tr("Case Insensitive"),__utf8("caseinsensitive"));

	_p->pSearchScopeComboBox->addItem(__tr("Current Document"),__utf8("document"));
	_p->pSearchScopeComboBox->addItem(__tr("Current Document Selection"),__utf8("selection"));
	_p->pSearchScopeComboBox->addItem(__tr("All Open Documents"),__utf8("allopen"));
	_p->pSearchScopeComboBox->addItem(__tr("Grep in Current Directory"),__utf8("grep.current"));
	_p->pSearchScopeComboBox->addItem(__tr("Grep in Workspace"),__utf8("grep.workspace"));
	_p->pSearchScopeComboBox->addItem(__tr("Grep in Workspace and Include Directories"),__utf8("grep.workspace.and.includes"));

	foreach(QString szRecent,_p->lRecentSearchStrings)
		_p->pSearchStringComboBox->addItem(szRecent);

	foreach(QString szRecent,_p->lRecentReplaceStrings)
		_p->pReplaceStringComboBox->addItem(szRecent);

}

C3FindWidget::~C3FindWidget()
{
	saveSettings();

	focusCurrentEditor();

	if(_p->pOperation)
		delete _p->pOperation;

	delete _p;
}

void C3FindWidget::grepInWorkspace(const QString &szText)
{
	grepInScope(szText,__ascii("grep.workspace"));
}

void C3FindWidget::grepInCurrentDirectory(const QString &szText)
{
	grepInScope(szText,__ascii("grep.current"));
}

void C3FindWidget::grepInScope(const QString &szText,const QString &szScope)
{
	_p->pSearchStringComboBox->lineEdit()->setText(szText);
	comboBoxSelectByData(_p->pSearchScopeComboBox,szScope);
	comboBoxSelectByData(_p->pSearchCaseSensitivityComboBox,__ascii("casesensitive"));
	comboBoxSelectByData(_p->pSearchTypeComboBox,__ascii("string"));
	_p->pSearchStringComboBox->lineEdit()->selectAll();

	find();
}

QString C3FindWidget::comboBoxGetSelectedData(QComboBox * pBox)
{
	int i = pBox->currentIndex();
	if(i < 0)
		return QString();
	return pBox->itemData(i).toString();
}

void C3FindWidget::comboBoxSelectByData(QComboBox * pBox,const QString &szData)
{
	int ic = pBox->count();
	for(int i=0;i<ic;i++)
	{
		QString szD = pBox->itemData(i).toString();
		if(szD == szData)
		{
			pBox->setCurrentIndex(i);
			return;
		}
	}
}

void C3FindWidget::rebuildFindOperation()
{
	if(_p->pOperation)
		delete _p->pOperation;
	_p->pOperation = new C3FindOperation();

	_p->pFindButton->setText(__tr("Find Next"));
	
	_p->pOperation->setId(C3FindOperation::nextOperationId());

	_p->lFindTargets.clear();

	QString szDir = comboBoxGetSelectedData(_p->pSearchDirectionComboBox);
	if(szDir == __utf8("backward"))
		_p->pOperation->setDirection(C3FindOperation::Backward);
	else
		_p->pOperation->setDirection(C3FindOperation::Forward);

	_p->lFindTargets.clear();

	QRegularExpression::PatternOptions oOptions = QRegularExpression::OptimizeOnFirstUsageOption;

	QString szCase = comboBoxGetSelectedData(_p->pSearchCaseSensitivityComboBox);
	if(szCase == __utf8("caseinsensitive"))
		oOptions |= QRegularExpression::CaseInsensitiveOption;

	QString szScope = comboBoxGetSelectedData(_p->pSearchScopeComboBox);
	if(szScope == __utf8("document"))
	{
		// document
		C3Editor * pEditor = C3MainWindow::instance()->currentEditor();
		if(pEditor)
			_p->lFindTargets.append(pEditor);
		_p->pOperation->setScope(C3FindOperation::Document);
	} else if(szScope == __utf8("selection"))
	{
		// selection
		C3Editor * pEditor = C3MainWindow::instance()->currentEditor();
		if(pEditor)
			_p->lFindTargets.append(pEditor);
		_p->pOperation->setScope(C3FindOperation::Selection);
	} else if(szScope.startsWith(__utf8("grep.")))
	{
		Q_ASSERT(false); // we should never be here.
	} else {
		// all open

		QHash<int,C3Editor *> & hEditors = C3Workspace::currentWorkspace()->editors();

		foreach(C3Editor * ed,hEditors)
			_p->lFindTargets.append(ed);

		_p->pOperation->setScope(C3FindOperation::Document);
	}

	QString szText = _p->pSearchStringComboBox->currentText();

	bool bContained = _p->lRecentSearchStrings.contains(szText);
	if(bContained)
		_p->lRecentSearchStrings.removeAll(szText);
	_p->lRecentSearchStrings.insert(0,szText);
	while(_p->lRecentSearchStrings.count() > MAX_RECENT_ENTRIES)
		_p->lRecentSearchStrings.takeLast();
	if(!bContained)
		_p->pSearchStringComboBox->insertItem(0,szText);

	QString szType = comboBoxGetSelectedData(_p->pSearchTypeComboBox);
	if(szType == __utf8("string"))
	{
		// string
		szText = QRegularExpression::escape(szText);
		//oOptions |= QRegularExpression::DontCaptureOption; <-- we need this to figure out the matched length
	} else if(szType == __utf8("wildcard"))
	{
		// wildcard
		szText = QRegularExpression::escape(szText);
		szText.replace("\\*",".*");
		szText.replace("\\.",".");
		//oOptions |= QRegularExpression::DontCaptureOption; <-- we need this to figure out the matched length
	} else {
		// regexp
		// nothing to change
		_p->pOperation->setCaptureReplacementsEnabled(true);
	}

	_p->pOperation->setRegularExpression(
			QRegularExpression(
					szText,
					oOptions
				)
		);
}

void C3FindWidget::resetFindOperation()
{
	if(_p->pOperation)
	{
		_p->pFindButton->setText(__tr("Find"));
		delete _p->pOperation;
		_p->pOperation = NULL;
	}
	_p->lFindTargets.clear();
}


void C3FindWidget::showEvent(QShowEvent * e)
{
	enableDisableButtons();

	C3Editor * pEditor = C3MainWindow::instance()->currentEditor();
	if(pEditor && pEditor->supportsFindOperations())
	{
		QString szInitial = pEditor->findOperationGetInitialSearchString();
		if(!szInitial.isEmpty())
		{
			_p->pSearchStringComboBox->lineEdit()->setText(szInitial);
			comboBoxSelectByData(_p->pSearchScopeComboBox,__utf8("selection"));
		}
		_p->pSearchStringComboBox->lineEdit()->selectAll();
	}

	_p->pSearchStringComboBox->lineEdit()->setFocus();
}


void C3FindWidget::hideEvent(QHideEvent * e)
{
	focusCurrentEditor();
}

void C3FindWidget::focusCurrentEditor()
{
	if(!C3MainWindow::instance())
		return; // closing down

	C3Editor * pEditor = C3MainWindow::instance()->currentEditor();
	if(!pEditor)
		return;
	
	pEditor->setFocus();
}

void C3FindWidget::editorClosed(C3Editor * pEditor)
{
	// FIXME: We probably could just remove the editor from the list?
	resetFindOperation();
}

void C3FindWidget::currentEditorChanged()
{
	enableDisableButtons();
}

void C3FindWidget::enableDisableButtons()
{
	C3Editor * pEditor = C3MainWindow::instance()->currentEditor();
	if(pEditor)
	{
		if(!pEditor->supportsFindOperations())
			pEditor = NULL;
	}

	bool bGotSearchString = !_p->pSearchStringComboBox->currentText().isEmpty();

	QString szScope = comboBoxGetSelectedData(_p->pSearchScopeComboBox);
	bool bScopeIsGrep = szScope.startsWith(__utf8("grep."));

	bool bGotOperationAndTargets = _p->pOperation && _p->lFindTargets.count();

	//qDebug("ENABLE REPLACE ALL %d",bEnableReplaceAll);

	_p->pFindButton->setEnabled((pEditor || bScopeIsGrep) && bGotSearchString);
	_p->pReplaceButton->setEnabled((!bScopeIsGrep) && pEditor && bGotOperationAndTargets);
	_p->pReplaceAndFindButton->setEnabled((!bScopeIsGrep) && pEditor && bGotOperationAndTargets);
	_p->pReplaceAllButton->setEnabled(!bScopeIsGrep);
	_p->pReplaceStringComboBox->setEnabled(!bScopeIsGrep);
	_p->pSearchDirectionComboBox->setEnabled(!bScopeIsGrep);
}

void C3FindWidget::slotReplaceStringComboBoxEditTextChanged(const QString &szText)
{
	//resetFindOperation(); <-- does not invalidate current search
	enableDisableButtons();
}

void C3FindWidget::slotSearchStringComboBoxEditTextChanged(const QString &szText)
{
	resetFindOperation();
	enableDisableButtons();
}

void C3FindWidget::slotSearchTypeComboBoxCurrentIndexChanged(int iIdx)
{
	resetFindOperation();
	enableDisableButtons();
}

void C3FindWidget::slotSearchDirectionComboBoxCurrentIndexChanged(int iIdx)
{
	resetFindOperation();
	enableDisableButtons();
}

void C3FindWidget::slotSearchScopeComboBoxCurrentIndexChanged(int iIdx)
{
	resetFindOperation();
	enableDisableButtons();
}

void C3FindWidget::slotSearchCaseSensitivityComboBoxCurrentIndexChanged(int iIdx)
{
	resetFindOperation();
	enableDisableButtons();
}

void C3FindWidget::slotSearchStringComboBoxLineEditReturnPressed()
{
	if(!_p->pFindButton->isEnabled())
		return;
	find();
}

void C3FindWidget::slotFindButtonClicked()
{
	find();
}

void C3FindWidget::slotReplaceButtonClicked()
{
	replace();
}

void C3FindWidget::slotReplaceAndFindButtonClicked()
{
	replaceAndFind();
}

void C3FindWidget::slotReplaceAllButtonClicked()
{
	replaceAll();
}

void C3FindWidget::findInternal()
{
	if(!_p->pOperation)
		rebuildFindOperation();

	if(_p->pOperation->regularExpression().pattern().isEmpty())
		return; // !

	for(;;)
	{
		if(_p->lFindTargets.isEmpty())
		{
			QMessageBox::information(this,__tr("No More Matches"),__tr("No More Matches Found"));
			resetFindOperation();
			return;
		}

		C3FindTarget * t = _p->lFindTargets.first();
		Q_ASSERT(t);

		if(t->findOperationRun(_p->pOperation))
		{
			if(t->type() == C3FindTarget::Editor)
			{
				C3Editor * ed = dynamic_cast<C3Editor *>(t);
				Q_ASSERT(ed);
				if(ed != C3MainWindow::instance()->currentEditor())
					C3MainWindow::instance()->activateEditor(ed);
			}
			return; // done
		}

		_p->lFindTargets.takeFirst();
	}
}

void C3FindWidget::findGrep()
{
	QString szGrep = C3ExternalToolManager::instance()->findTool("grep");
	if(szGrep.isEmpty())
	{
		QMessageBox::information(this,__tr("Grep Tool Not Found"),__tr("Could not find grep executable"));
		return;
	}
	
	C3ExternalCommand cmd;
	
	cmd.setType(C3ExternalCommand::ControlledProcessWithArguments);
	cmd.setCommand(szGrep);
	cmd.setSaveAllFiles(false);
	cmd.setHighlightPaths(true);
	cmd.setClearOutputOnStart(true);

	QStringList lArgs;

	// skip devices
	lArgs.append("-D");
	lArgs.append("skip");
	
	// recursive but don't follow symbolic links
	lArgs.append("-r");

	// append file name
	lArgs.append("-H");
	
	// append line number
	lArgs.append("-n");

	// suppress error messages about non-existing or unreadable files
	lArgs.append("-s");
	
	// limit number of matches to 10000
	lArgs.append("-m");
	lArgs.append("10000");
	
	// recursive following symbolic links
	//lArgs.append("-R");

	// ignore binary files at all
	lArgs.append("-I");

	QString szSlash("/");
	QString szBSlash("\\");

	QStringList lExclusionPatterns = C3Workspace::currentWorkspace()->fileExclusionPatterns();
	foreach(QString szPattern,lExclusionPatterns)
	{
		if(szPattern.contains(szSlash) || szPattern.contains(szBSlash))
		{
			lArgs.append(__utf8("--exclude-dir=%1").arg(szPattern));
			continue;
		}

		lArgs.append(__utf8("--exclude=%1").arg(szPattern));
	}

	QString szCase = comboBoxGetSelectedData(_p->pSearchCaseSensitivityComboBox);

	if(szCase == __utf8("caseinsensitive"))
		lArgs.append("-i");

	QString szText = _p->pSearchStringComboBox->currentText();

	QString szType = comboBoxGetSelectedData(_p->pSearchTypeComboBox);
	if(szType == __utf8("string"))
	{
		// string
		lArgs.append("-F");
	} else if(szType == __utf8("wildcard"))
	{
		// wildcard
		szText = QRegularExpression::escape(szText);
		szText.replace("\\*",".*");
		szText.replace("\\.",".");
	} else {
		// regexp
		// nothing to change
	}
	
	lArgs.append(szText);

	QString szScope = comboBoxGetSelectedData(_p->pSearchScopeComboBox);

	if((szScope == __utf8("grep.workspace")) || (szScope == __utf8("grep.workspace.and.includes")))
	{
		QString szRootPath = C3Workspace::currentWorkspace()->rootPath();
		if(szRootPath.isEmpty())
			szRootPath = C3MainWindow::instance()->currentDirectory();
		lArgs.append(szRootPath);
		
		if(szScope == __utf8("grep.workspace.and.includes"))
		{
			const QStringList & lPaths = C3Workspace::currentWorkspace()->includePaths();
			foreach(QString s,lPaths)
			{
				if(s.isEmpty())
					continue;
				lArgs.append(s);
			}
		}
	} else {
		// assume current directory
		lArgs.append(C3MainWindow::instance()->currentDirectory());
	}

	cmd.setName(__utf8("Grep Results"));
	cmd.setId(__utf8("internal.grep"));
	cmd.setArguments(lArgs);

	C3ExternalCommandManager::instance()->executeCommand(&cmd);
}

void C3FindWidget::find()
{
	QString szScope = comboBoxGetSelectedData(_p->pSearchScopeComboBox);
	if(szScope.startsWith(__utf8("grep.")))
	{
		findGrep();
		//_p->pFindButton->setEnabled(false); // so the user will need to change something to restart another grep
		// ^ this is problematic since the user can close the output window and the button would remain disabled...
		return;
	}
	
	// normal find
	findInternal();
	enableDisableButtons();
}

void C3FindWidget::replaceInternal()
{
	if(!_p->pOperation)
		return; // shouldn't be here

	QString szText = _p->pReplaceStringComboBox->currentText();

	bool bContained = _p->lRecentReplaceStrings.contains(szText);
	if(bContained)
		_p->lRecentReplaceStrings.removeAll(szText);
	_p->lRecentReplaceStrings.insert(0,szText);
	while(_p->lRecentReplaceStrings.count() > MAX_RECENT_ENTRIES)
		_p->lRecentReplaceStrings.takeLast();
	if(!bContained)
		_p->pReplaceStringComboBox->insertItem(0,szText);
	
	if(_p->lFindTargets.isEmpty())
		return;

	C3FindTarget * t = _p->lFindTargets.first();
	Q_ASSERT(t);
	
	t->findOperationReplace(_p->pOperation,szText);

	if(t->type() == C3FindTarget::Editor)
	{
		C3Editor * ed = dynamic_cast<C3Editor *>(t);
		Q_ASSERT(ed);
		if(ed != C3MainWindow::instance()->currentEditor())
			C3MainWindow::instance()->activateEditor(ed);
	}
}

void C3FindWidget::replace()
{
	replaceInternal();
	enableDisableButtons();
}

void C3FindWidget::replaceAndFind()
{
	replaceInternal();
	findInternal();
	enableDisableButtons();
}

void C3FindWidget::replaceAll()
{
	// Always a new operation
	rebuildFindOperation();

	if(_p->pOperation->regularExpression().pattern().isEmpty())
		return; // !

	int iMatches = 0;

	for(;;)
	{
		if(_p->lFindTargets.isEmpty())
			break;
	
		C3FindTarget * t = _p->lFindTargets.first();
		Q_ASSERT(t);
	
		if(t->findOperationRun(_p->pOperation))
		{
			replaceInternal();
			iMatches++;
		} else {
			t->findOperationReplaceAllFinished(_p->pOperation);
			_p->lFindTargets.takeFirst();
		}
	}

	if(iMatches > 0)
		QMessageBox::information(this,__tr("Replace All Finished"),__tr("Replaced %1 matches").arg(iMatches));
	else
		QMessageBox::information(this,__tr("Replace All Finished"),__tr("No matches found"));

	resetFindOperation();
	enableDisableButtons();
}

void C3FindWidget::loadSettings()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.findwidget");

	_p->lRecentSearchStrings = s.value("recentSearchStrings",QStringList()).toStringList();
	_p->lRecentReplaceStrings = s.value("recentReplaceStrings",QStringList()).toStringList();
}

void C3FindWidget::saveSettings()
{
	QSettings s(QSettings::NativeFormat,QSettings::UserScope,"pragmaware","c3.findwidget");

	s.setValue("recentSearchStrings",_p->lRecentSearchStrings);
	s.setValue("recentReplaceStrings",_p->lRecentReplaceStrings);

}