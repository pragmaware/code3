//=============================================================================
//
//   File : C3TextEditorModeParserSQL.cpp
//   Creation Date : sab 31 ott 2015 23:10:30
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

#include "C3TextEditorModeParserSQL.h"

#include "C3TextEditorOptions.h"


C3TextEditorModeParserSQL::C3TextEditorModeParserSQL(C3TextEditorModeParserState * pParserState)
	: C3TextEditorModeParser(pParserState)
{
}

C3TextEditorModeParserSQL::~C3TextEditorModeParserSQL()
{
}

void C3TextEditorModeParserSQL::buildIdentifierColorHash()
{
#define IDENTIFIER(_szIdentifier,_oColor) \
	m_hIdentifierColorHash.insert(__utf8(_szIdentifier),&(m_p->pCoreData->pOptions->_oColor))

#define KEYWORD(_szKeyword) IDENTIFIER(_szKeyword,oKeywordTextColor)
//#define BUILTIN_OBJECT(_szKeyword) IDENTIFIER(_szKeyword,oBuiltinObjectTextColor)

	// FIXME: Make this shared!
	KEYWORD("a");
	KEYWORD("abort");
	KEYWORD("abs");
	KEYWORD("absolute");
	KEYWORD("access");
	KEYWORD("action");
	KEYWORD("ada");
	KEYWORD("add");
	KEYWORD("admin");
	KEYWORD("after");
	KEYWORD("aggregate");
	KEYWORD("alias");
	KEYWORD("all");
	KEYWORD("allocate");
	KEYWORD("also");
	KEYWORD("alter");
	KEYWORD("always");
	KEYWORD("analyse");
	KEYWORD("analyze");
	KEYWORD("and");
	KEYWORD("any");
	KEYWORD("are");
	KEYWORD("array");
	KEYWORD("as");
	KEYWORD("asc");
	KEYWORD("asensitive");
	KEYWORD("assertion");
	KEYWORD("assignment");
	KEYWORD("asymmetric");
	KEYWORD("at");
	KEYWORD("atomic");
	KEYWORD("attribute");
	KEYWORD("attributes");
	KEYWORD("audit");
	KEYWORD("authorization");
	KEYWORD("auto_increment");
	KEYWORD("avg");
	KEYWORD("avg_row_length");
	KEYWORD("backup");
	KEYWORD("backward");
	KEYWORD("before");
	KEYWORD("begin");
	KEYWORD("bernoulli");
	KEYWORD("between");
	KEYWORD("bigint");
	KEYWORD("binary");
	KEYWORD("bit");
	KEYWORD("bit_length");
	KEYWORD("bitvar");
	KEYWORD("blob");
	KEYWORD("bool");
	KEYWORD("boolean");
	KEYWORD("both");
	KEYWORD("breadth");
	KEYWORD("break");
	KEYWORD("browse");
	KEYWORD("bulk");
	KEYWORD("by");
	KEYWORD("c");
	KEYWORD("cache");
	KEYWORD("call");
	KEYWORD("called");
	KEYWORD("cardinality");
	KEYWORD("cascade");
	KEYWORD("cascaded");
	KEYWORD("case");
	KEYWORD("cast");
	KEYWORD("catalog");
	KEYWORD("catalog_name");
	KEYWORD("ceil");
	KEYWORD("ceiling");
	KEYWORD("chain");
	KEYWORD("change");
	KEYWORD("char");
	KEYWORD("char_length");
	KEYWORD("character");
	KEYWORD("character_length");
	KEYWORD("character_set_catalog");
	KEYWORD("character_set_name");
	KEYWORD("character_set_schema");
	KEYWORD("characteristics");
	KEYWORD("characters");
	KEYWORD("check");
	KEYWORD("checked");
	KEYWORD("checkpoint");
	KEYWORD("checksum");
	KEYWORD("class");
	KEYWORD("class_origin");
	KEYWORD("clob");
	KEYWORD("close");
	KEYWORD("cluster");
	KEYWORD("clustered");
	KEYWORD("coalesce");
	KEYWORD("cobol");
	KEYWORD("collate");
	KEYWORD("collation");
	KEYWORD("collation_catalog");
	KEYWORD("collation_name");
	KEYWORD("collation_schema");
	KEYWORD("collect");
	KEYWORD("column");
	KEYWORD("column_name");
	KEYWORD("columns");
	KEYWORD("command_function");
	KEYWORD("command_function_code");
	KEYWORD("comment");
	KEYWORD("commit");
	KEYWORD("committed");
	KEYWORD("completion");
	KEYWORD("compress");
	KEYWORD("compute");
	KEYWORD("condition");
	KEYWORD("condition_number");
	KEYWORD("connect");
	KEYWORD("connection");
	KEYWORD("connection_name");
	KEYWORD("constraint");
	KEYWORD("constraint_catalog");
	KEYWORD("constraint_name");
	KEYWORD("constraint_schema");
	KEYWORD("constraints");
	KEYWORD("constructor");
	KEYWORD("contains");
	KEYWORD("containstable");
	KEYWORD("continue");
	KEYWORD("conversion");
	KEYWORD("convert");
	KEYWORD("copy");
	KEYWORD("corr");
	KEYWORD("corresponding");
	KEYWORD("count");
	KEYWORD("covar_pop");
	KEYWORD("covar_samp");
	KEYWORD("create");
	KEYWORD("createdb");
	KEYWORD("createrole");
	KEYWORD("createuser");
	KEYWORD("cross");
	KEYWORD("csv");
	KEYWORD("cube");
	KEYWORD("cume_dist");
	KEYWORD("current");
	KEYWORD("current_date");
	KEYWORD("current_default_transform_group");
	KEYWORD("current_path");
	KEYWORD("current_role");
	KEYWORD("current_time");
	KEYWORD("current_timestamp");
	KEYWORD("current_transform_group_for_type");
	KEYWORD("current_user");
	KEYWORD("cursor");
	KEYWORD("cursor_name");
	KEYWORD("cycle");
	KEYWORD("data");
	KEYWORD("database");
	KEYWORD("databases");
	KEYWORD("date");
	KEYWORD("datetime");
	KEYWORD("datetime_interval_code");
	KEYWORD("datetime_interval_precision");
	KEYWORD("day");
	KEYWORD("day_hour");
	KEYWORD("day_microsecond");
	KEYWORD("day_minute");
	KEYWORD("day_second");
	KEYWORD("dayofmonth");
	KEYWORD("dayofweek");
	KEYWORD("dayofyear");
	KEYWORD("dbcc");
	KEYWORD("deallocate");
	KEYWORD("dec");
	KEYWORD("decimal");
	KEYWORD("declare");
	KEYWORD("default");
	KEYWORD("defaults");
	KEYWORD("deferrable");
	KEYWORD("deferred");
	KEYWORD("defined");
	KEYWORD("definer");
	KEYWORD("degree");
	KEYWORD("delay_key_write");
	KEYWORD("delayed");
	KEYWORD("delete");
	KEYWORD("delimiter");
	KEYWORD("delimiters");
	KEYWORD("dense_rank");
	KEYWORD("deny");
	KEYWORD("depth");
	KEYWORD("deref");
	KEYWORD("derived");
	KEYWORD("desc");
	KEYWORD("describe");
	KEYWORD("descriptor");
	KEYWORD("destroy");
	KEYWORD("destructor");
	KEYWORD("deterministic");
	KEYWORD("diagnostics");
	KEYWORD("dictionary");
	KEYWORD("disable");
	KEYWORD("disconnect");
	KEYWORD("disk");
	KEYWORD("dispatch");
	KEYWORD("distinct");
	KEYWORD("distinctrow");
	KEYWORD("distributed");
	KEYWORD("div");
	KEYWORD("do");
	KEYWORD("domain");
	KEYWORD("double");
	KEYWORD("drop");
	KEYWORD("dual");
	KEYWORD("dummy");
	KEYWORD("dump");
	KEYWORD("dynamic");
	KEYWORD("dynamic_function");
	KEYWORD("dynamic_function_code");
	KEYWORD("each");
	KEYWORD("element");
	KEYWORD("else");
	KEYWORD("elseif");
	KEYWORD("enable");
	KEYWORD("enclosed");
	KEYWORD("encoding");
	KEYWORD("encrypted");
	KEYWORD("end");
	KEYWORD("end-exec");
	KEYWORD("enum");
	KEYWORD("equals");
	KEYWORD("errlvl");
	KEYWORD("escape");
	KEYWORD("escaped");
	KEYWORD("every");
	KEYWORD("except");
	KEYWORD("exception");
	KEYWORD("exclude");
	KEYWORD("excluding");
	KEYWORD("exclusive");
	KEYWORD("exec");
	KEYWORD("execute");
	KEYWORD("existing");
	KEYWORD("exists");
	KEYWORD("exit");
	KEYWORD("exp");
	KEYWORD("explain");
	KEYWORD("external");
	KEYWORD("extract");
	KEYWORD("false");
	KEYWORD("fetch");
	KEYWORD("fields");
	KEYWORD("file");
	KEYWORD("fillfactor");
	KEYWORD("filter");
	KEYWORD("final");
	KEYWORD("first");
	KEYWORD("float");
	KEYWORD("float4");
	KEYWORD("float8");
	KEYWORD("floor");
	KEYWORD("flush");
	KEYWORD("following");
	KEYWORD("for");
	KEYWORD("force");
	KEYWORD("foreign");
	KEYWORD("fortran");
	KEYWORD("forward");
	KEYWORD("found");
	KEYWORD("free");
	KEYWORD("freetext");
	KEYWORD("freetexttable");
	KEYWORD("freeze");
	KEYWORD("from");
	KEYWORD("full");
	KEYWORD("fulltext");
	KEYWORD("function");
	KEYWORD("fusion");
	KEYWORD("g");
	KEYWORD("general");
	KEYWORD("generated");
	KEYWORD("get");
	KEYWORD("global");
	KEYWORD("go");
	KEYWORD("goto");
	KEYWORD("grant");
	KEYWORD("granted");
	KEYWORD("grants");
	KEYWORD("greatest");
	KEYWORD("group");
	KEYWORD("grouping");
	KEYWORD("handler");
	KEYWORD("having");
	KEYWORD("header");
	KEYWORD("heap");
	KEYWORD("hierarchy");
	KEYWORD("high_priority");
	KEYWORD("hold");
	KEYWORD("holdlock");
	KEYWORD("host");
	KEYWORD("hosts");
	KEYWORD("hour");
	KEYWORD("hour_microsecond");
	KEYWORD("hour_minute");
	KEYWORD("hour_second");
	KEYWORD("identified");
	KEYWORD("identity");
	KEYWORD("identity_insert");
	KEYWORD("identitycol");
	KEYWORD("if");
	KEYWORD("ignore");
	KEYWORD("ilike");
	KEYWORD("immediate");
	KEYWORD("immutable");
	KEYWORD("implementation");
	KEYWORD("implicit");
	KEYWORD("in");
	KEYWORD("include");
	KEYWORD("including");
	KEYWORD("increment");
	KEYWORD("index");
	KEYWORD("indicator");
	KEYWORD("infile");
	KEYWORD("infix");
	KEYWORD("inherit");
	KEYWORD("inherits");
	KEYWORD("initial");
	KEYWORD("initialize");
	KEYWORD("initially");
	KEYWORD("inner");
	KEYWORD("inout");
	KEYWORD("input");
	KEYWORD("insensitive");
	KEYWORD("insert");
	KEYWORD("insert_id");
	KEYWORD("instance");
	KEYWORD("instantiable");
	KEYWORD("instead");
	KEYWORD("int");
	KEYWORD("int1");
	KEYWORD("int2");
	KEYWORD("int3");
	KEYWORD("int4");
	KEYWORD("int8");
	KEYWORD("integer");
	KEYWORD("intersect");
	KEYWORD("intersection");
	KEYWORD("interval");
	KEYWORD("into");
	KEYWORD("invoker");
	KEYWORD("is");
	KEYWORD("isam");
	KEYWORD("isnull");
	KEYWORD("isolation");
	KEYWORD("iterate");
	KEYWORD("join");
	KEYWORD("k");
	KEYWORD("key");
	KEYWORD("key_member");
	KEYWORD("key_type");
	KEYWORD("keys");
	KEYWORD("kill");
	KEYWORD("lancompiler");
	KEYWORD("language");
	KEYWORD("large");
	KEYWORD("last");
	KEYWORD("last_insert_id");
	KEYWORD("lateral");
	KEYWORD("leading");
	KEYWORD("least");
	KEYWORD("leave");
	KEYWORD("left");
	KEYWORD("length");
	KEYWORD("less");
	KEYWORD("level");
	KEYWORD("like");
	KEYWORD("limit");
	KEYWORD("lineno");
	KEYWORD("lines");
	KEYWORD("listen");
	KEYWORD("ln");
	KEYWORD("load");
	KEYWORD("local");
	KEYWORD("localtime");
	KEYWORD("localtimestamp");
	KEYWORD("location");
	KEYWORD("locator");
	KEYWORD("lock");
	KEYWORD("login");
	KEYWORD("logs");
	KEYWORD("long");
	KEYWORD("longblob");
	KEYWORD("longtext");
	KEYWORD("loop");
	KEYWORD("low_priority");
	KEYWORD("lower");
	KEYWORD("m");
	KEYWORD("map");
	KEYWORD("match");
	KEYWORD("matched");
	KEYWORD("max");
	KEYWORD("max_rows");
	KEYWORD("maxextents");
	KEYWORD("maxvalue");
	KEYWORD("mediumblob");
	KEYWORD("mediumint");
	KEYWORD("mediumtext");
	KEYWORD("member");
	KEYWORD("merge");
	KEYWORD("message_length");
	KEYWORD("message_octet_length");
	KEYWORD("message_text");
	KEYWORD("method");
	KEYWORD("middleint");
	KEYWORD("min");
	KEYWORD("min_rows");
	KEYWORD("minus");
	KEYWORD("minute");
	KEYWORD("minute_microsecond");
	KEYWORD("minute_second");
	KEYWORD("minvalue");
	KEYWORD("mlslabel");
	KEYWORD("mod");
	KEYWORD("mode");
	KEYWORD("modifies");
	KEYWORD("modify");
	KEYWORD("module");
	KEYWORD("month");
	KEYWORD("monthname");
	KEYWORD("more");
	KEYWORD("move");
	KEYWORD("multiset");
	KEYWORD("mumps");
	KEYWORD("myisam");
	KEYWORD("name");
	KEYWORD("names");
	KEYWORD("national");
	KEYWORD("natural");
	KEYWORD("nchar");
	KEYWORD("nclob");
	KEYWORD("nesting");
	KEYWORD("new");
	KEYWORD("next");
	KEYWORD("no");
	KEYWORD("no_write_to_binlog");
	KEYWORD("noaudit");
	KEYWORD("nocheck");
	KEYWORD("nocompress");
	KEYWORD("nocreatedb");
	KEYWORD("nocreaterole");
	KEYWORD("nocreateuser");
	KEYWORD("noinherit");
	KEYWORD("nologin");
	KEYWORD("nonclustered");
	KEYWORD("none");
	KEYWORD("normalize");
	KEYWORD("normalized");
	KEYWORD("nosuperuser");
	KEYWORD("not");
	KEYWORD("nothing");
	KEYWORD("notify");
	KEYWORD("notnull");
	KEYWORD("nowait");
	KEYWORD("null");
	KEYWORD("nullable");
	KEYWORD("nullif");
	KEYWORD("nulls");
	KEYWORD("number");
	KEYWORD("numeric");
	KEYWORD("object");
	KEYWORD("octet_length");
	KEYWORD("octets");
	KEYWORD("of");
	KEYWORD("off");
	KEYWORD("offline");
	KEYWORD("offset");
	KEYWORD("offsets");
	KEYWORD("oids");
	KEYWORD("old");
	KEYWORD("on");
	KEYWORD("online");
	KEYWORD("only");
	KEYWORD("open");
	KEYWORD("opendatasource");
	KEYWORD("openquery");
	KEYWORD("openrowset");
	KEYWORD("openxml");
	KEYWORD("operation");
	KEYWORD("operator");
	KEYWORD("optimize");
	KEYWORD("option");
	KEYWORD("optionally");
	KEYWORD("options");
	KEYWORD("or");
	KEYWORD("order");
	KEYWORD("ordering");
	KEYWORD("ordinality");
	KEYWORD("others");
	KEYWORD("out");
	KEYWORD("outer");
	KEYWORD("outfile");
	KEYWORD("output");
	KEYWORD("over");
	KEYWORD("overlaps");
	KEYWORD("overlay");
	KEYWORD("overriding");
	KEYWORD("owner");
	KEYWORD("pack_keys");
	KEYWORD("pad");
	KEYWORD("parameter");
	KEYWORD("parameter_mode");
	KEYWORD("parameter_name");
	KEYWORD("parameter_ordinal_position");
	KEYWORD("parameter_specific_catalog");
	KEYWORD("parameter_specific_name");
	KEYWORD("parameter_specific_schema");
	KEYWORD("parameters");
	KEYWORD("partial");
	KEYWORD("partition");
	KEYWORD("pascal");
	KEYWORD("password");
	KEYWORD("path");
	KEYWORD("pctfree");
	KEYWORD("percent");
	KEYWORD("percent_rank");
	KEYWORD("percentile_cont");
	KEYWORD("percentile_disc");
	KEYWORD("placing");
	KEYWORD("plan");
	KEYWORD("pli");
	KEYWORD("position");
	KEYWORD("postfix");
	KEYWORD("power");
	KEYWORD("preceding");
	KEYWORD("precision");
	KEYWORD("prefix");
	KEYWORD("preorder");
	KEYWORD("prepare");
	KEYWORD("prepared");
	KEYWORD("preserve");
	KEYWORD("primary");
	KEYWORD("print");
	KEYWORD("prior");
	KEYWORD("privileges");
	KEYWORD("proc");
	KEYWORD("procedural");
	KEYWORD("procedure");
	KEYWORD("process");
	KEYWORD("processlist");
	KEYWORD("public");
	KEYWORD("purge");
	KEYWORD("quote");
	KEYWORD("raid0");
	KEYWORD("raiserror");
	KEYWORD("range");
	KEYWORD("rank");
	KEYWORD("raw");
	KEYWORD("read");
	KEYWORD("reads");
	KEYWORD("readtext");
	KEYWORD("real");
	KEYWORD("recheck");
	KEYWORD("reconfigure");
	KEYWORD("recursive");
	KEYWORD("ref");
	KEYWORD("references");
	KEYWORD("referencing");
	KEYWORD("regexp");
	KEYWORD("regr_avgx");
	KEYWORD("regr_avgy");
	KEYWORD("regr_count");
	KEYWORD("regr_intercept");
	KEYWORD("regr_r2");
	KEYWORD("regr_slope");
	KEYWORD("regr_sxx");
	KEYWORD("regr_sxy");
	KEYWORD("regr_syy");
	KEYWORD("reindex");
	KEYWORD("relative");
	KEYWORD("release");
	KEYWORD("reload");
	KEYWORD("rename");
	KEYWORD("repeat");
	KEYWORD("repeatable");
	KEYWORD("replace");
	KEYWORD("replication");
	KEYWORD("require");
	KEYWORD("reset");
	KEYWORD("resignal");
	KEYWORD("resource");
	KEYWORD("restart");
	KEYWORD("restore");
	KEYWORD("restrict");
	KEYWORD("result");
	KEYWORD("return");
	KEYWORD("returned_cardinality");
	KEYWORD("returned_length");
	KEYWORD("returned_octet_length");
	KEYWORD("returned_sqlstate");
	KEYWORD("returns");
	KEYWORD("revoke");
	KEYWORD("right");
	KEYWORD("rlike");
	KEYWORD("role");
	KEYWORD("rollback");
	KEYWORD("rollup");
	KEYWORD("routine");
	KEYWORD("routine_catalog");
	KEYWORD("routine_name");
	KEYWORD("routine_schema");
	KEYWORD("row");
	KEYWORD("row_count");
	KEYWORD("row_number");
	KEYWORD("rowcount");
	KEYWORD("rowguidcol");
	KEYWORD("rowid");
	KEYWORD("rownum");
	KEYWORD("rows");
	KEYWORD("rule");
	KEYWORD("save");
	KEYWORD("savepoint");
	KEYWORD("scale");
	KEYWORD("schema");
	KEYWORD("schema_name");
	KEYWORD("schemas");
	KEYWORD("scope");
	KEYWORD("scope_catalog");
	KEYWORD("scope_name");
	KEYWORD("scope_schema");
	KEYWORD("scroll");
	KEYWORD("search");
	KEYWORD("second");
	KEYWORD("second_microsecond");
	KEYWORD("section");
	KEYWORD("security");
	KEYWORD("select");
	KEYWORD("self");
	KEYWORD("sensitive");
	KEYWORD("separator");
	KEYWORD("sequence");
	KEYWORD("serializable");
	KEYWORD("server_name");
	KEYWORD("session");
	KEYWORD("session_user");
	KEYWORD("set");
	KEYWORD("setof");
	KEYWORD("sets");
	KEYWORD("setuser");
	KEYWORD("share");
	KEYWORD("show");
	KEYWORD("shutdown");
	KEYWORD("signal");
	KEYWORD("similar");
	KEYWORD("simple");
	KEYWORD("size");
	KEYWORD("smallint");
	KEYWORD("some");
	KEYWORD("soname");
	KEYWORD("source");
	KEYWORD("space");
	KEYWORD("spatial");
	KEYWORD("specific");
	KEYWORD("specific_name");
	KEYWORD("specifictype");
	KEYWORD("sql");
	KEYWORD("sql_big_result");
	KEYWORD("sql_big_selects");
	KEYWORD("sql_big_tables");
	KEYWORD("sql_calc_found_rows");
	KEYWORD("sql_log_off");
	KEYWORD("sql_log_update");
	KEYWORD("sql_low_priority_updates");
	KEYWORD("sql_select_limit");
	KEYWORD("sql_small_result");
	KEYWORD("sql_warnings");
	KEYWORD("sqlca");
	KEYWORD("sqlcode");
	KEYWORD("sqlerror");
	KEYWORD("sqlexception");
	KEYWORD("sqlstate");
	KEYWORD("sqlwarning");
	KEYWORD("sqrt");
	KEYWORD("ssl");
	KEYWORD("stable");
	KEYWORD("start");
	KEYWORD("starting");
	KEYWORD("state");
	KEYWORD("statement");
	KEYWORD("static");
	KEYWORD("statistics");
	KEYWORD("status");
	KEYWORD("stddev_pop");
	KEYWORD("stddev_samp");
	KEYWORD("stdin");
	KEYWORD("stdout");
	KEYWORD("storage");
	KEYWORD("straight_join");
	KEYWORD("strict");
	KEYWORD("string");
	KEYWORD("structure");
	KEYWORD("style");
	KEYWORD("subclass_origin");
	KEYWORD("sublist");
	KEYWORD("submultiset");
	KEYWORD("substring");
	KEYWORD("successful");
	KEYWORD("sum");
	KEYWORD("superuser");
	KEYWORD("symmetric");
	KEYWORD("synonym");
	KEYWORD("sysdate");
	KEYWORD("sysid");
	KEYWORD("system");
	KEYWORD("system_user");
	KEYWORD("table");
	KEYWORD("table_name");
	KEYWORD("tables");
	KEYWORD("tablesample");
	KEYWORD("tablespace");
	KEYWORD("temp");
	KEYWORD("template");
	KEYWORD("temporary");
	KEYWORD("terminate");
	KEYWORD("terminated");
	KEYWORD("text");
	KEYWORD("textsize");
	KEYWORD("than");
	KEYWORD("then");
	KEYWORD("ties");
	KEYWORD("time");
	KEYWORD("timestamp");
	KEYWORD("timezone_hour");
	KEYWORD("timezone_minute");
	KEYWORD("tinyblob");
	KEYWORD("tinyint");
	KEYWORD("tinytext");
	KEYWORD("to");
	KEYWORD("toast");
	KEYWORD("top");
	KEYWORD("top_level_count");
	KEYWORD("trailing");
	KEYWORD("tran");
	KEYWORD("transaction");
	KEYWORD("transaction_active");
	KEYWORD("transactions_committed");
	KEYWORD("transactions_rolled_back");
	KEYWORD("transform");
	KEYWORD("transforms");
	KEYWORD("translate");
	KEYWORD("translation");
	KEYWORD("treat");
	KEYWORD("trigger");
	KEYWORD("trigger_catalog");
	KEYWORD("trigger_name");
	KEYWORD("trigger_schema");
	KEYWORD("trim");
	KEYWORD("true");
	KEYWORD("truncate");
	KEYWORD("trusted");
	KEYWORD("tsequal");
	KEYWORD("type");
	KEYWORD("uescape");
	KEYWORD("uid");
	KEYWORD("unbounded");
	KEYWORD("uncommitted");
	KEYWORD("under");
	KEYWORD("undo");
	KEYWORD("unencrypted");
	KEYWORD("union");
	KEYWORD("unique");
	KEYWORD("unknown");
	KEYWORD("unlisten");
	KEYWORD("unlock");
	KEYWORD("unnamed");
	KEYWORD("unnest");
	KEYWORD("unsigned");
	KEYWORD("until");
	KEYWORD("update");
	KEYWORD("updatetext");
	KEYWORD("upper");
	KEYWORD("usage");
	KEYWORD("use");
	KEYWORD("user");
	KEYWORD("user_defined_type_catalog");
	KEYWORD("user_defined_type_code");
	KEYWORD("user_defined_type_name");
	KEYWORD("user_defined_type_schema");
	KEYWORD("using");
	KEYWORD("utc_date");
	KEYWORD("utc_time");
	KEYWORD("utc_timestamp");
	KEYWORD("vacuum");
	KEYWORD("valid");
	KEYWORD("validate");
	KEYWORD("validator");
	KEYWORD("value");
	KEYWORD("values");
	KEYWORD("var_pop");
	KEYWORD("var_samp");
	KEYWORD("varbinary");
	KEYWORD("varchar");
	KEYWORD("varchar2");
	KEYWORD("varcharacter");
	KEYWORD("variable");
	KEYWORD("variables");
	KEYWORD("varying");
	KEYWORD("verbose");
	KEYWORD("view");
	KEYWORD("volatile");
	KEYWORD("waitfor");
	KEYWORD("when");
	KEYWORD("whenever");
	KEYWORD("where");
	KEYWORD("while");
	KEYWORD("width_bucket");
	KEYWORD("window");
	KEYWORD("with");
	KEYWORD("within");
	KEYWORD("without");
	KEYWORD("work");
	KEYWORD("write");
	KEYWORD("writetext");
	KEYWORD("x509");
	KEYWORD("xor");
	KEYWORD("year");
	KEYWORD("year_month");
	KEYWORD("zerofill");
	KEYWORD("zone");

	//BUILTIN_OBJECT("Function");
}

