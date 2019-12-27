#ifndef _C3TextEditorSymbolContext_h_
#define _C3TextEditorSymbolContext_h_
//=============================================================================
//
//   File : C3TextEditorSymbolContext.h
//   Creation Date : 2015/12/10 22:11:52
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
#include <QList>

#include "C3TextEditorRange.h"

//                               [range]
// [left text]  [left operator]  [text]
//  pPointer        ->           func
//    i             =            ''
//  ClassName       ::           func
//  void            ''           func
//  something       (            var
//  typename        *            varname
//  typename        ''           varname
//
class C3TextEditorSymbolContext
{
public:
	
	QString szFilePath;
	
	C3TextEditorRange oRange;

	class AdditionalContext
	{
	public:
		QString szText;
		QString szOperator;
	};

	// Note that the additional context goes from right to left
	// so index 0 is the rightmost of the left context parts
	//
	//  2         1                 0           text
	// this->function(params)->otherFunction()->x
	QList<AdditionalContext *> lLeftContext;

	QString szText;

public:
	~C3TextEditorSymbolContext()
	{
		qDeleteAll(lLeftContext);
	}
	
	void removeEmptyLeftContext()
	{
		while(lLeftContext.count() > 0)
		{
			AdditionalContext * c = lLeftContext.last();
			if(!(c->szOperator.isEmpty() && c->szText.isEmpty()))
				return;

			lLeftContext.takeLast();
			delete c;
		}
	}
	
	AdditionalContext * addLeftContext()
	{
		AdditionalContext * c = new AdditionalContext();
		lLeftContext.append(c);
		return c;
	}

	AdditionalContext * firstLeftContext()
	{
		if(lLeftContext.count() < 1)
			return NULL;
		return lLeftContext.at(0);
	}

}; // class C3TextEditorSymbolContext

#endif //!_C3TextEditorSymbolContext_h_