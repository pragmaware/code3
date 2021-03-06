/*
 * Generated by ./misc/optlib2c from optlib/markdown.ctags, Don't edit this manually.
 */
#include "general.h"
#include "parse.h"
#include "routines.h"
#include "field.h"
#include "xtag.h"


typedef enum {
	K_CHAPTER,
	K_SECTION,
	K_SUBSECTION,
	K_SUBSUBSECTION,
	K_L4SUBSECTION,
	K_L5SUBSECTION,
} MarkdownKind;


static void initializeMarkdownParser (const langType language)
{

	addLanguageRegexTable (language, "main");
	addLanguageRegexTable (language, "main_sharp");
	addLanguageRegexTable (language, "chapter");
	addLanguageRegexTable (language, "chapter_sharp");
	addLanguageRegexTable (language, "section");
	addLanguageRegexTable (language, "section_sharp");
	addLanguageRegexTable (language, "subsection");
	addLanguageRegexTable (language, "subsection_sharp");
	addLanguageRegexTable (language, "subsubsection");
	addLanguageRegexTable (language, "subsubsection_sharp");
	addLanguageRegexTable (language, "l4subsection");
	addLanguageRegexTable (language, "l4subsection_sharp");
	addLanguageRegexTable (language, "l5subsection");
	addLanguageRegexTable (language, "l5subsection_sharp");
	addLanguageRegexTable (language, "emptyLine");
	addLanguageRegexTable (language, "gfmLeave");
	addLanguageRegexTable (language, "rest");
	addLanguageRegexTable (language, "code");
	addLanguageRegexTable (language, "codeblockBacktick");
	addLanguageRegexTable (language, "codeblockTildes");

	addLanguageTagMultiTableRegex (language, "main",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=main_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^([^\n]+)[\n]=+[\n]",
	                               "\\1", "c", "{_field=sectionMarker:=}{scope=push}{tenter=chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^([^\n]+)[\n]-+[\n]",
	                               "\\1", "s", "{_field=sectionMarker:-}{scope=push}{tenter=section}", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "main",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^#[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "c", "{_field=sectionMarker:##}{scope=push}{tenter=chapter,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^#[ \t]+([^\n]+)[\n]*",
	                               "\\1", "c", "{_field=sectionMarker:#}{scope=push}{tenter=chapter,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^##[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "s", "{_field=sectionMarker:##}{scope=push}{tenter=section,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^##[ \t]+([^\n]+)[\n]*",
	                               "\\1", "s", "{_field=sectionMarker:#}{scope=push}{tenter=section,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^###[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "S", "{_field=sectionMarker:##}{scope=push}{tenter=subsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^###[ \t]+([^\n]+)[\n]*",
	                               "\\1", "S", "{_field=sectionMarker:#}{scope=push}{tenter=subsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:##}{scope=push}{tenter=subsubsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:#}{scope=push}{tenter=subsubsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^#####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:##}{scope=push}{tenter=l4subsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^#####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:#}{scope=push}{tenter=l4subsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^######[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:##}{scope=push}{tenter=l5subsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^######[ \t]+([^\n]+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:#}{scope=push}{tenter=l5subsection,main}", NULL);
	addLanguageTagMultiTableRegex (language, "main_sharp",
	                               "^.",
	                               "", "", "{tjump=main}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=chapter_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^([^\n]+)[\n]-+[\n]",
	                               "\\1", "s", "{_field=sectionMarker:-}{scope=push}{tenter=section}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^([^\n]+)[\n]=+[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "chapter",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^#{1,1}[ \t]+([^\n]+)",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^##[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "s", "{_field=sectionMarker:##}{scope=push}{tenter=section,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^##[ \t]+([^\n]+)[\n]*",
	                               "\\1", "s", "{_field=sectionMarker:#}{scope=push}{tenter=section,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^###[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "S", "{_field=sectionMarker:##}{scope=push}{tenter=subsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^###[ \t]+([^\n]+)[\n]*",
	                               "\\1", "S", "{_field=sectionMarker:#}{scope=push}{tenter=subsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:##}{scope=push}{tenter=subsubsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:#}{scope=push}{tenter=subsubsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^#####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:##}{scope=push}{tenter=l4subsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^#####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:#}{scope=push}{tenter=l4subsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^######[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:##}{scope=push}{tenter=l5subsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^######[ \t]+([^\n]+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:#}{scope=push}{tenter=l5subsection,chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "chapter_sharp",
	                               "^.",
	                               "", "", "{tjump=chapter}", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=section_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^([^\n]+)[\n](=+|-+)[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "section",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^#{1,2}[ \t]+([^\n]+)",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^###[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "S", "{_field=sectionMarker:##}{scope=push}{tenter=subsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^###[ \t]+([^\n]+)[\n]*",
	                               "\\1", "S", "{_field=sectionMarker:#}{scope=push}{tenter=subsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:##}{scope=push}{tenter=subsubsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:#}{scope=push}{tenter=subsubsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^#####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:##}{scope=push}{tenter=l4subsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^#####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:#}{scope=push}{tenter=l4subsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^######[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:##}{scope=push}{tenter=l5subsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^######[ \t]+([^\n]+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:#}{scope=push}{tenter=l5subsection,section}", NULL);
	addLanguageTagMultiTableRegex (language, "section_sharp",
	                               "^.",
	                               "", "", "{tjump=section}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=subsection_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^([^\n]+)[\n](=+|-+)[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "subsection",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^#{1,3}[ \t]+([^\n]+)",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:##}{scope=push}{tenter=subsubsection,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "t", "{_field=sectionMarker:#}{scope=push}{tenter=subsubsection,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^#####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:##}{scope=push}{tenter=l4subsection,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^#####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:#}{scope=push}{tenter=l4subsection,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^######[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:##}{scope=push}{tenter=l5subsection,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^######[ \t]+([^\n]+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:#}{scope=push}{tenter=l5subsection,subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsection_sharp",
	                               "^.",
	                               "", "", "{tjump=subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=subsubsection_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^([^\n]+)[\n](=+|-+)[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection_sharp",
	                               "^#{1,4}[ \t]+([^\n]+)",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection_sharp",
	                               "^#####[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:##}{scope=push}{tenter=l4subsection,subsubsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection_sharp",
	                               "^#####[ \t]+([^\n]+)[\n]*",
	                               "\\1", "T", "{_field=sectionMarker:#}{scope=push}{tenter=l4subsection,subsubsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection_sharp",
	                               "^######[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:##}{scope=push}{tenter=l5subsection,subsubsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection_sharp",
	                               "^######[ \t]+([^\n]+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:#}{scope=push}{tenter=l5subsection,subsubsection}", NULL);
	addLanguageTagMultiTableRegex (language, "subsubsection_sharp",
	                               "^.",
	                               "", "", "{tjump=subsubsection}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=l4subsection_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^([^\n]+)[\n](=+|-+)[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection_sharp",
	                               "^#{1,5}[ \t]+([^\n]+)",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection_sharp",
	                               "^######[ \t]+([^\n]+)([ \t]+#+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:##}{scope=push}{tenter=l5subsection,l4subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection_sharp",
	                               "^######[ \t]+([^\n]+)[\n]*",
	                               "\\1", "u", "{_field=sectionMarker:#}{scope=push}{tenter=l5subsection,l4subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "l4subsection_sharp",
	                               "^.",
	                               "", "", "{tjump=l4subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^#",
	                               "", "", "{_advanceTo=0start}{tjump=l5subsection_sharp}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^([^\n]+)[\n](=+|-+)[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection_sharp",
	                               "^#{1,6}[ \t]+([^\n]+)",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "l5subsection_sharp",
	                               "^.",
	                               "", "", "{tjump=l5subsection}", NULL);
	addLanguageTagMultiTableRegex (language, "emptyLine",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "gfmLeave",
	                               "^([\t ]*)[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "gfmLeave",
	                               "^([^\n]+)[\n](=+|-+)[\n]",
	                               "", "", "{scope=pop}{tleave}{_advanceTo=0start}", NULL);
	addLanguageTagMultiTableRegex (language, "rest",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "rest",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "code",
	                               "^[ \t]*````*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^`\n]*[\n]",
	                               "", "", "{tenter=codeblockBacktick}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "code",
	                               "^[ \t]*~~~~*([a-zA-Z0-9][-#+a-zA-Z0-9]*)?[^~\n]*[\n]",
	                               "", "", "{tenter=codeblockTildes}{_guest=\\1,0end,}", NULL);
	addLanguageTagMultiTableRegex (language, "codeblockBacktick",
	                               "^[ \t]*````*[ \t]*[\n]",
	                               "", "", "{tleave}{_guest=,,0start}", NULL);
	addLanguageTagMultiTableRegex (language, "codeblockBacktick",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "codeblockBacktick",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
	addLanguageTagMultiTableRegex (language, "codeblockTildes",
	                               "^[ \t]*~~~~*[ \t]*[\n]",
	                               "", "", "{tleave}{_guest=,,0start}", NULL);
	addLanguageTagMultiTableRegex (language, "codeblockTildes",
	                               "^[^\n]*[\n]+",
	                               "", "", "", NULL);
	addLanguageTagMultiTableRegex (language, "codeblockTildes",
	                               "^[^\n]*",
	                               "", "", "{tquit}", NULL);
}

extern parserDefinition* MarkdownParser (void)
{
	static const char *const extensions [] = {
		"md",
		"markdown",
		NULL
	};

	static const char *const aliases [] = {
		NULL
	};

	static const char *const patterns [] = {
		NULL
	};

	static kindDefinition MarkdownKindTable [] = {
		{
		  true, 'c', "chapter", "chapsters",
		},
		{
		  true, 's', "section", "sections",
		},
		{
		  true, 'S', "subsection", "subsections",
		},
		{
		  true, 't', "subsubsection", "subsubsections",
		},
		{
		  true, 'T', "l4subsection", "level 4 subsections",
		},
		{
		  true, 'u', "l5subsection", "level 5 subsections",
		},
	};
	static fieldDefinition MarkdownFieldTable [] = {
		{
		  .enabled     = false,
		  .name        = "sectionMarker",
		  .description = "character used for declaring section(#, ##, =, or -)",
		},
	};

	parserDefinition* const def = parserNew ("Markdown");

	def->enabled       = true;
	def->extensions    = extensions;
	def->patterns      = patterns;
	def->aliases       = aliases;
	def->method        = METHOD_NOT_CRAFTED|METHOD_REGEX;
	def->useCork       = 1;
	def->kindTable     = MarkdownKindTable;
	def->kindCount     = ARRAY_SIZE(MarkdownKindTable);
	def->fieldTable    = MarkdownFieldTable;
	def->fieldCount    = ARRAY_SIZE(MarkdownFieldTable);
	def->defaultScopeSeparator = "\"\"";
	def->initialize    = initializeMarkdownParser;

	return def;
}
