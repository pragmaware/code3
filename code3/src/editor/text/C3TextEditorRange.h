#ifndef _C3TextEditorRange_h_
#define _C3TextEditorRange_h_
//=============================================================================
//
//   File : C3TextEditorRange.h
//   Creation Date : lun 26 ott 2015 23:51:55
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

#include "C3TextEditorRowColumn.h"

///
/// \class C3TextEditorRange
/// \brief The C3TextEditorRange class
///
/// This class...
///
class C3TextEditorRange
{
public:
	// ASSUMPTION: start <= end (unless end.row == -1 which is an empty selection)
	C3TextEditorRowColumn start;
	C3TextEditorRowColumn end;

public:
	C3TextEditorRange()
	{
		end.row = -1; // empty
	}

	C3TextEditorRange(int r1,int c1,int r2,int c2)
		: start(r1,c1),
		end(r2,c2)
	{
	}

	C3TextEditorRange(const C3TextEditorRange &src)
		: start(src.start),
		end(src.end)
	{
	}

	bool isEmpty() const
	{
		if(end.row < 0)
			return true;
		if(end.row < start.row)
			return true;
		if(end.row > start.row)
			return false;
		return end.col <= start.col;
	}

	bool contains(const C3TextEditorRange &r)
	{
		if(end.row < 0)
			return false;
		if(r.end.row < 0)
			return false;
		if(r.start.row < start.row)
			return false;
		if(r.end.row > end.row)
			return false;
		if(r.start.row == start.row)
		{
			if(r.start.col < start.col)
				return false;
		}
		if(r.end.row == end.row)
		{
			if(r.end.col > end.col)
				return false;
		}
		return true;
	}

	void clear()
	{
		end.row = -1;
	}

	void set(const C3TextEditorRowColumn &anchor,const C3TextEditorRowColumn &other)
	{
		if(anchor < other)
		{
			start = anchor;
			end = other;
		} else {
			start = other;
			end = anchor;
		}
	}
	
	inline void operator = (const C3TextEditorRange &p)
	{
		start = p.start;
		end = p.end;
	}

	inline bool operator == (const C3TextEditorRange &p) const
	{
		if(isEmpty())
			return p.isEmpty();
		if(p.isEmpty())
			return false;
		return (start == p.start) && (end == p.end);
	}

	inline bool operator != (const C3TextEditorRange &p) const
	{
		if(isEmpty())
			return !p.isEmpty();
		if(p.isEmpty())
			return true;
		return (start != p.start) || (end != p.end);
	}

}; // class C3TextEditorRange

#endif //!_C3TextEditorRange_h_
