//=============================================================================
//
//   File : C3TextEditorModeCPP.cpp
//   Creation Date : gio 29 ott 2015 02:50:45
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

#include "C3TextEditorModeCPP.h"
#include "C3TextEditorModeCPPGenerateEnumSwitchDialog.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserCPP.h"
#include "C3TextEditorRowColumn.h"
#include "C3TextEditorLine.h"
#include "C3Workspace.h"
#include "C3TextEditor.h"
#include "C3TextEditorRange.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorModeCPPSharedData.h"
#include "C3TextEditorSymbolContext.h"
#include "C3TextEditorModeCPPMemberVariableGeneratorDialog.h"
#include "C3TextEditorModeCPPMemberFunctionGeneratorDialog.h"
#include "C3TextEditorModeCPPAddIncludeDialog.h"

#include <QHash>
#include <QString>
#include <QColor>
#include <QMenu>
#include <QKeyEvent>

C3TextEditorModeCPP::C3TextEditorModeCPP(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithSymbolAnalysis(
			pFactory,
			__utf8("cpp"),
			__tr("C/C++"),
			C3TextEditorModeParserCPP::CPPInterLineFlagMask,
			C3Symbol::Cpp
		)
{
	m_pParser = new C3TextEditorModeParserCPP(parserState());
}

C3TextEditorModeCPP::~C3TextEditorModeCPP()
{
	delete m_pParser;
}

void C3TextEditorModeCPP::fillCodeMenu(QMenu * pCodeMenu)
{
	QAction * a = pCodeMenu->addAction(__tr("Generate Member Variable..."));

	a->setShortcut(Qt::CTRL + Qt::Key_G);
	a->setShortcutContext(Qt::WidgetShortcut);

	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotGenerateMemberVariable()));

	a = pCodeMenu->addAction(__tr("Generate Member Function..."));

	a->setShortcut(Qt::CTRL + Qt::Key_H);
	a->setShortcutContext(Qt::WidgetShortcut);

	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotGenerateMemberFunction()));

	a = pCodeMenu->addAction(__tr("Add Include File..."));

	a->setShortcut(Qt::CTRL + Qt::Key_J);
	a->setShortcutContext(Qt::WidgetShortcut);

	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotAddIncludeFile()));
	
	
	a = pCodeMenu->addAction(__tr("Generate Enum Switch..."));

	a->setShortcut(Qt::CTRL + Qt::Key_K);
	a->setShortcutContext(Qt::WidgetShortcut);

	QObject::connect(a,SIGNAL(triggered()),this,SLOT(slotGenerateEnumSwitch()));

}

void C3TextEditorModeCPP::slotGenerateMemberVariable()
{
	C3TextEditorModeCPPMemberVariableGeneratorDialog oDialog(m_oParserState.pCoreData->pEditor);
	if(oDialog.exec() != QDialog::Accepted)
		return;
	triggerRescanFile();
}

void C3TextEditorModeCPP::slotGenerateMemberFunction()
{
	C3TextEditorModeCPPMemberFunctionGeneratorDialog oDialog(m_oParserState.pCoreData->pEditor);
	if(oDialog.exec() != QDialog::Accepted)
		return;
	triggerRescanFile();
}

void C3TextEditorModeCPP::slotAddIncludeFile()
{
	C3TextEditorModeCPPAddIncludeDialog oDialog(m_oParserState.pCoreData->pEditor);
	if(oDialog.exec() != QDialog::Accepted)
		return;
	triggerRescanFile();
}

void C3TextEditorModeCPP::slotGenerateEnumSwitch()
{
	C3TextEditorModeCPPGenerateEnumSwitchDialog oDialog(m_oParserState.pCoreData->pEditor);
	if(oDialog.exec() != QDialog::Accepted)
		return;
	triggerRescanFile();
}


void C3TextEditorModeCPP::computeMetadata()
{
	m_pParser->computeMetadata();
}

