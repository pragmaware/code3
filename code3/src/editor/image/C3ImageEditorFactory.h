#ifndef _C3ImageEditorFactory_h_
#define _C3ImageEditorFactory_h_
//=============================================================================
//
//   File : C3ImageEditorFactory.h
//   Creation Date : mar 20 ott 2015 03:45:33
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

#include "C3EditorFactory.h"

class C3ImageEditorFactoryPrivate;

///
/// \class C3ImageEditorFactory
/// \brief The C3ImageEditorFactory class
///
/// This class...
///
class C3ImageEditorFactory : public C3EditorFactory
{
public:

	///
	/// Creates an instance of C3ImageEditorFactory
	///
	C3ImageEditorFactory();

	///
	/// Destroys the instance of C3ImageEditorFactory
	/// and frees all the relevant resources
	///
	virtual ~C3ImageEditorFactory();

private:

	C3ImageEditorFactoryPrivate * m_pC3TEF;

public:

	virtual C3Editor * createEditor(
			const C3Link &oLink,
			QWidget * pParent
		);


	virtual int score(
			const C3Link &oLink,
			const QFileInfo &inf
		);

private:

	void fillExtensionMap();

}; // class C3ImageEditorFactory

#endif //!_C3ImageEditorFactory_h_
