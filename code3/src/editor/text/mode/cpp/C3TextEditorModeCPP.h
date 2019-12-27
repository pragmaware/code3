#ifndef _C3TextEditorModeCPP_h_
#define _C3TextEditorModeCPP_h_
//=============================================================================
//
//   File : C3TextEditorModeCPP.h
//   Creation Date : gio 29 ott 2015 02:50:45
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

#include "C3TextEditorModeWithSymbolAnalysis.h"

class C3TextEditorModeParserCPP;
class C3TextEditorRowColumn;

#include "C3TextEditorLine.h"
#include "C3TextEditorRange.h"

///
/// \class C3TextEditorModeCPP
/// \brief The C3TextEditorModeCPP class
///
/// This class...
///
class C3TextEditorModeCPP : public C3TextEditorModeWithSymbolAnalysis
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorModeCPP
	///
	C3TextEditorModeCPP(C3TextEditorFactory * pFactory);

	///
	/// Destroys the instance of C3TextEditorModeCPP
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorModeCPP();

private:

	C3TextEditorModeParserCPP * m_pParser;

protected:

	virtual void computeMetadata();
	virtual void triggerSymbolCompletion(const C3TextEditorRowColumn &rc,unsigned int uFlags);
	virtual bool triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags);
	virtual void fillCodeMenu(QMenu * pCodeMenu);
	virtual bool handleKeyEvent(QKeyEvent * e);

private Q_SLOTS:
	void slotGenerateMemberVariable();
	void slotGenerateMemberFunction();
	void slotAddIncludeFile();
	void slotGenerateEnumSwitch();

private:

	void completeIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock);
	void completePreprocessorToken(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock);

	bool identifyLinkToIncludeFile(const C3TextEditorRowColumn &rc,C3TextEditorLine * pLine,C3TextEditorLine::Block * pBlock);
	

}; // class C3TextEditorModeCPP

#endif //!_C3TextEditorModeCPP_h_
