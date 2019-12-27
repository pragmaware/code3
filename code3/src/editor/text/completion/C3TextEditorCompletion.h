#ifndef _C3TextEditorCompletion_h_
#define _C3TextEditorCompletion_h_
//=============================================================================
//
//   File : C3TextEditorCompletion.h
//   Creation Date : 2015/12/01 00:24:13
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

class C3TextEditorCompletion
{
public:
	enum Type
	{
		Function,
		Class,
		Namespace,
		Union,
		TypeDefinition,
		Macro,
		Enumeration,
		EnumerationMember,
		MemberVariable,
		LocalVariable,
		GlobalVariable,
		IncludeFile,
		Keyword,
		OtherSymbol
	};

public:
	// The type of completion
	Type eType;
	// The completed symbol
	QString szSymbol;
	// The higher, the more likely this is the correct completion
	// Do NOT consider the length of the token. The widget already gets
	// the token with shortest length at the best score.
	quint16 uScore;
	// The eventual suffix to add at the end
	QString szSuffix;
	// an eventual description
	QString szDescription;
	// the file path this symbol is referring to (empty if not known/used)
	QString szFilePath;
	// the line number this symbol is referring to (0 if not known/used)
	quint32 uLineNumber;
	// A key that can be used by the completion algorithm to choose which completions to show
	QString szKey;
public:
	C3TextEditorCompletion(
			Type type,
			const QString &symbol,
			const QString &suffix = QString(),
			quint16 score = 0,
			const QString &description = QString(),
			const QString &filePath = QString(),
			quint32 lineNumber = 0,
			const QString &key = QString()
		) :
		eType(type),
		szSymbol(symbol),
		szSuffix(suffix),
		uScore(score),
		szDescription(description),
		szFilePath(filePath),
		uLineNumber(lineNumber),
		szKey(key)
	{
	}

	static bool keyLessThan(const C3TextEditorCompletion * t1,const C3TextEditorCompletion * t2)
	{
		return t1->szKey < t2->szKey;
	}

}; // class C3TextEditorCompletion

#endif //!_C3TextEditorCompletion_h_