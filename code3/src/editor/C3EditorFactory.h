#ifndef _C3EditorFactory_h_
#define _C3EditorFactory_h_
//=============================================================================
//
//   File : C3EditorFactory.h
//   Creation Date : mar 20 ott 2015 03:29:13
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

#include "C3Editor.h"

#include "C3EditorFormat.h"
#include "C3Link.h"

#include <QHash>
#include <QString>

class QFileInfo;
class C3EditorFactoryPrivate;
class C3SettingsDialog;

///
/// \class C3EditorFactory
/// \brief The C3EditorFactory class
///
/// This class...
///
class C3EditorFactory
{
public:

	///
	/// Creates an instance of C3EditorFactory
	///
	C3EditorFactory(
			const QString &szId,
			const QString &szEditorName
		);

	///
	/// Destroys the instance of C3EditorFactory
	/// and frees all the relevant resources
	///
	virtual ~C3EditorFactory();

protected:

	QString m_szId;
	QString m_szEditorName;
	QHash<QString,C3EditorFormat *> m_oFormatMap;

public:

	QHash<QString,C3EditorFormat *> & formatMap()
	{
		return m_oFormatMap;
	}

	C3EditorFormat * format(const QString &szFormatId)
	{
		return m_oFormatMap.value(szFormatId,NULL);
	}

	const QString & id() const
	{
		return m_szId;
	}
	
	const QString & editorName() const
	{
		return m_szEditorName;
	}

	// Create an editor for the specified path (or for a new file is path is empty)
	// Returns null if the editor could not be created (and must emit the appropriate errors and warnings)
	// An empty path usually identifies a "new" file request.
	virtual C3Editor * createEditor(
			const C3Link &oLink,
			QWidget * pParent
		) = 0;
	
	enum Appropriateness
	{
		// can't open the file at all
		CantOpen = 0,
		// can open the file but this editor is not appropriate
		CanOpenNotAppropriate = 10,
		// can open the file
		CanOpen = 20,
		// this is the right editor for this kind of file
		Appropriate = 50,
		// this is the perfect match for this kind of file
		PerfectMatch = 100
	};
	
	// Return the score of "appropriateness" of the editors from this factory
	// for opening the specified file.
	// Should be a value between 0 and 100. Use the Appropriateness enum values
	// as starting points.
	virtual int score(
			const C3Link &oLink,
			const QFileInfo &inf
		) = 0;

	virtual void addWidgetsToSettingsDialog(C3SettingsDialog * pDialog);
	virtual void saveSettings();
	virtual void settingsChanged();

protected:

	void registerFormat(C3EditorFormat * pFormat)
	{
		Q_ASSERT(!m_oFormatMap.contains(pFormat->id()));
		m_oFormatMap.insert(pFormat->id(),pFormat);
	}

}; // class C3EditorFactory

#endif //!_C3EditorFactory_h_