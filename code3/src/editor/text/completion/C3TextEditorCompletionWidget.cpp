//=============================================================================
//
//   File : C3TextEditorCompletionWidget.cpp
//   Creation Date : dom 08 nov 2015 04:39:48
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

#include "C3TextEditorCompletionWidget.h"
#include "C3TextEditorFactory.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditor.h"
#include "C3PixmapCache.h"
#include "C3TextEditorMode.h"

#include <QPalette>
#include <QHideEvent>
#include <QListWidget>
#include <QLabel>
#include <QMutex>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QToolTip>
#include <QHash>

class C3TextEditorCompletionWidgetPrivate
{
public:
	C3TextEditor * pEditor;
	C3TextEditorFactory * pFactory;
	C3TextEditorOptions * pOptions;

	QListWidget * pListWidget;
	QLabel * pInfoLabel;
	
	int iLabelHeight;
	
	QList<C3TextEditorCompletion *> * pCompletions;
	
	QMutex oSnippetExtractorWorkerMutex;
	C3FileSnippetExtractorWorker * pSnippetExtractorWorker;
	
	QString szSnippet;
	
	QTimer oSnippetTimer;
	
	quint32 uBeforeContextLines;
};

#define _p m_pC3TECW

C3TextEditorCompletionWidget::C3TextEditorCompletionWidget(
		QWidget * pParent,
		C3TextEditor * pTextEditor
	)
	: QWidget(pParent)
{
	Q_ASSERT(pTextEditor);

	_p = new C3TextEditorCompletionWidgetPrivate();
	_p->pEditor = pTextEditor;
	_p->pCompletions = NULL;
	_p->pFactory = dynamic_cast<C3TextEditorFactory *>(_p->pEditor->factory());
	Q_ASSERT(_p->pFactory);
	_p->pOptions = _p->pFactory->options();
	Q_ASSERT(_p->pOptions);
	
	_p->pSnippetExtractorWorker = NULL;

	_p->pListWidget = new QListWidget(this);
	_p->pListWidget->setFrameStyle(QFrame::Plain);
	_p->pListWidget->setFrameShape(QFrame::Box);
	_p->pListWidget->setLineWidth(1);
	_p->pListWidget->setMidLineWidth(0);
	_p->pListWidget->setSortingEnabled(false);

	QObject::connect(
			_p->pListWidget,
			SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),
			this,
			SLOT(slotCurrentItemChanged(QListWidgetItem *,QListWidgetItem *))
		);

	_p->pListWidget->setFocusPolicy(Qt::NoFocus);

	_p->pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	_p->pInfoLabel = new QLabel(this);
	_p->pInfoLabel->setWordWrap(false);
	_p->pInfoLabel->setMargin(3);

	_p->iLabelHeight = _p->pInfoLabel->sizeHint().height();

	setFocusPolicy(Qt::NoFocus);

#if 0
	// doesn't seem to work proper
	QGraphicsOpacityEffect * eff1 = new QGraphicsOpacityEffect();
	eff1->setOpacity(0.7);
	setGraphicsEffect(eff1);
#endif

	setCursor(QCursor(Qt::ArrowCursor));
	updatePalette();
	
	QObject::connect(&(_p->oSnippetTimer),SIGNAL(timeout()),this,SLOT(slotSnippetTimerTimeout()));
	QObject::connect(this,SIGNAL(interThreadSnippetReady()),this,SLOT(slotSnippetReady()));

}

C3TextEditorCompletionWidget::~C3TextEditorCompletionWidget()
{
	_p->oSnippetTimer.stop();

	if(_p->pSnippetExtractorWorker)
		abortSnippetExtractorWorker();

	if(_p->pCompletions)
	{
		qDeleteAll(*(_p->pCompletions));
		delete _p->pCompletions;
	}
	delete _p;
}

