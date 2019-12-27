#ifndef _C3SettingsWidgetFileHeader_h_
#define _C3SettingsWidgetFileHeader_h_
//=============================================================================
//
//   File : C3SettingsWidgetFileHeader.h
//   Creation Date : mer 11 nov 2015 06:40:27
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

class C3SettingsWidgetFileHeaderPrivate;

///
/// \class C3SettingsWidgetFileHeader
/// \brief The C3SettingsWidgetFileHeader class
///
/// This class...
///
class C3SettingsWidgetFileHeader : public C3OptionsWidget
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3SettingsWidgetFileHeader
	///
	C3SettingsWidgetFileHeader(const QString &szLabel,QWidget * pParent);

	///
	/// Destroys the instance of C3SettingsWidgetFileHeader
	/// and frees all the relevant resources
	///
	virtual ~C3SettingsWidgetFileHeader();

private:

	C3SettingsWidgetFileHeaderPrivate * m_pC3SWFH;


}; // class C3SettingsWidgetFileHeader

#endif //!_C3SettingsWidgetFileHeader_h_
