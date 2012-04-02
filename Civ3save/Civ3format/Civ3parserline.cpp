#include "stdafx.h"
#include "Civ3global.h"

///////////////////////////////////////////////////////////
char linebuf[1024];
long begin_line;

enum errorcode g_errorcode;
enum token_line looknext;

char section[5];

int	bytes = 0;
char type[10];
char field[128];

char g_buf[1024];
char dbg_buf1[128];
char dbg_buf2[128];
char dbg_buf3[128];

//#define PROFILE
#ifdef PROFILE
#define profile() \
	do { \
		static int count; \
		debug("#%03d ", count++); \
	} while(0)
#else
#define profile()
#endif
///////////////////////////////////////////////////////////
void report_error(enum errorcode code)
{
	debug("errorcode = %d\n", (int)code);
    g_errorcode = code;
}

void inline get_line()
{
	begin_line = ftell(fp_src);
	fgets(linebuf, 511, fp_src);
}
void inline retrect_line()
{
	fseek(fp_src, begin_line, SEEK_SET);
}

enum token_line parse_section_header()
{
	wrap_regcomp("TOP ([[:graph:]]+) SECTION", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", section);
		debug("section %s\n", section);
		profile();
		return SECTION_HEADER;
	}

	return UNKNOWN_TOKEN;
}
enum token_line parse_section_name()
{
	wrap_regcomp("^  ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+\"([[:graph:]]+)\"", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, section);
		debug("\t%d\t%s\t%s\n", bytes, type, section);
		profile();
		return SECTION_NAME;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_section_number()
{
	wrap_regcomp("^  ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+number of (([[:lower:]]| )+)", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, g_buf);
		debug("\t%d\t%s\tnumber of %s\n", bytes, type, g_buf);
		profile();
		return SECTION_NUMBER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_header()
{
	wrap_regcomp("^  For each (([[:lower:]]| )+):", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("\tFor each %s #%d\n", g_buf);
		profile();
		return ITEM_HEADER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_field()
{
	wrap_regcomp("^    ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+(([[:lower:]]| )+)", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, field);
		debug("\t\t%d\t%s\t%s #%d\n", bytes, type, field);
		profile();
		return ITEM_FIELD;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_field_binary()
{
	return UNKNOWN_TOKEN;
	return ITEM_FIELD_BINARY;
}
enum token_line parse_item_field_binary_header()
{
	return UNKNOWN_TOKEN;
	return ITEM_FIELD_BINARY_HEADER;
}
enum token_line parse_item_field_binary_bitfield()
{
	return UNKNOWN_TOKEN;
	return ITEM_FIELD_BINARY_BITFIELD;
}
enum token_line parse_item_item_header()
{
	return UNKNOWN_TOKEN;
	return ITEM_ITEM_HEADER;
}
enum token_line parse_item_item_field()
{
	return UNKNOWN_TOKEN;
	return ITEM_ITEM_FIELD;
}
enum token_line parse_item_item_item_header()
{
	return UNKNOWN_TOKEN;
	return ITEM_ITEM_ITEM_HEADER;
}
enum token_line parse_item_item_item_field()
{
	return UNKNOWN_TOKEN;
	return ITEM_ITEM_ITEM_FIELD;
}
enum token_line parse_comment()
{
	wrap_regcomp("^((//.*|[[:space:]]*)*)\n", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("//COMMENT:%s\n", g_buf);
		profile();
		return COMMENT;
	}

	return UNKNOWN_TOKEN;
}
enum token_line parse_unknown()
{
	wrap_regcomp("^((//.*|[[:space:]]*)*)\n", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("//COMMENT:%s\n", g_buf);
		profile();
		return COMMENT;
	}

	return UNKNOWN_TOKEN;
}
parse_line_func_t *parse_line_funcs[] = {
	parse_item_field,
	parse_section_header,
	parse_section_name,
	parse_section_number,
	parse_item_header,
	parse_item_field_binary,
	parse_item_field_binary_header,
	parse_item_field_binary_bitfield,
	parse_item_item_header,
	parse_item_item_field,
	parse_item_item_item_header,
	parse_item_item_item_field,
	parse_comment,
	parse_unknown,
	NULL,
};

enum token_line parse_line()
{
	parse_line_func_t **func_ptr;

_start:
	get_line();
	for (func_ptr = parse_line_funcs; *func_ptr; ++func_ptr) {
		looknext = (*func_ptr)();
		if (looknext == COMMENT) {
			goto _start;
		}
		if (looknext != UNKNOWN_TOKEN) {
			return looknext;
		}
	}
	return (looknext = UNKNOWN_TOKEN);
}

void match_line(enum token_line t)
{
    if (looknext == t)
        looknext = parse_line();
    else
        report_error(SYNTAX);
}

int parse_item()
{
	match_line(ITEM_HEADER);

	match_line(ITEM_FIELD);
	return 0;
}

int parse_section()
{
	match_line(SECTION_HEADER);
	match_line(SECTION_NAME);
	match_line(SECTION_NUMBER);

	parse_item();

	return 0;
}


int parse_file()
{

	while (!feof(fp_src)) {

		parse_line();
		switch (looknext) {
		case SECTION_HEADER:
			parse_section();
			break;
//		default:
//			parse_line();
		}



//		fputs(linebuf, fp_out);
	}
	return 0;
}

///////////////////////////////////////////////////////////