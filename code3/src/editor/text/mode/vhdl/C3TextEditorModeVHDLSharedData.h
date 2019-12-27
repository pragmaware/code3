#ifndef _C3TextEditorModeVHDLSharedData_h_
#define _C3TextEditorModeVHDLSharedData_h_
//=============================================================================
//
//   File : C3TextEditorModeVHDLSharedData.h
//   Creation Date : 2015/11/18 00:48:16
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

#include <QHash>
#include <QString>
#include <QColor>
#include <QObject>

class C3TextEditorFactory;
class C3TextEditorOptions;
class C3Workspace;
class C3SettingsUserHighlightingSet;

class C3TextEditorModeVHDLSharedData : public QObject
{
	Q_OBJECT
public:
	C3TextEditorModeVHDLSharedData(C3TextEditorFactory * pFactory);
	~C3TextEditorModeVHDLSharedData();

private:

	C3TextEditorFactory * m_pFactory;
	C3TextEditorOptions * m_pOptions;

	QHash<QString,QColor *> m_hIdentifierColorHash;

public:

	const QHash<QString,QColor *> & identifierColorHash() const
	{
		return m_hIdentifierColorHash;
	}

public:

	void rebuildColorHashes();

private slots:

	void slotWorkspacePropertiesChanged();

}; // class C3TextEditorModeVHDLSharedData

#endif //!_C3TextEditorModeVHDLSharedData_h_