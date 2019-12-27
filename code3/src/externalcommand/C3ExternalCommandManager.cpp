//=============================================================================
//
//   File : C3ExternalCommandManager.cpp
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

#include "C3ExternalCommandManager.h"
#include "C3ExternalCommand.h"
#include "C3ExternalToolManager.h"
#include "C3GeneralActions.h"
#include "C3Workspace.h"
#include "C3MainWindow.h"
#include "C3DockTextConsole.h"
#include "C3Process.h"
#include "C3FileUtils.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QDir>
#include <QList>
#include <QInputDialog>

class C3ExternalCommandRun
{
public:
	C3ExternalCommand oCommand;
	C3Process oProcess;
	C3DockConsoleChannel oChannel;
public:
	C3ExternalCommandRun(const C3ExternalCommand &cmd)
		: oCommand(cmd)
	{
	}
};

class C3ExternalCommandManagerPrivate
{
public:
	QList<C3ExternalCommandRun *> lRuns;
};

#define _p m_pC3ECM

C3ExternalCommandManager * C3ExternalCommandManager::m_pInstance = NULL;

C3ExternalCommandManager::C3ExternalCommandManager()
	: QObject()
{
	m_pInstance = this;
	_p = new C3ExternalCommandManagerPrivate();
}

C3ExternalCommandManager::~C3ExternalCommandManager()
{
	killAllRuns();
	m_pInstance = NULL;
	delete _p;
}

C3ExternalCommandRun * C3ExternalCommandManager::findRunById(const QString &szId)
{
	foreach(C3ExternalCommandRun * pRun,_p->lRuns)
	{
		if(pRun->oCommand.id() == szId)
			return pRun;
	}
	return NULL;
}

C3ExternalCommandRun * C3ExternalCommandManager::findRunByProcess(C3Process * pProc)
{
	foreach(C3ExternalCommandRun * pRun,_p->lRuns)
	{
		if(&(pRun->oProcess) == pProc)
			return pRun;
	}
	return NULL;
}

void C3ExternalCommandManager::killRun(C3ExternalCommandRun * pRun)
{
	pRun->oProcess.kill();
	_p->lRuns.removeAll(pRun);
	delete pRun;

	C3MainWindow::instance()->updateExternalCommandToolBarControls();
}

void C3ExternalCommandManager::killRun(const QString &szId)
{
	C3ExternalCommandRun * pRun = findRunById(szId);
	if(!pRun)
		return;
	killRun(pRun);
}

void C3ExternalCommandManager::killAllRuns()
{
	while(_p->lRuns.count() > 0)
		killRun(_p->lRuns.first());
}

bool C3ExternalCommandManager::terminateCommandById(const QString &szId,bool bKill)
{
	C3ExternalCommandRun * pRun = findRunById(szId);
	if(!pRun)
		return false;
	// PROBLEM: This does not terminate children.
	// kill() doesn't seem to work either.
	pRun->oProcess.terminate();
	if(bKill)
		killRun(pRun);
	return true;
}

bool C3ExternalCommandManager::isCommandExecuting(const QString &szId)
{
	return findRunById(szId);
}

C3ExternalCommandRun * C3ExternalCommandManager::createRun(const C3ExternalCommand * pCommand)
{
	C3ExternalCommandRun * pRun = new C3ExternalCommandRun(*pCommand);
	
	pRun->oChannel.szId = pCommand->id();
	pRun->oChannel.szTitle = pCommand->name();
	pRun->oChannel.bCanBeTerminated = true;

	QObject::connect(&(pRun->oProcess),SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotProcessError(QProcess::ProcessError)));
	QObject::connect(&(pRun->oProcess),SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slotProcessFinished(int,QProcess::ExitStatus)));
	QObject::connect(&(pRun->oProcess),SIGNAL(readyReadStandardError()),this,SLOT(slotProcessReadyReadStandardError()));
	QObject::connect(&(pRun->oProcess),SIGNAL(readyReadStandardOutput()),this,SLOT(slotProcessReadyReadStandardOutput()));
	
	return pRun;
}