void C3TextEditorCompletionWidget::updatePalette()
{
	QPalette pal(palette());

	pal.setColor(QPalette::Base,_p->pOptions->oTextBackgroundColor);
	pal.setColor(QPalette::Window,_p->pOptions->oTextBackgroundColor);
	pal.setColor(QPalette::Text,_p->pOptions->oTextColor);
	pal.setColor(QPalette::WindowText,_p->pOptions->oTextColor);
	pal.setColor(QPalette::Highlight,_p->pOptions->oTextColor);
	pal.setColor(QPalette::HighlightedText,_p->pOptions->oEditorBackgroundColor);
	
	setPalette(pal);
	_p->pListWidget->setPalette(pal);
	
	setFont(_p->pOptions->oTextFont);
	_p->pListWidget->setFont(_p->pOptions->oTextFont);
	
	QFont f(_p->pOptions->oTextFont);

	f.setPointSize(9);

	_p->pInfoLabel->setFont(f);
}

void C3TextEditorCompletionWidget::resizeEvent(QResizeEvent * e)
{
	_p->pListWidget->setGeometry(0,0,width(),height() - _p->iLabelHeight);
	_p->pInfoLabel->setGeometry(0,height() - _p->iLabelHeight,width(),_p->iLabelHeight);
}

enum ExtractShortTokenHashScoringMode
{
	ExtractShortTokenHashScoringContextOnLeft,
	ExtractShortTokenHashScoringContextOnRight,
	ExtractShortTokenHashScoringNone
};

static void extract_short_token_hash(QHash<QString,int> &hHash,const QString &szText,ExtractShortTokenHashScoringMode eScoringMode)
{
	const QChar * c = szText.unicode();
	const QChar * e = c + szText.length();
	const QChar * b = NULL;
	
	bool bIsLower = false;
	
	QList<QString> lTokens;
	
	while(c < e)
	{
		if(c->isLetter())
		{
			if(b)
			{
				if(c->isLower())
				{
					bIsLower = true;
				} else {
					// is upper
					if(bIsLower)
					{
						// was lower, split here
						QString szToken(b,c-b);
						if(szToken.length() > 2)
							lTokens.append(szToken.toLower());
						b = c;
					}
					bIsLower = false;
				}
			} else {
				b = c;
				bIsLower = b->isLower();
			}
		} else {
			if(b)
			{
				QString szToken(b,c-b);
				if(szToken.length() > 2)
					lTokens.append(szToken.toLower());
				b = NULL;
			}
		}
		c++;
	}

	if(b)
	{
		QString szToken(b,c-b);
		if(szToken.length() > 2)
			lTokens.append(szToken.toLower());
	}

	switch(eScoringMode)
	{
		case ExtractShortTokenHashScoringContextOnLeft:
		{
			int iCount = lTokens.count();
			
			int iPosition = 1;
			foreach(QString szToken,lTokens)
			{
				int iScore = (iPosition * 50) / iCount;
				if(iScore > 0)
				{
					int iVal = hHash.value(szToken,0);
					if(iScore > iVal)
						hHash.insert(szToken,iScore);
				}
				iPosition++;
			}
		}
		break;
		case ExtractShortTokenHashScoringContextOnRight:
		{
			int iCount = lTokens.count();
			
			int iPosition = iCount;
			foreach(QString szToken,lTokens)
			{
				int iScore = (iPosition * 20) / iCount;
				if(iScore > 0)
				{
					int iVal = hHash.value(szToken,0);
					if(iScore > iVal)
						hHash.insert(szToken,iScore);
				}
				iPosition--;
			}
		}
		break;
		default:
			foreach(QString szToken,lTokens)
				hHash.insert(szToken,1);
		break;
	}

}

static int compute_common_entry_score(QHash<QString,int> &h1,QHash<QString,int> &h2)
{
	// h2 should be smaller!
	if(h1.isEmpty() || h2.isEmpty())
		return 0;
	int iRet = 0;
	for(QHash<QString,int>::Iterator it = h2.begin();it != h2.end();++it)
	{
		int val = h1.value(it.key(),0);
		if(val > 0)
			iRet += val * it.value();
	}
	return iRet;
}

class C3TextEditorCompletionWidgetListWidgetItem : public QListWidgetItem
{
private:
	int m_iIndex;
	quint16 m_uScore;
public:
	C3TextEditorCompletionWidgetListWidgetItem(const QString &szText,QListWidget *pWidget,int iIndex,quint16 uScore)
		: QListWidgetItem(szText,pWidget), m_iIndex(iIndex), m_uScore(uScore)
	{
	}
	
