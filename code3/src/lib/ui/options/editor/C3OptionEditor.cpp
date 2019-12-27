//=============================================================================
//
//   File : C3OptionEditor.cpp
//   Creation Date : mer 11 nov 2015 01:51:16
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

#include "C3OptionEditor.h"


C3OptionEditor::C3OptionEditor(C3OptionsWidget * pOptionsWidget)
	: QObject(), m_pOptionsWidget(pOptionsWidget)
{
}

C3OptionEditor::~C3OptionEditor()
{
}


