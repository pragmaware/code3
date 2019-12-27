//=============================================================================
//
//   File : C3SettingsUserHighlightingSetOptionsWidget.cpp
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

#include "C3SettingsUserHighlightingSetOptionsWidget.h"
#include "C3Workspace.h"
#include "C3OptionsWidgetContainer.h"
#include "C3OptionsWidget.h"
#include "C3SettingsUserHighlightingSet.h"

#include <QGridLayout>
#include <QMessageBox>

class C3SettingsUserHighlightingSetOptionsWidgetPrivate
{
public:
	C3OptionsWidgetContainer * pContainer;
	C3SettingsUserHighlightingSet * pSet;
};

#define _p m_pC3WPWH

C3SettingsUserHighlightingSetOptionsWidget::C3SettingsUserHighlightingSetOptionsWidget(const QString &szLabel,QWidget * pParent,C3SettingsUserHighlightingSet * pSet)
	: C3OptionsWidget(szLabel,pParent)
{
	_p = new C3SettingsUserHighlightingSetOptionsWidgetPrivate();
	_p->pSet = pSet;

	_p->pContainer = new C3OptionsWidgetContainer(this,C3OptionsWidgetContainer::SupportSetDefaults);
	QObject::connect(_p->pContainer,SIGNAL(setDefaultsButtonClicked()),this,SLOT(slotSetDefaultsButtonClicked()));
	
	gridLayout()->addWidget(_p->pContainer,0,0,1,3);
	
	gridLayout()->setMargin(0);

	createWidgets();
}

void C3SettingsUserHighlightingSetOptionsWidget::createWidgets()
{
	createWidgetsForLanguage("cpp","C/C++");
	createWidgetsForLanguage("java","Java");
	createWidgetsForLanguage("php","PHP");
}

void C3SettingsUserHighlightingSetOptionsWidget::createWidgetsForLanguage(const QString &szLanguageId,const QString &szLanguageName)
{
	QString szPart = __tr(
			"Use one keyword per line.<br>"
			"<ul>"
				"<li><b>word</b> matches itself."
				"<li><b>word*</b> matches the beginning of a word."
				"<li><b>word^</b> matches the beginning of a word followed by an uppercase letter."
				"<li><b>*word</b> matches the end of a word."
			"</ul>"
		);
	
	createPage(
			__tr("%1 Keywords").arg(szLanguageName),
			__tr(
					"Additional keywords to highlight in %1 mode (beside the proper language keywords).<br>"
					"Common examples are Qt's 'emit', 'signals', 'slots' or 'foreach'.<br>"
					"%2"
				).arg(szLanguageName).arg(szPart),
			_p->pSet->subset(__utf8("%1UserKeywords").arg(szLanguageId))
		);

	createPage(
			__tr("%1 Types").arg(szLanguageName),
			__tr(
					"Additional types to highlight in %1 mode (beside the proper language types).<br>"
					"Common examples are Qt's 'quint32', Windows 'DWORD' and such.<br>"
					"%2"
				).arg(szLanguageName).arg(szPart),
			_p->pSet->subset(__utf8("%1UserTypes").arg(szLanguageId))
		);

	createPage(
			__tr("%1 Objects").arg(szLanguageName),
			__tr(
					"Additional objects to highlight in %1 mode (beside the proper language objects).<br>"
					"Common examples are Qt's 'QSomething' classes.<br>"
					"%2"
				).arg(szLanguageName).arg(szPart),
			_p->pSet->subset(__utf8("%1UserObjects").arg(szLanguageId))
		);

	createPage(
			__tr("%1 Custom 1").arg(szLanguageName),
			__tr(
					"Additional objects to highlight in %1 mode using user-custom-1 color.<br>"
					"%2"
				).arg(szLanguageName).arg(szPart),
			_p->pSet->subset(__utf8("%1UserCustom1").arg(szLanguageId))
		);

	createPage(
			__tr("%1 Custom 2").arg(szLanguageName),
			__tr(
					"Additional objects to highlight in %1 mode using user-custom-2 color.<br>"
					"%2"
				).arg(szLanguageName).arg(szPart),
			_p->pSet->subset(__utf8("%1UserCustom2").arg(szLanguageId))
		);

	createPage(
			__tr("%1 Custom 3").arg(szLanguageName),
			__tr(
					"Additional objects to highlight in %1 mode using user-custom-3 color.<br>"
					"%2"
				).arg(szLanguageName).arg(szPart),
			_p->pSet->subset(__utf8("%1UserCustom3").arg(szLanguageId))
		);

}

C3SettingsUserHighlightingSetOptionsWidget::~C3SettingsUserHighlightingSetOptionsWidget()
{
	delete _p;
}

bool C3SettingsUserHighlightingSetOptionsWidget::commit()
{
	return _p->pContainer->commit();
}

void C3SettingsUserHighlightingSetOptionsWidget::createPage(const QString &szLabel,const QString &szText,QStringList * pOption)
{
	C3OptionsWidget * pWidget = new C3OptionsWidget(szLabel,_p->pContainer->parentForWidgets());
	
	pWidget->addStringListEditor(szText,pOption);

	_p->pContainer->addWidget(pWidget);
}

void C3SettingsUserHighlightingSetOptionsWidget::slotSetDefaultsButtonClicked()
{
	if(
			QMessageBox::question(
					this,
					__tr("Set Default Highlighting"),
					__tr("Do you really want to reset the user highligting sets to default?"),
					QMessageBox::Yes | QMessageBox::No
				) != QMessageBox::Yes
		)
		return;
	
	_p->pContainer->removeAllWidgets();

	_p->pSet->setDefaults();

	createWidgets();
}
