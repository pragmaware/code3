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

#include "C3TextEditorLine.h"
#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorMode.h"
#include "C3TextEditorOptions.h"
#include "C3TextEditorUndo.h"

#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QJsonDocument>
#include <QDomDocument>

void C3TextEditor::fillFXMenu(QMenu *pMenu)
{
	bool bHasSelection = !_p->oSelection.isEmpty();

	QAction * a;

#define ACTION(_pMenu,_szText,_slot) \
	do { \
		QString szText = _szText; \
		if(bHasSelection) \
			szText += __utf8(" %1").arg(__tr("(Selection)")); \
		a = _pMenu->addAction(szText); \
		if(readOnly()) \
			a->setEnabled(false); \
		QObject::connect(a,SIGNAL(triggered()),this,_slot); \
	} while(0)

	QMenu * pConvertMenu = pMenu->addMenu(__tr("Convert"));

	ACTION(
			pConvertMenu,
			__tr("Remove Trailing CRs (convert Windows to UNIX)"),
			SLOT(fxRemoveTrailingCRs())
		);

	ACTION(
			pConvertMenu,
			__tr("Add Trailing CRs (convert UNIX to Windows)"),
			SLOT(fxAddTrailingCRs())
		);

	ACTION(
			pConvertMenu,
			__tr("Replace Leading Spaces with Tabs"),
			SLOT(fxReplaceLeadingSpacesWithTabs())
		);

	ACTION(
			pConvertMenu,
			__tr("Remove Trailing Spaces"),
			SLOT(fxRemoveTrailingSpaces())
		);

	ACTION(
			pConvertMenu,
			__tr("Convert to Upper Case"),
			SLOT(fxConvertToUpperCase())
		);

	ACTION(
			pConvertMenu,
			__tr("Convert to Lower Case"),
			SLOT(fxConvertToLowerCase())
		);


	QMenu * pEncodeDecodeMenu = pMenu->addMenu(__ascii("Encode/Decode"));

	ACTION(
			pEncodeDecodeMenu,
			__tr("Encode to Base64"),
			SLOT(fxEncodeBase64())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("Decode from Base64"),
			SLOT(fxDecodeBase64())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("Encode to Hex"),
			SLOT(fxEncodeHex())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("Decode from Hex"),
			SLOT(fxDecodeHex())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("Transcode Base64 to Hex"),
			SLOT(fxTranscodeBase64ToHex())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("Transcode Hex to Base64"),
			SLOT(fxTranscodeHexToBase64())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("URL Encode"),
			SLOT(fxEncodeURL())
		);

	ACTION(
			pEncodeDecodeMenu,
			__tr("URL Decode"),
			SLOT(fxDecodeURL())
		);


	QMenu * pMiscMenu = pMenu->addMenu(__tr("Miscellaneous"));

	ACTION(
			pMiscMenu,
			__tr("Format JSON"),
			SLOT(fxFormatJSON())
		);

	ACTION(
			pMiscMenu,
			__tr("Format XML"),
			SLOT(fxFormatXML())
		);

	ACTION(
			pMiscMenu,
			__tr("Remove Empty Lines"),
			SLOT(fxRemoveEmptyLines())
		);

	ACTION(
			pMiscMenu,
			__tr("Sort Lines Alphabetically"),
			SLOT(fxSortLinesAlphabetically())
		);
		
	ACTION(
			pMiscMenu,
			__tr("Sort Lines Alphabetically (Case Insensitive)"),
			SLOT(fxSortLinesAlphabeticallyCaseInsensitive())
		);
}

static QString replaceLeadingSpacesWithTabs(int iTabStopSize,const QString &szText)
{
	QStringList sl = szText.split(QChar('\n'));
	
	QString szRet;
	
	QString szSpaces;
	while(szSpaces.length() < iTabStopSize)
		szSpaces.append(QChar(' '));
	QString szTab("\t");
	
	foreach(QString s,sl)
	{
		const QChar * b = s.unicode();
		const QChar * c = b;
		const QChar * e = c + s.length();
		while(c < e)
		{
			if((c->unicode() != ' ') && (c->unicode() != '\t'))
				break;
			c++;
		}
		int l = c - b;
		QString left = s.left(l);
		QString right = s.mid(l);
		
		left.replace(szSpaces,szTab);
		
		szRet.append(left);
		szRet.append(right);
		szRet.append(QChar('\n'));
	}

	return szRet;
}

