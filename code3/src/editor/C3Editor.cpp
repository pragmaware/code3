//=============================================================================
//
//   File : C3Editor.cpp
//   Creation Date : mar 20 ott 2015 01:03:26
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

#include "C3Editor.h"
#include "C3EditorFactory.h"
#include "C3EditorFormat.h"
#include "C3MainWindow.h"
#include "C3DockWorkspaceBrowser.h"
#include "C3Workspace.h"

#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QJsonObject>
#include <QKeyEvent>
#include <QShowEvent>

class C3EditorPrivate
{
public:
	C3EditorFactory * pFactory;
	QString szTitle;
	QString szPath;
	int iId;
	bool bModified;
	bool bReadOnly;
	C3DockWorkspaceBrowserEditorItem * pWorkspaceBrowserItem;
	qint64 iLastShowEventTime;
};

#define _p m_pC3E

static int g_iNextEditorId = 0;

C3Editor::C3Editor(C3EditorFactory * pFactory,QWidget * pParent,const QString &szAlertSourceName)
	: QWidget(pParent), C3AlertSource(szAlertSourceName), C3FindTarget(C3FindTarget::Editor)
{
	_p = new C3EditorPrivate();
	g_iNextEditorId++;
	_p->iId = g_iNextEditorId;
	_p->pFactory = pFactory;
	_p->szTitle = __utf8("<no title assigned>");
	_p->bModified = false;
	_p->bReadOnly = false;
	_p->pWorkspaceBrowserItem = NULL;
	_p->iLastShowEventTime = 0;
}

C3Editor::~C3Editor()
{
	delete _p;
}

bool C3Editor::saveState(QJsonObject &oObject)
{
	return false;
}

bool C3Editor::supportsFindOperations()
{
	return false;
}

QString C3Editor::findOperationGetInitialSearchString()
{
	return QString();
}

bool C3Editor::findOperationRun(C3FindOperation * pOperation)
{
	return false;
}

bool C3Editor::findOperationReplace(C3FindOperation * pOperation,const QString &szText)
{
	return false;
}


void C3Editor::fillViewMenu(QMenu *pMenu)
{
}

void C3Editor::fillEditMenu(QMenu *pMenu)
{
}

void C3Editor::fillFXMenu(QMenu *pMenu)
{
}

void C3Editor::setWorkspaceBrowserItem(C3DockWorkspaceBrowserEditorItem * it)
{
	_p->pWorkspaceBrowserItem = it;
}

C3DockWorkspaceBrowserEditorItem * C3Editor::workspaceBrowserItem()
{
	return _p->pWorkspaceBrowserItem;
}

int C3Editor::id() const
{
	return _p->iId;
}

C3EditorFactory * C3Editor::factory() const
{
	return _p->pFactory;
}

const QString & C3Editor::factoryId() const
{
	return _p->pFactory->id();
}

const QString & C3Editor::title() const
{
	return _p->szTitle;
}

void C3Editor::setTitle(const QString &szTitle)
{
	if(szTitle == _p->szTitle)
		return;
	
	_p->szTitle = szTitle;

	emit titleChanged(this);
}

const QString & C3Editor::path() const
{
	return _p->szPath;
}

QString C3Editor::fileName() const
{
	if(_p->szPath.isEmpty())
		return QString();
	int idx = _p->szPath.lastIndexOf(QDir::separator());
	if(idx < 0)
		return _p->szPath; // ????????
	return _p->szPath.mid(idx+1);
}

QString C3Editor::directory() const
{
	if(_p->szPath.isEmpty())
		return QString();
	int idx = _p->szPath.lastIndexOf(QDir::separator());
	if(idx < 0)
		return _p->szPath; // ????????
	return _p->szPath.mid(0,idx);
}

void C3Editor::setPath(const QString &szPath)
{
	if(szPath == _p->szPath)
		return;
	
	_p->szPath = szPath;

	emit pathChanged(this);
}

bool C3Editor::modified() const
{
	return _p->bModified;
}

void C3Editor::setModified(bool bModified)
{
	if(_p->bModified == bModified)
		return;
	_p->bModified = bModified;

	if(_p->bReadOnly && _p->bModified)
	{
		qDebug("WARNING: Setting a read only editor as modified!");
		setReadOnly(false);
	}

	emit modifiedStateChanged(this);
}

bool C3Editor::readOnly() const
{
	return _p->bReadOnly;
}

void C3Editor::setReadOnly(bool bReadOnly)
{
	if(_p->bReadOnly == bReadOnly)
		return;
	_p->bReadOnly = bReadOnly;

	if(_p->bReadOnly && _p->bModified)
	{
		qDebug("WARNING: Setting a modified editor as read only!");
		setModified(false);
	}

	emit readOnlyStateChanged(this);
}

