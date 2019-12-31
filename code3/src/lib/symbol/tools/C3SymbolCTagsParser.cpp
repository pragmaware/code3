//=============================================================================
//
//   File : C3SymbolCTagsParser.cpp
//   Creation Date : 2015/12/05 18:02:12
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

#include "C3SymbolCTagsParser.h"

#include "C3SymbolFile.h"
#include "C3SymbolFunctionDefinition.h"
#include "C3SymbolFunctionPrototype.h"
#include "C3SymbolClass.h"
#include "C3SymbolVariable.h"
#include "C3SymbolScope.h"
#include "C3SymbolImportedScope.h"
#include "C3SymbolTypeDefinition.h"
#include "C3SymbolNamespace.h"

#include <QHash>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QRegularExpression>

//#define DEBUG_CTAGS_PARSER 1

class C3SymbolCTagsParserPrivate
{
public:
	QHash<QString,C3SymbolFile *> * pFiles;

	// auxiliary chars (FIXME: this stuff could be placed in a shared class to avoid initialization/duplication cost)
	QChar cTab;
	QChar cExclamation;
	QChar cColon;

	QHash<QString,quint16> hLanguageMap;
	QHash<QString,quint16> hKindMap;
	QHash<QString,quint16> hAccessMap;

	QHash<QString,int> hDiscardInTypes; // tokens to discard in type names
	
	QString sPathToReplace;
	QString sPathReplacement;
};

#define _p m_pC3SCTP

