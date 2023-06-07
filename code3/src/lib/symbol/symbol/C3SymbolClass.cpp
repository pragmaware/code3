//=============================================================================
//
//   File : C3SymbolClass.cpp
//   Creation Date : 2015/12/09 20:42:37
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

#include "C3SymbolClass.h"

void C3SymbolClass::clear()
{
	C3SymbolScope::clear();
	if(m_pBaseClasses)
	{
		delete m_pBaseClasses;
		m_pBaseClasses = NULL;
	}
	m_bBaseClassResolutionAttempted = false;
}

QString C3SymbolClass::description() const
{
	switch(language())
	{
		case Cpp:
		{
			if(m_szBaseClassNames.isEmpty())
			{
				if(unresolvedScope().isEmpty())
					return __utf8("class %1").arg(identifier());
				else
					return __utf8("class %1::%2").arg(unresolvedScope()).arg(identifier());
			} else {
				if(unresolvedScope().isEmpty())
					return __utf8("class %1 : %2").arg(identifier()).arg(m_szBaseClassNames);
				else
					return __utf8("class %1::%2 : %3").arg(unresolvedScope()).arg(identifier()).arg(m_szBaseClassNames);
			}
		}
		break;
		default:
			return C3Symbol::description();
		break;
	}

}

//#define DEBUG_RESOLVE_BASE_CLASSES 1

QList<C3SymbolClass *> * C3SymbolClass::resolveBaseClasses()
{
#ifdef DEBUG_RESOLVE_BASE_CLASSES
	C3_TRACE_FUNCTION;
#endif

	Q_ASSERT(!m_bBaseClassResolutionAttempted);
	
	m_bBaseClassResolutionAttempted = true;

	if(m_szBaseClassNames.isEmpty())
		return NULL;

	C3SymbolScope * pScope = resolvedContainingScope();
	if(!pScope)
	{
		qDebug("[C3SymbolClass::resolveBaseClasses] WARNING: Class %s : %s has no containing scope",identifier().toUtf8().data(),m_szBaseClassNames.toUtf8().data());
		return NULL;
	}

#ifdef DEBUG_RESOLVE_BASE_CLASSES
	C3_TRACE("Resolve base classes of class %s: %s contained in scope %s",identifier().toUtf8().data(),baseClassNames().toUtf8().data(),pScope->identifier().toUtf8().data());
#endif

	// FIXME: Handle a qualified class names (with ::)
	// FIXME: Handle templates? (they are currently stripped off by splitBaseClassList()).
	
	QStringList lClassNames = splitBaseClassList(m_szBaseClassNames);

	Q_FOREACH(QString szClass,lClassNames)
	{
		C3SymbolScope * pBaseClass = pScope->resolveTypeToScope(szClass);

		if(!pBaseClass)
		{
			qDebug("[C3SymbolClass::resolveBaseClasses] WARNING: Can't resolve base class %s (of class %s)",szClass.toUtf8().data(),identifier().toUtf8().data());
			continue;
		}
		
		if(pBaseClass == this) // avoid recursion!
		{
			qDebug("[C3SymbolClass::resolveBaseClasses] WARNING: Class %s has base %s which resolved to the class itself!",identifier().toUtf8().data(),szClass.toUtf8().data());
			continue;
		}
		
		if(pBaseClass->type() != C3Symbol::Class)
		{
			qDebug("[C3SymbolClass::resolveBaseClasses] WARNING: Class %s has base %s which resolved to a non-class",identifier().toUtf8().data(),szClass.toUtf8().data());
			continue;
		}

		if(!m_pBaseClasses)
			m_pBaseClasses = new QList<C3SymbolClass *>();

		m_pBaseClasses->append(static_cast<C3SymbolClass *>(pBaseClass));
	}

	return m_pBaseClasses;
}

QStringList C3SymbolClass::splitBaseClassList(const QString &szClassList)
{
	QStringList ret;

	if(szClassList.isEmpty())
		return ret;

	//qDebug("SPLIT (%s)",szClassList.toUtf8().data());

	const QChar * b = szClassList.data();;
	const QChar * c = b;
	const QChar * e = c + szClassList.length();
	
	int iTemplateLevel = 0;
	
	while((c < e) && c->isSpace())
		c++;

	while(c < e)
	{
		switch(c->unicode())
		{
			case '<':
			{
				if(iTemplateLevel == 0)
				{
					if(c > b)
					{
						QString cl(b,c-b);
						//qDebug("+ (%s)",cl.toUtf8().data());
						ret.append(cl);
					}
				}

				iTemplateLevel++;

				c++;
			}
			break;
			case ',':
			{
				if(iTemplateLevel == 0)
				{
					if(c > b)
					{
						QString cl(b,c-b);
						//qDebug("+ (%s)",cl.toUtf8().data());
						ret.append(cl.trimmed());
					}
					c++;

					while((c < e) && c->isSpace())
						c++;

					b = c;
				} else {
					c++;
				}
			}
			break;
			case '>':
			{
				if(iTemplateLevel == 0)
				{
					// problems.
					// This is probably a complicated template with a less than operator in there
					// keep only the first class and return
					while(ret.count() > 1)
						ret.removeLast();
					//qDebug("!! Problematic!");
					return ret;
				}

				iTemplateLevel--;
				c++;
				
				if(iTemplateLevel == 0)
				{
					// run to a comma or something else interesting
					while(
						(c < e) &&
						(c->unicode() != ',') &&
						(c->unicode() != '<') &&
						(c->unicode() != '>')
					)
						c++;
				} else {
					while((c < e) && c->isSpace())
						c++;
				}
				b = c;
			}
			break;
			default:
				c++;
			break;
		}
	}

	if(c > b)
	{
		QString cl(b,c-b);
		//qDebug("+ (%s)",cl.toUtf8().data());
		ret.append(cl);
	}

	return ret;
}
