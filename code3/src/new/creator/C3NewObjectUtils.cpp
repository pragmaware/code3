//=============================================================================
//
//   File : C3NewObjectUtils.cpp
//   Creation Date : gio 12 nov 2015 19:49:05
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

#include "C3NewObjectUtils.h"

#include "C3Settings.h"
#include "C3Workspace.h"
#include "C3StringUtils.h"

#include <QDateTime>
#include <QStringList>

namespace C3NewObjectUtils
{

	QString buildFileHeader(
			const QString &szFilePath,
			BuildHeaderCommentStyle eCommentStyle
		)
	{
		C3Workspace * w = C3Workspace::currentWorkspace();
	
		QHash<QString,QString> hParams;
		hParams.insert("filepath",szFilePath);
		hParams.insert("filename",C3StringUtils::fileNameFromPath(szFilePath));
	
		QString szBaseHeader;
		
		if(w->isDefault())
		{
			szBaseHeader = C3Settings::instance()->fileHeader();
		} else {
			szBaseHeader = w->fileHeader();
		}

		if(szBaseHeader.isEmpty())
			return szBaseHeader;

	
		QString szHeader = C3Workspace::substituteVariables(szBaseHeader.trimmed(),hParams);

		QString szRet;
	
		switch(eCommentStyle)
		{
			case BuildHeaderCommentStyleCSingleLine:
			{
				szRet = QString(
						"//=============================================================================\n"
						"//\n"
					);
	
				QStringList sl = szHeader.split(QChar('\n'));
				
				foreach(QString s,sl)
					szRet += QString("// %1\n").arg(s);
	
				szRet.append(QString(
						"//\n"
						"//=============================================================================\n"
					));
			}
			break;
			case BuildHeaderCommentStyleCMultiLine:
			{
				szRet = QString(
						"/******************************************************************************\n"
						"* \n"
					);
	
				QStringList sl = szHeader.split(QChar('\n'));
				
				foreach(QString s,sl)
					szRet += QString("* %1\n").arg(s);
	
				szRet.append(QString(
						"* \n"
						"******************************************************************************/\n"
					));
			}
			break;
			case BuildHeaderCommentStyleShell:
			{
				szRet = QString(
						"#==============================================================================\n"
						"#\n"
					);
	
				QStringList sl = szHeader.split(QChar('\n'));
				
				foreach(QString s,sl)
					szRet += QString("# %1\n").arg(s);
	
				szRet.append(QString(
						"#\n"
						"#==============================================================================\n"
					));
			}
			break;
			//case BuildHeaderCommentTypeNone:
			default:
				szRet = szHeader;
			break;
		}

		return szRet;
	}
		
} // namespace C3NewObjectUtils