C3SymbolCTagsParser::C3SymbolCTagsParser()
{
	_p = new C3SymbolCTagsParserPrivate();

	_p->pFiles = new QHash<QString,C3SymbolFile *>();

	_p->cTab = QChar('\t');
	_p->cExclamation = QChar('!');
	_p->cColon = QChar(':');

	_p->hLanguageMap.insert("c++",C3Symbol::Cpp);
	_p->hLanguageMap.insert("C++",C3Symbol::Cpp);
	_p->hLanguageMap.insert("CPP",C3Symbol::Cpp);
	_p->hLanguageMap.insert("cpp",C3Symbol::Cpp);
	_p->hLanguageMap.insert("c",C3Symbol::Cpp);
	_p->hLanguageMap.insert("C",C3Symbol::Cpp);
	_p->hLanguageMap.insert("cuda",C3Symbol::Cpp);
	_p->hLanguageMap.insert("CUDA",C3Symbol::Cpp);
	_p->hLanguageMap.insert("Cuda",C3Symbol::Cpp);
	_p->hLanguageMap.insert("C#",C3Symbol::CSharp);
	_p->hLanguageMap.insert("c#",C3Symbol::CSharp);
	_p->hLanguageMap.insert("CSharp",C3Symbol::CSharp);
	_p->hLanguageMap.insert("csharp",C3Symbol::CSharp);
	_p->hLanguageMap.insert("Java",C3Symbol::Java);
	_p->hLanguageMap.insert("java",C3Symbol::Java);
	_p->hLanguageMap.insert("JAVA",C3Symbol::Java);
	_p->hLanguageMap.insert("Php",C3Symbol::Php);
	_p->hLanguageMap.insert("php",C3Symbol::Php);
	_p->hLanguageMap.insert("PHP",C3Symbol::Php);
	_p->hLanguageMap.insert("javascript",C3Symbol::Javascript);
	_p->hLanguageMap.insert("jscript",C3Symbol::Javascript);
	_p->hLanguageMap.insert("js",C3Symbol::Javascript);
	_p->hLanguageMap.insert("Javascript",C3Symbol::Javascript);
	_p->hLanguageMap.insert("JavaScript",C3Symbol::Javascript);
	_p->hLanguageMap.insert("JS",C3Symbol::Javascript);
	_p->hLanguageMap.insert("JScript",C3Symbol::Javascript);
	_p->hLanguageMap.insert("Python",C3Symbol::Python);
	_p->hLanguageMap.insert("python",C3Symbol::Python);
	_p->hLanguageMap.insert("py",C3Symbol::Python);

	_p->hKindMap.insert("enum",C3Symbol::Enumeration);
	_p->hKindMap.insert("enumerator",C3Symbol::EnumerationMember);
	_p->hKindMap.insert("macro",C3Symbol::Macro);
	_p->hKindMap.insert("class",C3Symbol::Class);
	_p->hKindMap.insert("struct",C3Symbol::Class);
	_p->hKindMap.insert("union",C3Symbol::Union);
	_p->hKindMap.insert("function",C3Symbol::FunctionDefinition);
	_p->hKindMap.insert("method",C3Symbol::FunctionDefinition);
	_p->hKindMap.insert("prototype",C3Symbol::FunctionPrototype);
	_p->hKindMap.insert("namespace",C3Symbol::Namespace);
	_p->hKindMap.insert("typedef",C3Symbol::TypeDefinition);
	_p->hKindMap.insert("member",C3Symbol::MemberVariable);
	_p->hKindMap.insert("property",C3Symbol::MemberVariable);
	_p->hKindMap.insert("local",C3Symbol::LocalVariable);
	_p->hKindMap.insert("parameter",C3Symbol::LocalVariable);
	_p->hKindMap.insert("variable",C3Symbol::GlobalVariable);
	_p->hKindMap.insert("using",C3Symbol::ImportedScope);
	
	_p->hAccessMap.insert("public",C3Symbol::AccessLevelPublic);
	_p->hAccessMap.insert("protected",C3Symbol::AccessLevelProtected);
	_p->hAccessMap.insert("private",C3Symbol::AccessLevelPrivate);

	_p->hDiscardInTypes.insert(__utf8("inline"),1);
	_p->hDiscardInTypes.insert(__utf8("public"),1);
	_p->hDiscardInTypes.insert(__utf8("private"),1);
	_p->hDiscardInTypes.insert(__utf8("protected"),1);
	_p->hDiscardInTypes.insert(__utf8("internal"),1);
	_p->hDiscardInTypes.insert(__utf8("package"),1);
	_p->hDiscardInTypes.insert(__utf8("abstract"),1);
	_p->hDiscardInTypes.insert(__utf8("override"),1);
	_p->hDiscardInTypes.insert(__utf8("overridable"),1);
	_p->hDiscardInTypes.insert(__utf8("mustoverride"),1);
	_p->hDiscardInTypes.insert(__utf8("virtual"),1);
	_p->hDiscardInTypes.insert(__utf8("volatile"),1);
	_p->hDiscardInTypes.insert(__utf8("mutable"),1);
	_p->hDiscardInTypes.insert(__utf8("__stdcall"),1);
	_p->hDiscardInTypes.insert(__utf8("__thiscall"),1);
	_p->hDiscardInTypes.insert(__utf8("__fastcall"),1);
	_p->hDiscardInTypes.insert(__utf8("explicit"),1);
	_p->hDiscardInTypes.insert(__utf8("static"),1);
}

C3SymbolCTagsParser::~C3SymbolCTagsParser()
{
	if(_p->pFiles)
	{
		qDeleteAll(*(_p->pFiles));
		delete _p->pFiles;
	}
	delete _p;
}

void C3SymbolCTagsParser::setReplaceInPath(const QString &sPath,const QString &sReplacement)
{
	_p->sPathToReplace = sPath;
	_p->sPathReplacement = sReplacement;
	
#ifdef DEBUG_CTAGS_PARSER
	qDebug("PATH REPLACEMENT %s -> %s",sPath.toUtf8().data(),sReplacement.toUtf8().data());
#endif
}

QHash<QString,C3SymbolFile *> * C3SymbolCTagsParser::takeSymbolFiles()
{
	QHash<QString,C3SymbolFile *> * pRet = _p->pFiles;
	_p->pFiles = NULL;
	return pRet;
}

