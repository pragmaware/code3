#ifndef _C3FindOperation_h_
#define _C3FindOperation_h_
//=============================================================================
//
//   File : C3FindOperation.h
//   Creation Date : dom 01 nov 2015 19:55:55
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

#include <QString>
#include <QRegularExpression>

///
/// \class C3FindOperation
/// \brief The C3FindOperation class
///
/// This class...
///
class C3FindOperation
{
public:
	enum CaseSensitivity
	{
		CaseSensitive,
		CaseInsensitive
	};
	
	enum Direction
	{
		Forward,
		Backward
	};
	
	enum Scope
	{
		Document,
		Selection
	};

public:

	///
	/// Creates an instance of C3FindOperation
	///
	C3FindOperation();

	///
	/// Destroys the instance of C3FindOperation
	/// and frees all the relevant resources
	///
	~C3FindOperation();

private:

	///
	/// the search operation unique id
	///
	int m_iId;

	///
	/// the regular expression to search for
	///
	QRegularExpression m_oRegularExpression;

	///
	/// the search direction
	///
	Direction m_eDirection;

	///
	/// the search scope
	///
	Scope m_eScope;

	///
	/// the match
	///
	QRegularExpressionMatch m_oMatch;
	
	///
	/// Are capture replacements enabled?
	///
	bool m_bCaptureReplacementsEnabled;

public:

	void setCaptureReplacementsEnabled(bool bEnabled)
	{
		m_bCaptureReplacementsEnabled = bEnabled;
	}

	bool captureReplacementsEnabled() const
	{
		return m_bCaptureReplacementsEnabled;
	}

	///
	/// Sets the search scope
	///
	void setScope(Scope eScope)
	{
		m_eScope = eScope;
	}

	///
	/// Returns the search scope
	///
	Scope scope() const
	{
		return m_eScope;
	}

	///
	/// Sets the search operation unique id
	///
	void setId(int iId)
	{
		m_iId = iId;
	}

	///
	/// Returns the search operation unique id
	///
	int id() const
	{
		return m_iId;
	}

	///
	/// Sets the search direction
	///
	void setDirection(Direction eDirection)
	{
		m_eDirection = eDirection;
	}

	///
	/// Returns the search direction
	///
	Direction direction() const
	{
		return m_eDirection;
	}

	///
	/// Sets the regular expression to search for
	///
	void setRegularExpression(const QRegularExpression &oRegularExpression)
	{
		m_oRegularExpression = oRegularExpression;
	}

	///
	/// Returns the regular expression to search for
	///
	const QRegularExpression & regularExpression() const
	{
		return m_oRegularExpression;
	}

	///
	/// Sets the match
	///
	void setMatch(const QRegularExpressionMatch &oMatch)
	{
		m_oMatch = oMatch;
	}

	///
	/// Returns the match
	///
	const QRegularExpressionMatch & match() const
	{
		return m_oMatch;
	}

	static int nextOperationId();

}; // class C3FindOperation

#endif //!_C3FindOperation_h_