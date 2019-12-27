#ifndef _C3TextEditorFontMetricsUtils_h_
#define _C3TextEditorFontMetricsUtils_h_
//=============================================================================
//
//   File : C3TextEditorFontMetricsUtils.h
//   Creation Date : sab 24 ott 2015 01:11:02
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

#include <QString>
#include <QFontMetricsF>

#include <math.h>

namespace C3TextEditorFontMetricsUtils
{
	// Call this to compute the width of a *WHOLE LINE* of text (not a block)
	inline qreal widthOfWholeLine(
			QFontMetricsF * pFontMetrics,
			const QString &szText,
			qreal fFontTabStopWidth
		)
	{
		return pFontMetrics->size(
				Qt::TextSingleLine | Qt::TextExpandTabs,
				szText,
				fFontTabStopWidth
			).width();
	}
	
	// Call this to compute the width of a block that does NOT contain tabs.
	inline qreal widthOfNonTabBlock(
			QFontMetricsF * pFontMetrics,
			const QString &szText
		)
	{
		return pFontMetrics->width(szText);
	}
	
	// Call this to compute the width of a block of consecutive tabs.
	inline qreal widthOfTabBlock(
			int iNumberOfTabs,
			qreal fStartXPosition,
			qreal fFontTabStopWidth
		)
	{
		if(iNumberOfTabs < 1)
			return 0;
		qreal fStartTabX = ::floor(fStartXPosition / fFontTabStopWidth) * fFontTabStopWidth;
		qreal fTabsWidth = iNumberOfTabs * fFontTabStopWidth;
		qreal fEndTabX = fStartTabX + fTabsWidth;
		return fEndTabX - fStartXPosition;
	}

	// Call this to find the cursor position nearests to a specific x coordinate
	// in a block of consecutive tabs.
	inline int charCountOfNearestTabStop(
			qreal fPointXOffset,
			qreal fStartXPosition,
			qreal fFontTabStopWidth
		)
	{
		qreal fStartTabX = ::floor(fStartXPosition / fFontTabStopWidth) * fFontTabStopWidth;
		qreal fPointX = fStartXPosition + fPointXOffset;
		qreal fBeforeTabX = ::floor(fPointX / fFontTabStopWidth) * fFontTabStopWidth;
		int iTabCount = (int)((fBeforeTabX - fStartTabX) / fFontTabStopWidth);
		qreal fBeforeTabPlusHalfX = fBeforeTabX + (fFontTabStopWidth / 2.0);
		if(fPointX > fBeforeTabPlusHalfX)
			iTabCount++;
		return iTabCount;
	}
}

#endif //!_C3TextEditorFontMetricsUtils_h_
