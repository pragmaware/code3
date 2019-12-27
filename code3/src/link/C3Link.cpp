//=============================================================================
//
//   File : C3Link.cpp
//   Creation Date : 2015/11/24 23:38:26
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

#include "C3Link.h"

#include "C3Workspace.h"
#include "C3JsonUtils.h"

C3Link::C3Link(const QJsonObject &ob)
	: QHash<QString,QVariant>()
{
	*this = C3JsonUtils::jsonObjectToVariantHash(ob);
}

QJsonObject C3Link::toJsonObject() const
{
	return C3JsonUtils::variantHashToJsonObject(*this);
}

QString C3Link::toHtml() const
{
	QString szText = value("text").toString();
	QString szDescription = value("description").toString().toHtmlEscaped();
	QString szPath = C3Workspace::currentWorkspace()->toWorkspaceDependantPath(path());
	QString szLocation = location();
	bool bIsCpp = !value("isCpp").toString().isEmpty();
	if(bIsCpp)
	{
		int idx;

		static QString aToReplace[] = {
				__ascii("::"),
				__ascii("."),
				__ascii(","),
				__ascii("("),
				__ascii(")"),
				__ascii("="),
				__ascii("*"),
				__ascii("&"),
				__ascii("["),
				__ascii("]")
			};
		
		static QString szReplaceStart = __ascii("@@@");
		static QString szReplaceEnd = __ascii("###");

		for(int i=0;i<10;i++)
		{
			idx = szText.indexOf(aToReplace[i]);
			if(idx >= 0)
			{
				
				QString szReplace = szReplaceStart + aToReplace[i] + szReplaceEnd;
				szText = szText.replace(aToReplace[i],szReplace);
			}
		}
		
		szText = szText.toHtmlEscaped();

		static QString szReplace2Start = __ascii("<span style='color: #909050'>");
		static QString szReplace2End = __ascii("</span>");
		
		szText = szText.replace(szReplaceStart,szReplace2Start);
		szText = szText.replace(szReplaceEnd,szReplace2End);

		QString szHighlight = value("symbol").toString();
		if(!szHighlight.isEmpty())
		{
			int idx = szText.lastIndexOf(szHighlight);
			if(idx >= 0)
			{
				QString szTmp = szText.left(idx);
				szTmp += __ascii("<span style='color: #ffffff'>");
				szTmp += szHighlight;
				szTmp += __ascii("</span>");
				szTmp += szText.mid(idx + szHighlight.length());
				szText = szTmp;
			}
		}

	} else {
		szText = szText.toHtmlEscaped();
	}


	if(!szLocation.isEmpty())
		szPath.append(__utf8(":%1").arg(szLocation));

	static QString szStyle1 = __utf8(
			"font-family: Monospace;"
			"font-weight: bold;"
			"font-size: normal;"
			"color: #eeffa0;"
			"margin-bottom: 4px;"
		);

	static QString szStyle2 = __utf8(
			"font-family: Monospace;"
			"font-weight: 300;"
			"font-size: normal;"
			"color: #a0a0a0;"
			"margin-bottom: 4px;"
			"margin-left: 16px;"
		);

	static QString szStyle3 = __utf8(
			"font-family: Monospace;"
			"font-weight: 300;"
			"font-size: small;"
			"color: #808080;"
			"margin-left: 16px;"
		);


	QString sz;

	if(!szText.isEmpty())
	{
		if(!szDescription.isEmpty())
		{
			sz = __utf8(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
					"<div style='%5'>%6</div>"
				)
					.arg(szStyle1).arg(szText)
					.arg(szStyle2).arg(szDescription)
					.arg(szStyle3).arg(szPath);
		} else {
			sz = __utf8(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
				)
					.arg(szStyle1).arg(szText)
					.arg(szStyle2).arg(szPath);
		}
	} else {
		if(!szDescription.isEmpty())
		{
			sz = __utf8(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
				)
					.arg(szStyle1).arg(szDescription)
					.arg(szStyle2).arg(szPath);
		} else {
			sz = __utf8(
					"<div style='%1'>%2</div>"
				)
					.arg(szStyle1).arg(szPath);
		}
	}
	
	return sz;
}
