#ifndef _C3TextEditorFactory_h_
#define _C3TextEditorFactory_h_
//=============================================================================
//
//   File : C3TextEditorFactory.h
//   Creation Date : mar 20 ott 2015 03:45:33
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

#include "C3EditorFactory.h"

#include <functional>

#include <QList>
#include <QPair>

class C3TextEditorFactoryPrivate;
class C3TextEditorFormat;
class C3TextEditorOptions;
class C3TextEditorMode;
class C3TextEditorModeCPPSharedData;
class C3TextEditorModeASMSharedData;
class C3TextEditorModePHPSharedData;
class C3TextEditorModeJavaSharedData;
class C3TextEditorModeVHDLSharedData;

class QMenu;

class C3TextEditorModeDescriptor
{
public:
	QString szName;
	std::function<C3TextEditorMode *()> fnCreate;
};

///
/// \class C3TextEditorFactory
/// \brief The C3TextEditorFactory class
///
/// This class...
///
class C3TextEditorFactory : public C3EditorFactory
{
public:

	///
	/// Creates an instance of C3TextEditorFactory
	///
	C3TextEditorFactory();

	///
	/// Destroys the instance of C3TextEditorFactory
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorFactory();

private:

	C3TextEditorFactoryPrivate * m_pC3TEF;

public:

	C3TextEditorOptions * options();

	virtual void addWidgetsToSettingsDialog(C3SettingsDialog * pDialog);

	virtual C3Editor * createEditor(
			const C3Link &oLink,
			QWidget * pParent
		);

	void getModeList(QList< QPair<QString,QString> > &lIdAndName);

	QString guessBestModeByExtension(const QString &szLowerCaseExt);

	C3TextEditorMode * createMode(const QString &szId);

	virtual int score(
			const C3Link &oLink,
			const QFileInfo &inf
		);

	QMenu * sharedContextMenu();

	C3TextEditorModeCPPSharedData * modeCPPSharedData();
	C3TextEditorModeJavaSharedData * modeJavaSharedData();
	C3TextEditorModeVHDLSharedData * modeVHDLSharedData();
	C3TextEditorModePHPSharedData * modePHPSharedData();
	C3TextEditorModeASMSharedData * modeASMSharedData();

	virtual void saveSettings();

private:

	void fillExtensionMap();
	void fillFormatMap();
	void fillModeMap();
	void fillModeByExtensionMap();

}; // class C3TextEditorFactory

#endif //!_C3TextEditorFactory_h_