#ifndef _C3TextEditorModePHPSharedData_h_
#define _C3TextEditorModePHPSharedData_h_
//=============================================================================
//
//   File : C3TextEditorModePHPSharedData.h
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

class C3TextEditorModePHPSharedData : public QObject
{
	Q_OBJECT
public:
	C3TextEditorModePHPSharedData(C3TextEditorFactory * pFactory);
	~C3TextEditorModePHPSharedData();

private:

	C3TextEditorFactory * m_pFactory;
	C3TextEditorOptions * m_pOptions;

	QHash<QString,QColor *> m_hIdentifierColorHash;
	QHash<QString,QColor *> m_hIdentifierPrefixColorHash;
	QHash<QString,QColor *> m_hIdentifierPrefixWithUppercaseColorHash;
	QHash<QString,QColor *> m_hIdentifierPostfixColorHash;
	QHash<QString,QColor *> m_hPreprocessorIdentifierColorHash;

public:

	const QHash<QString,QColor *> & identifierColorHash() const
	{
		return m_hIdentifierColorHash;
	}

	const QHash<QString,QColor *> & identifierPrefixColorHash() const
	{
		return m_hIdentifierPrefixColorHash;
	}

	const QHash<QString,QColor *> & identifierPrefixWithUppercaseColorHash() const
	{
		return m_hIdentifierPrefixWithUppercaseColorHash;
	}

	const QHash<QString,QColor *> & identifierPostfixColorHash() const
	{
		return m_hIdentifierPostfixColorHash;
	}

public:

	static void setDefaultUserHighlightingSet(C3SettingsUserHighlightingSet * pSet);

	void rebuildColorHashes();

private slots:

	void slotWorkspacePropertiesChanged();

}; // class C3TextEditorModePHPSharedData

#endif //!_C3TextEditorModePHPSharedData_h_