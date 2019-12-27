//=============================================================================
//
//   File : C3Symbol.cpp
//   Creation Date : 2015/12/05 00:10:10
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

#include "C3Symbol.h"

#include "C3SymbolScope.h"
#include "C3SymbolFile.h"

void C3Symbol::clear()
{
	//qDebug("Clear symbol %s::%s",m_szUnresolvedScope.toUtf8().data(),m_szIdentifier.toUtf8().data());
	m_pContainingScope = NULL;
}

const QString & C3Symbol::filePath() const
{
	return m_pFile->path();
}

bool C3Symbol::isScope() const
{
	return false;
}

QString C3Symbol::bestKnownContainingScopeFullyQualifiedName()
{
	if(m_pContainingScope)
		return m_pContainingScope->buildFullyQualifiedName();
	return __ascii("<unresolved:%1>").arg(m_szUnresolvedScope);
}

QString C3Symbol::buildFullyQualifiedName()
{
	if(!m_pContainingScope)
		return identifier();
	return __ascii("%1::%2").arg(m_pContainingScope->buildFullyQualifiedName(),identifier());
}

QStringList C3Symbol::buildFullyQualifiedNameAsList()
{
	QStringList sl;
	sl.append(identifier());
	C3SymbolScope * pScope = m_pContainingScope;
	while(pScope)
	{
		sl.prepend(pScope->identifier());
		pScope = pScope->resolvedContainingScope();
	}
	return sl;
}


QString C3Symbol::typeString() const
{
	switch(m_eType)
	{
		case Class:
		{
			static QString szClass = __tr("class");
			return szClass;
		}
		break;
		case Namespace:
		{
			static QString szNamespace = __tr("namespace");
			return szNamespace;
		}
		break;
		case Union:
		{
			static QString szUnion = __tr("union");
			return szUnion;
		}
		break;
		case FunctionDefinition:
		{
			static QString szFunctionDefinition = __tr("function definition");
			return szFunctionDefinition;
		}
		break;
		case FunctionPrototype:
		{
			static QString szFunctionPrototype = __tr("function prototype");
			return szFunctionPrototype;
		}
		break;
		case Enumeration:
		{
			static QString szEnumeration = __tr("enumeration");
			return szEnumeration;
		}
		break;
		case EnumerationMember:
		{
			static QString szEnumerationMember = __tr("enumeration member");
			return szEnumerationMember;
		}
		break;
		case LocalVariable:
		{
			static QString szLocalVariable = __tr("local variable");
			return szLocalVariable;
		}
		break;
		case GlobalVariable:
		{
			static QString szGlobalVariable = __tr("global variable");
			return szGlobalVariable;
		}
		break;
		case MemberVariable:
		{
			static QString szMemberVariable = __tr("member variable");
			return szMemberVariable;
		}
		break;
		case TypeDefinition:
		{
			static QString szTypeDefinition = __tr("type definition");
			return szTypeDefinition;
		}
		break;
		case Macro:
		{
			static QString szMacro = __tr("macro");
			return szMacro;
		}
		break;
		case ImportedScope:
		{
			static QString szImportedScope = __tr("imported scope");
			return szImportedScope;
		}
		break;
		default:
		{
			static QString szUnknown = __tr("unknown");
			return szUnknown;
		}
		break;
	}
}

QString C3Symbol::accessLevelKeyword(AccessLevel eLevel)
{
	switch(eLevel)
	{
		case AccessLevelPrivate:
		{
			static QString szPrivate("private");
			return szPrivate;
		}
		break;
		case AccessLevelProtected:
		{
			static QString szProtected("protected");
			return szProtected;
		}
		break;
		case AccessLevelPublic:
		{
			static QString szPublic("public");
			return szPublic;
		}
		break;
		default:
		{
			static QString szUnknown = __tr("unknown");
			return szUnknown;
		}
		break;
	}
}

QString C3Symbol::description() const
{
	switch(language())
	{
		case Cpp:
		{
			switch(type())
			{
				case EnumerationMember:
					if(unresolvedScope().isEmpty())
						return __utf8("<?>::%1").arg(identifier());
					return __utf8("%1::%2").arg(unresolvedScope()).arg(identifier());
				break;
				case Enumeration:
				case Union:
				case TypeDefinition:
					if(unresolvedScope().isEmpty())
						return identifier();
					return __utf8("%1::%2").arg(unresolvedScope()).arg(identifier());
				break;
				default:
				break;
			}
		}
		break;
		default:
		break;
	}

	if(m_szUnresolvedScope.isEmpty())
		return __ascii("%1 %2").arg(typeString(),m_szIdentifier);
	return __ascii("%1 %2 in scope %3").arg(typeString(),m_szIdentifier,m_szUnresolvedScope);
}

QString C3Symbol::completionKey() const
{
	return __ascii("%1 %2").arg(m_szIdentifier).arg(typeString());
}

void C3Symbol::dump(const QString &szPrefix)
{
	qDebug(
			"%s- %s '%s' in scope '%s' at '%s:%d'",
			szPrefix.toUtf8().data(),
			typeString().toUtf8().data(),
			m_szIdentifier.toUtf8().data(),
			m_szUnresolvedScope.toUtf8().data(),
			filePath().toUtf8().data(),
			lineNumber()
		);
}



