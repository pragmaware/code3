#ifndef _C3TextEditor_h_
#define _C3TextEditor_h_
//=============================================================================
//
//   File : C3TextEditor.h
//   Creation Date : gio 22 ott 2015 22:22:57
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

#include "C3Editor.h"
#include "C3TextEditorCompletion.h"
#include "C3TextEditorSymbolInfo.h"

#include <QLinkedList>
#include <QStringList>

#include <functional>

class C3TextEditorPrivate;
class C3TextEditorFormatReader;
class C3TextEditorLine;
class C3TextEditorRowColumn;
class C3TextEditorRange;
class C3TextEditorUndo;
class C3TextEditorSymbolContext;

///
/// \class C3TextEditor
/// \brief The C3TextEditor class
///
/// This class...
///
class C3TextEditor : public C3Editor
{
	friend class C3TextEditorFormatReader;
	friend class C3TextEditorViewport;

	Q_OBJECT
public:

	///
	/// Creates an instance of C3TextEditor
	///
	C3TextEditor(C3EditorFactory * pFactory,QWidget * pParent);

	///
	/// Destroys the instance of C3TextEditor
	/// and frees all the relevant resources
	///
	virtual ~C3TextEditor();

private:

	C3TextEditorPrivate * m_pC3TE;

protected:

	virtual void resizeEvent(QResizeEvent * e);
	virtual void showEvent(QShowEvent * e);
	virtual void hideEvent(QHideEvent * e);
#if 0
	virtual void paintEvent(QPaintEvent * e);
#endif
	virtual void focusInEvent(QFocusEvent * e);
	virtual void focusOutEvent(QFocusEvent * e);

protected:

	// Called by C3TextEditorFormatReader upon exit (slave thread!)
	void readerTerminated(C3TextEditorFormatReader * pReader,const QString &szError,bool bContentWasAltered,const QString &szAlterationReason);
	void readerAppendLines(C3TextEditorFormatReader * pReader,QLinkedList<C3TextEditorLine *> &lLines);

signals:
	// internal signal connected to readerSlotWakeUp(), inter-thread communication
	void signalReaderWakeUp();

public:


	int rowToViewportYPosition(int iRow);
	qreal columnToAbsoluteXPosition(int iRow,int iColumn);
	bool absolutePointToRowColumn(const QPoint &pnt,C3TextEditorRowColumn &rc,bool bAllowOutOfTextArea = false);
	bool viewportPointToRowColumn(const QPoint &pnt,C3TextEditorRowColumn &rc,bool bAllowOutOfTextArea = false);
	// returns -1 if bAllowOutOfTextArea is false and the fXPos position is actually out of the text area.
	bool absoluteXPositionToColumn(int iRow,int * piColumn,qreal fXPos,bool bAllowOutOfTextArea = false);

	// This one places the cursor in the middle of the screen (if possible)
	bool cursorMoveTo(const QString &szRowColumnSpec);

	void cursorMoveTo(const C3TextEditorRowColumn &rc,bool bExtendSelection);
	void cursorLeft(bool bExtendSelection);
	void cursorRight(bool bExtendSelection);
	void cursorUp(bool bExtendSelection);
	void cursorDown(bool bExtendSelection);
	void cursorPageUp(bool bExtendSelection);
	void cursorPageDown(bool bExtendSelection);
	void cursorHome(bool bExtendSelection);
	void cursorEnd(bool bExtendSelection);
	void cursorFileEnd(bool bExtendSelection);

	int cursorRow() const;
	int cursorColumn() const;
	
	const C3TextEditorRowColumn & cursorPosition() const;

	void escape();

	void indent();
	void unindent();
	void tab();
	void backspace();
	void newline();
	void del();
	
	void setTabStopSize(int iTabSize);

	void setAdditionalLineSpacing(int iSpacing);
	
	// Inserts text at cursor position.
	// Also deletes the selection.
	// If you need to make repeated changes then it's probably better to call insertTextInternal()
	// and update the editor manually via beginVisualStateChange() / endVisualStateChange().
	void insertText(const QString &szText);

	// calls insertText() and eventually shows or hides the completion widget.
	// This is primairly meant to be called in response to direct keyboard input.
	void insertTypedText(const QString &szText);

	// Deletes the specified range.
	// Also clears the selection (does NOT delete it) and moves the cursor to the begin of the range.
	// If you need to make repeated changes then it's probably better to call deleteRangeInternal()
	// and update the editor manually via beginVisualStateChange() / endVisualStateChange().
	void deleteRange(const C3TextEditorRange &oRange);

	// The flags are the mode SymbolIdentificationFlags!
	bool jumpToLinkAtCursor(unsigned int uFlags = 0);
	void triggerSymbolIdentification(const C3TextEditorRowColumn &rc);
	void triggerSymbolIdentification(const C3TextEditorRowColumn &rc,unsigned int uFlags);

