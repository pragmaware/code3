//=============================================================================
//
//   File : C3SymbolCTagsRunner.cpp
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

#include "C3SymbolCTagsRunner.h"
#include "C3SymbolCTagsParser.h"
#include "C3Process.h"

#include <QProcess>
#include <QByteArray>
#include <QThread>
#include <QElapsedTimer>
#include <QFileInfo>

#define DEBUG_RUNNER 1

#define DEBUG_RUNNER_WITH_CALLGRIND 1



class C3SymbolCTagsRunnerJob
{
public:
	QString szPath;
	QString szReplacePath;
	QStringList lExclusionPatterns;
	unsigned int uFlags;
};

class C3SymbolCTagsRunnerPrivate
{
public:
	QString szCTagsExecutablePath;
	QList<C3SymbolCTagsRunnerJob *> lJobs;
	bool bAbort;
	C3SymbolCTagsParser oParser;
	QString szError;
	
	QByteArray oInputBuffer;
};

#define _p m_pC3CTR

C3SymbolCTagsRunner::C3SymbolCTagsRunner(const QString &szCTagsExecutablePath)
{
	_p = new C3SymbolCTagsRunnerPrivate();
	
	_p->bAbort = false;
	_p->szCTagsExecutablePath = szCTagsExecutablePath;
}

C3SymbolCTagsRunner::~C3SymbolCTagsRunner()
{
	qDeleteAll(_p->lJobs);
	delete _p;
}

const QString & C3SymbolCTagsRunner::error()
{
	return _p->szError;
}

void C3SymbolCTagsRunner::abort()
{
	_p->bAbort = true;
}

void C3SymbolCTagsRunner::addJob(const QString &szPath,const QString &szReplacePath,unsigned int uFlags)
{
	C3SymbolCTagsRunnerJob * pJob = new C3SymbolCTagsRunnerJob();
	pJob->szPath = szPath;
	pJob->szReplacePath = szReplacePath;
	pJob->uFlags = uFlags;
	_p->lJobs.append(pJob);
}

void C3SymbolCTagsRunner::addJob(const QString &szPath,const QString &szReplacePath,const QStringList &lExclusionPatterns,unsigned int uFlags)
{
	C3SymbolCTagsRunnerJob * pJob = new C3SymbolCTagsRunnerJob();
	pJob->szPath = szPath;
	pJob->szReplacePath = szReplacePath;
	pJob->lExclusionPatterns = lExclusionPatterns;
	pJob->uFlags = uFlags;
	_p->lJobs.append(pJob);
}



QHash<QString,C3SymbolFile *> * C3SymbolCTagsRunner::takeSymbolFiles()
{
	return _p->oParser.takeSymbolFiles();
}

