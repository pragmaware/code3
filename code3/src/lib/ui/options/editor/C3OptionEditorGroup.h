#ifndef _C3OptionEditorGroup_h_
#define _C3OptionEditorGroup_h_
//=============================================================================
//
//   File : C3OptionEditorGroup.h
//   Creation Date : mer 11 nov 2015 04:10:31
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

class C3OptionEditorGroupPrivate;

///
/// \class C3OptionEditorGroup
/// \brief The C3OptionEditorGroup class
///
/// This class...
///
class C3OptionEditorGroup : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorGroup
	///
	C3OptionEditorGroup(C3OptionsWidget * pOptionsWidget);

	///
	/// Destroys the instance of C3OptionEditorGroup
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorGroup();

private:

	C3OptionEditorGroupPrivate * m_pC3OEG;

public:

	enum Options
	{
		Checkable = 1
	};
	
	void setChecked(bool bChecked);
	
	bool isChecked();
	
	C3OptionsWidget * childOptionsWidget();

	void create(const QString &szLabel,unsigned int uOptions = 0);

	virtual bool commit();

}; // class C3OptionEditorGroup

#endif //!_C3OptionEditorGroup_h_