	int index()
	{
		return m_iIndex;
	}
	
	quint16 score()
	{
		return m_uScore;
	}
};

//#define DEBUG_COMPLETION 1

int C3TextEditorCompletionWidget::showChoices(
		const QString &szTextToComplete,
		QList<C3TextEditorCompletion *> * pCompletions,
		int * piMaxLength,
		const QString &szWideContextLeft,
		const QString &szWideContextRight,
		unsigned int uFlags // C3TextEditorMode::SymbolCompletionFlags
	)
{
#ifdef DEBUG_COMPLETION
	C3_TRACE_FUNCTION;
#endif
	//qDebug("SHOW CHOICES!");
	
	C3PixmapCache * pPixmapCache = C3PixmapCache::instance();

	Q_ASSERT(pCompletions);

	if(_p->pCompletions)
	{
		qDeleteAll(*(_p->pCompletions));
		delete _p->pCompletions;
	}

	_p->pCompletions = pCompletions;

	_p->pListWidget->clear();

	int iLength = szTextToComplete.length();

	int iCount = 0;

	C3TextEditorCompletionWidgetListWidgetItem * it;
	C3TextEditorCompletionWidgetListWidgetItem * pBest = NULL;
	int iBestLength = 0;
	quint16 uBestScore = 0;
	quint32 uSymbolsWithBestScore = 0;

	bool bEmpty = szTextToComplete.isEmpty();

	int iMaxLength = 0;

	bool bMatchInTheMiddle = uFlags & C3TextEditorMode::SymbolCompletionMatchInTheMiddle;
	Qt::CaseSensitivity eCaseSensitivity = (uFlags & C3TextEditorMode::SymbolCompletionMatchCaseInsensitive) ?
				Qt::CaseInsensitive :
				Qt::CaseSensitive;

	foreach(C3TextEditorCompletion * c,*pCompletions)
	{
		int iSymLen = c->szSymbol.length();
		
		if(!bEmpty)
		{
			if(iSymLen < iLength)
				continue;
	
			if(bMatchInTheMiddle)
			{
				if(!c->szSymbol.contains(szTextToComplete,eCaseSensitivity))
					continue;
			} else {
				if(!c->szSymbol.startsWith(szTextToComplete,eCaseSensitivity))
					continue;
			}
		}

		if(c->szSymbol.length() > iMaxLength)
			iMaxLength = c->szSymbol.length();

		it = new C3TextEditorCompletionWidgetListWidgetItem(c->szSymbol,_p->pListWidget,iCount,c->uScore);
		
		switch(c->eType)
		{
			case C3TextEditorCompletion::Function:
				it->setIcon(pPixmapCache->pixmapCompletionFunction());
			break;
			case C3TextEditorCompletion::Class:
				it->setIcon(pPixmapCache->pixmapCompletionClass());
			break;
			case C3TextEditorCompletion::Namespace:
				it->setIcon(pPixmapCache->pixmapCompletionNamespace());
			break;
			case C3TextEditorCompletion::IncludeFile:
				it->setIcon(pPixmapCache->pixmapCompletionIncludeFile());
			break;
			case C3TextEditorCompletion::Enumeration:
				it->setIcon(pPixmapCache->pixmapCompletionEnumeration());
			break;
			case C3TextEditorCompletion::EnumerationMember:
				it->setIcon(pPixmapCache->pixmapCompletionEnumerationMember());
			break;
			case C3TextEditorCompletion::LocalVariable:
				it->setIcon(pPixmapCache->pixmapCompletionLocalVariable());
			break;
			case C3TextEditorCompletion::GlobalVariable:
				it->setIcon(pPixmapCache->pixmapCompletionGlobalVariable());
			break;
			case C3TextEditorCompletion::MemberVariable:
				it->setIcon(pPixmapCache->pixmapCompletionMemberVariable());
			break;
			case C3TextEditorCompletion::Macro:
				it->setIcon(pPixmapCache->pixmapCompletionMacro());
			break;
			case C3TextEditorCompletion::Keyword:
				it->setIcon(pPixmapCache->pixmapCompletionKeyword());
			break;
			case C3TextEditorCompletion::TypeDefinition:
				it->setIcon(pPixmapCache->pixmapCompletionTypeDefinition());
			break;
			default:
				it->setIcon(pPixmapCache->pixmapCompletionOther());
			break;
		}

		/*
		if(iSymLen == iLength)
		{
			// this is the right one (unless we have already matched an overload)
			if(uBestScore < 100)
			{
				pBest = it;
				iBestLength = iSymLen;
				uBestScore = 100;
				uSymbolsWithBestScore = 1;
			}
		} else {
		*/

		quint16 uScore = c->uScore;
		
		// matches at the beginning are a lot better
		if(bMatchInTheMiddle)
		{
			if(eCaseSensitivity == Qt::CaseInsensitive)
			{
				if(c->szSymbol.startsWith(szTextToComplete,Qt::CaseInsensitive))
				{
					uScore += 2;
					if(c->szSymbol.startsWith(szTextToComplete,Qt::CaseSensitive))
						uScore += 1;
				}
			} else {
				if(c->szSymbol.startsWith(szTextToComplete,eCaseSensitivity))
					uScore += 3;
			}
		} else {
			if(eCaseSensitivity == Qt::CaseInsensitive)
			{
				if(c->szSymbol.startsWith(szTextToComplete,Qt::CaseSensitive))
					uScore += 3;
			}
		}
		
		if(iSymLen == iLength)
			uScore++;

#ifdef DEBUG_COMPLETION
		C3_TRACE("Symbol [%s] with score [base %d final %d]",c->szSymbol.toUtf8().data(),c->uScore,uScore);
#endif

		if(!pBest)
		{
			pBest = it;
			iBestLength = iSymLen;
			uBestScore = uScore;
			uSymbolsWithBestScore = 1;
		} else {
			if(uScore > uBestScore)
			{
				pBest = it;
				iBestLength = iSymLen;
				uBestScore = uScore;
				uSymbolsWithBestScore = 1;
			} else if(uScore == uBestScore)
			{
				if(iSymLen < iBestLength)
				{
					pBest = it;
					iBestLength = iSymLen;
				}

				uSymbolsWithBestScore++;
			}
		}

		/*
		}
		*/
		
		_p->pListWidget->addItem(it);

		iCount++;
	}

	if(piMaxLength)
		*piMaxLength = iMaxLength;

	if(iCount < 1)
		return 0;

	// If there are multiple items with best score then should also look at the context
	// Get the range around, split text tokens on _ and camel case, lowercase it,
	// insert into a hash and try to find the one that best matches
	// ex:
	//        QString className()
	//        {
	//            return <pick m_szClassName and not, say, m_szOtherStuff here>
	//        }
	
	//qDebug("Symbols with best score: %d",uSymbolsWithBestScore);
	
	if(uSymbolsWithBestScore > 1)
	{
		QHash<QString,int> hWideContextMap;

		extract_short_token_hash(hWideContextMap,szWideContextLeft,ExtractShortTokenHashScoringContextOnLeft);
		extract_short_token_hash(hWideContextMap,szWideContextRight,ExtractShortTokenHashScoringContextOnRight);

		//for(QHash<QString,int>::Iterator it = hWideContextMap.begin();it != hWideContextMap.end();it++)
		//	qDebug("- token %s - score %d",it.key().toUtf8().data(),it.value());

		int iBestCommonEntryScore = 0;
		iBestLength = 0;

		if(hWideContextMap.count() > 0)
		{
			//qDebug("Wide context map has %d entries",hWideContextMap.count());
		
			int c = _p->pListWidget->count();
			int i = 0;

			while(i < c)
			{
				C3TextEditorCompletionWidgetListWidgetItem * it = (C3TextEditorCompletionWidgetListWidgetItem *)_p->pListWidget->item(i);
				Q_ASSERT(it);
				
				if(it->score() == uBestScore)
				{
					QHash<QString,int> hSymbolMap;
					const QString &txt = it->text();
					iLength = txt.length();
					extract_short_token_hash(hSymbolMap,txt,ExtractShortTokenHashScoringNone);
					int iCommonEntryScore = compute_common_entry_score(hWideContextMap,hSymbolMap); // hSymbolMap goes second as it should be smaller
					//qDebug("Common entries with %s: %d",txt.toUtf8().data(),iCommonEntryScore);
					if(iCommonEntryScore > iBestCommonEntryScore)
					{
						iBestCommonEntryScore = iCommonEntryScore;
						iBestLength = iLength;
						pBest = it;
					} else if(iCommonEntryScore == iBestCommonEntryScore)
					{
						if(iLength < iBestLength)
						{
							pBest = it;
							iBestLength = iLength;
						}
					}
				}
				
				i++;
			}
		}
	}

	Q_ASSERT(pBest);
	_p->pListWidget->setCurrentItem(pBest);

	int rh = _p->pListWidget->sizeHintForRow(0);

	int h = (iCount > 5) ? (rh * 5) : (rh * iCount);

	return h + _p->iLabelHeight + 4; // 4 for the frame
}