bool C3SymbolCTagsRunner::runJob(C3SymbolCTagsRunnerJob * pJob)
{
	//qDebug("Run job");

	// FIXME: Handle properties!

	QFileInfo inf(pJob->szPath);
	
	QString szPath = inf.canonicalFilePath();
	
	QStringList lArgs;
	
	lArgs.append(__literal("-f"));
	lArgs.append(__literal("-"));
	lArgs.append(__literal("-R"));
	lArgs.append(__literal("-u"));
	lArgs.append(__literal("-G")); // guess language eagerly : will open the file and look at first line even if extensionless (needed for STL)
	lArgs.append(__literal("--languages=c,c++,c#,java,php,javascript,cuda,cpreprocessor"));
	lArgs.append(__literal("--excmd=pattern"));
	lArgs.append(__literal("--extras=-qf"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("Q_REQUIRED_RESULT"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("Q_INLINE_TEMPLATE"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("cc_foreach(arg,...)=for(arg;;)"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("daa_foreach(arg,...)=for(arg;;)"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("foreach(arg,...)=for(arg;;)"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("Q_FOREACH(arg,...)=for(arg;;)"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("CC_INLINE=inline"));
	lArgs.append(__literal("-D"));
	lArgs.append(__literal("Q_DECLARE_FLAGS(a,b)=typedef QFlags<b> a;"));
	lArgs.append(__literal("--format=2"));
	lArgs.append(__literal("--file-scope=yes"));
	lArgs.append(__literal("--c++-kinds=*"));
	lArgs.append(__literal("--c-kinds=*"));
	lArgs.append(__literal("--php-kinds=cdfinltv"));
	lArgs.append(__literal("--javascript-kinds=fcmpv"));
	lArgs.append(__literal("--java-kinds=cefgilmp"));
	lArgs.append(__literal("--kinds-CPreProcessor=*"));
	lArgs.append(__literal("--fields=NFPaeiKlmnrSstzZ"));
	// FIXME: We aren't using the properties now!
	lArgs.append(__literal("--fields-c=+{properties}"));
	lArgs.append(__literal("--fields-c++=+{properties}+{template}+{captures}"));

	if(pJob->uFlags & JobLimitRecursionDepthTo1)
		lArgs.append(__literal("--maxdepth=1"));

	QString szSlash("/");
	
	foreach(QString szPattern,pJob->lExclusionPatterns)
	{
		szPattern.replace(szSlash,QString());
		lArgs.append(__literal("--exclude=%1").arg(szPattern));
	}

	lArgs.append(__literal("--exclude=tags.txt")); // FIXME: would also like to exclude "tags" file but not "tags" directories.
	lArgs.append(szPath);
 
	_p->oInputBuffer.clear();

#ifdef DEBUG_RUNNER
	QString szCmd;
	szCmd = _p->szCTagsExecutablePath;
	szCmd += __literal(" ");
	foreach(QString s,lArgs)
	{
		szCmd += s;
		szCmd += __literal(" ");
	}

	qDebug("[C3SymbolCTagsRunner] Starting job %s",szCmd.toUtf8().data());
#endif //DEBUG_RUNNER

	C3Process proc;
	proc.start(_p->szCTagsExecutablePath,lArgs,QIODevice::ReadWrite);

	proc.setProcessChannelMode(QProcess::SeparateChannels);
	//proc.setProcessChannelMode(QProcess::ForwardedChannels);
	proc.setReadChannel(QProcess::StandardOutput);

	// We need to directly waitForReadyRead() because waitForStarted() will not poll for process output
	// and will also expose us to a race condition where the process dies before we waitForReadyRead once.
	
	QElapsedTimer oTimeout;
	oTimeout.start();
	
	while(!proc.waitForReadyRead(100))
	{
		if(proc.state() == QProcess::NotRunning)
		{
#ifdef DEBUG_RUNNER
			qDebug("[C3SymbolCTagsRunner] CTags process died without emitting any data");
#endif //DEBUG_RUNNER
			_p->szError = __tr("Aborted");
			proc.kill();
			return false;
		}

		if(_p->bAbort)
		{
#ifdef DEBUG_RUNNER
			qDebug("[C3SymbolCTagsRunner] Aborted");
#endif //DEBUG_RUNNER

			_p->szError = __tr("Aborted");
			proc.kill();
			return false;
		}
		
#ifdef DEBUG_RUNNER_WITH_CALLGRIND
		if(oTimeout.elapsed() > 600000) // project may be laaaaarge
#else
		if(oTimeout.elapsed() > 60000) // project may be laaaaarge
#endif
		{
#ifdef DEBUG_RUNNER
			qDebug("[C3SymbolCTagsRunner] Failed to provide output in 60 seconds");
#endif //DEBUG_RUNNER

			_p->szError = __tr("ctags failed to provide output in 60 seconds");
			proc.kill();
			return false;
		}
		
		QThread::msleep(50);
	}

	QString szError;

	oTimeout.start();
	
	// It turns out that we may have data to read with state still "starting".
	// This is probably a Qt bug.
	
	_p->oParser.setReplaceInPath(pJob->szPath,pJob->szReplacePath);

	while(
			(proc.state() == QProcess::Running) ||
			(proc.bytesAvailable() > 0)
		)
	{
		if(_p->bAbort)
		{
#ifdef DEBUG_RUNNER
			qDebug("[C3SymbolCTagsRunner] Aborted");
#endif //DEBUG_RUNNER

			_p->szError = __tr("Aborted");
			proc.kill();
			return false;
		}

		if(proc.bytesAvailable() < 1)
		{
			QElapsedTimer oShortTimeout;
			oShortTimeout.start();

			while(
					(!proc.waitForReadyRead(100)) &&
					(proc.state() == QProcess::Running)
				)
			{
				if(_p->bAbort)
				{
#ifdef DEBUG_RUNNER
					qDebug("[C3SymbolCTagsRunner] Aborted");
#endif //DEBUG_RUNNER
		
					_p->szError = __tr("Aborted");
					proc.kill();
					return false;
				}
				
#ifdef DEBUG_RUNNER_WITH_CALLGRIND
				if(oShortTimeout.elapsed() > 200000)
#else
				if(oShortTimeout.elapsed() > 20000)
#endif
				{
#ifdef DEBUG_RUNNER
					qDebug("[C3SymbolCTagsRunner] Failed to continue output in 20 seconds");
#endif //DEBUG_RUNNER
		
					_p->szError = __tr("ctags failed to continue output in 20 seconds");
					proc.kill();
					return false;
				}
				
				QThread::msleep(50);
			}
		}
		
		if((proc.state() != QProcess::Running) && (proc.state() != QProcess::Starting))
			continue; // will exit loop
		
#ifdef DEBUG_RUNNER_WITH_CALLGRIND
		if(oTimeout.elapsed() > 9000000)
#else
		if(oTimeout.elapsed() > 120000)
#endif
		{
#ifdef DEBUG_RUNNER
			qDebug("[C3SymbolCTagsRunner] Failed to provide output in 120 seconds");
#endif //DEBUG_RUNNER

			_p->szError = __tr("ctags failed to provide output in 120 seconds");
			proc.kill();
			return false;
		}

		//qDebug("Process ready to read");

		int iAvailable = proc.bytesAvailable();
		
		if(iAvailable < 1)
		{
			//qDebug("No data availabe");

			QThread::msleep(50);
			continue;
		}
		
		int iSize = _p->oInputBuffer.size();
		int iEndSize = iSize + iAvailable;
		_p->oInputBuffer.resize(iEndSize);
		char * p = _p->oInputBuffer.data() + iSize;

		int iRead = proc.read(p,iAvailable);

		//qDebug("Read %d of %d bytes",iRead,iAvailable);

		if(iRead < iAvailable)
		{
			if(iRead < 0)
			{
#ifdef DEBUG_RUNNER
				qDebug("[C3SymbolCTagsRunner] Error reading ctags output");
#endif //DEBUG_RUNNER
				_p->szError = __tr("Error reading process output");
				proc.kill();
				return false;
			}
		
			qDebug("WARNING: read %d bytes of %d available",iRead,iAvailable);
			_p->oInputBuffer.resize(iSize + iRead);
			p = _p->oInputBuffer.data() + iSize; // pointer might have been changed
		}
		
		char * d = _p->oInputBuffer.data();
		char * e = d + _p->oInputBuffer.size();
		
		while(p < e)
		{
			if(*p != '\n')
			{
				p++;
				continue;
			}

			// got line!
			int iLen = p - d;
			if(iLen > 0)
			{
				//qDebug("TAG LINE: %s",szLine.toUtf8().data());
				if(!_p->oParser.parseLine(d,iLen,szError))
				{
#ifdef DEBUG_RUNNER
					qDebug("[C3SymbolCTagsRunner] Failed to parse tag line: %s",szError.toUtf8().data());
#endif //DEBUG_RUNNER

					_p->szError = __tr("Failed to parse line: %1").arg(szError);
					proc.kill();
					return false;
				}

				if(_p->bAbort)
				{
#ifdef DEBUG_RUNNER
					qDebug("[C3SymbolCTagsRunner] Aborted");
#endif //DEBUG_RUNNER
					_p->szError = __tr("Aborted");
					proc.kill();
					return false;
				}
			}
			
			p++; // skip newline
			d = p; // new start
		}

		_p->oInputBuffer.remove(0,d - _p->oInputBuffer.data());
	}

	// last line?
	if(_p->oInputBuffer.size() > 0)
	{
		//qDebug("LAST TAG LINE: %s",szLine.toUtf8().data());
		if(!_p->oParser.parseLine(_p->oInputBuffer.data(),_p->oInputBuffer.length(),szError))
		{
#ifdef DEBUG_RUNNER
			qDebug("[C3SymbolCTagsRunner] Failed to parse last line: %s",szError.toUtf8().data());
#endif //DEBUG_RUNNER
			_p->szError = __tr("Failed to parse line: %1").arg(szError);
			proc.kill();
			return false;
		}
	}

	oTimeout.start();

	if(proc.state() == QProcess::Running)
	{
		// This shouldn't actually happen, I guess.
		while(!proc.waitForFinished(100))
		{
			if(_p->bAbort)
			{
#ifdef DEBUG_RUNNER
				qDebug("[C3SymbolCTagsRunner] Aborted while waiting for finished");
#endif //DEBUG_RUNNER
				_p->szError = __tr("Aborted");
				proc.kill();
				return false;
			}
	
#ifdef DEBUG_RUNNER_WITH_CALLGRIND
			if(oTimeout.elapsed() > 200000)
#else
			if(oTimeout.elapsed() > 20000)
#endif
			{
#ifdef DEBUG_RUNNER
				qDebug("[C3SymbolCTagsRunner] Failed wait for finish in 20 seconds");
#endif //DEBUG_RUNNER
	
				_p->szError = __tr("ctags didn't finish 20 seconds");
				proc.kill();
				return false;
			}
	
			QThread::msleep(50);
		}
	}
	
#ifdef DEBUG_RUNNER
	qDebug("[C3SymbolCTagsRunner] Finished");
#endif //DEBUG_RUNNER

	return true;
}


bool C3SymbolCTagsRunner::run()
{
#ifdef DEBUG_RUNNER
	QElapsedTimer timer;
	timer.start();

	qDebug("[C3SymbolCTagsRunner] Starting up");
#endif //DEBUG_RUNNER


	while(_p->lJobs.count() > 0)
	{
		C3SymbolCTagsRunnerJob * pJob = _p->lJobs.takeFirst();
		Q_ASSERT(pJob);
		
		bool bRet = runJob(pJob);
		
		delete pJob;
		
		if((!bRet) || _p->bAbort)
			return false;
	}

#ifdef DEBUG_RUNNER
	qDebug("[C3SymbolCTagsRunner] Finished processing in %lld msecs",timer.elapsed());
#endif //DEBUG_RUNNER
	
	return true;
}
