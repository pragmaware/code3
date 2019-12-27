#ifndef _C3ExternalCommandEditorWidget_h_
#define _C3ExternalCommandEditorWidget_h_
//=============================================================================
//
//   File : C3ExternalCommandEditorWidget.h
//   Creation Date : 2015/11/19 23:05:53
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
#include "C3ExternalCommand.h"

class C3ExternalCommandEditorWidgetPrivate;

class C3ExternalCommandEditorWidget : public C3OptionsWidget
{
	Q_OBJECT
public:
	C3ExternalCommandEditorWidget(QWidget * pParent,const C3ExternalCommand &cmd);
	virtual ~C3ExternalCommandEditorWidget();

private:
	C3ExternalCommandEditorWidgetPrivate * m_pC3ECEW;

public:
	const C3ExternalCommand & command() const;

	virtual bool commit();

private:
	void enableDisableClearOutputOnStart();

private slots:
	void slotNameEdited();
	void slotTypeEdited();
	void slotSelectIcon();

}; // class C3ExternalCommandEditorWidget

#endif //!_C3ExternalCommandEditorWidget_h_
