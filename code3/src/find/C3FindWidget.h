#ifndef _C3FindWidget_h_
#define _C3FindWidget_h_
//=============================================================================
//
//   File : C3FindWidget.h
//   Creation Date : dom 01 nov 2015 20:21:28
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
#include <QShowEvent>

class C3FindWidgetPrivate;
class C3Editor;
class QComboBox;


///
/// \class C3FindWidget
/// \brief The C3FindWidget class
///
/// This class...
///
class C3FindWidget : public QWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3FindWidget
	///
	C3FindWidget(QWidget * pParent);

	///
	/// Destroys the instance of C3FindWidget
	/// and frees all the relevant resources
	///
	virtual ~C3FindWidget();

private:

	C3FindWidgetPrivate * m_pC3FW;

public:

	// Called by C3MainWindow
	void currentEditorChanged();
	void editorClosed(C3Editor * pEditor);

	void grepInWorkspace(const QString &szText);
	void grepInCurrentDirectory(const QString &szText);

protected:

	virtual void showEvent(QShowEvent * e);
	virtual void hideEvent(QHideEvent * e);

private:

	void grepInScope(const QString &szText,const QString &szScope);

	void loadSettings();
	void saveSettings();
	
	void enableDisableButtons();
	
	void focusCurrentEditor();

	void rebuildFindOperation();
	void resetFindOperation();

	QString comboBoxGetSelectedData(QComboBox * pBox);
	void comboBoxSelectByData(QComboBox * pBox,const QString &szData);

	void findInternal();
	void find();
	void replace();
	void replaceInternal();
	void replaceAll();
	void replaceAndFind();

	void findGrep();

protected slots:

	void slotFindButtonClicked();
	void slotReplaceButtonClicked();
	void slotReplaceAndFindButtonClicked();
	void slotReplaceAllButtonClicked();
	void slotSearchStringComboBoxEditTextChanged(const QString &szText);
	void slotReplaceStringComboBoxEditTextChanged(const QString &szText);
	void slotSearchTypeComboBoxCurrentIndexChanged(int iIdx);
	void slotSearchDirectionComboBoxCurrentIndexChanged(int iIdx);
	void slotSearchScopeComboBoxCurrentIndexChanged(int iIdx);
	void slotSearchCaseSensitivityComboBoxCurrentIndexChanged(int iIdx);
	void slotSearchStringComboBoxLineEditReturnPressed();

}; // class C3FindWidget

#endif //!_C3FindWidget_h_