QString C3SymbolCTagsParser::filterTypeFromSearchPattern(const QString &szType)
{
	// FIXME: Handle template types!
#ifdef DEBUG_CTAGS_PARSER
	//qDebug("TRY FILTER TYPE FROM %s",szType.toUtf8().data());
#endif

	if(szType.isEmpty())
		return szType;

	// split words/symbols/spaces
	const QChar * b = szType.unicode();
	const QChar * p = b;
	const QChar * e = b + szType.length();

	int iGroup,iGroup2;
	
	if(p->isSpace())
		iGroup = 1;
	else if(p->unicode() == '*' || p->unicode() == '&')
		iGroup = 2;
	else
		iGroup = 3;

	p++;

	QString szFiltered;

	static QString szAPI("API");
	static QString szEXPORT("EXPORT");
	static QString szDeclspec("declspec");
	static QString szStdcall("stdcall");
	static QString szFastcall("fastcall");

	int iParts = 0;

	while(p <= e)
	{
		if(p->isSpace() || (p->unicode() == 0))
			iGroup2 = 1;
		else if(p->unicode() == '*' || p->unicode() == '&')
			iGroup2 = 2;
		else if(p->unicode() == '=' || p->unicode() == '(') // complex declaration like int a = 10,b or QString a(something), b(somethingElse)
			return QString();
		else
			iGroup2 = 3;
	
		if(iGroup2 != iGroup)
		{
			switch(iGroup)
			{
				case 1:
					// skip
				break;
				case 2:
					// keep
					if(iParts > 0)
					{
						if(iParts > 3)
							return QString(); // assume not valid
						szFiltered.append(' ');
					}
					szFiltered.append(b,p-b);
					iParts++;
				break;
				case 3:
				{
					// keep if good
					const QChar * pp = p - 1;
					
					QString szToken(b,p-b);
					
					if(
							(pp->unicode() != ':') && // namespace
							(pp->unicode() != '.') && // namespace
							(_p->hDiscardInTypes.value(szToken,0) == 0) && // discardable
							(!szToken.contains(szAPI)) &&
							(!szToken.contains(szEXPORT)) &&
							(!szToken.contains(szDeclspec)) &&
							(!szToken.contains(szFastcall)) &&
							(!szToken.contains(szStdcall))
						)
					{
						if(iParts > 0)
						{
							if(iParts > 4)
								return QString(); // assume not valid
							szFiltered.append(' ');
						}
						szFiltered.append(szToken);
						iParts++;
					}
				}
				break;
			}
			b = p;
			iGroup = iGroup2;
		}
		p++;
	}

	// int a,b; -> we end up with type="int a,"
	if(szFiltered.endsWith(','))
		return QString();

	return szFiltered;
}

