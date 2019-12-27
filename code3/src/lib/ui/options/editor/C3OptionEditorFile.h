#ifndef _C3OptionEditorFile_h_
#define _C3OptionEditorFile_h_
//=============================================================================
//
//   File : C3OptionEditorFile.h
//   Creation Date : mer 11 nov 2015 01:53:07
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

#include "C3OptionEditor.h"

class C3OptionEditorFilePrivate;

///
/// \class C3OptionEditorFile
/// \brief The C3OptionEditorFile class
///
/// This class...
///
class C3OptionEditorFile : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorFile
	///
	C3OptionEditorFile(C3OptionsWidget * pWidget);

	///
	/// Destroys the instance of C3OptionEditorFile
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorFile();

public:

	enum Options
	{
		DontTrim = 1
	};

private:

	C3OptionEditorFilePrivate * m_pC3OES;

public slots:

	virtual void setEnabled(bool bEnabled);

signals:

	void edited();

public:

	void create(const QString &szLabel,QString * pOption,unsigned int uOptions = 0);
	
	QString currentValue();
	void setCurrentValue(const QString &szValue);

	virtual bool commit();

private slots:

	void slotTextEdited(const QString &);

}; // class C3OptionEditorFile

#endif //!_C3OptionEditorFile_h_