	QString getSelectionText();
	
	QString getRangeText(const C3TextEditorRange &oRange);
	QString getLineText(int iLine);
	
	int scrollRowOffset();
	void setScrollRowOffset(int iOffset);

	int scrollColumnOffset();
	void setScrollColumnOffset(int iOffset);
	
	class CursorAndScrollBarState
	{
	public:
		C3TextEditorRowColumn oCursorPos;
		int iXOffset;
		int iYOffset;
	};
	
	void saveCursorAndScrollBarState(CursorAndScrollBarState &oState);
	void restoreCursorAndScrollBarState(const CursorAndScrollBarState &oState);
	
	void selectRange(const C3TextEditorRange &oRange);

	const C3TextEditorRange & selectedRange();

	// Returns false if no reasonable token could be found.
	bool computeNearestTokenRange(int iRow,int iCol,C3TextEditorRange &oRange);

	// This attempts to extract a "word" token useful for completion.
	// Returns false if no reasonable token could be found.
	bool computeRangeOfWordBeforePosition(
			const C3TextEditorRowColumn &oPos,
			C3TextEditorRange &oRange,
			bool bIncludeDots = false,
			bool bIncludeSlashes = false
		);

	enum ComputeSymbolContextFlags
	{
		ExtendSymbolToRight = 1
	};

	bool computeSymbolContext(
			const C3TextEditorRowColumn &oPos,
			C3TextEditorSymbolContext &oContext,
			unsigned int uFlags = 0
		);

	C3EditorFormat * format();

	bool open(const C3Link &oLink);
	virtual void repeatedOpen(const C3Link &oLink);

	virtual bool newFile();

	virtual const QString & privateInfoText1() const;
	virtual const QString & privateInfoText2() const;
	
	virtual void fillViewMenu(QMenu * pMenu);
	virtual void fillEditMenu(QMenu * pMenu);
	virtual void fillFXMenu(QMenu * pMenu);
	
	void fillSymbolMenu(QMenu * pMenu);
	
	void fillCodeMenu(QMenu * pMenu);
	
	void setMode(const QString &szModeId);

	QString guessBestMode();
	QString guessBestMode(const QByteArray &data);
	QString getNextMode();
	QString guessBestFormat(const QByteArray &data);
	
	bool undoAvailable();

	virtual bool supportsFindOperations();
	virtual bool findOperationInitialSearchInSelection();
	virtual QString findOperationGetInitialSearchString();
	virtual bool findOperationRun(C3FindOperation * pOperation);
	virtual bool findOperationReplace(C3FindOperation * pOperation,const QString &szText);
	virtual void findOperationReplaceAllFinished(C3FindOperation * pOperation);

	virtual bool saveState(QJsonObject &oObject);
	virtual bool createLink(C3Link &link);

	void showContextMenu(const QPoint &oGlobalPos);

	// Returns true if a tab completion could be made and false otherwise
	bool tabCompletion();
	// Returns true if a enter completion could be made and false otherwise
	bool enterCompletion();
	
	// Move one completion up. returns false if the completion widget is not visible.
	bool completionUp();
	// Move one completion down. returns false if the completion widget is not visible.
	bool completionDown();


	// Show the completion widget.
	// oCompletedRange is the range of text being actually completed
	// which may be different from the range of text that will be actually
	// replaced by the completion (and is specified in each completion separately)
	void showCompletionWidget(
			const C3TextEditorRange &oCompletedRange,
			QList<C3TextEditorCompletion * > * pCompletions,
			unsigned int uFlags // C3TextEditorMode::SymbolCompletionFlags
		);

	void symbolCompletionFailed();

	void symbolIdentified(
			const C3TextEditorRange &oRange,
			QList<C3TextEditorSymbolInfo *> * pInfo
		);

	virtual QString relatedFilePath(bool bTryHarder = false);
	virtual bool canOpenRelatedFile();
	
	bool isLoading();

	// This can be called from slave threads!
	// You can pass a pbAborted pointer and set it to true concurrently.
	// You MUST call C3Workspace::lockEditor() on this editor identifier
	// before calling this function or you may end up with a dead pointer in the middle
	// of the call. You must then call unlockEditor().
	bool saveTemporaryFile(const QString &szFilePath,QString &szError,bool * pbAborted,const QString &szTrailer);

	virtual bool handleKeyEvent(QKeyEvent * e);

public slots:

	void goToStart();
	void goToLine();
	void goToEnd();

	void increaseTabStopSize();
	void decreaseTabStopSize();

	void increaseAdditionalLineSpacing();
	void decreaseAdditionalLineSpacing();

	virtual void openRelatedFile();

	void copy();
	void cut();
	void paste();
	
	void undo();

	void selectAll();
	
	void clearSelection();

	void cycleMode();
	void setBestGuessMode();