#define BLOCK(_pBegin,_iLength,_oColor,_uSpecialFlags) \
		createBlock(_pBegin,_iLength,&(m_p->pCoreData->pOptions->_oColor),_uSpecialFlags)



void C3TextEditorModeParserSQL::computeBlocksParseIdentifier()
{
	Q_ASSERT(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'));
	
	const QChar * b = m_p->p;
	
	m_p->p++;

	while((m_p->p < m_p->e) && (m_p->p->isLetterOrNumber() || (m_p->p->unicode() == (ushort)'_')))
		m_p->p++;

	C3TextEditorLine::Block * pBlock = createBlock(b,m_p->p - b,NULL,0);

	if(m_hIdentifierColorHash.isEmpty())
		buildIdentifierColorHash();

	pBlock->pColor = m_hIdentifierColorHash.value(pBlock->szText.toLower(),NULL);
	
	if(!pBlock->pColor)
		pBlock->pColor = &(m_p->pCoreData->pOptions->oTextColor);
}

void C3TextEditorModeParserSQL::computeBlocksParseSQLMultiLineString(ushort uDelimiter,unsigned int uInterLineFlag)
{
	// This is called when m_p->p is point AFTER the initial delimiter!

	Q_ASSERT(m_p->uInterLineFlags & uInterLineFlag); // you must set the flag before calling this function

	const QChar * b = m_p->p;

	while(m_p->p < m_p->e)
	{
		ushort uChar = m_p->p->unicode();

		if(uChar == uDelimiter)
		{
			m_p->p++;
			BLOCK(b,m_p->p - b,oStringTextColor,0);
			m_p->uInterLineFlags &= ~uInterLineFlag;
			if(m_p->p >= m_p->e)
			{
				// end of line just after a multi line comment end. store an additional empty block to signal state change
				BLOCK(0,0,oTextColor,0);
			}
			return;
		}

		switch(uChar)
		{
			case '\t':
			{
				if(m_p->p > b)
				{
					BLOCK(b,m_p->p - b,oStringTextColor,0);
					b = m_p->p;
				}
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oStringTextColor,C3TextEditorLine::Block::IsTabBlock);
				b = m_p->p;
			}
			break;
			default:
			{
				// non tab
				m_p->p++;
				
				// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
				const QChar * tmpEnd = m_p->p + 256;
				if(tmpEnd > m_p->e)
					tmpEnd = m_p->e;
				
				while(m_p->p < tmpEnd)
				{
					ushort u = m_p->p->unicode();
					if(
							(u == (ushort)'\t') ||
							(u == uDelimiter)
						)
						break;
					m_p->p++;
				}

				BLOCK(b,m_p->p - b,oStringTextColor,0);
				b = m_p->p;
			}
			break;
		}
	}

	if(m_p->p > b)
		BLOCK(b,m_p->p - b,oStringTextColor,0);
}


