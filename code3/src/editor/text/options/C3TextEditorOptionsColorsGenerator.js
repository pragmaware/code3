// Run with node.

var aDescriptors = [
	{
		id: "marginTextColor",
		variable: "oMarginTextColor",
		name: "Margin Text",
		description: "Color of inactive line margin text",
		red: 78,
		green: 95,
		blue: 68
	},
	{
		id: "marginBackgroundColor",
		variable: "oMarginBackgroundColor",
		name: "Margin Background",
		description: "Background color of inactive line margin",
		red: 30,
		green: 41,
		blue: 30
	},
	{
		id: "marginActiveTextColor",
		variable: "oMarginActiveTextColor",
		name: "Active Margin Text",
		description: "Color of active line margin text",
		red: 110,
		green: 119,
		blue: 63
	},
	{
		id: "marginActiveBackgroundColor",
		variable: "oMarginActiveBackgroundColor",
		name: "Active Margin Background",
		description: "Background color of active line margin",
		red: 49,
		green: 66,
		blue: 49
	},
	
	{
		id: "cursorColor",
		variable: "oCursorColor",
		name: "Cursor",
		description: "Color of the cursor",
		red: 255,
		green: 0,
		blue: 0
	},

	{
		id: "textBackgroundColor",
		variable: "oTextBackgroundColor",
		name: "Text Background",
		description: "Background color of the text area of the editor",
		red: 0,
		green: 0,
		blue: 0
	},
	{
		id: "editorBackgroundColor",
		variable: "oEditorBackgroundColor",
		name: "Editor Background",
		description: "Background color of the non-text area of the editor",
		red: 10,
		green: 10,
		blue: 10
	},
	{
		id: "markBackgroundColor",
		variable: "oMarkBackgroundColor",
		name: "Mark Background",
		description: "Background color of the marked text lines",
		red: 20,
		green: 0,
		blue: 30
	},
	{
		id: "currentLineBackgroundColor",
		variable: "oCurrentLineBackgroundColor",
		name: "Current Line Background",
		description: "Background color of the current line",
		red: 2,
		green: 2,
		blue: 2
	},

	{
		id: "textColor",
		variable: "oTextColor",
		name: "Default Text",
		description: "The color used for text when no other category matches",
		red: 90,
		green: 255,
		blue: 0
	},
	{
		id: "tabColor",
		variable: "oTabColor",
		name: "Tab Marker",
		description: "Color of the tab markers",
		red: 40,
		green: 40,
		blue: 40
	},

	{
		id: "errorTextColor",
		variable: "oErrorTextColor",
		name: "Errors",
		description: "Color of errors detected at syntax highlighting stage",
		red: 255,
		green: 0,
		blue: 0
	},
	{
		id: "stringTextColor",
		variable: "oStringTextColor",
		name: "Strings",
		description: "Color for strings (text sequences between quotes)",
		red: 99,
		green: 172,
		blue: 255
	},
	{
		id: "unterminatedStringTextColor",
		variable: "oUnterminatedStringTextColor",
		name: "Unterminated Strings",
		description: "Text color marking a string that is not properly terminated",
		red: 188,
		green: 53,
		blue: 244
	},

	{
		id: "numberTextColor",
		variable: "oNumberTextColor",
		name: "Numbers",
		description: "Color for numbers",
		red: 190,
		green: 200,
		blue: 255
	},
	{
		id: "singleLineCommentTextColor",
		variable: "oSingleLineCommentTextColor",
		name: "Single Line Comments",
		description: "Color for single line comments. Usually starting with # or //",
		red: 20,
		green: 180,
		blue: 0
	},
	{
		id: "multiLineCommentTextColor",
		variable: "oMultiLineCommentTextColor",
		name: "Multi Line Comments",
		description: "Color for multi line comments. Usually starting with /* or <!--",
		red: 40,
		green: 150,
		blue: 0
	},
	{
		id: "operatorTextColor",
		variable: "oOperatorTextColor",
		name: "Operators",
		description: "Color for generic operators like +, -, * etc...",
		red: 150,
		green: 150,
		blue: 40
	},
	{
		id: "markupTagTextColor",
		variable: "oMarkupTagTextColor",
		name: "Markup Tags",
		description: "Color for markup tags. For example <...> in HTML/XML",
		red: 171,
		green: 97,
		blue: 188
	},
	{
		id: "markupTagNameTextColor",
		variable: "oMarkupTagNameTextColor",
		name: "Markup Tag Names",
		description: "Color for the names of the tags. For example <name> in HTML/XML",
		red: 171,
		green: 140,
		blue: 231
	},
	{
		id: "variableTextColor",
		variable: "oVariableTextColor",
		name: "Variables",
		description: "Color of variables, for the languages that have an explicit variable marker character (like $ in php)",
		red: 230,
		green: 210,
		blue: 25
	},
	{
		id: "variableInStringTextColor",
		variable: "oVariableInStringTextColor",
		name: "Variable in String",
		description: "Color of variables that appear inside a string. CMake and bash have this.",
		red: 177,
		green: 161,
		blue: 255
	},

	{
		id: "annotationTextColor",
		variable: "oAnnotationTextColor",
		name: "Annotations",
		description: "This is for java @annotations and similar.",
		red: 210,
		green: 130,
		blue: 200
	},

	{
		id: "keywordTextColor",
		variable: "oKeywordTextColor",
		name: "Keywords",
		description: "Builtin language keywords. while/if/function etc...",
		red: 130,
		green: 120,
		blue: 130
	},
	{
		id: "userKeywordTextColor",
		variable: "oUserKeywordTextColor",
		name: "User Keywords",
		description: "Color for user defined keywords",
		red: 130,
		green: 140,
		blue: 150
	},
	{
		id: "builtinBuiltinTypeTextColor",
		variable: "oBuiltinTypeTextColor",
		name: "Types",
		description: "Color for simple language builtin types. int/unsigned int/boolean etc...",
		red: 140,
		green: 140,
		blue: 110
	},
	{
		id: "userBuiltinTypeTextColor",
		variable: "oUserTypeTextColor",
		name: "User Types",
		description: "Color for user defined language types.",
		red: 150,
		green: 140,
		blue: 100
	},
	{
		id: "builtinObjectTextColor",
		variable: "oBuiltinObjectTextColor",
		name: "Objects",
		description: "Color for builtin language objects. Math and Number in javascript, for example.",
		red: 200,
		green: 200,
		blue: 100
	},
	{
		id: "userBuiltinObjectTextColor",
		variable: "oUserObjectTextColor",
		name: "User Objects",
		description: "Color for user defined objects.",
		red: 255,
		green: 255,
		blue: 50
	},
	{
		id: "codeBlockDelimiterTextColor",
		variable: "oCodeBlockDelimiterTextColor",
		name: "Code Block Delimiters",
		description: "Code block delimiters. Usually brackets {}",
		red: 255,
		green: 30,
		blue: 30
	},
	{
		id: "parenthesisTextColor",
		variable: "oParenthesisTextColor",
		name: "Parenthesis",
		description: "Color for round parentheses ().",
		red: 160,
		green: 160,
		blue: 0
	},
	{
		id: "arrayDelimiterTextColor",
		variable: "oArrayDelimiterTextColor",
		name: "Array Delimiters",
		description: "Color of array delimiters. Usually [].",
		red: 190,
		green: 160,
		blue: 0
	},
	{
		id: "preprocessorTextColor",
		variable: "oPreprocessorTextColor",
		name: "Preprocessor Statements",
		description: "Color for preprocessor statements. Mainly for C/C++",
		red: 255,
		green: 255,
		blue: 255
	},
	{
		id: "includeFileTextColor",
		variable: "oIncludeFileTextColor",
		name: "Include Files",
		description: "Color for include files. Mainly for C/C++",
		red: 200,
		green: 200,
		blue: 200
	},
	{
		id: "underscoreIdentifierTextColor",
		variable: "oUnderscoreIdentifierTextColor",
		name: "Underscore Identifiers",
		description: "Color for identifiers that start with an underscore. Usually reserved words/macros.",
		red: 255,
		green: 80,
		blue: 255
	},
	{
		id: "mUnderscoreIdentifierTextColor",
		variable: "oMUnderscoreIdentifierTextColor",
		name: "M-Underscore Identifiers",
		description: "Color for identifiers that start with m_*. Usually member variables.",
		red: 190,
		green: 170,
		blue: 80
	},
	{
		id: "gUnderscoreIdentifierTextColor",
		variable: "oGUnderscoreIdentifierTextColor",
		name: "G-Underscore Identifiers",
		description: "Color for identifiers that start with g_*. Usually global variables.",
		red: 210,
		green: 170,
		blue: 80
	},
	{
		id: "userCustom1TextColor",
		variable: "oUserCustom1TextColor",
		name: "User Custom 1",
		description: "Color for user-defined identifiers in section custom 1",
		red: 255,
		green: 200,
		blue: 0
	},
	{
		id: "userCustom2TextColor",
		variable: "oUserCustom2TextColor",
		name: "User Custom 2",
		description: "Color for user-defined identifiers in section custom 2",
		red: 200,
		green: 200,
		blue: 80
	},
	{
		id: "userCustom3TextColor",
		variable: "oUserCustom3TextColor",
		name: "User Custom 3",
		description: "Color for user-defined identifiers in section custom 3",
		red: 210,
		green: 200,
		blue: 100
	},
	{
		id: "regExpTextColor",
		variable: "oRegExpTextColor",
		name: "Regular Expressions",
		description: "Regular expressions when part of the language. Javascript has this.",
		red: 140,
		green: 142,
		blue: 245
	},
	{
		id: "fixmeToDoTextColor",
		variable: "oFixmeTodoTextColor",
		name: "FIXME/TODO Markers",
		description: "Color for FIXME and TODO keywords in comments",
		red: 255,
		green: 150,
		blue: 12
	},
	{
		id: "heredocTextColor",
		variable: "oHeredocTextColor",
		name: "Heredocs",
		description: "Color for Heredocs (shell scripts)",
		red: 140,
		green: 162,
		blue: 235
	},
	{
		id: "variableInHeredocTextColor",
		variable: "oVariableInHeredocTextColor",
		name: "Variable in Heredoc",
		description: "Color of variables that appear inside a heredoc.",
		red: 207,
		green: 151,
		blue: 235
	},

];

