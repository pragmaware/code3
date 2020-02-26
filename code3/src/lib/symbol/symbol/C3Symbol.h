#ifndef _C3Symbol_h_
#define _C3Symbol_h_
//=============================================================================
//
//   File : C3Symbol.h
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

#include "c3_config.h"

#include <QString>
#include <QStringList>

#include <QList>
#include <QMultiMap>


class C3SymbolFile;
class C3SymbolScope;

class C3Symbol
{
	friend class C3SymbolManager;
	friend class C3SymbolFile;
public:
	enum Type : quint16
	{
		// full scope
		// example:
		//   Classona test2.cpp /^  class Classona$/;" kind:class line:8 language:C++ namespace:NameSpace inherits:BaseClass
		Class = 1,

		// full scope
		// example:
		//   NameSpace test2.cpp  /^namespace NameSpace$/;" kind:namespace line:5 language:C++ file:
		Namespace = (1 << 1),

		// variable scope
		// example:
		//   Unione test2.cpp /^union Unione$/;" kind:union line:5 language:C++ file:
		Union = (1 << 2),

		// leaf
		// ctags kind:prototype (int function();)
		// example:
		//   proto test2.cpp /^    int proto();$/;" kind:prototype line:28 language:C++ class:NameSpace::Classona file: access:public signature:()
		FunctionPrototype = (1 << 3),

		// variable scope
		// example:
		//   definition test2.cpp /^ int definition()$/;" kind:function line:30 language:C++ class:NameSpace::Classona access:public signature:()
		FunctionDefinition = (1 << 4),

		// special (contains only enumeration members)
		// example:
		//   Enummo test2.cpp /^    enum Enummo$/;" kind:enum line:11 language:C++ class:NameSpace::Classona file: access:public
		Enumeration = (1 << 5),

		// leaf, file-scope
		// example:
		//   Enum1 test2.cpp /^ Enum1,$/;" kind:enumerator line:13 language:C++ enum:NameSpace::Classona::Enummo file:
		// this is always added to Enumeration at parsing stage
		EnumerationMember = (1 << 6),

		// leaf, file-scope
		// examples:
		//   l test2.cpp /^      int l;$/;" kind:local line:30 language:C++
		// this is always added to Function at parsing stage
		LocalVariable = (1 << 7),

		// leaf
		// examples:
		//  globla test2.cpp /^int globla;$/;" kind:variable line:5 language:C++
		GlobalVariable = (1 << 8),

		// leaf
		// examples:
		//   a test2.cpp /^  int a;$/;" kind:member line:7 language:C++ union:Unione file: access:public
		//   x test2.cpp /^    Enummo x;$/;" kind:member line:23 language:C++ class:NameSpace::Classona file: access:private
		MemberVariable = (1 << 9),

		// leaf
		// ctags kind:type (typedef Anything X)
		// examples:
		//   Typo test2.cpp /^typedef QString<int> Typo;$/;" kind:typedef line:50 language:C++ file:
		TypeDefinition = (1 << 10),

		// leaf and always toplevel
		// ctags kind:macro (#define X something)
		// example:
		//    MACRONA test2.cpp 3;" kind:macro line:3 language:C++ file:
		Macro = (1 << 11),

		// leaf
		// ctags kind:tparam (template<class X>)
		// examples:
		TemplateParameter = (1 << 12),
		
		// using namespace something
		ImportedScope = (1 << 13),

		InvalidType = (1 << 14)
	};

	// The language.
	enum Language : quint8
	{
		// This contains C and CPP
		Cpp,
		Java,
		Php,
		Javascript,
		CSharp,
		Python,

		InvalidLanguage,
		LanguageCount = InvalidLanguage
	};

	enum AccessLevel : quint8
	{
		AccessLevelUnknown,
		AccessLevelPublic,
		AccessLevelProtected,
		AccessLevelPrivate,

