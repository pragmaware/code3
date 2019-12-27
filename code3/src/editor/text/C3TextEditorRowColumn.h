#ifndef _C3TextEditorRowColumn_h_
#define _C3TextEditorRowColumn_h_
//=============================================================================
//
//   File : C3TextEditorRowColumn.h
//   Creation Date : lun 26 ott 2015 02:33:10
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

///
/// \class C3TextEditorRowColumn
/// \brief The C3TextEditorRowColumn class
///
/// This class...
///
class C3TextEditorRowColumn
{
public:
	int row;
	int col;

public:
	C3TextEditorRowColumn()
	{
	}

	C3TextEditorRowColumn(int r,int c)
		: row(r), col(c)
	{
	}
	
	C3TextEditorRowColumn(const C3TextEditorRowColumn &src)
		: row(src.row), col(src.col)
	{
		
	}

	inline void operator = (const C3TextEditorRowColumn &p)
	{
		row = p.row;
		col = p.col;
	}

	inline void set(int r,int c)
	{
		row = r;
		col = c;
	}
	
	inline bool operator < (const C3TextEditorRowColumn &p) const
	{
		if(row < p.row)
			return true;
		if(row > p.row)
			return false;
		if(col < p.col)
			return true;
		return false;
	}

	inline bool operator > (const C3TextEditorRowColumn &p) const
	{
		if(row > p.row)
			return true;
		if(row < p.row)
			return false;
		if(col > p.col)
			return true;
		return false;
	}

	inline bool operator >= (const C3TextEditorRowColumn &p) const
	{
		if(row > p.row)
			return true;
		if(row < p.row)
			return false;
		if(col >= p.col)
			return true;
		return false;
	}
	
	inline bool operator == (const C3TextEditorRowColumn &p) const
	{
		return (row == p.row) && (col == p.col);
	}

	inline bool operator != (const C3TextEditorRowColumn &p) const
	{
		return (row != p.row) || (col != p.col);
	}

}; // class C3TextEditorRowColumn

#endif //!_C3TextEditorRowColumn_h_