var l = aDescriptors.length;

var decls = "";
var loads = "";
var saves = "";
var editors = "";
var copies = "";

for(var i=0;i<l;i++)
{
	var d = aDescriptors[i];
	
	decls += "	QColor " + d.variable + "; // " + d.name + "\n";
	
	loads += "	LOAD_COLOR(" + d.variable + ",\"" + d.id + "\"," + d.red + "," + d.green + "," + d.blue + ");\n";
	
	saves += "	SAVE_COLOR(" + d.variable + ",\"" + d.id + "\");\n";
	
	copies += "	" + d.variable + " = src." + d.variable + ";\n";

	editors += "	ed = w->addColorEditor(__tr(\"" + d.name + "\"),&(_p->oColors." + d.variable + "));\n";
	editors += "	_p->lEditors.append(ed);\n";
}


var header = "" +
"#ifndef _C3TextEditorOptionsColors_h_\n" +
"#define _C3TextEditorOptionsColors_h_\n" +
"//=============================================================================\n" +
"//\n" +
"// File : C3TextEditorOptionsColors.h\n" +
"// Creation date : sab 24 ott 2015 04:54:22\n" +
"// Project : Code 3\n" +
"// Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>\n" +
"//\n" +
"//=============================================================================\n" +
"\n" +
"\n" +
"#include \"c3_config.h\"\n" +
"\n" +
"class QSettings;\n" +
"\n" +
"#include <QFont>\n" +
"#include <QColor>\n" +
"\n" +
"class C3TextEditorOptionsColors\n" +
"{\n" +
"public:\n" +
"\n" +
decls +
"\n" +
"public:\n" +
"	void loadColors(QSettings &s);\n" +
"	void saveColors(QSettings &s);\n" +
"	\n" +
"	void copyColors(const C3TextEditorOptionsColors &src);\n" +
"\n" +
"};\n" +
"\n" +
"#endif //!_C3TextEditorOptionsColors_h_\n";

