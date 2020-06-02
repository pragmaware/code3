//=============================================================================
//
//   File : C3TextEditor.cpp
//   Creation Date : gio 22 ott 2015 22:22:57
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

#include "C3TextEditor.h"
#include "C3TextEditor_private.h"

#include "C3TextEditorViewport.h"
#include "C3TextEditorLine.h"
#include "C3TextEditorMode.h"
#include "C3LinkSelectionDialog.h"
#include "C3Workspace.h"
#include "C3TextEditorSymbolContext.h"
#include "C3MainWindow.h"
#include "C3FindWidget.h"
#include "C3TextEditorModeParser.h"

#include <QHash>
#include <QAction>
#include <QMenu>


void C3TextEditor::fillSymbolMenu(QMenu * pMenu)
{
	C3TextEditorSymbolContext oContextLeft;
	computeSymbolContext(_p->oCursor,oContextLeft,0);

	C3TextEditorSymbolContext oContextFull;
	computeSymbolContext(_p->oCursor,oContextFull,ExtendSymbolToRight);

	QAction * a;

#define ACTION(_szText,_slot,_bEnabled,_eShortcut) \
	do { \
		QString szText = _szText; \
		a = pMenu->addAction(szText); \
		a->setEnabled(_bEnabled); \
		a->setShortcut(_eShortcut); \
		a->setShortcutContext(Qt::WidgetShortcut); \
		QObject::connect(a,SIGNAL(triggered()),this,_slot); \
	} while(0)

	ACTION(
			oContextFull.szText.isEmpty() ? 
				__tr("Identify") :
				__tr("Identify '%1'").arg(oContextFull.szText),
			SLOT(triggerSymbolIdentification()),
			!oContextFull.szText.isEmpty(),
			Qt::CTRL + Qt::Key_I
		);

	ACTION(
			oContextFull.szText.isEmpty() ? 
				__tr("Identify") :
				__tr("Identify '%1' (Ignore Context)").arg(oContextFull.szText),
			SLOT(triggerSymbolIdentificationNoContextFilter()),
			!oContextFull.szText.isEmpty(),
			Qt::CTRL + Qt::ALT + Qt::Key_I
		);

	pMenu->addSeparator();

	ACTION(
			oContextLeft.szText.isEmpty() ? 
				__tr("Complete") :
				__tr("Complete '%1'").arg(oContextLeft.szText),
			SLOT(triggerSymbolCompletion()),
			!readOnly(),
			Qt::CTRL + Qt::Key_Space
		);

	ACTION(
			oContextLeft.szText.isEmpty() ? 
				__tr("Complete (Ignore Context)") :
				__tr("Complete '%1' (Ignore Context)").arg(oContextLeft.szText),
			SLOT(triggerSymbolCompletionNoContextFilter()),
			(!readOnly()) && (!oContextLeft.szText.isEmpty()),
			Qt::CTRL + Qt::ALT + Qt::Key_Space
		);

	pMenu->addSeparator();

	ACTION(
			oContextFull.szText.isEmpty() ? 
				__tr("Grep in Workspace") :
				__tr("Grep '%1' in Workspace").arg(oContextFull.szText),
			SLOT(grepCurrentSymbolInWorkspace()),
			(!oContextFull.szText.isEmpty()) && (!C3Workspace::currentWorkspace()->isDefault()),
			Qt::CTRL + Qt::ALT + Qt::Key_F
		);

	ACTION(
			oContextFull.szText.isEmpty() ? 
				__tr("Grep in Current Directory") :
				__tr("Grep '%1' in Current Directory").arg(oContextFull.szText),
			SLOT(grepCurrentSymbolInCurrentDirectory()),
			!oContextFull.szText.isEmpty(),
			Qt::CTRL + Qt::SHIFT + Qt::Key_F
		);
}

void C3TextEditor::grepCurrentSymbolInWorkspace()
{
	if(C3Workspace::currentWorkspace()->isDefault())
		return;

	C3TextEditorSymbolContext oContextFull;
	computeSymbolContext(_p->oCursor,oContextFull,ExtendSymbolToRight);

	if(oContextFull.szText.isEmpty())
		return;
	
	C3FindWidget * pWidget = C3MainWindow::instance()->showFindWidget();
	Q_ASSERT(pWidget);

	pWidget->grepInWorkspace(oContextFull.szText);
}