void C3TextEditorCompletionWidget::slotCurrentItemChanged(QListWidgetItem *cur,QListWidgetItem *prev)
{
	updateDescription();
	
	C3TextEditorCompletion * pCompletion = currentCompletion();
	if(!pCompletion)
		return;

	if(pCompletion->eType == C3TextEditorCompletion::Function)
		restartSnippetTimer();
	else
		_p->oSnippetTimer.stop();
}

C3TextEditorCompletion * C3TextEditorCompletionWidget::currentCompletion()
{
	C3TextEditorCompletionWidgetListWidgetItem * it = dynamic_cast<C3TextEditorCompletionWidgetListWidgetItem *>(_p->pListWidget->currentItem());
	if(!it)
		return NULL;

	if(!_p->pCompletions)
		return NULL;

	int idx = it->index();
	if((idx < 0) || (idx >= _p->pCompletions->count()))
		return NULL;

	return _p->pCompletions->at(idx);
}

void C3TextEditorCompletionWidget::updateDescription()
{
	C3TextEditorCompletion * pCompletion = currentCompletion();
	if(!pCompletion)
	{
		_p->pInfoLabel->setText(QString());
		return;
	}

	_p->pInfoLabel->setText(pCompletion->szDescription);
}

void C3TextEditorCompletionWidget::slotSnippetTimerTimeout()
{
	//qDebug("Snippet timer timeout!");

	_p->oSnippetTimer.stop();
	C3TextEditorCompletion * pCompletion = currentCompletion();
	if(!pCompletion)
		return;

	quint32 uFromLine = pCompletion->uLineNumber;
	QString szPath = pCompletion->szFilePath;
	if(uFromLine < 1)
		return; // no line number available
	if(szPath.isEmpty())
		return; // no file available

	quint32 uToLine = uFromLine;
	if(uFromLine < 11)
	{
		_p->uBeforeContextLines = uFromLine - 1;
		uFromLine = 1;
	} else {
		_p->uBeforeContextLines = 10;
		uFromLine -= 10;
	}
	uToLine += 10;

	startSnippetExtractorWorker(szPath,uFromLine,uToLine);
}