void C3TextEditor::fxRemoveTrailingCRs()
{
	QChar cr('\r');
	QString szCR = cr;

	beginVisualStateChange();
	
	beginMultiUndo();

	_p->fMaximumLineWidth = 0.0;

	int iFirstRow,iLastRow;
	
	int iLineCountMinusOne = _p->lLines.count() - 1;
	
	if(!_p->oSelection.isEmpty())
	{
		iFirstRow = _p->oSelection.start.row;
		iLastRow = _p->oSelection.end.row;
		_p->oSelection.clear();
	} else {
		iFirstRow = 0;
		iLastRow = iLineCountMinusOne;
	}
	
	_p->oMark.clear();

	qreal fMaximumInitialLineWidth = 0.0;
	qreal fMaximumFinalLineWidth = 0.0;

	{
		C3TextEditorLinesLocker oLocker(_p);
	
		for(int iRow = iFirstRow;iRow <= iLastRow;iRow++)
		{
			C3TextEditorLine * pLine = _p->lLines.at(iRow);
		
			if(!pLine->szText.endsWith(cr))
			{
				if(pLine->fWidth > _p->fMaximumLineWidth)
					_p->fMaximumLineWidth = pLine->fWidth;
				continue;
			}
	
			int iPos = pLine->szText.length() - 1;
	
			pushUndo(
					new C3TextEditorUndo(
							C3TextEditorUndo::Delete,
							C3TextEditorRange(iRow,iPos,iRow,iPos+1),
							szCR
						)
				);
	
			pLine->szText.truncate(iPos);
	
			if(pLine->fWidth > fMaximumInitialLineWidth)
				fMaximumInitialLineWidth = pLine->fWidth;
	
			pLine->fWidth = C3TextEditorFontMetricsUtils::widthOfWholeLine(
					_p->pFontMetrics,
					pLine->szText,
					_p->fFontTabStopWidth
				);
		
			if(pLine->fWidth > fMaximumFinalLineWidth)
				fMaximumFinalLineWidth = pLine->fWidth;
		}
	}
	
	updateOrInvalidateMaximumLineWidthAfterChange(fMaximumInitialLineWidth,fMaximumFinalLineWidth);

	_p->pMode->textModified(iFirstRow,iLastRow);
	
	endMultiUndo();
	
	endVisualStateChange(ForceFullRepaint);

	setModified(true);
}

void C3TextEditor::fxAddTrailingCRs()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QChar cCR('\r');
				QString szCRLF("\r\n");

				QStringList sl = szText.split(QChar('\n'));
				
				QString szRet;
				
				foreach(QString s,sl)
				{
					szRet += s;
					if(!s.endsWith(cCR))
						szRet += szCRLF;
					else
						szRet += cCR;
				}

				szText = szRet;
				return true;
			}
		);

}


void C3TextEditor::fxSelectionOrWholeBufferManipulationWithLambda(std::function<bool(QString &)> fnLambda)
{
	beginVisualStateChange();
	
	C3TextEditorRange oSavedSelection = _p->oSelection;
	C3TextEditorRowColumn oSavedCursor = _p->oCursor;
	
	if(_p->oSelection.isEmpty())
		selectAll();

	_p->oMark.clear();

	QString szText = getSelectionText();

	if(fnLambda(szText))
	{
		insertTextInternal(szText,true,true);
	} else {
		_p->oSelection = oSavedSelection;
		_p->oCursor = oSavedCursor;
	}

	endVisualStateChange();
}

void C3TextEditor::fxEncodeBase64()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.toUtf8();
				QByteArray b2 = b1.toBase64();
				szText = QString::fromUtf8(b2);
				return true;
			}
		);
}

void C3TextEditor::fxDecodeBase64()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[this](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.trimmed().toUtf8();
				QByteArray b2 = b1.replace('\n',"");
				b1 = b2.replace(' ',"");
				b2 = QByteArray::fromBase64(b1);
				int iExpectedSize = ((b1.size() / 4) * 3) - 2;
				// FIXME: Replace nulls?
				if(b2.size() < iExpectedSize)
				{
					QMessageBox::critical(
							this,
							__tr("Conversion Failed"),
							__tr("The input doesn't seem to be encoded in Base64"),
							QMessageBox::Ok
						);
			
					return false;
				}
				szText = QString::fromUtf8(b2);
				return true;
			}
		);
}

void C3TextEditor::fxEncodeHex()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.toUtf8();
				QByteArray b2 = b1.toHex();
				szText = QString::fromUtf8(b2);
				return true;
			}
		);
}

void C3TextEditor::fxDecodeHex()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[this](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.trimmed().toUtf8();
				QByteArray b2 = b1.replace('\n',"");
				b1 = b2.replace(' ',"");
				b2 = QByteArray::fromHex(b1);
				// FIXME: Replace nulls?
				int iExpectedSize = (b1.size() / 2);
				if(b2.size() < iExpectedSize)
				{
					QMessageBox::critical(
							this,
							__tr("Conversion Failed"),
							__tr("The input doesn't seem to be encoded in Hex"),
							QMessageBox::Ok
						);
			
					return false;
				}
				szText = QString::fromUtf8(b2);
				return true;
			}
		);
}