	// FX
	void fxRemoveTrailingCRs();
	void fxRemoveTrailingSpaces();
	void fxAddTrailingCRs();
	void fxEncodeBase64();
	void fxDecodeBase64();
	void fxEncodeHex();
	void fxDecodeHex();
	void fxTranscodeBase64ToHex();
	void fxTranscodeHexToBase64();
	void fxEncodeURL();
	void fxDecodeURL();
	void fxFormatJSON();
	void fxFormatXML();
	void fxReplaceLeadingSpacesWithTabs();
	void fxRemoveEmptyLines();
	void fxConvertToUpperCase();
	void fxConvertToLowerCase();
	void fxSortLinesAlphabetically();
	void fxSortLinesAlphabeticallyCaseInsensitive();

	void triggerSymbolCompletion();
	void triggerSymbolCompletionNoContextFilter();

	void triggerSymbolIdentification();
	void triggerSymbolIdentificationNoContextFilter();

	void grepCurrentSymbolInWorkspace();
	void grepCurrentSymbolInCurrentDirectory();

private slots:

	// internal slot connected to readerSignalWakeUp(), inter-thread communication
	void slotReaderWakeUp();

	void slotVerticalScrollBarValueChanged(int iVal);
	void slotHorizontalScrollBarValueChanged(int iVal);
	
	void slotMouseScrollTimerTimeout();

	void slotViewMenuModeSubmenuActionActivated();
	
	void slotCursorTimerTimeout();

private:

	virtual void workspacePropertiesChanged();
	virtual void generalSettingsChanged();


	void fxSelectionOrWholeBufferManipulationWithLambda(std::function<bool(QString &)> fnLambda);

	void layout();

	void ensureNoRunningRead();

	void resetInheritedSettings();
	void computeFontMetrics();
	void recomputeLineWidths();

	QString guessBestModeByNameOnly();

	void handleCursorMoved(bool bExtendSelection,unsigned int uEndVisualStateChangeFlags = 0);

	void repaintCursorRow();
	void repaintRow(int iRow);

	void killMouseScrollTimer();
	void startMouseScrollTimer();

	void beginVisualStateChange();
	
	enum EndVisualStateChangeFlags
	{
		// ForceFullRepaint should be set if the change affects rows different than the
		// cursor row before or after the change.
		// This is the case of massive modifications of multiple lines which don't affect selection.
		// If it's set to false then the editor *might* try to optimize the painting
		// operation by limiting it to the cursor rows only (though it will not optimize
		// when the selection, mark or maximum line width change).
		ForceFullRepaint = 1,
		// This is set only in the cursor timer callback. Do not use otherwise.
		DontRestartCursorTimer = 2,
		// This is set only in cursorUp() and cusorDown(). It's used to keep the "target" X position
		// of the cursor fixed when moving up or down.
		DontResetCursorUpDownX = 4,
		// This is set only in the functions that trigger or preserve completion
		DontHideCompletionWidget = 8,
		// Attempt to place the cursor in the middle of the screen
		PlaceCursorRowInMiddleOfScreen = 16
	};
	
	void endVisualStateChange(unsigned int uFlags = 0);

	// Deletes the specified range. Also clears the selection (does NOT delete it)
	// Does NOT update the editor. This call must be surrounded by calls to
	// beginVisualStateChange() / endVisualStateChange().
	void deleteRangeInternal(const C3TextEditorRange &oRange,bool bAddUndo = true);

	// Inserts text at cursor position.
	// Also deletes the selection.
	// Does NOT update the editor. This call must be surrounded by calls to
	// beginVisualStateChange() / endVisualStateChange().
	void insertTextInternal(const QString &szText,bool bAddUndo = true,bool bSelectInsertedText = false);

	virtual bool saveInternal(
			const QString &szPath,
			C3EditorFormat * pFormat,
			QString &szError
		);

	void pushUndo(C3TextEditorUndo * pUndo);
	C3TextEditorUndo * popUndo();
	
	void beginMultiUndo();
	void endMultiUndo();
	
	void undoInternal(C3TextEditorUndo * pUndo);
	
	void updateOrInvalidateMaximumLineWidthAfterChange(qreal fMaximumPreChangeLineWidth,qreal fMaximumPostChangeLineWidth);

	//void showOrHideCompletionWidget();
	
	// the flags are the mode context flags
	bool triggerSymbolIdentificationInternal(const C3TextEditorRowColumn &rc,bool bJumpToLinkAfterIdentification,unsigned int uFlags = 0);
	
	void fillEditMenuAddCopyCutAndPaste(QMenu * pMenu);
	void fillEditMenuAddUndo(QMenu * pMenu);

	QString relatedFilePathWithExtension(const QString &szExt,bool bTryHarder,void * pFindFileOptions);

	void markMatchingParenthesisInternal();

}; // class C3TextEditor

#endif //!_C3TextEditor_h_