static int charactersInCommon(const QString &s1,const QString &s2)
{
	int l1 = s1.length();
	int l2 = s2.length();
	
	int l = l1 < l2 ? l1 : l2; // ctags croaks on the < sign

	const QChar * p = s1.unicode();
	const QChar * p1 = p;
	const QChar * e = p1 + l;
	const QChar * p2 = s2.unicode();
	
	while(p1 < e)
	{
		if(p1->unicode() != p2->unicode())
			break;
		p1++;
		p2++;
	}
	
	return p1 - p;
}

bool C3TextEditorCompletionWidget::tabCompletion(const QString &szTextToComplete,QString &szResult,QString &szSuffix)
{
	QToolTip::hideText();

	int c = _p->pListWidget->count();
	int i = 0;

	int iMinLength = szTextToComplete.length();

	//qDebug("To complete %s",szTextToComplete.toUtf8().data());

	QString szBest;
	
	bool bThereAreLongerCompletions = false;

	C3TextEditorCompletionWidgetListWidgetItem * best = NULL;
	
	while(i < c)
	{
		C3TextEditorCompletionWidgetListWidgetItem * it = dynamic_cast<C3TextEditorCompletionWidgetListWidgetItem *>(_p->pListWidget->item(i));
		i++;
		if(!it)
			continue;
		
		QString szText = it->text();

		if(szText.length() < iMinLength)
			continue;

		if(!szText.startsWith(szTextToComplete))
			continue;
		
		QString szPart = szText.mid(iMinLength);
		if(szBest.isEmpty())
		{
			best = it;
			szBest = szPart;
			continue;
		}
		
		int iLen = charactersInCommon(szBest,szPart);
		
		if(iLen == 0)
			return false;

		if(iLen == szBest.length())
			continue;

		bThereAreLongerCompletions = true;

		szBest = szPart.left(iLen);
		best = it;
	}
	
	if(!best)
	{
		//qDebug("no best");
		return false;
	}
	
	if(!_p->pCompletions)
	{
		//qDebug("no completions");
		return false; // error
	}
	
	int idx = best->index();
	if((idx < 0) || (idx >= _p->pCompletions->count()))
	{
		//qDebug("no clue");
		return false; // error
	}

	C3TextEditorCompletion * pCompletion = _p->pCompletions->at(idx);
	Q_ASSERT(pCompletion);

	if(!bThereAreLongerCompletions) // did the best we could do
	{
		szResult = pCompletion->szSymbol;
		szSuffix = pCompletion->szSuffix;
		return true;
	}

	szResult = szTextToComplete + szBest;
	return true;
}

