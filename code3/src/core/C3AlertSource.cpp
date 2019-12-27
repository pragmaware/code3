//=============================================================================
//
//   File : C3AlertSource.cpp
//   Creation Date : mar 20 ott 2015 01:30:54
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

#include "C3AlertSource.h"

#include "C3MainWindow.h"

C3AlertSource::C3AlertSource(const QString &szName)
	: m_szName(szName)
{
}

C3AlertSource::~C3AlertSource()
{
}

void C3AlertSource::alert(
		AlertLevel eLevel,
		const QString &szMessage
	)
{
	C3MainWindow::instance()->alert(this,eLevel,szMessage);
}

QString C3AlertSource::alertLevelDescription(AlertLevel eLevel)
{
	switch(eLevel)
	{
		case Paranoia:
			return __tr("Paranoia");
		break;
		case Debug:
			return __tr("Debug");
		break;
		case Info:
			return __tr("Info");
		break;
		case Warning:
			return __tr("Warning");
		break;
		case Error:
			return __tr("Error");
		break;
		case Fatal:
			return __tr("Fatal");
		break;
		default:
		break;
	}
	return __tr("Unknown");
}