void C3TextEditorModeParserSQL::computeMetadata()
{
	if(m_p->p == m_p->e)
	{
		// add a single empty block if we don't have any yet
		if(m_p->pLine->lBlocks.count() < 1)
			BLOCK(0,0,oTextColor,0); // color doesn't matter
		return;
	}

	while(m_p->p < m_p->e)
	{
		if(m_p->uInterLineFlags & InSQLMultiLineComment)
		{
			computeBlocksParseCLikeMultiLineComment(InSQLMultiLineComment);
			continue;
		}
		
		if(m_p->uInterLineFlags & InSQLSingleQuoteString)
		{
			computeBlocksParseSQLMultiLineString('\'',InSQLSingleQuoteString);
			continue;
		}

		if(m_p->uInterLineFlags & InSQLDoubleQuoteString)
		{
			computeBlocksParseSQLMultiLineString('"',InSQLDoubleQuoteString);
			continue;
		}

		if(m_p->uInterLineFlags & InSQLBackTickString)
		{
			computeBlocksParseSQLMultiLineString('`',InSQLBackTickString);
			continue;
		}

		// not in multiline comment
	
		const QChar * b = m_p->p;

		switch(m_p->p->unicode())
		{
			case '/':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				switch(m_p->p->unicode())
				{
					case '*':
						m_p->uInterLineFlags |= InSQLMultiLineComment;
						m_p->p++;
						BLOCK(b,m_p->p - b,oMultiLineCommentTextColor,0);
						if(m_p->p >= m_p->e)
							return;
						computeBlocksParseCLikeMultiLineComment(InSQLMultiLineComment);
					break;
					default: // anything else
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
				}
			}
			break;
			case '-':
			{
				m_p->p++;
				if(m_p->p >= m_p->e)
				{
					BLOCK(b,m_p->p - b,oOperatorTextColor,0);
					return;
				}
				switch(m_p->p->unicode())
				{
					case '-':
						m_p->p--;
						computeBlocksParseSingleLineComment();
					break;
					default: // anything else
						BLOCK(b,m_p->p - b,oOperatorTextColor,0);
						b = m_p->p;
					break;
				}
			}
			break;
			case '#':
				computeBlocksParseSingleLineComment();
			break;
			case '"':
				m_p->uInterLineFlags |= InSQLDoubleQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseSQLMultiLineString('"',InSQLDoubleQuoteString);
			break;
			case '\'':
				m_p->uInterLineFlags |= InSQLSingleQuoteString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseSQLMultiLineString('\'',InSQLSingleQuoteString);
			break;
			case '`':
				m_p->uInterLineFlags |= InSQLBackTickString;
				m_p->p++;
				BLOCK(b,m_p->p - b,oStringTextColor,0);
				if(m_p->p >= m_p->e)
					return;
				computeBlocksParseSQLMultiLineString('`',InSQLBackTickString);
			break;
			case '(':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)'(')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oParenthesisTextColor,0);
			}
			break;
			case ')':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)')')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oParenthesisTextColor,0);
			}
			break;
			case '[':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)'[')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oArrayDelimiterTextColor,0);
			}
			break;
			case ']':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(u != (ushort)']')
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oArrayDelimiterTextColor,0);
			}
			break;
			case '=':
			case ':':
			case '*':
			case '+':
			case '?':
			case '!':
			case '>':
			case '<':
			case '|':
			case '&':
			case '%':
			case ';':
			case ',':
			case '.':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					ushort u = m_p->p->unicode();
					if(
						(u != (ushort)'=') &&
						(u != (ushort)':') &&
						(u != (ushort)'+') &&
						(u != (ushort)'?') &&
						(u != (ushort)'!') &&
						(u != (ushort)'>') &&
						(u != (ushort)'<') &&
						(u != (ushort)'|') &&
						(u != (ushort)'&') &&
						(u != (ushort)'%') &&
						(u != (ushort)';') &&
						(u != (ushort)',') &&
						(u != (ushort)'.') &&
						(u != (ushort)'*')
					)
						break;
					m_p->p++;
				}
				BLOCK(b,m_p->p - b,oOperatorTextColor,0);
			}
			break;
			case '\t':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)'\t')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oTextColor,C3TextEditorLine::Block::IsTabBlock);
			}
			break;
			case ' ':
			{
				m_p->p++;
				while(m_p->p < m_p->e)
				{
					if(m_p->p->unicode() != (ushort)' ')
						break;
					m_p->p++;
				}
	
				BLOCK(b,m_p->p - b,oTextColor,0);
			}
			break;
			default:
			{
				if(m_p->p->isLetter() || (m_p->p->unicode() == (ushort)'_'))
				{
					computeBlocksParseIdentifier();
				} else if(m_p->p->isNumber())
				{
					computeBlocksParseNumber();
				} else {
					m_p->p++;

					// We avoid creating blocks longer than 256 characters. This improves painting performance with veeeery long lines.
					const QChar * tmpEnd = m_p->p + 256;
					if(tmpEnd > m_p->e)
						tmpEnd = m_p->e;
					
					while(m_p->p < tmpEnd)
					{
						ushort u = m_p->p->unicode();
						if(
								m_p->p->isLetterOrNumber() ||
								(u == (ushort)'_') ||
								(u == (ushort)'\t') ||
								(u == (ushort)'/') ||
								(u == (ushort)'(') ||
								(u == (ushort)')') ||
								(u == (ushort)'[') ||
								(u == (ushort)']') ||
								(u == (ushort)'=') ||
								(u == (ushort)':') ||
								(u == (ushort)'*') ||
								(u == (ushort)'+') ||
								(u == (ushort)'?') ||
								(u == (ushort)'>') ||
								(u == (ushort)'<') ||
								(u == (ushort)'#') ||
								(u == (ushort)'!') ||
								(u == (ushort)'|') ||
								(u == (ushort)'&') ||
								(u == (ushort)'%') ||
								(u == (ushort)';') ||
								(u == (ushort)'.') ||
								(u == (ushort)',') ||
								(u == (ushort)'\'') ||
								(u == (ushort)'"') ||
								(u == (ushort)'`') ||
								(u == (ushort)' ') ||
								(u == (ushort)'-')
							)
							break;
						m_p->p++;
					}

					BLOCK(b,m_p->p - b,oErrorTextColor,0);
				}
			}
			break;
		}
	}
}