void C3TextEditor::grepCurrentSymbolInCurrentDirectory()
{
	C3TextEditorSymbolContext oContextFull;
	computeSymbolContext(_p->oCursor,oContextFull,ExtendSymbolToRight);

	if(oContextFull.szText.isEmpty())
		return;
	
	C3FindWidget * pWidget = C3MainWindow::instance()->showFindWidget();
	Q_ASSERT(pWidget);

	pWidget->grepInCurrentDirectory(oContextFull.szText);
}

void C3TextEditor::triggerSymbolCompletion()
{
	_p->pMode->triggerSymbolCompletion(
			_p->oCursor,
			C3TextEditorMode::SymbolCompletionMatchInTheMiddle | C3TextEditorMode::SymbolCompletionMatchCaseInsensitive
	);
}

void C3TextEditor::triggerSymbolCompletionNoContextFilter()
{
	_p->pMode->triggerSymbolCompletion(
			_p->oCursor,
			C3TextEditorMode::SymbolCompletionNoContextFilter | C3TextEditorMode::SymbolCompletionMatchInTheMiddle | C3TextEditorMode::SymbolCompletionMatchCaseInsensitive
		);
}

void C3TextEditor::triggerSymbolIdentification()
{
	_p->bJumpToLinkAfterIdentification = true;

	if(!_p->pMode->triggerSymbolIdentification(_p->oCursor,0))
	{
		if(!_p->oMark.isEmpty())
		{
			beginVisualStateChange();
			_p->oMark.clear();
			endVisualStateChange();
		}
	}
}

void C3TextEditor::triggerSymbolIdentificationNoContextFilter()
{
	_p->bJumpToLinkAfterIdentification = true;

	if(!_p->pMode->triggerSymbolIdentification(_p->oCursor,C3TextEditorMode::SymbolIdentificationNoContextFilter))
	{
		if(!_p->oMark.isEmpty())
		{
			beginVisualStateChange();
			_p->oMark.clear();
			endVisualStateChange();
		}
	}
}

