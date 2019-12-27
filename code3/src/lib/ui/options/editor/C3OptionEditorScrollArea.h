#ifndef _C3OptionEditorScrollArea_h_
#define _C3OptionEditorScrollArea_h_
//=============================================================================
//
//   File : C3OptionEditorScrollArea.h
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

class C3OptionEditorScrollAreaPrivate;

///
/// \class C3OptionEditorScrollArea
/// \brief The C3OptionEditorScrollArea class
///
/// This class...
///
class C3OptionEditorScrollArea : public C3OptionEditor
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3OptionEditorScrollArea
	///
	C3OptionEditorScrollArea(C3OptionsWidget * pOptionsWidget);

	///
	/// Destroys the instance of C3OptionEditorScrollArea
	/// and frees all the relevant resources
	///
	virtual ~C3OptionEditorScrollArea();

private:

	C3OptionEditorScrollAreaPrivate * m_pC3OEG;

public:

	enum Options
	{
		Dummy = 1
	};
	
	C3OptionsWidget * childOptionsWidget();

	void create(unsigned int uOptions = 0);

	virtual bool commit();

}; // class C3OptionEditorScrollArea

#endif //!_C3OptionEditorScrollArea_h_