bool C3ExternalCommandManager::executeCommandInternal(const C3ExternalCommand * pCommand,QString &szError)
{
	QString szWorkingDir = pCommand->workingDirectory();
	if(szWorkingDir.isEmpty())
	{
		szWorkingDir = C3Workspace::currentWorkspace()->rootPath();
		if(szWorkingDir.isEmpty())
			szWorkingDir = C3MainWindow::instance()->currentDirectory();
	} else {
		if(!C3FileUtils::isAbsolutePath(szWorkingDir))
		{
			QString szRootPath = C3Workspace::currentWorkspace()->rootPath();
			if(szRootPath.isEmpty())
				szRootPath = C3MainWindow::instance()->currentDirectory();
			szWorkingDir = C3FileUtils::mergePathComponents(szRootPath,szWorkingDir);
		}
	}
	
	if(!szWorkingDir.isEmpty())
	{
		if(!QDir::setCurrent(szWorkingDir))
		{
			szError = __tr("Can't change current directory to %1").arg(szWorkingDir);
			return false;
		}
	}
	
	bool bNeedShell = (pCommand->type() == C3ExternalCommand::DetachedShellScript) || (pCommand->type() == C3ExternalCommand::ControlledShellScript);

	QString szShell;

	if(bNeedShell)
	{
		szShell = C3ExternalToolManager::instance()->findTool(__utf8("bash"));
		if(szShell.isEmpty())
		{
			C3ExternalToolManager::instance()->findTool(__utf8("sh"));
			if(szShell.isEmpty())
			{
				szError = __tr("Could not find an apropriate shell executable");
				return false;
			}
		}
	}

	QString szCommand;
	
	if(pCommand->needsPassword())
	{
		QHash<QString,QString> hVariables;
		hVariables.insert("password",C3Workspace::currentWorkspace()->cachedPassword());
		szCommand = C3Workspace::substituteVariables(pCommand->command(),hVariables);
	} else {
		szCommand = pCommand->command();
	}

	C3ExternalCommandRun * pRun;

	switch(pCommand->type())
	{
		case C3ExternalCommand::DetachedProcess:
			if(!C3Process::startDetached(szCommand))
			{
				szError = __tr("Command failed to start");
				return false;
			}
			return true;
		break;
		case C3ExternalCommand::DetachedShellScript:
		{
			QStringList lArgs;
			lArgs.append("-c");
			lArgs.append(szCommand);
			if(!C3Process::startDetached(szShell,lArgs))
			{
				szError = __tr("Command failed to start");
				return false;
			}
			return true;
		}
		break;
		case C3ExternalCommand::ControlledProcessWithArguments:
		case C3ExternalCommand::ControlledProcess:
		case C3ExternalCommand::ControlledShellScript:
		{
			pRun = findRunById(pCommand->id());
			if(pRun)
			{
				C3MainWindow::instance()->textConsole()->appendText(
						pRun->oChannel,
						__tr("Killing existing external command '%1'\n").arg(pCommand->name()),
						__utf8("color: #707070;")
					);
				killRun(pRun);
			}
	
			pRun = createRun(pCommand);
			_p->lRuns.append(pRun);

			C3MainWindow::instance()->textConsole()->activateChannel(pRun->oChannel);

			if(pCommand->clearOutputOnStart())
				C3MainWindow::instance()->textConsole()->clear(pRun->oChannel);
			else
				C3MainWindow::instance()->textConsole()->appendText(
						pRun->oChannel,
						__utf8("\n\n")
					);

			C3MainWindow::instance()->textConsole()->appendText(
					pRun->oChannel,
					__tr("Starting external command '%1'\n").arg(pCommand->name()),
					__utf8("color: #707070;")
				);
		}
		break;
	}


	switch(pCommand->type())
	{
		case C3ExternalCommand::DetachedProcess:
		case C3ExternalCommand::DetachedShellScript:
		break;
		case C3ExternalCommand::ControlledProcessWithArguments:
		{
			pRun->oProcess.start(szCommand,pCommand->arguments());
		}
		break;
		case C3ExternalCommand::ControlledProcess:
		{
			pRun->oProcess.start(szCommand);
		}
		break;
		case C3ExternalCommand::ControlledShellScript:
		{
			QStringList lArgs;
			lArgs.append("-c");
			lArgs.append(szCommand);

			pRun->oProcess.start(szShell,lArgs);
		}
		break;
	}

	return true;
}

