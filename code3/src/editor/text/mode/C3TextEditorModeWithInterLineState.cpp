//=============================================================================
//
//   File : C3TextEditorModeWithInterLineState.cpp
//   Creation Date : ven 30 ott 2015 00:30:55
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

#include "C3TextEditorModeWithInterLineState.h"

C3TextEditorModeWithInterLineState::C3TextEditorModeWithInterLineState(C3TextEditorFactory * pFactory,const QString &szId,const QString &szName,unsigned int uInterLineFlagMask)
	: C3TextEditorMode(pFactory,szId,szName)
{
	m_oParserState.pMode = this;
	m_oParserState.uInterLineFlagMask = uInterLineFlagMask;
	m_iLastValidMetadataLine = -1;
}

C3TextEditorModeWithInterLineState::~C3TextEditorModeWithInterLineState()
{
}

void C3TextEditorModeWithInterLineState::attach(
		C3TextEditorCoreData * pData
	)
{
	m_oParserState.pCoreData = pData;

	// Not strictly necessary.
	//foreach(C3TextEditorLine * pLine,pData->lLines)
	//	pLine->clearMetadata();

	m_iLastValidMetadataLine = -1;
}

// Note that this macro ASSUMES that _iFirstLine - 1 has valid metadata!
// It *will* crash if this isn't true.
#define COMPUTE_METADATA_FOR_LINE_RANGE(_iFirstLine,_iLastLine) \
	do { \
		int iFirst = _iFirstLine; \
		int iLast = _iLastLine; \
		\
		if(iFirst > 0) \
			m_oParserState.uInterLineFlags = m_oParserState.pCoreData->lLines.at(iFirst - 1)->lBlocks.last()->uFlags & m_oParserState.uInterLineFlagMask; \
		else \
			m_oParserState.uInterLineFlags = 0; \
		\
		for(int i=iFirst;i<=iLast;i++) \
		{ \
			m_oParserState.iLineRow = i; \
			m_oParserState.pLine = m_oParserState.pCoreData->lLines.at(i); \
			m_oParserState.pLine->clearMetadata(); \
			m_oParserState.p = m_oParserState.pLine->szText.constData(); \
			m_oParserState.e = m_oParserState.p + m_oParserState.pLine->szText.length(); \
			m_oParserState.fX = 0; \
			computeMetadata(); \
		} \
	} while(0)

bool C3TextEditorModeWithInterLineState::textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		)
{
	// FIXME: Check the return values, they don't seem to be used. Should be removed if meaningless.

	// iFirstLine is still in the editor, but it's metadata is no longer valid.
	// The other lines are no longer in the editor.

	if(m_iLastValidMetadataLine < iFirstLine)
		return true; // everything was already invalid

	COMPUTE_METADATA_FOR_LINE_RANGE(iFirstLine,iFirstLine);

	if(m_iLastValidMetadataLine <= iLastLine)
	{
		// The last valid metadata was within the deleted block.
		// We don't have anything to "save".
		m_iLastValidMetadataLine = iFirstLine;
		return true;
	}
	
	// There is at least one valid line after iFirstLine (because m_iLastValidMetadataLine was pointing after iLastLine)
	unsigned int uInterLineFlags1 = m_oParserState.pCoreData->lLines.at(iFirstLine)->lBlocks.last()->uFlags & m_oParserState.uInterLineFlagMask;
	unsigned int uInterLineFlags2 = (iFirstLine < (m_oParserState.pCoreData->lLines.count()-1)) ? (m_oParserState.pCoreData->lLines.at(iFirstLine+1)->lBlocks.first()->uFlags & m_oParserState.uInterLineFlagMask) : uInterLineFlags1;

	if(uInterLineFlags1 == uInterLineFlags2)
	{
		// State seems to be properly preserved. Save it.
		//qDebug("State properly preserved");
		int iRemovedLines = iLastLine - iFirstLine;
		m_iLastValidMetadataLine -= iRemovedLines;
		return true;
	}
	
	// State not properly preserved. Invalidate after.
	m_iLastValidMetadataLine = iFirstLine;
	return false;
}

