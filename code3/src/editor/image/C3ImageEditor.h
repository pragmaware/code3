#ifndef _C3ImageEditor_h_
#define _C3ImageEditor_h_
//=============================================================================
//
//   File : C3ImageEditor.h
//   Creation Date : mar 20 ott 2015 04:24:03
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

class C3ImageEditorPrivate;

///
/// \class C3ImageEditor
/// \brief The C3ImageEditor class
///
/// This class...
///
class C3ImageEditor : public C3Editor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3ImageEditor
	///
	C3ImageEditor(C3EditorFactory * pFactory,QWidget * pParent);

	///
	/// Destroys the instance of C3ImageEditor
	/// and frees all the relevant resources
	///
	virtual ~C3ImageEditor();

private:

	C3ImageEditorPrivate * m_pC3IE;

public:

	virtual C3EditorFormat * format();

	bool open(const C3Link &oLink);

	virtual bool closeFile(bool bCanAbort);

	virtual bool newFile();

	virtual bool saveInternal(
			const QString &szPath,
			C3EditorFormat * pFormat,
			QString &szError
		);

	virtual bool saveState(QJsonObject &oObject);

	virtual const QString & privateInfoText1() const;
	virtual const QString & privateInfoText2() const;


}; // class C3ImageEditor

#endif //!_C3ImageEditor_h_