void C3TextEditorModeCPP::completeIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock)
{
	C3TextEditorRange oRange;
	
	if(!m_oParserState.pCoreData->pEditor->computeRangeOfWordBeforePosition(rc,oRange,true,true))
		return;

	QString szText = m_oParserState.pCoreData->pEditor->getRangeText(oRange);
	if(szText.isEmpty())
		return;

	QStringList lCompletions;

	C3Workspace::currentWorkspace()->getFileCompletions(szText,lCompletions);

	//if(lCompletions.isEmpty())
	//	return;

	QString szTrailer;

	if(oRange.start.col > 0)
	{
		const QChar less('<');
		const QChar quote('"');

		C3TextEditorLine * pLine = m_oParserState.pCoreData->lLines.at(oRange.start.row);
		QChar cStart = pLine->szText.at(oRange.start.col - 1);

		if(cStart == less)
			szTrailer = QString(">");
		else if(cStart == quote)
			szTrailer = quote;
	}

	// must contain the ".h" string
	
	static QString szH1(".h");
	static QString szH2(".H");
	static QChar dot('.');

	QList<C3TextEditorCompletion *> * pCompletions = new QList<C3TextEditorCompletion *>();
	
	foreach(QString s,lCompletions)
	{
		if(
				s.endsWith(szH1) || // .h
				s.endsWith(szH2) || // .H
				(s.indexOf(dot) < 0) // no extension (QWidget,QString crap...)
			)
		{
			pCompletions->append(
					new C3TextEditorCompletion(
							C3TextEditorCompletion::IncludeFile,
							s,
							szTrailer,
							0, // score
							0 // delete length FIXME!
						)
				);
		}
	}

	// include empty completions
	m_oParserState.pCoreData->pEditor->showCompletionWidget(oRange,pCompletions,0);
}

void C3TextEditorModeCPP::completePreprocessorToken(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock)
{
	C3TextEditorRange oRange;
	
	if(!m_oParserState.pCoreData->pEditor->computeRangeOfWordBeforePosition(rc,oRange))
		return;

	QString szText = m_oParserState.pCoreData->pEditor->getRangeText(oRange);
	if(szText.isEmpty())
		return;

	const QHash<QString,QColor *> & hHash = factory()->modeCPPSharedData()->preprocessorIdentifierColorHash();

	static QString szDefined("defined");
	static QString szIfdef("ifdef");
	static QString szElif("elif");
	static QString szIf("if");
	static QString szInclude("include");

	QList<C3TextEditorCompletion *> * pCompletions = new QList<C3TextEditorCompletion *>();

	for(QHash<QString,QColor *>::const_iterator it = hHash.begin();it != hHash.end();++it)
	{
		QString szToken = it.key();
		if(!szToken.startsWith(szText))
			continue;

		if(szToken == szDefined)
			continue; // we explicitly skip this one, as it may happen only in the middle of the preprocessor line, and we don't complete that.

		pCompletions->append(
				new C3TextEditorCompletion(
						C3TextEditorCompletion::IncludeFile,
						szToken,
						((szToken == szIf) || (szToken == szIfdef) || (szToken == szElif) || (szToken == szInclude)) ? QString(" ") : QString(),
						0, // score
						0 // delete length FIXME!
					)
			);
	}

	m_oParserState.pCoreData->pEditor->showCompletionWidget(oRange,pCompletions,0);
}