bool C3TextEditorModeWithInterLineState::textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		)
{
	// FIXME: Check the return values, they don't seem to be used. Should be removed if meaningless.

	// The number of added lines is actually
	//     int iAddedLines = iLastLine - iFirstLine - 1;
	// It may be 0: this would mean that a single line was modified.
	// If it is greater than 0 then the first line was splitted in two
	// and the remaining lines were added and their metadata is invalid.

	if(m_iLastValidMetadataLine < iFirstLine)
		return true; // everything was already invalid

	// The insertion point was actually valid.
	int iLineCount = (iLastLine - iFirstLine) + 1;
	if(iLineCount > 50)
	{
		// it's far easier to just invalidate everything after the first line (it's out of the screen anyway)
		m_iLastValidMetadataLine = iFirstLine - 1;
		return false;
	}

	COMPUTE_METADATA_FOR_LINE_RANGE(iFirstLine,iLastLine);

	if(m_iLastValidMetadataLine == iFirstLine)
	{
		// The last valid metadata line was the inserted line
		// We don't have anything to "save".
		// This also catches the special case of inserting stuff at the end.
		m_iLastValidMetadataLine = iLastLine;
		return true;
	}
	
	// There is at least one valid line after iLastLine (because m_iLastValidMetadataLine was pointing after iFirstLine)
	unsigned int uInterLineFlags1 = m_oParserState.pCoreData->lLines.at(iLastLine)->lBlocks.last()->uFlags & m_oParserState.uInterLineFlagMask;
	unsigned int uInterLineFlags2 = (iLastLine < (m_oParserState.pCoreData->lLines.count()-1)) ? (m_oParserState.pCoreData->lLines.at(iLastLine+1)->lBlocks.first()->uFlags & m_oParserState.uInterLineFlagMask) : uInterLineFlags1;

	if(uInterLineFlags1 == uInterLineFlags2)
	{
		// State seems to be properly preserved. Save it.
		int iAddedLines = iLineCount - 1;
		m_iLastValidMetadataLine += iAddedLines;
		return true;
	}
	
	// State not properly preserved. Invalidate after.
	m_iLastValidMetadataLine = iLastLine;
	return false;
}

bool C3TextEditorModeWithInterLineState::textModified(
		int iFirstLine,
		int iLastLine
	)
{
	// FIXME: Check the return values, they don't seem to be used. Should be removed if meaningless.
	if(m_iLastValidMetadataLine < iLastLine)
	{
		m_iLastValidMetadataLine = iFirstLine - 1;
		return true; // something was already invalid, nothing to save after the change
	}

	int iLineCount = (iLastLine - iFirstLine) + 1;
	if(iLineCount > 50)
	{
		// it's far easier to just invalidate everything after the first line (it's out of the screen anyway)
		m_iLastValidMetadataLine = iFirstLine - 1;
		return false;
	}

	if(iLastLine >= m_oParserState.pCoreData->lLines.count())
	{
		// nothing to really save after the change
		m_iLastValidMetadataLine = iFirstLine - 1;
		return true;
	}

	COMPUTE_METADATA_FOR_LINE_RANGE(iFirstLine,iLastLine);

	if(m_iLastValidMetadataLine == iFirstLine)
	{
		// The last valid metadata line was the first modified line (so actually Q_ASSERT(iFirstLine == iLastLine) ?)
		// We don't have anything to "save".
		// This also catches the special case of inserting stuff at the end.
		m_iLastValidMetadataLine = iLastLine; // Probably not needed?
		return true;
	}
	
	// There is at least one valid line after iLastLine (because m_iLastValidMetadataLine was pointing after iFirstLine)
	unsigned int uInterLineFlags1 = m_oParserState.pCoreData->lLines.at(iLastLine)->lBlocks.last()->uFlags & m_oParserState.uInterLineFlagMask;
	unsigned int uInterLineFlags2 = (iLastLine < (m_oParserState.pCoreData->lLines.count()-1)) ? (m_oParserState.pCoreData->lLines.at(iLastLine+1)->lBlocks.first()->uFlags & m_oParserState.uInterLineFlagMask) : uInterLineFlags1;

	if(uInterLineFlags1 == uInterLineFlags2)
	{
		// State seems to be properly preserved.
		return true;
	}
	
	// State not properly preserved. Invalidate after.
	m_iLastValidMetadataLine = iLastLine;
	return false;
}


void C3TextEditorModeWithInterLineState::ensureMetadataUpToDate(int iFirstLine,int iLastLine)
{
	if(iLastLine <= m_iLastValidMetadataLine)
		return; // everything is valid up till here

	// the last line is after our last valid metadata
	// something is not valid in the part we're painting
	
	// we actually ignore iFirstLine, whatever it is. 

	COMPUTE_METADATA_FOR_LINE_RANGE(m_iLastValidMetadataLine + 1,iLastLine);

	m_iLastValidMetadataLine = iLastLine;
}