/*
bool C3SymbolCTagsParser::parseLineOld(const QString &szLine,QString &szError)
{
#ifdef DEBUG_CTAGS_PARSER
	qDebug("TAGLINE: %s",szLine.toUtf8().data());
#endif

	if(szLine.startsWith(_p->cExclamation))
		return true; // ctags meta-tag

	static QString szVITrick(";\"");

	// tag_name<TAB>file_name<TAB>ex_cmd;"<TAB>extension_field<TAB>extension_field<TAB>...

	int idx = szLine.indexOf(szVITrick);
	if(idx <= 3)
		return true; // something unreadable
	
	QString szLeft = szLine.left(idx);
	QString szRest = szLine.mid(idx+2);

	idx = szLeft.indexOf(_p->cTab); // tab
	if(idx < 1)
		return true; // something unreadable

	QString szIdentifier = szLeft.left(idx);
	szLeft.remove(0,idx+1);

	idx = szLeft.indexOf(_p->cTab); // tab
	if(idx < 1)
		return true; // something unreadable

	QString szFileName = szLeft.left(idx);
	
	if(!_p->sPathReplacement.isEmpty())
		szFileName.replace(_p->sPathToReplace,_p->sPathReplacement);
	
	QString szPattern = szLeft.mid(idx+1);

	QString szKind;
	QString szLineNumber;
	QString szEndLineNumber;
	QString szLanguage;
	QString szInherits;
	QString szScope;
	QString szSignature;
	QString szType;
	QString szProperties;

	C3Symbol::AccessLevel eAccessLevel = C3Symbol::AccessLevelUnknown;

	static QString szLanguageTag("language");
	static QString szKindTag("kind");
	static QString szInheritsTag("inherits");
	static QString szScopeTag("scope");
	static QString szSignatureTag("signature");
	static QString szTyperefTag("typeref");
	static QString szLineTag("line");
	static QString szEndTag("end");
	static QString szAccessTag("access");
	static QString szDoubleColon("::");
	static QString szPropertiesTag("properties");
	static QString szConst("const");

	static QChar cOpenParenthesis('(');
	static QChar cClosedParenthesis(')');

	static QString szClosedParenthesisConst(") const");

	QStringList sl = szRest.split(_p->cTab);

	int i = 0;
	while(i < sl.count())
	{
		QString s = sl.at(i);
		i++;
		
		idx = s.indexOf(_p->cColon);
		if(idx <= 0)
			continue; // ???
		
		QString szTagType = s.left(idx).toLower();
		QString szTagValue = s.mid(idx+1);
		
		if(szTagType.isEmpty())
			continue;
		if(szTagValue.isEmpty())
			continue;

		if(szTagType == szLanguageTag)
			szLanguage = szTagValue.toLower();
		else if(szTagType == szKindTag)
			szKind = szTagValue.toLower();
		else if(szTagType == szInheritsTag)
			szInherits = szTagValue;
		else if(szTagType == szScopeTag)
		{
			// the scope also contains "class:" "namespace:" etc.. prefix.
			idx = szTagValue.indexOf(_p->cColon);
			if(idx <= 0)
				szScope = szTagValue.trimmed();
			else
				szScope = szTagValue.mid(idx+1).trimmed();
			//szScope.replace("::",".");
			//if(szScope.startsWith('.'))
			//	szScope = szScope.mid(1);
		
			if(szScope.startsWith(szDoubleColon))
				szScope = szScope.mid(2);
		} else if(szTagType == szSignatureTag)
		{
			if(szTagValue.startsWith(cOpenParenthesis))
			{
				if(szTagValue.endsWith(cClosedParenthesis))
					szSignature = szTagValue.mid(1,szTagValue.length() - 2).trimmed();
				else if(szTagValue.endsWith(szClosedParenthesisConst))
					szSignature = szTagValue.mid(1,szTagValue.length() - 8).trimmed();
				else
					szSignature = szTagValue.trimmed();
			} else {
				szSignature = szTagValue.trimmed();
			}
		} else if(szTagType == szLineTag)
		{
			szLineNumber = szTagValue;
		} else if(szTagType == szEndTag)
		{
			szEndLineNumber = szTagValue;
		} else if(szTagType == szTyperefTag)
		{
			idx = szTagValue.indexOf(_p->cColon);
			if(idx <= 0)
				szType = szTagValue.trimmed();
			else
				szType = szTagValue.mid(idx+1).trimmed();
		} else if(szTagType == szAccessTag)
		{
			eAccessLevel = (C3Symbol::AccessLevel)_p->hAccessMap.value(szTagValue,C3Symbol::AccessLevelUnknown);
		} else if(szTagType == szPropertiesTag)
		{
			szProperties = szTagValue;
			//qDebug("Properties: %s",szProperties.toUtf8().data());
		}
	}

	if(szLanguage.isEmpty())
		return true;
	if(szKind.isEmpty())
		return true;
	if(szLineNumber.isEmpty())
		return true;

	bool ok;
	
	unsigned int uLineNumber = szLineNumber.toUInt(&ok);
	if(!ok)
		return true;

	unsigned int uEndLineNumber;
	if(szEndLineNumber.isEmpty())
	{
		uEndLineNumber = uLineNumber;
	} else {
		uEndLineNumber = szEndLineNumber.toUInt(&ok);
		if(!ok)
			uEndLineNumber = uLineNumber;
	}

	// ctags returns 1-based lines!
	if(uLineNumber > 0)
		uLineNumber--;
	if(uEndLineNumber > 0)
		uEndLineNumber--;

	// C/C++:
	// kind: enum -> enum declaration
	// kind: enumerator -> enum entry (enum stuff { ..., X, ... })
	// kind: function -> function implementation (may have class:Y attribute)
	// kind: prototype -> function prototype (may have class:Y attribute)
	// kind: class -> class declaration (class X)
	// kind: typedef -> typedef someting X
	// kind: member -> member variable (has class:Y attribute too)
	// kind: using -> using namespace X
	
	quint16 uLanguage = _p->hLanguageMap.value(szLanguage,C3Symbol::InvalidLanguage);

	if(uLanguage >= C3Symbol::InvalidLanguage)
		return true; // ignore

	quint16 uType = _p->hKindMap.value(szKind,C3Symbol::InvalidType);
	
	if(uType >= C3Symbol::InvalidType)
		return true; // ignore

	//qDebug("Language %s, kind %s, line num %s",szLanguage.toUtf8().data(),szKind.toUtf8().data(),szLineNumber.toUtf8().data());

	if(szType.isEmpty() && ((uLanguage == C3Symbol::Cpp) || (uLanguage == C3Symbol::Java) || (uLanguage == C3Symbol::CSharp)))
	{
		if(uType & (C3Symbol::FunctionDefinition | C3Symbol::FunctionPrototype | C3Symbol::GlobalVariable | C3Symbol::MemberVariable | C3Symbol::LocalVariable))
		{
			// look up the type in the pattern
			int idx = szPattern.indexOf(szIdentifier);
			if(idx > 0)
			{
				// FIXME: Do this better. Do this faster.
				if(szPattern.startsWith("/^"))
					szType = filterTypeFromSearchPattern(szPattern.mid(2,idx-2));
				else
					szType = filterTypeFromSearchPattern(szPattern.left(idx));
			}
		}
	}

#ifdef DEBUG_CTAGS_PARSER
	if(!szType.isEmpty())
		qDebug("Type is %s",szType.toUtf8().data());
#endif

	Q_ASSERT(_p->pFiles);
	C3SymbolFile * pFile = _p->pFiles->value(szFileName,NULL);
	if(!pFile)
	{
		pFile = new C3SymbolFile(szFileName);
		_p->pFiles->insert(szFileName,pFile);
	}

	switch(uType)
	{
		case C3Symbol::Class:
		{
			C3SymbolClass * pTag = new C3SymbolClass(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szInherits,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::Namespace:
		{
			C3SymbolNamespace * pTag = new C3SymbolNamespace(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::Union:
		case C3Symbol::Enumeration:
		{
			C3SymbolScope * pTag = new C3SymbolScope(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::FunctionDefinition:
		{
			quint8 uFunctionFlags = 0;
		
			if(szProperties.contains(szConst))
				uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsConst;
		
			C3SymbolFunctionDefinition * pTag = new C3SymbolFunctionDefinition(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szSignature,
					szType,
					uFunctionFlags
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::FunctionPrototype:
		{
			quint8 uFunctionFlags = 0;
		
			if(szProperties.contains(szConst))
				uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsConst;

			C3SymbolFunctionPrototype * pTag = new C3SymbolFunctionPrototype(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szSignature,
					szType,
					uFunctionFlags
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::GlobalVariable:
		case C3Symbol::MemberVariable:
		case C3Symbol::LocalVariable:
		{
			C3SymbolVariable * pTag = new C3SymbolVariable(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szType
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::TypeDefinition:
		{
			C3SymbolTypeDefinition * pTag = new C3SymbolTypeDefinition(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szType
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::ImportedScope:
		{
			C3SymbolImportedScope * pTag = new C3SymbolImportedScope(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);

			pFile->addImportedScope(pTag);
		}
		break;
		default:
		{
			C3Symbol * pTag = new C3Symbol(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
	}

#ifdef DEBUG_CTAGS_PARSER
	qDebug("TAG(%s) %s %s:%d",szLanguage.toUtf8().data(),szIdentifier.toUtf8().data(),szFileName.toUtf8().data(),uLineNumber);
#endif
	return true;
}*/