bool C3TextEditorCompletionWidget::enterCompletion(const QString &szTextToComplete,QString &szResult,QString &szSuffix)
{
	QToolTip::hideText();

	C3TextEditorCompletion * pCompletion = currentCompletion();
	if(!pCompletion)
		return false;

	szResult = pCompletion->szSymbol;
	szSuffix = pCompletion->szSuffix;
	//qDebug("ENTER COMPLETION RETURNING %s",oResult.szText.toUtf8().data());
	return true;
}
 
void C3TextEditorCompletionWidget::keyUp()
{
	QToolTip::hideText();
	int r = _p->pListWidget->currentRow();
	if(r < 1)
		return;
	_p->pListWidget->setCurrentRow(r - 1);
}

void C3TextEditorCompletionWidget::keyDown()
{
	QToolTip::hideText();
	int r = _p->pListWidget->currentRow();
	if(r >= (_p->pListWidget->count() - 1))
		return;
	_p->pListWidget->setCurrentRow(r + 1);
}

void C3TextEditorCompletionWidget::hideEvent(QHideEvent *e)
{
	_p->oSnippetTimer.stop();

	if(_p->pSnippetExtractorWorker)
		abortSnippetExtractorWorker();

	QToolTip::hideText();

	QWidget::hideEvent(e);
}

void C3TextEditorCompletionWidget::restartSnippetTimer()
{
	_p->oSnippetTimer.stop();
	_p->oSnippetTimer.start(1000);
}

void C3TextEditorCompletionWidget::slotSnippetReady()
{
	if(!isVisible())
		return;

	//qDebug("Snippet ready (%d chars)!",_p->szSnippet.length());

	if(_p->szSnippet.isEmpty())
		return;

	QListWidgetItem * it = _p->pListWidget->currentItem();

	QPoint pnt = _p->pListWidget->viewport()->mapToGlobal(_p->pListWidget->visualItemRect(it).topRight());

	_p->oSnippetExtractorWorkerMutex.lock();

	QString s = __utf8(
			"<div style='white-space:pre;'>"
			"%1"
			"</div>"
		).arg(_p->szSnippet);

	QToolTip::showText(
			pnt,
			s,
			this
		);
	_p->oSnippetExtractorWorkerMutex.unlock();
}


void C3TextEditorCompletionWidget::startSnippetExtractorWorker(const QString &szPath,quint32 uFromLine,quint32 uToLine)
{
	if(_p->pSnippetExtractorWorker)
		abortSnippetExtractorWorker();
	
	_p->oSnippetExtractorWorkerMutex.lock();
	_p->szSnippet = QString();
	_p->pSnippetExtractorWorker = new C3FileSnippetExtractorWorker(this,szPath,uFromLine,uToLine);
	_p->pSnippetExtractorWorker->setAutoDelete(true);
	_p->oSnippetExtractorWorkerMutex.unlock();

	QThreadPool::globalInstance()->start(_p->pSnippetExtractorWorker);
}

