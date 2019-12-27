#ifndef _C3OptionsWidget_h_
#define _C3OptionsWidget_h_
//=============================================================================
//
//   File : C3OptionsWidget.h
//   Creation Date : ven 06 nov 2015 03:31:45
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

#include <QWidget>

#include "C3OptionEditorEnumeration.h"

class C3OptionsWidgetPrivate;
class QGridLayout;
class QLabel;
class QLineEdit;
class QTextEdit;
class QGroupBox;
class QCheckBox;
class QComboBox;
class QPushButton;
class QScrollArea;
class QListWidget;
class QColor;
class C3OptionEditorString;
class C3OptionEditorStringList;
class C3OptionEditorBoolean;
class C3OptionEditorFile;
class C3OptionEditorScrollArea;
class C3OptionEditorGroup;
class C3OptionEditorInteger;
class C3OptionEditorText;
class C3OptionEditorColor;
class C3OptionEditorFont;
class C3OptionEditor;

class C3OptionsWidgetContainer;
class C3OptionsWidgetDescriptor;

///
/// \class C3OptionsWidget
/// \brief The C3OptionsWidget class
///
/// This class...
///
class C3OptionsWidget : public QWidget
{
	friend class C3OptionsDialog;
	friend class C3OptionsWidgetContainer;
	friend class C3OptionsWidgetDescriptor;
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionsWidget
	///
	C3OptionsWidget(const QString &szTitle,QWidget * pParent);

	///
	/// Destroys the instance of C3OptionsWidget
	/// and frees all the relevant resources
	///
	virtual ~C3OptionsWidget();

private:

	C3OptionsWidgetPrivate * m_pC3OW;

public:

	const QString & title() const;
	void setTitle(const QString &szTitle);

	QGridLayout * gridLayout();

	QScrollArea * addScrollArea(int iRow,int iCol,int iRowSpan,int iColSpan);
	QLabel * addLabel(const QString &szText,int iRow,int iCol,int iRowSpan,int iColSpan);
	QPushButton * addPushButton(const QString &szText,int iRow,int iCol,int iRowSpan,int iColSpan);
	QLineEdit * addLineEdit(int iRow,int iCol,int iRowSpan,int iColSpan);
	QTextEdit * addTextEdit(int iRow,int iCol,int iRowSpan,int iColSpan);
	QGroupBox * addGroupBox(const QString &szLabel,int iRow,int iCol,int iRowSpan,int iColSpan);
	QCheckBox * addCheckBox(const QString &szLabel,int iRow,int iCol,int iRowSpan,int iColSpan);
	QComboBox * addComboBox(int iRow,int iCol,int iRowSpan,int iColSpan);
	QListWidget * addListWidget(int iRow,int iCol,int iRowSpan,int iColSpan);

	int allocateRows(int iCount);

	C3OptionEditorString * addStringEditor(
			const QString &szLabel,
			QString * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorString::Options
		);

	C3OptionEditorInteger * addIntegerEditor(
			const QString &szLabel,
			int * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorInteger::Options
		);

	C3OptionEditorText * addTextEditor(
			const QString &szLabel,
			QString * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorText::Options
		);

	C3OptionEditorStringList * addStringListEditor(
			const QString &szLabel,
			QStringList * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorStringList::Options
		);

	C3OptionEditorFile * addFileEditor(
			const QString &szLabel,
			QString * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorFile::Options
		);

	C3OptionEditorColor * addColorEditor(
			const QString &szLabel,
			QColor * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorColor::Options
		);

	C3OptionEditorFont * addFontEditor(
			const QString &szLabel,
			QFont * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorFont::Options
		);

	C3OptionEditorBoolean * addBooleanEditor(
			const QString &szLabel,
			bool * pOption,
			unsigned int uOptions = 0 // SC3OptionEditorBoolean::Options
		);

	C3OptionEditorGroup * addGroup(
			const QString &szLabel,
			unsigned int uOptions = 0 // C3OptionEditorGroup::Options
		);

	C3OptionEditorScrollArea * addScrollArea(
			unsigned int uOptions = 0 // C3OptionEditorScrollArea::Options
		);
		
	enum AddLabelOptions
	{
		AddLabelFullWidth = 1
	};
		
	QLabel * addLabel(
			const QString &szText,
			unsigned int uOptions = 0
		);

	template<typename T> C3OptionEditorEnumeration<T> * addEnumerationEditor(
			const QString &szLabel,
			T * pOption,
			const QList< QPair<int,QString> > &lOptions,
			unsigned int uOptions = 0
		)
	{
		C3OptionEditorEnumeration<T> * pEditor = new C3OptionEditorEnumeration<T>(this);
		pEditor->create(szLabel,pOption,lOptions,uOptions);
		addOptionEditor(pEditor);
		return pEditor;
	}

	void addSpaceEater();

public:
	virtual bool commit();

signals:

	void aboutToShow();
	void titleChanged(C3OptionsWidget * pWidget);

protected:

	virtual void showEvent(QShowEvent * e);

	void setOptionsWidgetDescriptor(C3OptionsWidgetDescriptor * pDescriptor);
	C3OptionsWidgetDescriptor * optionsWidgetDescriptor();
	
	void addOptionEditor(C3OptionEditor * pEditor);

}; // class C3OptionsWidget

#endif //!_C3OptionsWidget_h_