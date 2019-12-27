//=============================================================================
//
//   File : C3PixmapCache.cpp
//   Creation Date : mar 20 ott 2015 02:35:33
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

#include "C3PixmapCache.h"
#include "C3Application.h"

#include <QCache>
#include <QDir>

C3PixmapCache * C3PixmapCache::m_pInstance = NULL;

class C3PixmapCachePrivate
{
public:
	QCache<QString,QPixmap> oCache;
	QPixmap oDefaultPixmap;
	
	QPixmap * pCompletionNamespacePixmap;
	QPixmap * pCompletionClassPixmap;
	QPixmap * pCompletionFunctionPixmap;
	QPixmap * pCompletionOtherPixmap;
	QPixmap * pCompletionIncludeFilePixmap;
	QPixmap * pCompletionEnumerationPixmap;
	QPixmap * pCompletionEnumerationMemberPixmap;
	QPixmap * pCompletionLocalVariablePixmap;
	QPixmap * pCompletionGlobalVariablePixmap;
	QPixmap * pCompletionMemberVariablePixmap;
	QPixmap * pCompletionMacroPixmap;
	QPixmap * pCompletionKeywordPixmap;
	QPixmap * pCompletionTypeDefinitionPixmap;
};

#define _p m_pC3PC

C3PixmapCache::C3PixmapCache()
{
	m_pInstance = this;

	_p = new C3PixmapCachePrivate();

	_p->oCache.setMaxCost(1024);
	
	_p->pCompletionClassPixmap = new QPixmap(pixmap(__ascii("completion/16px/class.png")));
	_p->pCompletionNamespacePixmap = new QPixmap(pixmap(__ascii("completion/16px/namespace.png")));
	_p->pCompletionFunctionPixmap = new QPixmap(pixmap(__ascii("completion/16px/function.png")));
	_p->pCompletionOtherPixmap = new QPixmap(pixmap(__ascii("completion/16px/other.png")));
	_p->pCompletionIncludeFilePixmap = new QPixmap(pixmap(__ascii("completion/16px/include-file.png")));
	_p->pCompletionEnumerationPixmap = new QPixmap(pixmap(__ascii("completion/16px/enumeration.png")));
	_p->pCompletionEnumerationMemberPixmap = new QPixmap(pixmap(__ascii("completion/16px/enumeration-member.png")));
	_p->pCompletionLocalVariablePixmap = new QPixmap(pixmap(__ascii("completion/16px/variable-local.png")));
	_p->pCompletionGlobalVariablePixmap = new QPixmap(pixmap(__ascii("completion/16px/variable-global.png")));
	_p->pCompletionMemberVariablePixmap = new QPixmap(pixmap(__ascii("completion/16px/variable-member.png")));
	_p->pCompletionMacroPixmap = new QPixmap(pixmap(__ascii("completion/16px/macro.png")));
	_p->pCompletionKeywordPixmap = new QPixmap(pixmap(__ascii("completion/16px/keyword.png")));
	_p->pCompletionTypeDefinitionPixmap = new QPixmap(pixmap(__ascii("completion/16px/type-definition.png")));
}

C3PixmapCache::~C3PixmapCache()
{
	delete _p->pCompletionClassPixmap;
	delete _p->pCompletionEnumerationMemberPixmap;
	delete _p->pCompletionEnumerationPixmap;
	delete _p->pCompletionFunctionPixmap;
	delete _p->pCompletionIncludeFilePixmap;
	delete _p->pCompletionNamespacePixmap;
	delete _p->pCompletionOtherPixmap;
	delete _p->pCompletionLocalVariablePixmap;
	delete _p->pCompletionGlobalVariablePixmap;
	delete _p->pCompletionMemberVariablePixmap;
	delete _p->pCompletionMacroPixmap;
	delete _p->pCompletionKeywordPixmap;
	delete _p->pCompletionTypeDefinitionPixmap;
	
	delete _p;
	
	m_pInstance = NULL;
}

const QPixmap & C3PixmapCache::pixmapCompletionNamespace() const
{
	return *(_p->pCompletionNamespacePixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionClass() const
{
	return *(_p->pCompletionClassPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionFunction() const
{
	return *(_p->pCompletionFunctionPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionIncludeFile() const
{
	return *(_p->pCompletionIncludeFilePixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionEnumeration() const
{
	return *(_p->pCompletionEnumerationPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionEnumerationMember() const
{
	return *(_p->pCompletionEnumerationMemberPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionOther() const
{
	return *(_p->pCompletionOtherPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionLocalVariable() const
{
	return *(_p->pCompletionLocalVariablePixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionGlobalVariable() const
{
	return *(_p->pCompletionGlobalVariablePixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionMemberVariable() const
{
	return *(_p->pCompletionMemberVariablePixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionMacro() const
{
	return *(_p->pCompletionMacroPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionKeyword() const
{
	return *(_p->pCompletionKeywordPixmap);
}

const QPixmap & C3PixmapCache::pixmapCompletionTypeDefinition() const
{
	return *(_p->pCompletionTypeDefinitionPixmap);
}

QPixmap * C3PixmapCache::tryLoadImage(const QString &szPath,const QString &szName)
{
	QString szFullPath = szPath;
	szFullPath += QDir::separator();
	szFullPath += szName;
	
	if(!QFile::exists(szFullPath))
		return NULL;
	
	QPixmap * pPix = new QPixmap(szFullPath);
	if(pPix->isNull())
	{
		delete pPix;
		return NULL;
	}
	
	_p->oCache.insert(szName,pPix);
	return pPix;
}

const QPixmap & C3PixmapCache::pixmap(const QString &szName)
{
	QPixmap * pPix = _p->oCache.object(szName);
	if(pPix)
		return *pPix;
	
	// lookup the image
	
	pPix = tryLoadImage(__utf8("%1/images").arg(C3_RESOURCES_DIR),szName);
	if(pPix)
		return *pPix;

	pPix = tryLoadImage(__utf8("%1/images").arg(C3Application::applicationDirPath()),szName);
	if(pPix)
		return *pPix;

#ifdef Q_OS_MAC
	pPix = tryLoadImage(__utf8("%1/../Contents/Resources/images").arg(C3Application::applicationDirPath()),szName);
	if(pPix)
		return *pPix;
#endif
	return _p->oDefaultPixmap;
}

