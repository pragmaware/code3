#ifndef _C3TextEditorCompletionWidget_h_
#define _C3TextEditorCompletionWidget_h_
//=============================================================================
//
//   File : C3TextEditorCompletionWidget.h
//   Creation Date : dom 08 nov 2015 04:39:48
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

#include <QWidget>
#include <QList>

#include "C3TextEditorCompletion.h"
#include "C3FileSnippetExtractorWorker.h"

class C3TextEditorCompletionWidgetPrivate;
class C3TextEditor;
class QListWidgetItem;

///
/// \class C3TextEditorCompletionWidget
/// \brief The C3TextEditorCompletionWidget class
///
/// This class...
///
class C3TextEditorCompletionWidget : public QWidget, public C3FileSnippetExtractorWorkerListener
{
	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditorCompletionWidget
	///
	C3TextEditorCompletionWidget(
			QWidget * pParent,
			C3TextEditor * pTextEditor
		);

	///
	/// Destroys the instance of C3TextEditorCompletionWidget
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditorCompletionWidget();

private:

	C3TextEditorCompletionWidgetPrivate * m_pC3TECW;

public:

	// Returns the hint for the height of the widget. Returns 0 if no choices are visible.
	int showChoices(
			const QString &szTextToComplete,
			QList<C3TextEditorCompletion *> * pCompletions,
			int * piMaxLength,
			const QString &szWideContextLeft,
			const QString &szWideContextRight,
			unsigned int uFlags // C3TextEditorMode::SymbolCompletionFlags
		);

	bool tabCompletion(const QString &szTextToComplete,QString &szResult,QString &szSuffix);
	bool enterCompletion(const QString &szTextToComplete,QString &szResult,QString &szSuffix);

	void keyUp();
	void keyDown();

protected:

	virtual void hideEvent(QHideEvent *e);

private:

	virtual void resizeEvent(QResizeEvent * e);

	void updatePalette();
	void updateDescription();

	virtual void onFileSnippetExtractWorkerTerminated(
			C3FileSnippetExtractorWorker * pWorker,
			QStringList * pLines,
			const QString &szError
		);

	void abortSnippetExtractorWorker();
	void startSnippetExtractorWorker(const QString &szPath,quint32 uFromLine,quint32 uToLine);
	void restartSnippetTimer();

	C3TextEditorCompletion * currentCompletion();

signals:
	// self inter-thread communication
	void interThreadSnippetReady();

private slots:
	void slotCurrentItemChanged(QListWidgetItem *cur,QListWidgetItem *prev);
	void slotSnippetTimerTimeout();
	void slotSnippetReady();

}; // class C3TextEditorCompletionWidget

#endif //!_C3TextEditorCompletionWidget_h_