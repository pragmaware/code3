#ifndef _C3TextEditorMode_h_
#define _C3TextEditorMode_h_
//=============================================================================
//
//   File : C3TextEditorMode.h
//   Creation Date : sab 24 ott 2015 23:38:14
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

#include "C3TextEditorCoreData.h"
#include "C3TextEditorRange.h"
#include "C3Link.h"

#include <QHash>
#include <QVariant>
#include <QObject>

class QMenu;
class QKeyEvent;

class C3TextEditorRowColumn;
class C3TextEditorFactory;

///
/// \class C3TextEditorMode
/// \brief The C3TextEditorMode class
///
/// The base class of all the text editor modes.
///
/// Its main job is to build the Block objects for each C3TextEditorLine.
/// 
/// Rules:
///  - A mode is always attached to an existing file.
///
///  - The file doesn't change during the life-span of a mode (when the file is closed the mode is destroyed)
///    In other words, the mode is NOT reused over multiple files.
///
///  - The mode *MUST* compute at least one block for each line. If there is no block the line is assumed to be empty.
///
///  - The mode *MUST* separate tab blocks from non-tab blocks. (i.e. tab sequences must reside in a block on their own)
///
///  - The editor may add lines at the end of the buffer without notifying the mode.
///    The mode will know the next time a request to one of its functions is made.
///
///  - All the other changes to the buffer are notified.
///
///  - When creating a block the mode *MUST* set its szText, iWidth and uFlags members.
///       szText is the block of text extracted
///       iWidth is the width of the block that must be computed by using the C3TextEditorFontMetricsUtils functions
///       uFlags *MUST* contain C3TextEditorLine::Block::IsTabBlock if the block is a run of tab characters
///              and *MUST NOT* contain this flag is the block is a run of non-tab characters. Other flags are ignored
///              and can be used by the mode itself to keep state.
///
/// - Zero length blocks are allowed. Often they are useful at the end of the line to store an "end of line state".
///
class C3TextEditorMode : public QObject
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorMode
	///
	C3TextEditorMode(C3TextEditorFactory * pFactory,const QString &szId,const QString &szName);

	///
	/// Destroys the instance of C3TextEditorMode
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorMode();

private:

	C3TextEditorFactory * m_pFactory;
	QString m_szId;
	QString m_szName;

public:

	const QString & id() const
	{
		return m_szId;
	}

	const QString & name() const
	{
		return m_szName;
	}
	
	C3TextEditorFactory * factory()
	{
		return m_pFactory;
	}
	
	virtual void fillCodeMenu(QMenu * pCodeMenu);
	
	virtual bool handleKeyEvent(QKeyEvent * e);

	virtual void fileSaved(bool bWasModified);

	//
	// This function is called when the mode is attached to an existing buffer
	// or the buffer has been so modified that it's not worth to apply incremental updates
	// to the mode.
	//
	// You should use it to invalidate the current buffer metadata.
	//
	// For modes that need inter-line state a good optimisation is to keep
	// track of the index of the highest-numbered line that has a valid state.
	// With this optimisation you can just set this index to -1 here.
	//
	// Other modes might sweep through pData->lLines and clear all the blocks.
	//
	// Note that this function can be called multiple times.
	//
	virtual void attach(
			C3TextEditorCoreData * pData
		) = 0;

	//
	// This is called just before painting.
	// It *MUST* ensure that the metadata for the specified line range is up to date.
	//
	virtual void ensureMetadataUpToDate(
			int iFirstLine,
			int iLastLine
		) = 0;

	//
	// This is called just after the text has been inserted.
	// iFirstLine is a line that was existing in the editor but was changed (and the editor cleared its blocks)
	// The other lines (if any) are new and their blocks and state are empty.
	// Return false if the insertion has invalidated the metadata of lines after iLastLine and true otherwise.
	//
	virtual bool textInserted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		) = 0;

	//
	// This is called just after the text has been deleted.
	// iFirstLine is a line that remains in the editor after the deletion (but has invalid metadata)
	// The other lines no longer exist (and other lines may be in their place).
	// Return false if the deletion has invalidated the metadata of lines after iFirstLine and true otherwise.
	//
	virtual bool textDeleted(
			const QString &szText,
			int iFirstLine,
			int iLastLine
		) = 0;

	//
	// This is called just after the text has been modified in multiple lines.
	// Both iFirstLine and iLastLine do still exist in the editor.
	// Return false if the modification has invalidated the metadata of lines after iLastLine and true otherwise.
	//
	virtual bool textModified(
			int iFirstLine,
			int iLastLine
		) = 0;


	enum SymbolCompletionFlags
	{
		// Don't filter the symbols depending on context. Just complete it.
		SymbolCompletionNoContextFilter = 1,
		SymbolCompletionMatchInTheMiddle = 2,
		// Match in the middle, but only if the text is longer than 1 character, otherwise match at beginning only
		SymbolCompletionMatchInTheMiddleIfLongerThanOne = 4,
		// The case insensitive match works only if matching in the middle
		SymbolCompletionMatchCaseInsensitive = 8
	};

	//
	// Trigger an asynchronous symbol completion.
	//
	virtual void triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags);

	enum SymbolIdentificationFlags
	{
		// Don't filter the symbols depending on context. Just identify anything with this name.
		SymbolIdentificationNoContextFilter = 1
	};

	//
	// Trigger asynchronous symbol identification.
	//
	virtual bool triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags);

}; // class C3TextEditorMode

#endif //!_C3TextEditorMode_h_