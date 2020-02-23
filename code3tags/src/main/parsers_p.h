/*
*   Copyright (c) 2000-2003, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License version 2 or (at your option) any later version.
*
*   External interface to all language parsing modules.
*
*   To add a new language parser, you need only modify this single input
*   file to add the name of the parser definition function.
*/
#ifndef CTAGS_MAIN_PARSERS_H
#define CTAGS_MAIN_PARSERS_H

#ifdef HAVE_LIBXML
#define XML_PARSER_LIST \
	DbusIntrospectParser, \
	GladeParser,  \
	Maven2Parser, \
	PlistXMLParser, \
	RelaxNGParser, \
	SvgParser, \
	XmlParser, \
	XsltParser
#else
#define XML_PARSER_LIST
#endif

#ifdef HAVE_LIBYAML
#define YAML_PARSER_LIST						\
	YamlParser,									\
	AnsiblePlaybookParser
#else
#define YAML_PARSER_LIST
#endif

#ifdef HAVE_PACKCC
#define PEG_PARSER_LIST						\
	VarlinkParser
#else
#define PEG_PARSER_LIST
#endif

/* Add the name of any new parser definition function here */
#define PARSER_LIST \
	AsmParser, \
	AspParser, \
	BasicParser, \
	ClojureParser, \
	CMakeParser, \
	CParser, \
	CppParser, \
	CPreProParser, \
	CssParser, \
	CsharpParser, \
	CtagsParser, \
	CobolParser, \
	CUDAParser, \
	DParser, \
	DiffParser, \
	FortranParser, \
	GoParser, \
	HtmlParser, \
	JavaParser, \
	JavaPropertiesParser, \
	JavaScriptParser, \
	JsonParser, \
	ObjcParser, \
	OldCppParser, \
	OldCParser, \
	PascalParser, \
	PerlParser, \
	Perl6Parser, \
	PhpParser, \
	PythonParser, \
	QtMocParser, \
	RParser, \
	RubyParser, \
	RustParser, \
	ShParser, \
	TclParser, \
	TypeScriptParser, \
	VeraParser, \
	VerilogParser, \
	SystemVerilogParser, \
	VhdlParser

#endif  /* CTAGS_MAIN_PARSERS_H */