void C3ExternalCommandManager::executeCommand(const C3ExternalCommand * pCommand)
{
	if(pCommand->saveAllFiles())
	{
		if(!C3GeneralActions::instance()->documentSaveAll())
		{
			if(
					QMessageBox::question(
							C3MainWindow::instance(),
							__tr("Save Incomplete"),
							__tr("Not all documents were saved. Do you want to execute the command anyway?"),
							QMessageBox::Yes | QMessageBox::No
						) != QMessageBox::Yes
				)
				return;
		}
	}

	if(pCommand->needsPassword())
	{
		if(C3Workspace::currentWorkspace()->cachedPassword().isEmpty())
		{
			bool ok;
		
			QString szPass = QInputDialog::getText(
					C3MainWindow::instance(),
					__tr("Insert Password"),
					__tr("Please insert the password to continue"),
					QLineEdit::Password,
					QString(),
					&ok
				);
			
			if(!ok)
				return; // ignore
			
			C3Workspace::currentWorkspace()->setCachedPassword(szPass);
		}
	}

	QString szError;

	if(!executeCommandInternal(pCommand,szError))
	{
		QMessageBox::critical(
				C3MainWindow::instance(),
				__tr("Command Launch Failed"),
				__tr("Failed to execute command '%1': %2").arg(pCommand->name()).arg(szError),
				QMessageBox::Ok
			);
	}

	C3MainWindow::instance()->updateExternalCommandToolBarControls();
}

#define GET_SENDER_PROCESS() \
	QObject * ob = sender(); \
	if(!ob) \
		return; \
	C3Process * pProc = dynamic_cast<C3Process *>(ob); \
	if(!pProc) \
		return;

void C3ExternalCommandManager::slotProcessError(QProcess::ProcessError error)
{
	GET_SENDER_PROCESS();
	
	C3DockTextConsole * pConsole = C3MainWindow::instance()->textConsole();

	C3ExternalCommandRun * pRun = findRunByProcess(pProc);
	if(!pRun)
		return;

	switch(error)
	{
		case QProcess::FailedToStart:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Process failed to start\n"),
					__utf8("color: #ff8080;"),
					true
				);
		break;
		case QProcess::Crashed:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Process crashed\n"),
					__utf8("color: #ff8080;"),
					true
				);
		break;
		case QProcess::WriteError:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Process write error\n"),
					__utf8("color: #ff8080;"),
					true
				);
		break;
		case QProcess::ReadError:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Process read error\n"),
					__utf8("color: #ff8080;"),
					true
				);
		break;
		//case C3Process::UnknownError:
		default:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Process reported unknown error\n"),
					__utf8("color: #ff8080;")
				);
		break;
	}

	pRun->oChannel.bCanBeTerminated = false;
	pConsole->updateChannel(pRun->oChannel);

	//killRun(pRun); <-- this causes Qt to crash
}

void C3ExternalCommandManager::slotProcessFinished(int exitCode,QProcess::ExitStatus exitStatus)
{
	GET_SENDER_PROCESS();

	C3DockTextConsole * pConsole = C3MainWindow::instance()->textConsole();

	C3ExternalCommandRun * pRun = findRunByProcess(pProc);
	if(!pRun)
		return;

	switch(exitStatus)
	{
		case QProcess::CrashExit:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Execution terminated with a crash"),
					__utf8("color: #ff8080;"),
					true
				);
		break;
		//case QProcess::NormalExit:
		default:
			pConsole->appendText(
					pRun->oChannel,
					__tr("Execution terminated"),
					__utf8("color: #707070;")
				);
		break;
	}

	pRun->oChannel.bCanBeTerminated = false;
	pConsole->updateChannel(pRun->oChannel);

	killRun(pRun);
}