bool C3SymbolCTagsParser::parseLine(const char * pLine,int len,QString &szError)
{
#ifdef DEBUG_CTAGS_PARSER
	qDebug("TAGLINE: %s",pLine);
#endif

	Q_ASSERT(pLine);

	const char * p = pLine;
	const char * e = pLine + len;

	while((p < e) && ((*p == ' ') || (*p == '\r') || (*p == '\n')))
		p++;

	if(p >= e)
		return true;
	
	if(*p == '!')
		return true; // ctags meta-tag

	// tag_name<TAB>file_name<TAB>ex_cmd;"<TAB>extension_field<TAB>extension_field<TAB>...

	// Identifier
	const char * b = p;
	while((p < e) && (*p != '\t'))
		p++;
	if((p >= e) || (p == b))
		return true; // something unreadable

	QString szIdentifier = QString::fromUtf8(b,p-b);

	p++;
	if(p >= e)
		return true; // something unreadable

	// File name
	b = p;
	while((p < e) && (*p != '\t'))
		p++;
	if((p >= e) || (p == b))
		return true; // something unreadable

	QString szFileName = QString::fromUtf8(b,p-b);

	if(!_p->sPathReplacement.isEmpty())
		szFileName.replace(_p->sPathToReplace,_p->sPathReplacement);

	p++;
	if(p >= e)
		return true; // something unreadable

	// End of ex_cmd
	b = p;
	while(p < e)
	{
		while((p < e) && (*p != ';'))
			p++;
		if(p >= (e-1))
			return true;
		if(*(p + 1) == '\"')
			break;
		p++;
	}
	if((p >= e) || (p == b))
		return true; // something unreadable

	QString szPattern = QString::fromUtf8(b,p-b);

	p += 2;

	// kill trailing \r\n, if any

	while(p < e)
	{
		char x = *(e-1);
		if((x != '\r') && (x != '\n') && (x != ' '))
			break;
		e--;
	}

	if(p >= e)
		return true; // something unreadable

	qint16 uType = C3Symbol::InvalidType;
	quint16 uLanguage = C3Symbol::InvalidLanguage;

	QString szInherits;
	QString szType;
	QString szScope;
	QString szLineNumber;
	QString szEndLineNumber;
	QString szSignature;
	QString szProperties;

	C3Symbol::AccessLevel eAccessLevel = C3Symbol::AccessLevelUnknown;

	static QChar cOpenParenthesis('(');
	static QChar cClosedParenthesis(')');

	static QString szClosedParenthesisConst(") const");

	while(p < e)
	{
		b = p;

		while((p < e) && (*p != ':') && (*p != '\t'))
			p++;
		if(p >= e)
			break; // no colon
		if(*p == '\t')
		{
			p++;
			continue; // no colon
		}
		
		int iTagTypeLen = p - b;

		p++;
		const char * pAfterColon = p;

		while((p < e) && (*p != '\t'))
			p++;

		if(iTagTypeLen > 0)
		{
			// got tag name and tag value
			switch(iTagTypeLen)
			{
				case 3:
					if(memcmp(b,"end",3) == 0)
						szEndLineNumber = QString::fromUtf8(pAfterColon,p - pAfterColon);
				break;
				case 4:
					if(memcmp(b,"kind",4) == 0)
						uType = _p->hKindMap.value(QString::fromUtf8(pAfterColon,p - pAfterColon),C3Symbol::InvalidType);
					else if(memcmp(b,"line",4) == 0)
						szLineNumber = QString::fromUtf8(pAfterColon,p - pAfterColon);
				break;
				case 5:
					if(memcmp(b,"scope",5) == 0)
					{
						// skip initial ::, if any
						while((pAfterColon < p) && (*pAfterColon == ':'))
							pAfterColon++;

						// the scope also contains "class:" "namespace:" etc.. prefix.
					
						const char * x = pAfterColon;
						while((x < p) && (*x != ':'))
							x++;
						if(x < p)
							x++;
						if(x >= p)
							szScope = QString::fromUtf8(pAfterColon,p-pAfterColon);
						else
							szScope = QString::fromUtf8(x,p-x);
					}
				break;
				case 6:
					if(memcmp(b,"access",6) == 0)
						eAccessLevel = (C3Symbol::AccessLevel)_p->hAccessMap.value(QString::fromUtf8(pAfterColon,p - pAfterColon),C3Symbol::AccessLevelUnknown);
				break;
				case 7:
					if(memcmp(b,"typeref",7) == 0)
					{
						const char * x = pAfterColon;
						while((x < p) && (*x != ':'))
							x++;
						if(x < p)
							x++;
						if(x >= p)
							szType = QString::fromUtf8(pAfterColon,p-pAfterColon);
						else
							szType = QString::fromUtf8(x,p-x);			
					}
				break;
				case 8:
					if(memcmp(b,"language",8) == 0)
						uLanguage = _p->hLanguageMap.value(QString::fromUtf8(pAfterColon,p - pAfterColon),C3Symbol::InvalidLanguage);
					else if(memcmp(b,"inherits",8) == 0)
						szInherits = QString::fromUtf8(pAfterColon,p - pAfterColon);
				break;
				case 9:
					if(memcmp(b,"signature",9) == 0)
					{
						// FIXME: Optimize this!
	
						int iSigLen = p - pAfterColon;
					
						if((iSigLen >= 2) && (*pAfterColon == '('))
						{
							// (...) ?
							if(*(p-1) == ')')
								szSignature = QString::fromUtf8(pAfterColon+1,p - pAfterColon - 2);
							// (...) const ?
							else if((iSigLen >= 8) && (memcmp(p-7,") const",7) == 0))
								szSignature = QString::fromUtf8(pAfterColon+1,p - pAfterColon - 8);
							else
								szSignature = QString::fromUtf8(pAfterColon,p - pAfterColon);
						} else {
							szSignature = QString::fromUtf8(pAfterColon,p - pAfterColon);
						}
					}
				break;
				case 10:
					if(memcmp(b,"properties",10) == 0)
						szProperties = QString::fromUtf8(pAfterColon,p - pAfterColon);
				break;
			}
		}

		if(p >= e)
			break;
		p++;
	}

	if(uType >= C3Symbol::InvalidType)
		return true; // ignore

	if(uLanguage >= C3Symbol::InvalidLanguage)
		return true; // ignore

	if(szLineNumber.isEmpty())
		return true;

	static QString szConst("const");
	static QString szVirtual("virtual");
	static QString szStatic("static");
	static QString szInline("inline");
	static QString szPure("pure");

	bool ok;
	
	unsigned int uLineNumber = szLineNumber.toUInt(&ok);
	if(!ok)
		return true;

	unsigned int uEndLineNumber;
	if(szEndLineNumber.isEmpty())
	{
		uEndLineNumber = uLineNumber;
	} else {
		uEndLineNumber = szEndLineNumber.toUInt(&ok);
		if(!ok)
			uEndLineNumber = uLineNumber;
	}

	// ctags returns 1-based lines!
	if(uLineNumber > 0)
		uLineNumber--;
	if(uEndLineNumber > 0)
		uEndLineNumber--;

	// C/C++:
	// kind: enum -> enum declaration
	// kind: enumerator -> enum entry (enum stuff { ..., X, ... })
	// kind: function -> function implementation (may have class:Y attribute)
	// kind: prototype -> function prototype (may have class:Y attribute)
	// kind: class -> class declaration (class X)
	// kind: typedef -> typedef someting X
	// kind: member -> member variable (has class:Y attribute too)
	// kind: using -> using namespace X

	//qDebug("Language %s, kind %s, line num %s",szLanguage.toUtf8().data(),szKind.toUtf8().data(),szLineNumber.toUtf8().data());

	if(szType.isEmpty() && ((uLanguage == C3Symbol::Cpp) || (uLanguage == C3Symbol::Java) || (uLanguage == C3Symbol::CSharp)))
	{
		if(uType & (C3Symbol::FunctionDefinition | C3Symbol::FunctionPrototype | C3Symbol::GlobalVariable | C3Symbol::MemberVariable | C3Symbol::LocalVariable))
		{
			// look up the type in the pattern
			int idx = szPattern.indexOf(szIdentifier);
			if(idx > 0)
			{
				// FIXME: Do this better. Do this faster.
				if(szPattern.startsWith("/^"))
					szType = filterTypeFromSearchPattern(szPattern.mid(2,idx-2));
				else
					szType = filterTypeFromSearchPattern(szPattern.left(idx));
			}
		}
	}

#ifdef DEBUG_CTAGS_PARSER
	if(!szType.isEmpty())
		qDebug("Type is %s",szType.toUtf8().data());
#endif

	Q_ASSERT(_p->pFiles);
	C3SymbolFile * pFile = _p->pFiles->value(szFileName,NULL);
	if(!pFile)
	{
		pFile = new C3SymbolFile(szFileName);
		_p->pFiles->insert(szFileName,pFile);
	}

	switch(uType)
	{
		case C3Symbol::Class:
		{
			C3SymbolClass * pTag = new C3SymbolClass(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szInherits,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::Namespace:
		{
			C3SymbolNamespace * pTag = new C3SymbolNamespace(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::Union:
		case C3Symbol::Enumeration:
		{
			C3SymbolScope * pTag = new C3SymbolScope(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::FunctionDefinition:
		{
			quint8 uFunctionFlags = 0;
		
			if(!szProperties.isEmpty())
			{
				if(szProperties.contains(szConst))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsConst;
				if(szProperties.contains(szStatic))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsStatic;
				if(szProperties.contains(szVirtual))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsVirtual;
				if(szProperties.contains(szInline))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsInline;
				if(szProperties.contains(szPure))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsPure;
			}

			C3SymbolFunctionDefinition * pTag = new C3SymbolFunctionDefinition(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szSignature,
					szType,
					uFunctionFlags
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::FunctionPrototype:
		{
			quint8 uFunctionFlags = 0;
		
			if(!szProperties.isEmpty())
			{
				if(szProperties.contains(szConst))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsConst;
				if(szProperties.contains(szStatic))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsStatic;
				if(szProperties.contains(szVirtual))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsVirtual;
				if(szProperties.contains(szInline))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsInline;
				if(szProperties.contains(szPure))
					uFunctionFlags |= C3SymbolFunctionSignature::FunctionIsPure;
			}

			C3SymbolFunctionPrototype * pTag = new C3SymbolFunctionPrototype(
					pFile,
					(C3Symbol::Language)uLanguage,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szSignature,
					szType,
					uFunctionFlags
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::GlobalVariable:
		case C3Symbol::MemberVariable:
		case C3Symbol::LocalVariable:
		{
			C3SymbolVariable * pTag = new C3SymbolVariable(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szType
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::TypeDefinition:
		{
			C3SymbolTypeDefinition * pTag = new C3SymbolTypeDefinition(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber,
					szType
				);

			pFile->addSymbol(pTag);
		}
		break;
		case C3Symbol::ImportedScope:
		{
			C3SymbolImportedScope * pTag = new C3SymbolImportedScope(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);

			pFile->addImportedScope(pTag);
		}
		break;
		default:
		{
			C3Symbol * pTag = new C3Symbol(
					pFile,
					(C3Symbol::Language)uLanguage,
					(C3Symbol::Type)uType,
					szIdentifier,
					szScope,
					eAccessLevel,
					uLineNumber,
					uEndLineNumber
				);
		
			pFile->addSymbol(pTag);
		}
		break;
	}

#ifdef DEBUG_CTAGS_PARSER
	qDebug("TAG(%s) %s %s:%d",szLanguage.toUtf8().data(),szIdentifier.toUtf8().data(),szFileName.toUtf8().data(),uLineNumber);
#endif
	return true;
}

bool C3SymbolCTagsParser::parseFile(const QString &szPath,QString &szError,bool * pbAbortSignal)
{
	QFile f(szPath);
	if(!f.open(QFile::ReadOnly))
	{
		szError = __tr("Could not open tags file");
		return false;
	}

	while(!f.atEnd())
	{
		if(*pbAbortSignal)
		{
			szError = "Aborted";
			return false;
		}

		QByteArray a = f.readLine(4096);

		if(!parseLine(a.data(),a.length(),szError))
			return false;
	}

	return true;
}