bool C3TextEditor::computeSymbolContext(
		const C3TextEditorRowColumn &oPos,
		C3TextEditorSymbolContext &oContext,
		unsigned int uFlags
	)
{
	oContext.szFilePath = path();

	int iRow = oPos.row;
	int iCol = oPos.col;

	if(iRow < 0)
		return false;
	if(iRow >= _p->lLines.count())
		return false;

	C3TextEditorLine * pLine = _p->lLines.at(iRow);
	Q_ASSERT(pLine);

	if(pLine->szText.length() < 1)
		return false;

	oContext.oRange.start.row = iRow;
	oContext.oRange.end.row = iRow;

	int iLength = pLine->szText.length();

	if(iCol > iLength)
		iCol = iLength;

	int iRightLen = 0;

	if(uFlags & ExtendSymbolToRight)
	{
		const QChar * bx = pLine->szText.unicode() + iCol;
		const QChar * ex = bx + iLength;
		const QChar * px = bx;

		if(
				px->isLetterOrNumber() ||
				(px->unicode() == '_')
			)
		{
			// got text
			px++;
			
			while(
				(px < ex) &&
				(
					px->isLetterOrNumber() ||
					(px->unicode() == '_')
				)
			)
				px++;

			iRightLen = px - bx;
		}
	}


	if(iCol <= 0)
	{
		// at beginning of line
		oContext.oRange.start.col = 0;
		oContext.oRange.end.col = iRightLen;
		return true;
	}

	const QChar * b = pLine->szText.unicode();
	const QChar * e = b + iCol;
	const QChar * p = e - 1;

	int iLinesVisited = 1;

	// Note that all the modes share the flag for SingleLineComments (tricky!)

#define PREVIOUS_LINE() \
	do { \
		iLinesVisited++; \
		if(iLinesVisited > 10) \
		{ \
			oContext.removeEmptyLeftContext(); \
			return true; \
		} \
		if(iRow <= 0) \
		{ \
			oContext.removeEmptyLeftContext(); \
			return true; \
		} \
		iRow--; \
		pLine = _p->lLines.at(iRow); \
		if((pLine->lBlocks.count() > 0) && (pLine->lBlocks.last()->uFlags & C3TextEditorModeParser::InCPPSingleLineComment)) \
		{ \
			oContext.removeEmptyLeftContext(); \
			return true; \
		} \
		b = pLine->szText.unicode(); \
		e = b + pLine->szText.length(); \
		p = e - 1; \
		while((p >= b) && p->isSpace()) \
			p--; \
	} while(p < b)

	if(
			p->isLetterOrNumber() ||
			(p->unicode() == '_')
		)
	{
		// got text
		p--;
		
		while(
			(p >= b) &&
			(
				p->isLetterOrNumber() ||
				(p->unicode() == '_')
			)
		)
			p--;

		// now pointing before the start of the buffer (everything is token)
		// or to a non-token char.

		p++; // go back to the token char

		oContext.szText.setUnicode(p,(e-p) + iRightLen);

		oContext.oRange.end.col = (e - b) + iRightLen;
		oContext.oRange.start.col = p - b;

		if(p <= b)
			PREVIOUS_LINE();
		else
			p--;
	} else {
		if(iRightLen > 0)
			oContext.szText.setUnicode(b + oPos.col,iRightLen);
		oContext.oRange.start.col = oPos.col;
		oContext.oRange.end.col = oPos.col + iRightLen;
	}

	// got symbol.

	// look for left context now

	C3TextEditorSymbolContext::AdditionalContext * pCtx = oContext.addLeftContext();

	for(;;)
	{
		// skip space
		while((p >= b) && p->isSpace())
			p--;
	
		if(p < b)
		{
			PREVIOUS_LINE();
			continue; // maybe try again
		}

		e = p + 1;
		
		if(!(p->isLetterOrNumber() || (p->unicode() == '_')))
		{
			if((p->unicode() == ';') || (p->unicode() == '{') || (p->unicode() == '}') || (p->unicode() == ',') || (p->unicode() == '('))
			{
				oContext.removeEmptyLeftContext();
				return true; // this is context separator in most languages
			}
			
			ushort pu = p->unicode();
			
			int iParenthesisLevel = pu == ')';
			int iSquareParenthesisLevel = pu == ']';
			int iSingleQuoteLevel = pu == '\'';
			int iDoubleQuoteLevel = pu == '"';

			const QChar * pFirstParenthesis = (iParenthesisLevel > 0) ? p : NULL;
			const QChar * pFirstSquareParenthesis = (iSquareParenthesisLevel > 0) ? p : NULL;
		
			// FIXME: Handle comments?
		
			// something like an operator
			p--;
			
			for(;;)
			{
				while(
					(p >= b) &&
					(
						(!(
							p->isLetterOrNumber() ||
							(p->unicode() == '_') ||
							p->isSpace()
						)) ||
						(
							(iSquareParenthesisLevel > 0) ||
							(iParenthesisLevel > 0) ||
							(iSingleQuoteLevel > 0) ||
							(iDoubleQuoteLevel > 0)
						)
					)
				)
				{
					ushort u = p->unicode();

					if(iDoubleQuoteLevel)
					{
						// FIXME: Handle escapes!
						if(u == '"')
							iDoubleQuoteLevel = 0;
					} else if(iSingleQuoteLevel)
					{
						// FIXME: Handle escapes!
						if(u == '\'')
							iSingleQuoteLevel = 0;
					} else {
						switch(u)
						{
							case ';':
							case '{':
							case '}':
								oContext.removeEmptyLeftContext();
								return true; // this is context separator in most languages
							break;
							case '"':
								iDoubleQuoteLevel = 1;
							break;
							case '\'':
								iSingleQuoteLevel = 1;
							break;
							case '[':
								if(iSquareParenthesisLevel <= 0)
								{
									oContext.removeEmptyLeftContext();
									return true;
								}
								iSquareParenthesisLevel--;
							break;
							case ']':
								iSquareParenthesisLevel++;
								if(!pFirstSquareParenthesis)
									pFirstSquareParenthesis = p;
							break;
							case '(':
								if(iParenthesisLevel <= 0)
								{
									oContext.removeEmptyLeftContext();
									return true;
								}
								iParenthesisLevel--;
							break;
							case ')':
								iParenthesisLevel++;
								if(!pFirstParenthesis)
									pFirstParenthesis = p;
							break;
						}
					}

					p--;
				}

				if(pFirstParenthesis)
				{
					pCtx->szOperator.insert(0,QString(pFirstParenthesis,e-pFirstParenthesis));
					pCtx->szOperator.insert(0,QChar('('));
					pFirstParenthesis = NULL;
				} else if(pFirstSquareParenthesis)
				{
					pCtx->szOperator.insert(0,QString(pFirstSquareParenthesis,e-pFirstSquareParenthesis));
					pCtx->szOperator.insert(0,QChar('['));
					pFirstSquareParenthesis = NULL;
				} else {
					p++;
					pCtx->szOperator.insert(0,QString(p,e-p));
					p--;
				}

				for(;;)
				{
					while((p >= b) && p->isSpace())
						p--;
				
					if(p >= b)
						break;

					PREVIOUS_LINE();
					if(pFirstSquareParenthesis)
						pFirstSquareParenthesis = NULL; // can happen that both pFirstParenthesis and pFirstSquareParenthesis are set...
				}
				
				if((p >= b) && (p->isLetterOrNumber() || (p->unicode() == '_')))
					break;
			}
		}

		for(;;)
		{
			// skip space
			while((p >= b) && p->isSpace())
				p--;
		
			if(p >= b)
				break;

			PREVIOUS_LINE();
		}

		if(pCtx->szOperator.isEmpty() && (iLinesVisited > 1))
		{
			// if the operator is empty, newline breaks.
			oContext.removeEmptyLeftContext();
			return true;
		}

		static QString szSpecial(">::"); // template special case

		if(pCtx->szOperator == szSpecial)
		{
			pCtx->szOperator = "::";
			// skip back until we find a <

			for(;;)
			{
				while((p >= b) && (p->unicode() != '<'))
					p--;
			
				if(p >= b)
					break;
	
				PREVIOUS_LINE();
			}
			
			if(p >= b)
				p--;
			else
				PREVIOUS_LINE();
		}

		e = p + 1;

		if(
				p->isLetterOrNumber() ||
				(p->unicode() == '_')
			)
		{
			// token
			p--;
			
			while(
				(p >= b) &&
				(
					p->isLetterOrNumber() ||
					(p->unicode() == '_')
				)
			)
				p--;
	
			// now pointing before the start of the buffer (everything is token)
			// or to a non-token char.
	
			p++; // go back to the token char
	
			pCtx->szText.setUnicode(p,e-p);
		}

		if(p <= b)
			PREVIOUS_LINE();
		else
			p--;

		// Don't go too much left
		if(oContext.lLeftContext.count() > 6)
		{
			oContext.removeEmptyLeftContext();
			return true;
		}
		
		pCtx = oContext.addLeftContext();
	}

	Q_ASSERT(false); // not reached
	oContext.removeEmptyLeftContext();
	return true;
}