void C3ExternalCommandManager::handleProcessOutput(
		C3ExternalCommandRun * pRun,
		QString &szText,
		bool bIsStdError
	)
{
	if(szText.isEmpty())
		return;

	C3DockTextConsole * pConsole = C3MainWindow::instance()->textConsole();

	static QString szErrorColor = __ascii("color: #ffa0a0;");
	static QString szErrorPathColor = __ascii("color: #e090e0;");
	static QString szNormalColor = __ascii("color: #c0c0c0;");
	static QString szNormalPathColor = __ascii("color: #a0a0e0;");

	QString szWorkspacePath = C3Workspace::currentWorkspace()->rootPath();
	if((!szWorkspacePath.isEmpty()) && (!szWorkspacePath.endsWith(QChar('/'))))
		szWorkspacePath.append(QChar('/'));

	//qDebug("WORKSPACE PATH IS %s",szWorkspacePath.toUtf8().data());

	if(pRun->oCommand.highlightPaths())
	{
		QStringList sl = szText.split(QChar('\n'));
	
		int cnt = sl.count();
		int line = 0;
		
		foreach(QString szLine,sl)
		{
			line++;
		
			QString szCaptured;
		
#define TRY_REGEXP(_szRegexp) \
			do { \
				QRegularExpression rx(_szRegexp); \
				QRegularExpressionMatch match = rx.match(szLine); \
				if(match.hasMatch()) \
				{ \
					szCaptured = match.captured(0).trimmed(); \
					if(!szCaptured.isEmpty()) \
						goto got_match; \
				} \
			} while(0);
	
			// absolute, without spaces, at beginning
			TRY_REGEXP("^/[A-Za-z0-9\\-\\+_\\./]+:([0-9]+([:,][0-9]+)?)?");
		
			// relative, without spaces, at beginning
			//TRY_REGEXP("^[A-Za-z0-9\\-\\+_\\./]+:([0-9]+([:,][0-9]+)?)?");

			// absolute, with spaces, with extension, at beginning
			TRY_REGEXP("^/[A-Za-z0-9\\-\\+_\\./ ]+\\.[A-Za-z]+:([0-9]+([:,][0-9]+)?)?");
		
			// absolute, without spaces, not at beginning, with a leading space
			//TRY_REGEXP(" /[A-Za-z0-9\\-\\+_\\./]+(:[0-9]+([:,][0-9]+)?)?");
		
			// absolute, with spaces, not at beginning, with a leading space
			//TRY_REGEXP(" /[A-Za-z0-9\\-\\+_\\./ ]+(:[0-9]+([:,][0-9]+)?)?");
		
			// relative, with spaces, with extension, not at beginning, with a leading space
			//TRY_REGEXP(" [A-Za-z0-9\\-\\+_\\./ ]+\\.[A-Za-z]+(:[0-9]+([:,][0-9]+)?)?");
	
			// no match
			pConsole->appendText(
					pRun->oChannel,
					(line == cnt) ? szLine : (szLine + QChar('\n')),
					bIsStdError ? szErrorColor : szNormalColor,
					bIsStdError
				);
			continue;
	
got_match:

			//qDebug("Got match %s",szCaptured.toUtf8().data());

			int idx = szLine.indexOf(szCaptured);
	
			QString szLast = (line == cnt) ? szLine.mid(idx + szCaptured.length()) : (szLine.mid(idx + szCaptured.length()) + QChar('\n'));

			QString szFixedPath;
			
			if(!szWorkspacePath.isEmpty())
			{
				if(szCaptured.startsWith(szWorkspacePath))
					szFixedPath = szCaptured.mid(szWorkspacePath.length()); // make it relative, we'll get it anyway
				else
					szFixedPath = szCaptured;
			} else {
				szFixedPath = szCaptured;
			}

			QString szFormatted = __ascii(
					"<span style=\"%1 white-space: pre;\">"
						"%2"
						"<span style=\"%3 white-space: pre;\">"
							"%4"
						"</span>"
						"%5"
					"</span>"
				).arg(
					bIsStdError ? szErrorColor : szNormalColor,
					szLine.mid(0,idx).toHtmlEscaped(),
					bIsStdError ? szErrorPathColor : szNormalPathColor,
					szFixedPath.toHtmlEscaped(),
					szLast.toHtmlEscaped()
				);
	
			pConsole->appendHtml(
					pRun->oChannel,
					szFormatted,
					bIsStdError
				);
		}
	} else {
	
		pConsole->appendText(
				pRun->oChannel,
				szText,
				bIsStdError ? szErrorColor : szNormalColor,
				bIsStdError
			);
	}

}

void C3ExternalCommandManager::slotProcessReadyReadStandardOutput()
{
	GET_SENDER_PROCESS();

	QByteArray a = pProc->readAllStandardOutput();

	if(a.isEmpty())
		return;
		
	QString szText = QString::fromUtf8(a);

	C3ExternalCommandRun * pRun = findRunByProcess(pProc);
	if(!pRun)
		return;

	handleProcessOutput(pRun,szText,false);
}

void C3ExternalCommandManager::slotProcessReadyReadStandardError()
{
	GET_SENDER_PROCESS();

	QByteArray a = pProc->readAllStandardError();

	if(a.isEmpty())
		return;
		
	QString szText = QString::fromUtf8(a);

	C3ExternalCommandRun * pRun = findRunByProcess(pProc);
	if(!pRun)
		return;

	handleProcessOutput(pRun,szText,true);
}