bool C3Editor::closeFile(bool bCanAbort)
{
	if(!modified())
		return true;

	QMessageBox::StandardButtons oButtons = QMessageBox::Yes | QMessageBox::No;
	
	if(bCanAbort)
		oButtons |= QMessageBox::Cancel;

	QMessageBox::StandardButton eButton = QMessageBox::question(
			this,
			__tr("Save Changes?"),
			__tr("The file '%1' has been modified. Do you want to save the changes?").arg(path()),
			oButtons,
			QMessageBox::Yes
		);

	switch(eButton)
	{
		case QMessageBox::Yes:
			return save();
		break;
		case QMessageBox::No:
			// fall down
		break;
		default:
			// aborted
			return false;
		break;
	}

	return true;
}

bool C3Editor::save()
{
	if(!modified())
		return true;
	
	if(path().isEmpty())
		return saveAs();

	QString szError;

	if(saveInternal(path(),NULL,szError))
		return true;

	QMessageBox::StandardButton eButton = QMessageBox::critical(
			this,
			__tr("File Saving Failed"),
			__tr("Failed to save the file '%1': %2").arg(path()).arg(szError),
			QMessageBox::Cancel | QMessageBox::Retry
		);

	switch(eButton)
	{
		case QMessageBox::Retry:
			return saveAs();
		break;
		//case QMessageBox::Cancel:
		default:
			// fall down
		break;
	}

	return false;
}

bool C3Editor::saveAs()
{
	for(;;)
	{
		QHash<QString,C3EditorFormat *> & hFormats = _p->pFactory->formatMap();

		QStringList lNameFilters;
		
		foreach(C3EditorFormat * pFormat,hFormats)
			lNameFilters << pFormat->fileDialogNameFilter();

		lNameFilters.sort();

		QFileDialog dlg(
			this,
			__tr("Select File Name"),
			C3MainWindow::instance()->currentDirectory()
		);
		
		dlg.setOptions(QFileDialog::DontUseNativeDialog);
		dlg.setFileMode(QFileDialog::AnyFile);
		dlg.setAcceptMode(QFileDialog::AcceptSave);
		dlg.setNameFilters(lNameFilters);
		if(format())
			dlg.selectNameFilter(format()->fileDialogNameFilter());

		if(!path().isEmpty())
			dlg.selectFile(path());

		if(dlg.exec() != QDialog::Accepted)
			return false;

		QStringList lFiles = dlg.selectedFiles();
		if(lFiles.count() < 1)
			continue; // no files selected?

		QString szPath = lFiles.at(0);
		if(szPath.isEmpty())
			continue; // ???

		QString szNameFilter = dlg.selectedNameFilter();

		C3EditorFormat * pSelectedFormat = NULL;
	
		foreach(C3EditorFormat * pFormat,hFormats)
		{
			if(pFormat->fileDialogNameFilter() == szNameFilter)
			{
				pSelectedFormat = pFormat;
				break;
			}
		}


		QMessageBox::StandardButton eButton;

#if 0
		// This is done by QFileDialog now
		QFileInfo inf(szPath);

		if(inf.exists())
		{
			eButton = QMessageBox::critical(
					this,
					__tr("Confirm Overwrite"),
					__tr("File '%1' already exists. Do you want to overwrite it?").arg(szPath),
					QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
				);

			switch(eButton)
			{
				case QMessageBox::Cancel:
					return false;
				break;
				case QMessageBox::No:
					continue;
				break;
				//case QMessageBox::Yes:
				default:
					// continue
				break;
			}
		}
#endif

		QString szError;

		if(saveInternal(szPath,pSelectedFormat,szError))
			break;

		eButton = QMessageBox::critical(
				this,
				__tr("File Saving Failed"),
				__tr("Failed to save the file '%1': %2").arg(szPath).arg(szError),
				QMessageBox::Cancel | QMessageBox::Retry
			);

		switch(eButton)
		{
			case QMessageBox::Cancel:
				return false;
			break;
			//case QMessageBox::Retry:
			default:
				// continue
			break;
		}
		// Try again.
	}

	C3Workspace::currentWorkspace()->notifyRescanNeeded();

	return true;
}

void C3Editor::repeatedOpen(const C3Link &oLink)
{
	// does nothing
}

QString C3Editor::relatedFilePath()
{
	return QString();
}

bool C3Editor::canOpenRelatedFile()
{
	return false;
}

void C3Editor::openRelatedFile()
{
}

void C3Editor::workspacePropertiesChanged()
{
}

void C3Editor::generalSettingsChanged()
{
}

bool C3Editor::createLink(C3Link &link)
{
	link.setPath(path());
	link.setEditorFactoryId(_p->pFactory->id());
	return true;
}

bool C3Editor::handleKeyEvent(QKeyEvent * e)
{
	return false;
}


void C3Editor::showEvent(QShowEvent * e)
{
	QWidget::showEvent(e);
	_p->iLastShowEventTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

qint64 C3Editor::lastShowEventTime() const
{
	return _p->iLastShowEventTime;
}