var cpp = "" +
"//=============================================================================\n" +
"//\n" +
"// File : C3TextEditorOptionsColors.cpp\n" +
"// Creation date : sab 24 ott 2015 04:54:22\n" +
"// Project : Code 3\n" +
"// Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>\n" +
"//\n" +
"//=============================================================================\n" +
"\n" +
"#include \"C3TextEditorOptions.h\"\n" +
"\n" +
"#include <QSettings>\n" +
"\n" +
"void C3TextEditorOptionsColors::loadColors(QSettings &s)\n" +
"{\n" +
"\n" +
"#define LOAD_COLOR(_oObject,_szOptionName,_iDefaultR,_iDefaultG,_iDefaultB) \\\n" +
"	do { \\\n" +
"		QVariant v = s.value(_szOptionName); \\\n" +
"		if((!v.isNull()) && v.canConvert<QColor>()) \\\n" +
"		{ \\\n" +
"			_oObject = v.value<QColor>(); \\\n" +
"		} else { \\\n" +
"			_oObject = QColor(_iDefaultR,_iDefaultG,_iDefaultB); \\\n" +
"		} \\\n" +
"	} while(0)\n" +
"\n" +
loads +
"\n" +
"}\n" +
"\n" +
"void C3TextEditorOptionsColors::saveColors(QSettings &s)\n" +
"{\n" +
"\n" +
"#define SAVE_COLOR(_oObject,_szOptionName) \\\n" +
"	do { \\\n" +
"		s.setValue(_szOptionName,QVariant(_oObject)); \\\n" +
"	} while(0)\n" +
"\n" +
saves +
"\n" +
"}\n" +
"\n" +
"void C3TextEditorOptionsColors::copyColors(const C3TextEditorOptionsColors &src)\n" +
"{\n" +
"\n" +
copies +
"\n" +
"}\n" +
"\n";