void C3TextEditorModeCPP::triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	Q_ASSERT((rc.row >= 0) && (rc.row < m_oParserState.pCoreData->lLines.count()));

	C3TextEditorLine * pLine = m_oParserState.pCoreData->lLines.at(rc.row);
	Q_ASSERT(pLine);

	C3TextEditorLine::Block * pBlock = pLine->findBlock(rc.col);

	if(!pBlock)
		return;

	if(!(uFlags & SymbolCompletionNoContextFilter))
	{
		if(
				pBlock->uFlags & (
					C3TextEditorModeParser::InCPPMultiLineComment |
					C3TextEditorModeParser::InCPPSingleLineComment |
					C3TextEditorModeParser::InCPPString
				)
			)
			return;
	
		if(pBlock->uFlags & C3TextEditorModeParser::InCPPIncludeFile)
		{
			completeIncludeFile(rc,pLine,pBlock);
			return;
		}
		
		if(pBlock->uFlags & C3TextEditorModeParser::InCPPPreprocessorToken)
		{
			completePreprocessorToken(rc,pLine,pBlock);
			return;
		}
	}

	C3TextEditorModeWithSymbolAnalysis::triggerSymbolCompletion(rc,uFlags);
}

bool C3TextEditorModeCPP::identifyLinkToIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock)
{
	C3TextEditorRange oRange;
	
	if(!m_oParserState.pCoreData->pEditor->computeRangeOfWordBeforePosition(rc,oRange,true,true))
		return false;

	QString szText = m_oParserState.pCoreData->pEditor->getRangeText(oRange);
	if(szText.isEmpty())
		return false;

	QString szPath = C3Workspace::currentWorkspace()->findFile(szText);

	if(szPath.isEmpty())
		return false;

	QList<C3TextEditorSymbolInfo *> * pBuffer = new QList<C3TextEditorSymbolInfo *>();
	
	C3TextEditorSymbolInfo * li = new C3TextEditorSymbolInfo();
	li->oRange = oRange;
	//li->oLink.setDescription(szPath);
	li->oLink.setPath(szPath);
	li->oLink.setEditorFactoryId("text");
	li->oLink.setTextEditorModeId("cpp");
	pBuffer->append(li);

	m_oParserState.pCoreData->pEditor->symbolIdentified(oRange,pBuffer);

	return true;
}

bool C3TextEditorModeCPP::triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	Q_ASSERT((rc.row >= 0) && (rc.row < m_oParserState.pCoreData->lLines.count()));

	C3TextEditorLine * pLine = m_oParserState.pCoreData->lLines.at(rc.row);
	Q_ASSERT(pLine);

	C3TextEditorLine::Block * pBlock = pLine->findBlock(rc.col);

	if(!pBlock)
		return false;

	if(!(uFlags & SymbolIdentificationNoContextFilter))
	{
		if(pBlock->uFlags & (
				C3TextEditorModeParser::InCPPMultiLineComment |
				C3TextEditorModeParser::InCPPSingleLineComment |
				C3TextEditorModeParser::InCPPString
			))
			return false;
	
		if(pBlock->uFlags & C3TextEditorModeParser::InCPPIncludeFile)
			return identifyLinkToIncludeFile(rc,pLine,pBlock);
	}

	C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

	if(!m_oParserState.pCoreData->pEditor->computeSymbolContext(rc,*pContext,C3TextEditor::ExtendSymbolToRight))
	{
		delete pContext;
		return false;
	}

	//qDebug("CPP symbol context is %s %s %s",pContext->szLeftText.toUtf8().data(),pContext->szLeftOperator.toUtf8().data(),pContext->szText.toUtf8().data());

	C3TextEditorModeWithSymbolAnalysis::symbolIdentification(pContext,uFlags);
	return true;
}

bool C3TextEditorModeCPP::handleKeyEvent(QKeyEvent * e)
{
	if(e->modifiers() & Qt::ControlModifier)
	{
		switch(e->key())
		{
			case Qt::Key_G:
				slotGenerateMemberVariable();
				return true;
			break;
			case Qt::Key_H:
				slotGenerateMemberFunction();
				return true;
			break;
			case Qt::Key_J:
				slotAddIncludeFile();
				return true;
			break;
			case Qt::Key_K:
				slotGenerateEnumSwitch();
				return true;
			break;
		}
	}
	
	return false;
}

