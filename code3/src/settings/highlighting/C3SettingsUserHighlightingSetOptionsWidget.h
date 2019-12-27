#ifndef _C3SettingsUserHighlightingSetOptionsWidget_h_
#define _C3SettingsUserHighlightingSetOptionsWidget_h_
//=============================================================================
//
//   File : C3SettingsUserHighlightingSetOptionsWidget.h
//   Creation Date : 2015/11/18 01:34:35
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

#include "C3OptionsWidget.h"

class QStringList;
class C3SettingsUserHighlightingSetOptionsWidgetPrivate;
class C3SettingsUserHighlightingSet;

class C3SettingsUserHighlightingSetOptionsWidget : public C3OptionsWidget
{
	Q_OBJECT
public:
	C3SettingsUserHighlightingSetOptionsWidget(const QString &szLabel,QWidget * pParent,C3SettingsUserHighlightingSet * pSet);
	virtual ~C3SettingsUserHighlightingSetOptionsWidget();

private:
	C3SettingsUserHighlightingSetOptionsWidgetPrivate * m_pC3WPWH;

public:

	virtual bool commit();

	void createPage(const QString &szLabel,const QString &szText,QStringList * pOption);

private:

	void createWidgets();
	void createWidgetsForLanguage(const QString &szLanguageId,const QString &szLanguageName);

private slots:
	void slotSetDefaultsButtonClicked();

}; // class C3SettingsUserHighlightingSetOptionsWidget

#endif //!_C3SettingsUserHighlightingSetOptionsWidget_h_
