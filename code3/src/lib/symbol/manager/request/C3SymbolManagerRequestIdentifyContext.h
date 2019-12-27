#ifndef _C3SymbolManagerRequestIdentifyContext_h_
#define _C3SymbolManagerRequestIdentifyContext_h_
//=============================================================================
//
//   File : C3SymbolManagerRequestIdentifyContext.h
//   Creation Date : 2016/09/13 03:19:12
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

#include "C3SymbolManagerRequest.h"

#include "C3Symbol.h"
#include "C3SymbolManagerSymbolDescription.h"

class C3SymbolManagerRequestIdentifyContext : public C3SymbolManagerRequest
{
private:
	QString m_szFilePath;
	unsigned int m_uLine;
	C3Symbol::Language m_eLanguage;
	C3SymbolManagerSymbolDescription * m_pSymbolDescription;

public:
	C3SymbolManagerRequestIdentifyContext(
			const QString &szFilePath,
			unsigned int uLine,
			C3Symbol::Language eLanguage
		);
	~C3SymbolManagerRequestIdentifyContext();

public:
	void setSymbolDescription(C3SymbolManagerSymbolDescription * pDescription)
	{
		if(m_pSymbolDescription)
			delete m_pSymbolDescription;
		m_pSymbolDescription = pDescription;
	}
	
	C3SymbolManagerSymbolDescription * symbolDescription()
	{
		return m_pSymbolDescription;
	}
	
	C3SymbolManagerSymbolDescription * takeSymbolDescription()
	{
		C3SymbolManagerSymbolDescription * s = m_pSymbolDescription;
		m_pSymbolDescription = NULL;
		return s;
	}

	const QString & filePath() const
	{
		return m_szFilePath;
	}

	C3Symbol::Language language() const
	{
		return m_eLanguage;
	}

	unsigned int lineNumber() const
	{
		return m_uLine;
	}


}; // class C3SymbolManagerRequestIdentifyContext

#endif //!_C3SymbolManagerRequestIdentifyContext_h_