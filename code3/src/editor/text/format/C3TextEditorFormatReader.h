#ifndef _C3TextEditorFormatReader_h_
#define _C3TextEditorFormatReader_h_
//=============================================================================
//
//   File : C3TextEditorFormatReader.h
//   Creation Date : gio 22 ott 2015 22:50:40
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

#include <QRunnable>
#include <QString>

class C3TextEditor;
class C3TextEditorLine;
class C3TextEditorFormatReaderPrivate;
class QFontMetricsF;

///
/// \class C3TextEditorFormatReader
/// \brief The C3TextEditorFormatReader class
///
/// This class...
///
class C3TextEditorFormatReader : public QRunnable
{
public:

	///
	/// Creates an instance of C3TextEditorFormatReader
	///
	C3TextEditorFormatReader();

	///
	/// Destroys the instance of C3TextEditorFormatReader
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFormatReader();

private:

	C3TextEditorFormatReaderPrivate * m_pTEFRPP;

public:

	// Called before starting
	void setup(const QString &szPath,C3TextEditor * pEditor,const QFontMetricsF &oFontMetrics,int fFontTabStopWidth);

	// clears the so you should not access it anymore.
	void abort();

protected:

	// Returns true if aborted
	bool aborted();

	// Returns the path of the file to be opened
	const QString & path();

	// Call this in case of error and then return.
	void error(const QString &szError);

	// Call this if you couldn't read the contents exactly
	// as they are on disk and saving them would result in
	// file corruption.
	void markContentAsAltered(const QString &szReason);

	// Emit a list of lines.
	// The list is cleared upon return and ownership is transferred
	void emitLines(QLinkedList<C3TextEditorLine *> &lLines);

	virtual void runInternal() = 0;

private:

	virtual void run();

}; // class C3TextEditorFormatReader

#endif //!_C3TextEditorFormatReader_h_