void C3TextEditor::fxTranscodeHexToBase64()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[this](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.trimmed().toUtf8();
				QByteArray b2 = b1.replace('\n',"");
				b1 = b2.replace(' ',"");
				b2 = QByteArray::fromHex(b1);
				int iExpectedSize = (b1.size() / 2);
				if(b2.size() < iExpectedSize)
				{
					QMessageBox::critical(
							this,
							__tr("Conversion Failed"),
							__tr("The input doesn't seem to be encoded in Hex"),
							QMessageBox::Ok
						);
			
					return false;
				}
				b1 = b2.toBase64();
				szText = QString::fromUtf8(b1);
				return true;
			}
		);
}

void C3TextEditor::fxTranscodeBase64ToHex()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[this](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.trimmed().toUtf8();
				QByteArray b2 = b1.replace('\n',"");
				b1 = b2.replace(' ',"");
				b2 = QByteArray::fromBase64(b1);
				int iExpectedSize = ((b1.size() / 4) * 3) - 2;
				// FIXME: Replace nulls?
				if(b2.size() < iExpectedSize)
				{
					QMessageBox::critical(
							this,
							__tr("Conversion Failed"),
							__tr("The input doesn't seem to be encoded in Base64"),
							QMessageBox::Ok
						);
			
					return false;
				}
				b1 = b2.toHex();
				szText = QString::fromUtf8(b1);
				return true;
			}
		);
}

void C3TextEditor::fxEncodeURL()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.toUtf8();
				QByteArray b2 = b1.toPercentEncoding();
				szText = QString::fromUtf8(b2);
				return true;
			}
		);
}

void C3TextEditor::fxDecodeURL()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;
				QByteArray b1 = szText.toUtf8();
				QByteArray b2 = QByteArray::fromPercentEncoding(b1);
				// FIXME: Replace eventual nulls?
				szText = QString::fromUtf8(b2);
				return true;
			}
		);
}

void C3TextEditor::fxFormatJSON()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[this](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QByteArray b = szText.toUtf8();
				QJsonParseError error;
				QJsonDocument oDoc = QJsonDocument::fromJson(b,&error);
				
				if(error.error != QJsonParseError::NoError)
				{
					QMessageBox::critical(
							this,
							__tr("JSON Parse Error"),
							error.errorString(),
							QMessageBox::Ok
						);
					return false;
				}

				b = oDoc.toJson(); // indented
				
				szText = QString::fromUtf8(b);
				return true;
			}
		);
}


void C3TextEditor::fxFormatXML()
{
	fxSelectionOrWholeBufferManipulationWithLambda(
			[this](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QDomDocument oDoc;
				QString szError;
				if(!oDoc.setContent(szText,&szError))
				{
					QMessageBox::critical(
							this,
							__tr("XML Parse Error"),
							szError,
							QMessageBox::Ok
						);
					return false;
				}
				
				szText = oDoc.toString(1); // indented with one space
				szText = replaceLeadingSpacesWithTabs(1,szText); // indented with one tab
				return true;
			}
		);
}

void C3TextEditor::fxReplaceLeadingSpacesWithTabs()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				szText = replaceLeadingSpacesWithTabs(p->iTabStopSize,szText);
				return true;
			}
		);

}

void C3TextEditor::fxRemoveTrailingSpaces()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QStringList sl = szText.split(QChar('\n'));
				
				QString szRet;
				
				foreach(QString s,sl)
				{
					int n = s.size() - 1;
					for (; n >= 0; --n)
					{
						if(!s.at(n).isSpace())
							break;
					}
					s = s.left(n + 1);

					if(!szRet.isEmpty())
						szRet.append(QChar('\n'));
					szRet += s;
				}

				szText = szRet;
				return true;
			}
		);

}


void C3TextEditor::fxRemoveEmptyLines()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QStringList sl = szText.split(QChar('\n'));
				
				QString szRet;
				
				foreach(QString s,sl)
				{
					if(s.trimmed().isEmpty())
						continue;
						
					if(!szRet.isEmpty())
						szRet.append(QChar('\n'));
					szRet += s;
				}

				szText = szRet;
				return true;
			}
		);

}

void C3TextEditor::fxConvertToLowerCase()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				szText = szText.toLower();
				return true;
			}
		);

}

void C3TextEditor::fxConvertToUpperCase()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				szText = szText.toUpper();
				return true;
			}
		);

}

void C3TextEditor::fxSortLinesAlphabetically()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QStringList sl = szText.split(QChar('\n'));

				sl.sort();

				szText = sl.join(QChar('\n'));
				return true;
			}
		);
}

void C3TextEditor::fxSortLinesAlphabeticallyCaseInsensitive()
{
	C3TextEditorPrivate * p = m_pC3TE;

	fxSelectionOrWholeBufferManipulationWithLambda(
			[this,p](QString &szText) -> bool {
				if(szText.isEmpty())
					return true;

				QStringList sl = szText.split(QChar('\n'));

				sl.sort(Qt::CaseInsensitive);

				szText = sl.join(QChar('\n'));
				return true;
			}
		);
}