void C3TextEditor::triggerSymbolIdentification(const C3TextEditorRowColumn &rc)
{
	triggerSymbolIdentificationInternal(rc,false);
}

void C3TextEditor::triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags)
{
	triggerSymbolIdentificationInternal(rc,false,uFlags);
}

bool C3TextEditor::triggerSymbolIdentificationInternal(const C3TextEditorRowColumn &rc,bool bJumpToLinkAfterIdentification,unsigned int uFlags)
{
	_p->bJumpToLinkAfterIdentification = bJumpToLinkAfterIdentification;

	if(!_p->pMode->triggerSymbolIdentification(rc,uFlags))
	{
		if(!_p->oMark.isEmpty())
		{
			beginVisualStateChange();
			_p->oMark.clear();
			endVisualStateChange();
		}
		return false;
	}
	
	return true;
}

void C3TextEditor::symbolIdentified(
		const C3TextEditorRange &oRange,
		QList<C3TextEditorSymbolInfo *> * pInfo
	)
{
	if(!pInfo)
	{
		// no link identified.
		return;
	}
	
	if(pInfo->isEmpty())
	{
		// no link identified.
		delete pInfo;
		return;
	}

	C3TextEditorSymbolInfo * li = pInfo->first();
	Q_ASSERT(li);

	beginVisualStateChange();
	_p->oMark = li->oRange;
	endVisualStateChange();

	if(_p->bJumpToLinkAfterIdentification)
	{
		QList<C3Link> lLinks;
		foreach(C3TextEditorSymbolInfo * lli,*pInfo)
			lLinks.append(lli->oLink);
	
		C3LinkSelectionDialog dlg(
				this,
				__tr("Jump to Link"),
				lLinks
			);
	
		if(dlg.exec() == QDialog::Accepted)
			C3Workspace::currentWorkspace()->openFileAsync(dlg.selectedLink());
	}
	
	qDeleteAll(*pInfo);
	delete pInfo;
}
