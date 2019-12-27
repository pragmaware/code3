//=============================================================================
//
//   File : C3TextEditorModeASM.cpp
//   Creation Date : 2018/02/15 01:25:06
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

#include "C3TextEditorModeASM.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorModeParserASM.h"
#include "C3TextEditorRowColumn.h"
#include "C3TextEditorLine.h"
#include "C3Workspace.h"
#include "C3TextEditor.h"
#include "C3TextEditorRange.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorModeASMSharedData.h"
#include "C3TextEditorSymbolContext.h"

#include <QHash>
#include <QString>
#include <QColor>
#include <QMenu>
#include <QKeyEvent>

C3TextEditorModeASM::C3TextEditorModeASM(C3TextEditorFactory * pFactory)
	: C3TextEditorModeWithInterLineState(
			pFactory,
			__utf8("asm"),
			__tr("ASM"),
			C3TextEditorModeParser::ASMInternLineFlagMask
		)
{
	m_pParser = new C3TextEditorModeParserASM(parserState());
}

C3TextEditorModeASM::~C3TextEditorModeASM()
{
	delete m_pParser;
}

void C3TextEditorModeASM::fillCodeMenu(QMenu * pCodeMenu)
{
}

void C3TextEditorModeASM::computeMetadata()
{
	m_pParser->computeMetadata();
}

void C3TextEditorModeASM::completeIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock)
{
	C3TextEditorRange oRange;
	
	if(!m_oParserState.pCoreData->pEditor->computeRangeOfWordBeforePosition(rc,oRange,true,true))
		return;

	QString szText = m_oParserState.pCoreData->pEditor->getRangeText(oRange);
	if(szText.isEmpty())
		return;

	QStringList lCompletions;

	C3Workspace::currentWorkspace()->getFileCompletions(szText,lCompletions);

	if(lCompletions.isEmpty())
		return;

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

	if(!pCompletions->isEmpty())
		m_oParserState.pCoreData->pEditor->showCompletionWidget(oRange,pCompletions,0);
	else
		delete pCompletions;
}

void C3TextEditorModeASM::completePreprocessorToken(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock)
{
	C3TextEditorRange oRange;
	
	if(!m_oParserState.pCoreData->pEditor->computeRangeOfWordBeforePosition(rc,oRange))
		return;

	QString szText = m_oParserState.pCoreData->pEditor->getRangeText(oRange);
	if(szText.isEmpty())
		return;

	const QHash<QString,QColor *> & hHash = factory()->modeASMSharedData()->preprocessorIdentifierColorHash();

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

	if(!pCompletions->isEmpty())
		m_oParserState.pCoreData->pEditor->showCompletionWidget(oRange,pCompletions,0);
	else
		delete pCompletions;
}

void C3TextEditorModeASM::triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags)
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
					C3TextEditorModeParser::InASMSingleLineComment |
					C3TextEditorModeParser::InASMString
				)
			)
			return;
	
		if(pBlock->uFlags & C3TextEditorModeParser::InASMIncludeFile)
		{
			completeIncludeFile(rc,pLine,pBlock);
			return;
		}
		
		if(pBlock->uFlags & C3TextEditorModeParser::InASMPreprocessorToken)
		{
			completePreprocessorToken(rc,pLine,pBlock);
			return;
		}
	}
/*
	C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

	if(!m_oParserState.pCoreData->pEditor->computeSymbolContext(rc,*pContext))
	{
		delete pContext;
		return;
	}

	//qDebug("ASM symbol context is %s %s %s",pContext->szLeftText.toUtf8().data(),pContext->szLeftOperator.toUtf8().data(),pContext->szText.toUtf8().data());

	C3TextEditorModeWithSymbolAnalysis::symbolCompletion(pContext,uFlags);
*/
}

bool C3TextEditorModeASM::identifyLinkToIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock)
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
	//li->oLink.setTextEditorModeId("cpp");
	pBuffer->append(li);

	m_oParserState.pCoreData->pEditor->symbolIdentified(oRange,pBuffer);

	return true;
}

bool C3TextEditorModeASM::triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags)
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
				C3TextEditorModeParser::InASMMultiLineComment |
				C3TextEditorModeParser::InASMSingleLineComment |
				C3TextEditorModeParser::InASMString
			))
			return false;
	
		if(pBlock->uFlags & C3TextEditorModeParser::InASMIncludeFile)
			return identifyLinkToIncludeFile(rc,pLine,pBlock);
	}

/*
	C3TextEditorSymbolContext * pContext = new C3TextEditorSymbolContext();

	if(!m_oParserState.pCoreData->pEditor->computeSymbolContext(rc,*pContext,C3TextEditor::ExtendSymbolToRight))
	{
		delete pContext;
		return false;
	}

	//qDebug("ASM symbol context is %s %s %s",pContext->szLeftText.toUtf8().data(),pContext->szLeftOperator.toUtf8().data(),pContext->szText.toUtf8().data());

	C3TextEditorModeWithSymbolAnalysis::symbolIdentification(pContext,uFlags);
*/
	return true;
}

bool C3TextEditorModeASM::handleKeyEvent(QKeyEvent * e)
{
	return false;
}



