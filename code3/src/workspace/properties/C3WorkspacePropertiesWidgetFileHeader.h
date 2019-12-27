#ifndef _C3WorkspacePropertiesWidgetFileHeader_h_
#define _C3WorkspacePropertiesWidgetFileHeader_h_
//=============================================================================
//
//   File : C3WorkspacePropertiesWidgetFileHeader.h
//   Creation Date : mer 11 nov 2015 06:55:28
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

class C3WorkspacePropertiesWidgetFileHeaderPrivate;

///
/// \class C3WorkspacePropertiesWidgetFileHeader
/// \brief The C3WorkspacePropertiesWidgetFileHeader class
///
/// This class...
///
class C3WorkspacePropertiesWidgetFileHeader : public C3OptionsWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3WorkspacePropertiesWidgetFileHeader
	///
	C3WorkspacePropertiesWidgetFileHeader(QWidget * pParent);

	///
	/// Destroys the instance of C3WorkspacePropertiesWidgetFileHeader
	/// and frees all the relevant resources
	///
	virtual ~C3WorkspacePropertiesWidgetFileHeader();

private:

	C3WorkspacePropertiesWidgetFileHeaderPrivate * m_pC3WPWFH;


}; // class C3WorkspacePropertiesWidgetFileHeader

#endif //!_C3WorkspacePropertiesWidgetFileHeader_h_
