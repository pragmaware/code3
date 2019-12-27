//=============================================================================
//
//   File : C3TextEditorOptionsWidgetColors.cpp
//   Creation Date : mer 11 nov 2015 01:44:42
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

#include "C3TextEditorOptionsWidgetColors.h"

#include "C3Settings.h"
#include "C3OptionEditorScrollArea.h"
#include "C3OptionEditorColor.h"
#include "C3OptionEditorGroup.h"
#include "C3TextEditorOptions.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QList>
#include <QSettings>
#include <QMessageBox>

class C3TextEditorOptionsWidgetColorsPrivate
{
public:
	C3TextEditorOptions * pOptions;
	C3TextEditorOptionsColors oColors;
	QList<C3OptionEditorColor *> lEditors;

};

#define _p m_pC3SWU

C3TextEditorOptionsWidgetColors::C3TextEditorOptionsWidgetColors(const QString &szLabel,QWidget * pParent,C3TextEditorOptions * pOptions)
	: C3OptionsWidget(szLabel,pParent)
{
	_p = new C3TextEditorOptionsWidgetColorsPrivate();
	_p->pOptions = pOptions;
	_p->oColors.copyColors(*pOptions);

	C3OptionEditorScrollArea * pArea = addScrollArea();

	C3OptionsWidget * w = pArea->childOptionsWidget();
	w->gridLayout()->setMargin(10);

	C3OptionEditorColor * ed;

	ed = w->addColorEditor(__tr("Margin Text"),&(_p->oColors.oMarginTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Margin Background"),&(_p->oColors.oMarginBackgroundColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Active Margin Text"),&(_p->oColors.oMarginActiveTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Active Margin Background"),&(_p->oColors.oMarginActiveBackgroundColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Cursor"),&(_p->oColors.oCursorColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Text Background"),&(_p->oColors.oTextBackgroundColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Editor Background"),&(_p->oColors.oEditorBackgroundColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Mark Background"),&(_p->oColors.oMarkBackgroundColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Current Line Background"),&(_p->oColors.oCurrentLineBackgroundColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Default Text"),&(_p->oColors.oTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Tab Marker"),&(_p->oColors.oTabColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Errors"),&(_p->oColors.oErrorTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Strings"),&(_p->oColors.oStringTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Unterminated Strings"),&(_p->oColors.oUnterminatedStringTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Numbers"),&(_p->oColors.oNumberTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Single Line Comments"),&(_p->oColors.oSingleLineCommentTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Multi Line Comments"),&(_p->oColors.oMultiLineCommentTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Operators"),&(_p->oColors.oOperatorTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Markup Tags"),&(_p->oColors.oMarkupTagTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Markup Tag Names"),&(_p->oColors.oMarkupTagNameTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Variables"),&(_p->oColors.oVariableTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Variable in String"),&(_p->oColors.oVariableInStringTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Annotations"),&(_p->oColors.oAnnotationTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Keywords"),&(_p->oColors.oKeywordTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("User Keywords"),&(_p->oColors.oUserKeywordTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Types"),&(_p->oColors.oBuiltinTypeTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("User Types"),&(_p->oColors.oUserTypeTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Objects"),&(_p->oColors.oBuiltinObjectTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("User Objects"),&(_p->oColors.oUserObjectTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Code Block Delimiters"),&(_p->oColors.oCodeBlockDelimiterTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Parenthesis"),&(_p->oColors.oParenthesisTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Array Delimiters"),&(_p->oColors.oArrayDelimiterTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Preprocessor Statements"),&(_p->oColors.oPreprocessorTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Include Files"),&(_p->oColors.oIncludeFileTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Underscore Identifiers"),&(_p->oColors.oUnderscoreIdentifierTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("M-Underscore Identifiers"),&(_p->oColors.oMUnderscoreIdentifierTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("G-Underscore Identifiers"),&(_p->oColors.oGUnderscoreIdentifierTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("User Custom 1"),&(_p->oColors.oUserCustom1TextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("User Custom 2"),&(_p->oColors.oUserCustom2TextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("User Custom 3"),&(_p->oColors.oUserCustom3TextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Regular Expressions"),&(_p->oColors.oRegExpTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("FIXME/TODO Markers"),&(_p->oColors.oFixmeTodoTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Heredocs"),&(_p->oColors.oHeredocTextColor));
	_p->lEditors.append(ed);
	ed = w->addColorEditor(__tr("Variable in Heredoc"),&(_p->oColors.oVariableInHeredocTextColor));
	_p->lEditors.append(ed);

	w->addSpaceEater();

	C3OptionEditorGroup * gb = addGroup(__tr("Import/Export Color Set"));

	QPushButton * pb = gb->childOptionsWidget()->addPushButton(__tr("Import from File..."),0,0,1,1);
	QObject::connect(pb,SIGNAL(clicked()),this,SLOT(loadFromFile()));

	pb = gb->childOptionsWidget()->addPushButton(__tr("Export to File..."),0,1,1,1);
	QObject::connect(pb,SIGNAL(clicked()),this,SLOT(saveToFile()));
}

C3TextEditorOptionsWidgetColors::~C3TextEditorOptionsWidgetColors()
{
	delete _p;
}

bool C3TextEditorOptionsWidgetColors::commit()
{
	if(!C3OptionsWidget::commit())
		return false;
	_p->pOptions->copyColors(_p->oColors);
	return true;
}

void C3TextEditorOptionsWidgetColors::resetEditors()
{
	Q_FOREACH(C3OptionEditorColor * ed,_p->lEditors)
		ed->resetValue();
}

void C3TextEditorOptionsWidgetColors::saveToFile()
{
	QString szFileName = QFileDialog::getSaveFileName(this,__tr("Select File to Save"),"colors.c3colors","Code-3 Color Sets (*.c3colors)");
	if(szFileName.isEmpty())
		return;
	QSettings oSettings(szFileName,QSettings::IniFormat);
	_p->oColors.saveColors(oSettings);
	oSettings.sync();
	if(oSettings.status() != QSettings::NoError)
		QMessageBox::warning(this,__tr("Save Failed"),__tr("Failed to save the file"));
}

void C3TextEditorOptionsWidgetColors::loadFromFile()
{
	QString szFileName = QFileDialog::getOpenFileName(this,__tr("Select File to Load"),QString(),"Code-3 Color Sets (*.c3colors)");
	if(szFileName.isEmpty())
		return;
	QSettings oSettings(szFileName,QSettings::IniFormat);
	if(oSettings.status() != QSettings::NoError)
	{
		QMessageBox::warning(this,__tr("Load Failed"),__tr("Failed to load the file"));
		return;
	}
	_p->oColors.loadColors(oSettings);
	resetEditors();
}