var optionsWidgetHeader = "" +
"#ifndef _C3TextEditorOptionsWidgetColors_h_\n" +
"#define _C3TextEditorOptionsWidgetColors_h_\n" +
"//=============================================================================\n" +
"//\n" +
"// File : C3TextEditorOptionsWidgetColors.h\n" +
"// Creation date : mer 11 nov 2015 01:44:42\n" +
"// Project : Code 3\n" +
"// Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>\n" +
"//\n" +
"//=============================================================================\n" +
"\n" +
"\n" +
"#include \"c3_config.h\"\n" +
"\n" +
"#include \"C3OptionsWidget.h\"\n" +
"\n" +
"class C3TextEditorOptions;\n" +
"class C3TextEditorOptionsWidgetColorsPrivate;\n" +
"\n" +
"///\n" +
"/// \class C3TextEditorOptionsWidgetColors\n" +
"/// \brief The C3TextEditorOptionsWidgetColors class\n" +
"///\n" +
"/// This class...\n" +
"///\n" +
"class C3TextEditorOptionsWidgetColors : public C3OptionsWidget\n" +
"{\n" +
"	Q_OBJECT\n" +
"public:\n" +
"\n" +
"	///\n" +
"	/// Creates an instance of C3TextEditorOptionsWidgetColors\n" +
"	///\n" +
"	C3TextEditorOptionsWidgetColors(const QString &szLabel,QWidget * pParent,C3TextEditorOptions * pOptions);\n" +
"\n" +
"	///\n" +
"	/// Destroys the instance of C3TextEditorOptionsWidgetColors\n" +
"	/// and frees all the relevant resources\n" +
"	///\n" +
"	virtual ~C3TextEditorOptionsWidgetColors();\n" +
"\n" +
"private:\n" +
"\n" +
"	C3TextEditorOptionsWidgetColorsPrivate * m_pC3SWU;\n" +
"\n" +
"protected:\n" +
"\n" +
"	virtual bool commit();\n" +
"	void resetEditors();\n" +
"\n" +
"protected slots:\n" +
"\n" +
"	void saveToFile();\n" +
"	void loadFromFile();\n" +
"\n" +
"}; // class C3TextEditorOptionsWidgetColors\n" +
"\n" +
"#endif //!_C3TextEditorOptionsWidgetColors_h_\n";



