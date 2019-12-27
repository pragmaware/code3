#ifndef _C3TextEditorLine_h_
#define _C3TextEditorLine_h_
//=============================================================================
//
//   File : C3TextEditorLine.h
//   Creation Date : ven 23 ott 2015 05:49:18
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


#include "c3_config.h"

#include <QString>
#include <QLinkedList>
#include <QtAlgorithms>
#include <QColor>

class C3TextEditorLinePayload
{
public:
	virtual ~C3TextEditorLinePayload()
	{
	}
};

///
/// \class C3TextEditorLine
/// \brief The C3TextEditorLine class
///
/// This class...
///
class C3TextEditorLine
{
public:
	// A single block of text.
	//
	// The blocks are created by C3TextEditorMode instances.
	//
	// Rule for tab managment: tab runs must be blocks on their own. You can't mix tab and non tab characters
	// in a single block since it would mess up tab-stop painting.
	class Block
	{
	public:
		enum BasicFlags
		{
			IsTabBlock = 1
		};

	public:
		// The text in this block. 
		QString szText;
		// The width of this block. Must be computed by using C3TextEditorFontMetricsUtils functions.
		qreal fWidth;
		// The flags. If this block is a tab run (i.e is made all of tabs) then uFlags must contain IsTabBlock. If this block
		// is NOT a tab run (i.e does not contain tabs) then uFlags must NOT contain IsTabBlock.
		// The other flags can be used by modes to signal their state. 
		// WARNING: Multiple C3TextEditorModeParser objects can work on the same lines. In the cases they do they do
		// extra care should be taken to avoid flag collisions.
		unsigned int uFlags;
		// The color
		QColor * pColor;
	};

public:

	QString szText;

	// Width, depending on font metrics.
	qreal fWidth;

	// Line metadata.
	// This is written by the modes.
	
	// The line blocks
	QLinkedList<Block *> lBlocks;
	
	C3TextEditorLinePayload * pPayload;

public:

	inline void clearMetadata()
	{
		if(lBlocks.count() < 1)
			return;
		qDeleteAll(lBlocks);
		lBlocks.clear();
		if(pPayload)
		{
			delete pPayload;
			pPayload = NULL;
		}
	}

	//
	// This one finds the block the specified character is in or NULL if the block is not in any character.
	// For this purpose we assume that the character exactly
	// at the end of a block is IN (and thus the character exactly
	// at the beginning of a block is NOT IN.
	// Reason: this is mainly used for completion.
	//
	Block * findBlock(int iCharacterIdx)
	{
		int iTotal = 0;
		if(lBlocks.count() < 1)
			return NULL;
		
		foreach(Block * b,lBlocks)
		{
			iTotal += b->szText.length();
			if(iCharacterIdx <= iTotal)
				return b;
		}
		
		return lBlocks.last(); // because cursor may be _after_ the last block too (but it is rendered at the end).
	}

	C3TextEditorLine()
	{
		fWidth = 0.0;
		pPayload = NULL;
	}
	
	~C3TextEditorLine()
	{
		if(lBlocks.count() > 0)
			qDeleteAll(lBlocks);
		if(pPayload)
			delete pPayload;
	}

}; // class C3TextEditorLine

#endif //!_C3TextEditorLine_h_