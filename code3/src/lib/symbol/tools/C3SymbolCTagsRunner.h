#ifndef _C3SymbolCTagsRunner_h_
#define _C3SymbolCTagsRunner_h_
//=============================================================================
//
//   File : C3SymbolCTagsRunner.h
//   Creation Date : 2015/11/18 20:06:41
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

class C3SymbolCTagsRunnerPrivate;

class C3SymbolCTagsRunnerJob;

#include "C3SymbolFile.h"

#include <QHash>
#include <QString>

class C3SymbolCTagsRunner
{
public:
	C3SymbolCTagsRunner(const QString &szCTagsExecutablePath);
	~C3SymbolCTagsRunner();

private:
	C3SymbolCTagsRunnerPrivate * m_pC3CTR;

public:

	enum JobFlags
	{
		// limit path scan depth to 1
		JobLimitRecursionDepthTo1 = 1
	};
	
	// add a job for the specified path and flags.
	// If szReplacePath is not empty then any occurence of szPath
	// in the ctags output will be replaced with szReplacePath.
	void addJob(const QString &szPath,const QString &szReplacePath,unsigned int uFlags);
	
	void addJob(const QString &szPath,const QString &szReplacePath,const QStringList &lExclusionPatterns,unsigned int uFlags);

	// This is the ONLY function that can be called concurrently.
	void abort();

	bool run();
	
	const QString & error();
	
	QHash<QString,C3SymbolFile *> * takeSymbolFiles();
	
private:

	bool runJob(C3SymbolCTagsRunnerJob * pJob);

}; // class C3SymbolCTagsRunner

#endif //!_C3SymbolCTagsRunner_h_
