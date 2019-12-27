//=============================================================================
//
//   File : C3TextEditorModePlainText.cpp
//   Creation Date : sab 24 ott 2015 23:41:10
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

#include "C3TextEditorModePlainText.h"
#include "C3TextEditorFontMetricsUtils.h"
#include "C3TextEditorOptions.h"

class C3TextEditorModePlainTextPrivate
{
public:
	C3TextEditorCoreData * pData;
};

#define _p m_pC3TEMPT

C3TextEditorModePlainText::C3TextEditorModePlainText(C3TextEditorFactory * pFactory)
	: C3TextEditorMode(pFactory,__utf8("plain"),__tr("Plain Text"))
{
	_p = new C3TextEditorModePlainTextPrivate();

}

C3TextEditorModePlainText::~C3TextEditorModePlainText()
{
	delete _p;
}

void C3TextEditorModePlainText::attach(
		C3TextEditorCoreData * pData
	)
{
	_p->pData = pData;

	foreach(C3TextEditorLine * pLine,pData->lLines)
		pLine->clearMetadata();
}

bool C3TextEditorModePlainText::textModified(
		int iFirstLine,
		int iLastLine
	)
{
	for(int iRow = iFirstLine;iRow <= iLastLine;iRow++)
		_p->pData->lLines.at(iRow)->clearMetadata();
	
	return true;
}

bool C3TextEditorModePlainText::textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		)
{
	_p->pData->lLines.at(iFirstLine)->clearMetadata();
	// the other lines have no blocks yet
	return true;
}

bool C3TextEditorModePlainText::textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		)
{
	_p->pData->lLines.at(iFirstLine)->clearMetadata();
	// the other lines have been deleted
	return true;
}


void C3TextEditorModePlainText::ensureMetadataUpToDate(
		int iFirstLine,
		int iLastLine
	)
{
	for(int i=iFirstLine;i<=iLastLine;i++)
	{
		C3TextEditorLine * pLine = _p->pData->lLines.at(i);
		if(pLine->lBlocks.count() > 1)
			continue; // already ok
		computeBlocks(pLine);
	}
}

void C3TextEditorModePlainText::computeBlocks(C3TextEditorLine * pLine)
{
	pLine->clearMetadata();

	// start
	const QChar * s = pLine->szText.constData();
	// end
	const QChar * e = s + pLine->szText.length();
	// pointer
	const QChar * p = s;
	// begin of block
	const QChar * b = p;

	C3TextEditorLine::Block * pBlock;

	if(p == e)
	{
		// single empty block
		pBlock = new C3TextEditorLine::Block();
		pBlock->fWidth = 0;
		pBlock->uFlags = 0;
		pBlock->pColor = &(_p->pData->pOptions->oTextColor);
		pLine->lBlocks.append(pBlock);
		return;
	}

	float fX = 0.0;

	while(p < e)
	{
		if(p->unicode() == (ushort)'\t')
		{
			p++;
			while(p < e)
			{
				if(p->unicode() != (ushort)'\t')
					break;
				p++;
			}

			pBlock = new C3TextEditorLine::Block();
			pBlock->szText.setUnicode(b,p-b);
			pBlock->fWidth = C3TextEditorFontMetricsUtils::widthOfTabBlock(pBlock->szText.length(),fX,_p->pData->fFontTabStopWidth);
			pBlock->uFlags = C3TextEditorLine::Block::IsTabBlock;
			pBlock->pColor = &(_p->pData->pOptions->oTextColor);
			pLine->lBlocks.append(pBlock);
			fX += pBlock->fWidth;
			b = p;
		} else {
			// non tab
			p++;
			
			// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
			const QChar * tmpEnd = p + 256;
			if(tmpEnd > e)
				tmpEnd = e;
			
			while(p < tmpEnd)
			{
				if(p->unicode() == (ushort)'\t')
					break;
				p++;
			}

			pBlock = new C3TextEditorLine::Block();
			pBlock->szText.setUnicode(b,p-b);
			pBlock->fWidth = C3TextEditorFontMetricsUtils::widthOfNonTabBlock(_p->pData->pFontMetrics,pBlock->szText);
			pBlock->uFlags = 0;
			pBlock->pColor = &(_p->pData->pOptions->oTextColor);
			pLine->lBlocks.append(pBlock);
			fX += pBlock->fWidth;
			b = p;
		}
	}
}
