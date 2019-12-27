#ifndef _C3TextEditorModeWithInterLineState_h_
#define _C3TextEditorModeWithInterLineState_h_
//=============================================================================
//
//   File : C3TextEditorModeWithInterLineState.h
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


#include "c3_config.h"

#include "C3TextEditorMode.h"
#include "C3TextEditorModeParser.h"


///
/// \class C3TextEditorModeWithInterLineState
/// \brief The C3TextEditorModeWithInterLineState class
///
/// This class...
///
class C3TextEditorModeWithInterLineState : public C3TextEditorMode
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModeWithInterLineState
	///
	C3TextEditorModeWithInterLineState(C3TextEditorFactory * pFactory,const QString &szId,const QString &szName,unsigned int uInterLineFlagMask);

	///
	/// Destroys the instance of C3TextEditorModeWithInterLineState
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeWithInterLineState();

private:

	int m_iLastValidMetadataLine;

protected:

	C3TextEditorModeParserState m_oParserState;

public:

	virtual void attach(
			C3TextEditorCoreData * pData
		);

	virtual void ensureMetadataUpToDate(
			int iFirstLine,
			int iLastLine
		);

	virtual bool textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		);

	virtual bool textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		);

	virtual bool textModified(
			int iFirstLine,
			int iLastLine
		);

protected:

	///
	/// This one must compute the blocks for the currently set line (m_pLine)
	/// and update m_uInterLineFlags with the state at the end of the line.
	/// It can use the m_pLine, m_pCoreData and m_uInterLineFlags members.
	///
	virtual void computeMetadata() = 0;

	///
	/// Returns the parser state.
	///
	C3TextEditorModeParserState * parserState()
	{
		return &m_oParserState;
	}
	
}; // class C3TextEditorModeWithInterLineState

#endif //!_C3TextEditorModeWithInterLineState_h_