		AccessLevelCount
	};

private:
	Type m_eType;
	Language m_eLanguage;
	QString m_szIdentifier;
	QString m_szUnresolvedScope; // SomeNameSpace.SomeClass, SomeUnion, SomeNameSpace.Enum etc...)
	C3SymbolFile * m_pFile;
	quint32 m_uLineNumber;
	quint32 m_uEndLineNumber;
	C3SymbolScope * m_pContainingScope; // the real scope, if identified
	quint16 m_uScopePartCount; // number of parts in scope
	AccessLevel m_eAccess;
protected:
	// called by C3SymbolFile and C3SymbolScope
	C3Symbol(C3SymbolFile * pFile)
		:
		m_pFile(pFile),
		m_pContainingScope(NULL)
	{
		
	}

public:
	C3Symbol(
			C3SymbolFile * pFile,
			Language eLanguage,
			Type eType,
			const QString &szIdentifier,
			const QString &szScope,
			AccessLevel eAccess,
			quint32 uLineNumber,
			quint32 uEndLineNumber
		) :
		m_pFile(pFile),
		m_eLanguage(eLanguage),
		m_eType(eType),
		m_eAccess(eAccess),
		m_szIdentifier(szIdentifier),
		m_szUnresolvedScope(szScope),
		m_uLineNumber(uLineNumber),
		m_uEndLineNumber(uEndLineNumber),
		m_pContainingScope(NULL)
	{
		static QString szSep1("::");
		static QChar szSep2('.');
	
		if(m_szUnresolvedScope.isEmpty())
			m_uScopePartCount = 0;
		else if(eLanguage == Cpp)
			m_uScopePartCount = (quint16)m_szUnresolvedScope.count(szSep1) + 1;
		else
			m_uScopePartCount = (quint16)m_szUnresolvedScope.count(szSep2) + 1;
	}
	
	virtual ~C3Symbol()
	{
	}

public:
	AccessLevel accessLevel() const
	{
		return m_eAccess;
	}

	inline quint16 unresolvedScopePartCount() const
	{
		return m_uScopePartCount;
	}

	inline C3SymbolFile* file()
	{
		return m_pFile;
	}

	const QString & filePath() const;

	inline Type type() const
	{
		return m_eType;
	}
	
	inline bool isNamespace() const
	{
		return m_eType == Namespace;
	}

	inline bool isEnumerationMember() const
	{
		return m_eType == EnumerationMember;
	}

	virtual void clear();

	void setType(Type eType)
	{
		m_eType = eType;
	}
	
	QString typeString() const;

	inline Language language() const
	{
		return m_eLanguage;
	}
	
	void setLanguage(Language eLanguage)
	{
		m_eLanguage = eLanguage;
	}

	inline const QString & identifier() const
	{
		return m_szIdentifier;
	}
	
	void setIdentifier(const QString &szIdentifier)
	{
		m_szIdentifier = szIdentifier;
	}

	inline const QString & unresolvedScope() const
	{
		return m_szUnresolvedScope;
	}
	
#if 0
	void setScope(const QString &szScope)
	{
		m_szUnresolvedScope = szScope;
	}
#endif
	inline quint32 lineNumber() const
	{
		return m_uLineNumber;
	}
	
	void setLineNumber(quint32 uLineNumber)
	{
		m_uLineNumber = uLineNumber;
	}

	inline quint32 endLineNumber() const
	{
		return m_uEndLineNumber;
	}
	
	void setEndLineNumber(quint32 uEndLineNumber)
	{
		m_uEndLineNumber = uEndLineNumber;
	}

	C3SymbolScope * resolvedContainingScope()
	{
		return m_pContainingScope;
	}
	
	QString bestKnownContainingScopeFullyQualifiedName();
	
	QString buildFullyQualifiedName();

	QStringList buildFullyQualifiedNameAsList();

	static bool symbolIdentifierLessThan(const C3Symbol * t1,const C3Symbol * t2)
	{
		return t1->m_szIdentifier < t2->m_szIdentifier;
#if 0
		int c = QString::compare(t1->m_szIdentifier,t2->m_szIdentifier);
		if(c < 0)
			return true;
		if(c > 0)
			return false;
		return ((int)(t1->m_eType)) < ((int)(t2->m_eType)); // function prototypes come before function definitions
#endif
	}
	
	static bool symbolLineNumberLessThan(const C3Symbol * t1,const C3Symbol * t2)
	{
	    return t1->m_uLineNumber < t2->m_uLineNumber;
	}

	virtual bool isScope() const;

	virtual void dump(const QString &szPrefix);
	
	// A full description possibly containing also the type and stuff
	virtual QString description() const;

	// A key that should be unique for each completion.
	// Symbols that should NOT be duplicated in completion must have the same key.
	// Symbols with different keys will be duplicated (even if the identifier is the same).
	// This is useful for function completions which may have different overloads.
	virtual QString completionKey() const;

	static QString accessLevelKeyword(AccessLevel eLevel);

private:
	friend class C3SymbolScope;

	// May be called only by C3SymbolScope
	void setParentScope(C3SymbolScope * pScope)
	{
		m_pContainingScope = pScope;
	}

}; // class C3Symbol

typedef QMultiMap<QString,C3Symbol *> C3SymbolMap;
typedef QList<C3Symbol *> C3SymbolList;

#endif //!_C3Symbol_h_
