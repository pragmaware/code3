//=============================================================================
//
//   File : C3SymbolFile.cpp
//   Creation Date : 2015/12/05 00:23:39
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

#include "C3SymbolFile.h"
#include "C3SymbolScope.h"

void C3SymbolFile::clearAdditionalSymbolLookupScopes(C3Symbol::Language eLanguage)
{
	if(m_lAdditionalSymbolLookupScopes.isEmpty())
		return;
	
	if(containsOnlyLanguageFlag(1 << eLanguage))
	{
		m_lAdditionalSymbolLookupScopes.clear();
		return;
	}
	
	// mutliple languages
	QList<C3SymbolScope *> lKeep;
	Q_FOREACH(C3SymbolScope * pScope,m_lAdditionalSymbolLookupScopes)
	{
		if(pScope->language() != eLanguage)
			lKeep.append(pScope);
	}

	m_lAdditionalSymbolLookupScopes = lKeep;
}

void C3SymbolFile::buildContextMap()
{
	Q_ASSERT(m_uFlags & ContextMapDirty);
	
	m_lContextMap.clear();
	
	foreach(C3Symbol * pSymbol,m_lSymbols)
	{
		switch(pSymbol->type())
		{
			case C3Symbol::Class:
			case C3Symbol::Namespace:
			case C3Symbol::FunctionDefinition:
				m_lContextMap.append(pSymbol);
			break;
			default:
				// ignore anything else
			break;
		}
	}
	
	m_uFlags &= ~ContextMapDirty;
}

//#define DEBUG_FIND_CONTEXT 1

C3Symbol * C3SymbolFile::findContext(unsigned int uLineNumber)
{
#ifdef DEBUG_FIND_CONTEXT
	qDebug("Find context at line %d",uLineNumber);
#endif

	uLineNumber++; // ctags reports one-based line numbers!

	if(m_uFlags & ContextMapDirty)
		buildContextMap();

	if(m_lContextMap.isEmpty())
		return NULL;

	m_oSearchSymbol.setLineNumber(uLineNumber);

	C3SymbolList::iterator i = qLowerBound(m_lContextMap.begin(),m_lContextMap.end(),&(m_oSearchSymbol),C3Symbol::symbolLineNumberLessThan);

	C3Symbol * t;

	if(i == m_lContextMap.begin())
	{
		t = *i;
		if(t->lineNumber() == uLineNumber) // got a symbol that starts at the line
			return t;
		// all symbols start after that line
#ifdef DEBUG_FIND_CONTEXT
		qDebug("No context symbol for %s:%d",m_szPath.toUtf8().data(),uLineNumber);
		qDebug("Map is:");
		Q_FOREACH(C3Symbol * s,m_lContextMap)
			qDebug("- Sym %s at %d",s->identifier().toUtf8().data(),s->lineNumber());
#endif
		return NULL;
	}

	if(i == m_lContextMap.end())
	{
		// all symbols start before that line
		t = m_lContextMap.last();
		
#ifdef DEBUG_FIND_CONTEXT
		qDebug("Context seems to be symbol %s from %d to %d",t->identifier().toUtf8().data(),t->lineNumber(),t->endLineNumber());
#endif
		
		if((t->endLineNumber() + 5) < uLineNumber)
		{
#ifdef DEBUG_FIND_CONTEXT
			qDebug("Ends quite far away [1]...");
#endif

			// ends quite far away, check if the previous symbol is a container?
			if(m_lContextMap.size() > 1)
			{
				C3Symbol * t2 = m_lContextMap.at(m_lContextMap.size() - 2);

#ifdef DEBUG_FIND_CONTEXT
				qDebug("Context2 seems to be symbol %s from %d to %d",t2->identifier().toUtf8().data(),t2->lineNumber(),t2->endLineNumber());
#endif

				if((t2->lineNumber() <= uLineNumber) && (t2->endLineNumber() >= uLineNumber))
					return t2;
			}
		}
		return t;
	}
	
	t = *i;
	if(t->lineNumber() <= uLineNumber)
	{
		// This should be always ==
#ifdef DEBUG_FIND_CONTEXT
		qDebug("Context seems to be symbol %s from %d to %d",t->identifier().toUtf8().data(),t->lineNumber(),t->endLineNumber());
#endif

		return t;
	}

	--i;
	
	t = *i;

#ifdef DEBUG_FIND_CONTEXT
	qDebug("Context seems to be symbol %s from %d to %d",t->identifier().toUtf8().data(),t->lineNumber(),t->endLineNumber());
#endif

	if((t->endLineNumber() + 5) < uLineNumber)
	{
#ifdef DEBUG_FIND_CONTEXT
		qDebug("Ends quite far away [2]...");
#endif

		// ends quite far away, check if the previous symbol is a container?
		if(i != m_lContextMap.begin())
		{
			--i;
			C3Symbol * t2 = *i;

#ifdef DEBUG_FIND_CONTEXT
			qDebug("Context2 seems to be symbol %s from %d to %d",t2->identifier().toUtf8().data(),t2->lineNumber(),t2->endLineNumber());
#endif

			if((t2->lineNumber() <= uLineNumber) && (t2->endLineNumber() >= uLineNumber))
				return t2;
		}
	}

	return t;
}
