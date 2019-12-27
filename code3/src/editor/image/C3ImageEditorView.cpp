//=============================================================================
//
//   File : C3ImageEditorView.cpp
//   Creation Date : mar 20 ott 2015 04:50:04
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

#include "C3ImageEditorView.h"
#include "C3PixmapCache.h"

#include <QPaintEvent>
#include <QPainter>
#include <QShowEvent>
#include <QResizeEvent>
#include <QScrollBar>

#include <qdrawutil.h>

#include <math.h>

class C3ImageEditorViewPrivate
{
public:
	C3ImageEditor * pEditor;
	
	QScrollBar * pVScrollBar;
	QScrollBar * pHScrollBar;
	
	QPixmap oBackground;
	QImage oImage;
	
	float fScale;
	
	float fScaledImageWidth;
	float fScaledImageHeight;
	
	int iContentsWidth; // ceil(scaledImageWidth)
	int iContentsHeight; // ceil(scaledImageHeight)
	
	int iScrollBarSize;
	
	int iViewportWidth;
	int iViewportHeight;
	
	int iXOffset; // unscaled, related to iContentsWidth
	int iYOffset; // unscaled, related to iContentsHeight
	
	bool bIgnoreScrollBarChanges;
};

#define _p m_pC3IEV

C3ImageEditorView::C3ImageEditorView(C3ImageEditor * pEditor)
	: QWidget(pEditor)
{
	_p = new C3ImageEditorViewPrivate();

	_p->pEditor = pEditor;
	_p->pVScrollBar = new QScrollBar(Qt::Vertical,this);
	_p->pHScrollBar = new QScrollBar(Qt::Horizontal,this);
	
	_p->pVScrollBar->setAutoFillBackground(true);
	_p->pHScrollBar->setAutoFillBackground(true);
	
	_p->iScrollBarSize = _p->pVScrollBar->sizeHint().width();

	QObject::connect(_p->pVScrollBar,SIGNAL(valueChanged(int)),this,SLOT(slotVerticalScrollBarValueChanged(int)));
	QObject::connect(_p->pHScrollBar,SIGNAL(valueChanged(int)),this,SLOT(slotHorizontalScrollBarValueChanged(int)));

	_p->iViewportWidth = 0;
	_p->iViewportHeight = 0;
	_p->fScale = 1.0;

	_p->iXOffset = 0;
	_p->iYOffset = 0;
	
	_p->bIgnoreScrollBarChanges = false;
	
	layout();
}

C3ImageEditorView::~C3ImageEditorView()
{
	delete _p;
}


bool C3ImageEditorView::open(const C3Link &oLink)
{
	QString szPath = oLink.path();
	
	if(szPath.isEmpty())
	{
		_p->pEditor->error(__tr("Empty path specified"));
		return false;
	}

	_p->oImage = QImage(szPath);
	if(_p->oImage.isNull())
	{
		_p->pEditor->error(__tr("Could not load image at '%1'").arg(szPath));
		return false;
	}

	layout();
	
	return true;
}

void C3ImageEditorView::slotVerticalScrollBarValueChanged(int iVal)
{
	if(_p->bIgnoreScrollBarChanges)
		return;

	_p->iYOffset = iVal;
	
	//qDebug("SETYOFFSET %d",iVal);

	update();
}

void C3ImageEditorView::slotHorizontalScrollBarValueChanged(int iVal)
{
	if(_p->bIgnoreScrollBarChanges)
		return;

	_p->iXOffset = iVal;

	//qDebug("SETXOFFSET %d",iVal);

	update();
}