void C3TextEditorCompletionWidget::onFileSnippetExtractWorkerTerminated(
		C3FileSnippetExtractorWorker * pWorker,
		QStringList * pLines,
		const QString &szError
	)
{
	Q_ASSERT(_p->pSnippetExtractorWorker);
	Q_ASSERT(_p->pSnippetExtractorWorker == pWorker);
	_p->oSnippetExtractorWorkerMutex.lock();
	_p->pSnippetExtractorWorker = NULL;
	if(pLines)
	{
		// replace tabs with 

		int iMinSpaceLen = 9999;

		QStringList lReplaced;
		foreach(QString s,*pLines)
		{
			QString rep = s;
			rep.replace(QString("	"),QString("    "));
			const QChar * b = rep.unicode();
			const QChar * c = b;
			const QChar * e = c + rep.length();
			while(c < e)
			{
				if(!c->isSpace())
					break;
				c++;
			}
			int iSpaceLen = c - b;
			if(iSpaceLen < iMinSpaceLen)
				iMinSpaceLen = iSpaceLen;
			lReplaced.append(rep);
		}
		
		QStringList lReplaced2;
		
		foreach(QString s2,lReplaced)
		{
			if(iMinSpaceLen > 0)
				lReplaced2.append(s2.mid(iMinSpaceLen));
			else
				lReplaced2.append(s2);
		}
		
		int count = lReplaced2.count();
		
		if(count > _p->uBeforeContextLines)
		{
			int iTotalCount = count;
			// should be
			for(int i=_p->uBeforeContextLines;i<count;i++)
			{
				QString txt = lReplaced2.at(i);
				if(txt.contains(QChar(';')) || txt.contains(QChar('}')))
				{
					// this is the last line.
					iTotalCount = i+1;
					break;
				}
			}

			while(lReplaced2.count() > iTotalCount)
				lReplaced2.removeLast();
			
			int iKill = 0;
			
			for(int j=_p->uBeforeContextLines-1;j>=0;j--)
			{
				QString txt = lReplaced2.at(j);
				if(txt.contains(QChar(';')) || txt.contains(QChar('}')))
				{
					// this is a line that should be excluded
					iKill = j+1;
					break;
				}
			}

			while(iKill > 0)
			{
				lReplaced2.removeFirst();
				iKill--;
			}
		}
		
		_p->szSnippet = lReplaced2.join(QString());
		
	} else {
		_p->szSnippet = QString();
	}
	
	delete pLines;
	_p->oSnippetExtractorWorkerMutex.unlock();
	
	emit interThreadSnippetReady();
}

void C3TextEditorCompletionWidget::abortSnippetExtractorWorker()
{
	if(!_p->pSnippetExtractorWorker)
		return;
	_p->oSnippetExtractorWorkerMutex.lock();
	if(!_p->pSnippetExtractorWorker)
	{
		_p->oSnippetExtractorWorkerMutex.unlock();
		return;
	}
	if(!_p->pSnippetExtractorWorker->abort())
	{
		// abort failed because the worker is already in the termination call.
		// unlock the mutex and wait for it to exit
		_p->oSnippetExtractorWorkerMutex.unlock();

		qDebug("[C3TextEditorCompletionWidget::abortSnippetExtractorWorker] Waiting for the snippet extraction worker to exit...");

		while(_p->pSnippetExtractorWorker)
			QThread::msleep(50);

		_p->oSnippetExtractorWorkerMutex.lock();
	} else {
		// Abort succeeded. The worker will no longer call us back.
#if QT_VERSION >= 0x50500
		QThreadPool::globalInstance()->cancel(_p->pSnippetExtractorWorker);
#endif
		_p->pSnippetExtractorWorker = NULL;
	}

	_p->szSnippet = QString();
	_p->oSnippetExtractorWorkerMutex.unlock();
}
