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

class SignaturePart
{
public:
	enum Type
	{
		Space,
		Identifier,
		OpenParenthesis,
		ClosedParenthesis,
		Comma,
		OtherSymbols
	};

public:
	Type eType;
	QString sText;
};

static void splitSignature(const QString &s,QList<SignaturePart *> & lParts)
{
	const QChar * c = s.unicode();
	const QChar * e = c + s.length();

	SignaturePart * sp;
	
	while(c < e)
	{
		const QChar * b = c;

		if(c->isSpace())
		{
			c++;
			while((c < e) && c->isSpace())
				c++;

			sp = new SignaturePart();
			sp->eType = SignaturePart::Space;
			sp->sText = QString(b,c-b);
			lParts.append(sp);
			continue;
		}
		
		if(c->isLetterOrNumber() || (c->unicode() == '_'))
		{
			c++;
			while((c < e) && (c->isLetterOrNumber() || (c->unicode() == '_')))
				c++;

			sp = new SignaturePart();
			sp->eType = SignaturePart::Identifier;
			sp->sText = QString(b,c-b);
			lParts.append(sp);
			continue;
		}
		
		if(c->unicode() == '(')
		{
			c++;
			sp = new SignaturePart();
			sp->eType = SignaturePart::OpenParenthesis;
			sp->sText = __literal("(");
			lParts.append(sp);
			continue;
		}

		if(c->unicode() == ')')
		{
			c++;
			sp = new SignaturePart();
			sp->eType = SignaturePart::ClosedParenthesis;
			sp->sText = __literal(")");
			lParts.append(sp);
			continue;
		}

		if(c->unicode() == ',')
		{
			c++;
			sp = new SignaturePart();
			sp->eType = SignaturePart::Comma;
			sp->sText = __literal(",");
			lParts.append(sp);
			continue;
		}

		while(
				(c < e) &&
				(!c->isLetterOrNumber()) &&
				(!c->isSpace()) &&
				(c->unicode() != '_') &&
				(c->unicode() != ')') &&
				(c->unicode() != '(') &&
				(c->unicode() != ',')
			)
			c++;

		sp = new SignaturePart();
		sp->eType = SignaturePart::OtherSymbols;
		sp->sText = QString(b,c-b);
		lParts.append(sp);
	}
}

static QString reformatSignature(const QString &s,const QString &sHighlight)
{
	QList<SignaturePart *> lParts;
	
	splitSignature(s,lParts);

	QString sRet;

	int cnt = lParts.count();
	int idx = 0;
	
	while(idx < cnt)
	{
		SignaturePart * sp = lParts.at(idx);
		idx++;
	
		switch(sp->eType)
		{
			case SignaturePart::Identifier:
				if(sp->sText == sHighlight)
					sRet += __literal("<span style='color: #ffffff'>%1</span>").arg(sp->sText);
				else if(
						(sp->sText == __literal("const")) ||
						(sp->sText == __literal("virtual")) ||
						(sp->sText == __literal("static")) ||
						(sp->sText == __literal("void")) ||
						(sp->sText == __literal("int"))
					)
					sRet += __literal("<span style='color: #898940'>%1</span>").arg(sp->sText);
				else
					sRet += __literal("<span style='color: #ffff40'>%1</span>").arg(sp->sText);
			break;
			case SignaturePart::OpenParenthesis:
				sRet += __literal("<span style='color: #909050'>%1</span>").arg(sp->sText);
				if((idx < cnt) && (lParts.at(idx)->eType != SignaturePart::ClosedParenthesis))
					sRet += __literal("<br>&nbsp; &nbsp; &nbsp; &nbsp;");
			break;
			case SignaturePart::Comma:
				sRet += __literal("<span style='color: #909050'>%1</span>").arg(sp->sText);
				sRet += __literal("<br>&nbsp; &nbsp; &nbsp; &nbsp;");
			break;
			case SignaturePart::ClosedParenthesis:
				if((idx > 1) && (lParts.at(idx-2)->eType != SignaturePart::OpenParenthesis))
					sRet += __literal("<br>&nbsp; &nbsp;");
				sRet += __literal("<span style='color: #909050'>%1</span>").arg(sp->sText);
			break;
			case SignaturePart::OtherSymbols:
				sRet += __literal("<span style='color: #909050'>%1</span>").arg(sp->sText.toHtmlEscaped());
			break;
			case SignaturePart::Space:
				sRet += sp->sText;
			break;
			default:
				Q_ASSERT(false);
				sRet += sp->sText.toHtmlEscaped();
			break;
		}
	}
	
	qDeleteAll(lParts);
	
	return sRet;
}


QString C3Link::toHtml() const
{
	QString szText = value("text").toString();
	QString szDescription = value("description").toString().toHtmlEscaped();
	QString szPath = C3Workspace::currentWorkspace()->toWorkspaceDependantPath(path());
	QString szLocation = location();
	
	szText = reformatSignature(szText,value("symbol").toString());

	if(!szLocation.isEmpty())
		szPath.append(__literal(":%1").arg(szLocation));

	static QString szStyle1 = __literal(
			"font-family: Monospace;"
			"font-weight: bold;"
			"font-size: normal;"
			"color: #eeffa0;"
			"margin-bottom: 4px;"
		);

	static QString szStyle2 = __literal(
			"font-family: Monospace;"
			"font-weight: 300;"
			"font-size: normal;"
			"color: #a0a0a0;"
			"margin-bottom: 4px;"
			"margin-left: 16px;"
		);

	static QString szStyle3 = __literal(
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
			sz = __literal(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
					"<div style='%5'>%6</div>"
				)
					.arg(szStyle1).arg(szText)
					.arg(szStyle2).arg(szDescription)
					.arg(szStyle3).arg(szPath);
		} else {
			sz = __literal(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
				)
					.arg(szStyle1).arg(szText)
					.arg(szStyle2).arg(szPath);
		}
	} else {
		if(!szDescription.isEmpty())
		{
			sz = __literal(
					"<div style='%1'>%2</div>"
					"<div style='%3'>%4</div>"
				)
					.arg(szStyle1).arg(szDescription)
					.arg(szStyle2).arg(szPath);
		} else {
			sz = __literal(
					"<div style='%1'>%2</div>"
				)
					.arg(szStyle1).arg(szPath);
		}
	}
	
	return sz;
}