void C3ImageEditorView::layout()
{
	_p->fScaledImageWidth = _p->oImage.width() * _p->fScale;
	_p->fScaledImageHeight = _p->oImage.height() * _p->fScale;

	_p->iContentsWidth = ::ceil(_p->fScaledImageWidth);
	_p->iContentsHeight = ::ceil(_p->fScaledImageHeight);

	int iInitialViewportWidth = _p->iViewportWidth;
	int iInitialViewportHeight = _p->iViewportHeight;

	_p->iViewportWidth = width() - 2;
	_p->iViewportHeight = height() - 2;

	bool bNeedVScroll = false;
	bool bNeedHScroll = false;
	
	if(_p->iContentsHeight > _p->iViewportHeight)
	{
		bNeedVScroll = true;
		_p->iViewportWidth -= _p->iScrollBarSize;
		if(_p->iContentsWidth > _p->iViewportWidth)
		{
			bNeedHScroll = true;
			_p->iViewportHeight -= _p->iScrollBarSize;
		}
	} else if(_p->iContentsWidth > _p->iViewportWidth)
	{
		bNeedHScroll = true;
		_p->iViewportHeight -= _p->iScrollBarSize;
		if(_p->iContentsHeight > _p->iViewportHeight)
		{
			bNeedVScroll = true;
			_p->iViewportWidth -= _p->iScrollBarSize;
		}
	}

	// Manage scrollbars
	if(bNeedHScroll)
	{
		_p->pHScrollBar->setGeometry(
				0,
				height() - _p->iScrollBarSize,
				bNeedVScroll ? (width() - _p->iScrollBarSize) : width(),
				_p->iScrollBarSize
			);

		int iMax = _p->iContentsWidth - _p->iViewportWidth;
		Q_ASSERT(iMax > 0);

		if(_p->iXOffset > iMax)
			_p->iXOffset = iMax;

		_p->bIgnoreScrollBarChanges = true;
		_p->pHScrollBar->setMaximum(iMax);
		_p->pHScrollBar->setPageStep(_p->iViewportWidth);
		_p->pHScrollBar->setValue(_p->iXOffset);
		_p->bIgnoreScrollBarChanges = false;

		if(!_p->pHScrollBar->isVisible())
			_p->pHScrollBar->show();
	} else {
		if(_p->pHScrollBar->isVisible())
			_p->pHScrollBar->hide();
	}

	if(bNeedVScroll)
	{
		_p->pVScrollBar->setGeometry(
				width() - _p->iScrollBarSize,
				0,
				_p->iScrollBarSize,
				bNeedHScroll ? (height() - _p->iScrollBarSize) : height()
			);

		int iMax =  _p->iContentsHeight - _p->iViewportHeight;
		Q_ASSERT(iMax > 0);

		if(_p->iYOffset > iMax)
			_p->iYOffset = iMax;

		_p->bIgnoreScrollBarChanges = true;
		_p->pVScrollBar->setMaximum(iMax);
		_p->pVScrollBar->setPageStep(_p->iViewportHeight);
		_p->pVScrollBar->setValue(_p->iYOffset);
		_p->bIgnoreScrollBarChanges = false;

		if(!_p->pVScrollBar->isVisible())
			_p->pVScrollBar->show();
	} else {
		if(_p->pVScrollBar->isVisible())
			_p->pVScrollBar->hide();
	}

	if(
		(iInitialViewportWidth != _p->iViewportWidth) ||
		(iInitialViewportHeight != _p->iViewportHeight)
	)
		update(); // shaded rect
}

void C3ImageEditorView::resizeEvent(QResizeEvent * e)
{
	layout();
	update();
}

void C3ImageEditorView::showEvent(QShowEvent * e)
{
	layout();
	QWidget::showEvent(e);
}

void C3ImageEditorView::paintEvent(QPaintEvent * e)
{
	// this is a paint event on the viewport!
	if(_p->oBackground.isNull())
		_p->oBackground = C3PixmapCache::instance()->pixmap("imageeditor/background.png");

	QRect rct = rect();

	QPainter p(this);

	p.drawTiledPixmap(1,1,_p->iViewportWidth,_p->iViewportHeight,_p->oBackground);

	qreal fVW = _p->iViewportWidth;
	qreal fVH = _p->iViewportHeight;

	qreal fX,fY;
	
	if(_p->fScaledImageWidth <= fVW)
		fX = (fVW - _p->fScaledImageWidth) / 2.0f;
	else
		fX = -_p->iXOffset;

	if(_p->fScaledImageHeight <= fVH)
		fY = (fVH - _p->fScaledImageHeight) / 2.0f;
	else
		fY = -_p->iYOffset;

	//qDebug("DRAW %f,%f,%f,%f",fX,fY,_p->fScaledImageWidth,_p->fScaledImageHeight);

	fX += 1.0;
	fY += 1.0;

	p.drawImage(QRectF(fX,fY,_p->fScaledImageWidth,_p->fScaledImageHeight),_p->oImage);

	qDrawShadePanel(&p,0,0,_p->iViewportWidth+2,_p->iViewportHeight+2,palette(),true);
}