var optionsWidgetCpp = "" +
"//=============================================================================\n" +
"//\n" +
"// File : C3TextEditorOptionsWidgetColors.cpp\n" +
"// Creation date : mer 11 nov 2015 01:44:42\n" +
"// Project : Code 3\n" +
"// Author : Szymon Tomasz Stefanek <sts at pragmaware dot net>\n" +
"//\n" +
"//=============================================================================\n" +
"\n" +
"#include \"C3TextEditorOptionsWidgetColors.h\"\n" +
"\n" +
"#include \"C3Settings.h\"\n" +
"#include \"C3OptionEditorScrollArea.h\"\n" +
"#include \"C3OptionEditorColor.h\"\n" +
"#include \"C3OptionEditorGroup.h\"\n" +
"#include \"C3TextEditorOptions.h\"\n" +
"\n" +
"#include <QGridLayout>\n" +
"#include <QGroupBox>\n" +
"#include <QPushButton>\n" +
"#include <QFileDialog>\n" +
"#include <QList>\n" +
"#include <QSettings>\n" +
"#include <QMessageBox>\n" +
"\n" +
"class C3TextEditorOptionsWidgetColorsPrivate\n" +
"{\n" +
"public:\n" +
"	C3TextEditorOptions * pOptions;\n" +
"	C3TextEditorOptionsColors oColors;\n" +
"	QList<C3OptionEditorColor *> lEditors;\n" +
"\n" +
"};\n" +
"\n" +
"#define _p m_pC3SWU\n" +
"\n" +
"C3TextEditorOptionsWidgetColors::C3TextEditorOptionsWidgetColors(const QString &szLabel,QWidget * pParent,C3TextEditorOptions * pOptions)\n" +
"	: C3OptionsWidget(szLabel,pParent)\n" +
"{\n" +
"	_p = new C3TextEditorOptionsWidgetColorsPrivate();\n" +
"	_p->pOptions = pOptions;\n" +
"	_p->oColors.copyColors(*pOptions);\n" +
"\n" +
"	C3OptionEditorScrollArea * pArea = addScrollArea();\n" +
"\n" +
"	C3OptionsWidget * w = pArea->childOptionsWidget();\n" +
"	w->gridLayout()->setMargin(10);\n" +
"\n" +
"	C3OptionEditorColor * ed;\n" +
"\n" +
editors +
"\n" +
"	w->addSpaceEater();\n" +
"\n" +
"	C3OptionEditorGroup * gb = addGroup(__tr(\"Import/Export Color Set\"));\n" +
"\n" +
"	QPushButton * pb = gb->childOptionsWidget()->addPushButton(__tr(\"Import from File...\"),0,0,1,1);\n" +
"	QObject::connect(pb,SIGNAL(clicked()),this,SLOT(loadFromFile()));\n" +
"\n" +
"	pb = gb->childOptionsWidget()->addPushButton(__tr(\"Export to File...\"),0,1,1,1);\n" +
"	QObject::connect(pb,SIGNAL(clicked()),this,SLOT(saveToFile()));\n" +
"}\n" +
"\n" +
"C3TextEditorOptionsWidgetColors::~C3TextEditorOptionsWidgetColors()\n" +
"{\n" +
"	delete _p;\n" +
"}\n" +
"\n" +
"bool C3TextEditorOptionsWidgetColors::commit()\n" +
"{\n" +
"	if(!C3OptionsWidget::commit())\n" +
"		return false;\n" +
"	_p->pOptions->copyColors(_p->oColors);\n" +
"	return true;\n" +
"}\n" +
"\n" +
"void C3TextEditorOptionsWidgetColors::resetEditors()\n" +
"{\n" +
"	Q_FOREACH(C3OptionEditorColor * ed,_p->lEditors)\n" +
"		ed->resetValue();\n" +
"}\n" +
"\n" +
"void C3TextEditorOptionsWidgetColors::saveToFile()\n" +
"{\n" +
"	QString szFileName = QFileDialog::getSaveFileName(this,__tr(\"Select File to Save\"),\"colors.c3colors\",\"Code-3 Color Sets (*.c3colors)\");\n" +
"	if(szFileName.isEmpty())\n" +
"		return;\n" +
"	QSettings oSettings(szFileName,QSettings::IniFormat);\n" +
"	_p->oColors.saveColors(oSettings);\n" +
"	oSettings.sync();\n" +
"	if(oSettings.status() != QSettings::NoError)\n" +
"		QMessageBox::warning(this,__tr(\"Save Failed\"),__tr(\"Failed to save the file\"));\n" +
"}\n" +
"\n" +
"void C3TextEditorOptionsWidgetColors::loadFromFile()\n" +
"{\n" +
"	QString szFileName = QFileDialog::getOpenFileName(this,__tr(\"Select File to Load\"),QString(),\"Code-3 Color Sets (*.c3colors)\");\n" +
"	if(szFileName.isEmpty())\n" +
"		return;\n" +
"	QSettings oSettings(szFileName,QSettings::IniFormat);\n" +
"	if(oSettings.status() != QSettings::NoError)\n" +
"	{\n" +
"		QMessageBox::warning(this,__tr(\"Load Failed\"),__tr(\"Failed to load the file\"));\n" +
"		return;\n" +
"	}\n" +
"	_p->oColors.loadColors(oSettings);\n" +
"	resetEditors();\n" +
"}\n" +
"\n";



var fs = require('fs');

fs.writeFile('./C3TextEditorOptionsColors.h',header,function(err)
		{
			if (err) throw err;
		}
	);

fs.writeFile('./C3TextEditorOptionsColors.cpp',cpp,function(err)
		{
			if (err) throw err;
		}
	);

fs.writeFile('./C3TextEditorOptionsWidgetColors.h',optionsWidgetHeader,function(err)
		{
			if (err) throw err;
		}
	);

fs.writeFile('./C3TextEditorOptionsWidgetColors.cpp',optionsWidgetCpp,function(err)
		{
			if (err) throw err;
		}
	);
