#ifndef _C3ExternalCommandManager_h_
#define _C3ExternalCommandManager_h_
//=============================================================================
//
//   File : C3ExternalCommandManager.h
//   Creation Date : 2015/11/20 01:45:03
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

#include <QObject>

#include "C3Process.h"

class C3ExternalCommand;
class C3ExternalCommandManagerPrivate;
class C3ExternalCommandRun;

class C3ExternalCommandManager : public QObject
{
	Q_OBJECT
public:
	C3ExternalCommandManager();
	virtual ~C3ExternalCommandManager();

private:
	C3ExternalCommandManagerPrivate * m_pC3ECM;

	static C3ExternalCommandManager * m_pInstance;

public:

	static C3ExternalCommandManager * instance()
	{
		return m_pInstance;
	}

	void executeCommand(const C3ExternalCommand * pCommand);
	bool terminateCommandById(const QString &szId,bool bKill = false);
	bool isCommandExecuting(const QString &szId);

private:

	C3ExternalCommandRun * findRunById(const QString &szId);
	C3ExternalCommandRun * findRunByProcess(C3Process * pProc);
	void killRun(C3ExternalCommandRun * pRun);
	void killRun(const QString &szId);
	void killAllRuns();

	void handleProcessOutput(
			C3ExternalCommandRun * pRun,
			QString &szText,
			bool bIsStdError
		);

	C3ExternalCommandRun * createRun(const C3ExternalCommand * pCommand);

	bool executeCommandInternal(const C3ExternalCommand * pCommand,QString &szError);

private slots:
	void slotProcessError(QProcess::ProcessError error);
	void slotProcessFinished(int exitCode,QProcess::ExitStatus exitStatus);
	void slotProcessReadyReadStandardError();
	void slotProcessReadyReadStandardOutput();

}; // class C3ExternalCommandManager

#endif //!_C3ExternalCommandManager_h_