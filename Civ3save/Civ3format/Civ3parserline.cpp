#include "stdafx.h"
#include "Civ3global.h"

///////////////////////////////////////////////////////////
char linebuf[1024];
long begin_line;

enum errorcode g_errorcode;
enum token_line looknext;

char section[5];

int	bytes = 0;
char type[1024];
char field[1024];

char g_buf[1024];
char dbg_buf1[1024];
char dbg_buf2[1024];
char dbg_buf3[1024];

#define PROFILE
#ifdef PROFILE
#define profile() \
	do { \
		static int count; \
		debug("[%s#%d]", __FUNCTION__, count++); \
	} while(0)
#else
#define profile()
#endif
///////////////////////////////////////////////////////////
void report_error(enum errorcode code)
{
	printf("errorcode = %d\n", (int)code);
	printf("%s\n", linebuf);
    g_errorcode = code;
}

void inline get_line()
{
	begin_line = ftell(fp_src);
	fgets(linebuf, 1023, fp_src);
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
		debug("%s", section);
		profile();
		debug("\n");
		return SECTION_HEADER;
	}

	return UNKNOWN_TOKEN;
}
enum token_line parse_section_name()
{
	wrap_regcomp("^  ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+\"([[:graph:]]+)\"", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, section);
		debug("\t%d\t%s\t%s", bytes, type, section);
		profile();
		debug("\n");
		return SECTION_NAME;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_section_number()
{
	wrap_regcomp("^  ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+number of (([[:lower:]]| )+)", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, g_buf);
		debug("\t%d\t%s\tnumber of %s", bytes, type, g_buf);
		profile();
		debug("\n");
		return SECTION_NUMBER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_header()
{
	wrap_regcomp("^  For each (([[:lower:]]| )+):", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("\tFor each %s", g_buf);
		profile();
		debug("\n");
		return ITEM_HEADER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_field()
{
	wrap_regcomp("^    ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+(([[:alnum:]]| )+)", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, field);
		debug("\t\t%d\t%s\t%s", bytes, type, field);
		profile();
		debug("\n");

//		if (!strncmp(field, "number of", 9)) {
//			return ITEM_ITEM_HEADER;
//		}

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
	wrap_regcomp("00000000", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
//		wrap_regmatch("%s", g_buf);
		debug("\t\t\tBINARY_HEADER:");
		profile();
		debug("\n");
		return ITEM_FIELD_BINARY_HEADER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_field_binary_bitfield()
{
//	wrap_regcomp("[[:blank:]]+([[:graph:]]+)[[:blank:]]+[.1]{8}", REG_EXTENDED);
	wrap_regcomp("(([[:graph:]]| )+)\t+([.1]{8})", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s%s%s", g_buf, dbg_buf1, field);
		debug("\t\t\t%s\t%s", field, g_buf);
		profile();
		debug("\n");
		return ITEM_FIELD_BINARY_BITFIELD;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_item_header()
{
	wrap_regcomp("^    For each (([[:lower:]]| )+):", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("\t\tFor each %s", g_buf);
		profile();
		debug("\n");
		return ITEM_ITEM_HEADER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_item_field()
{
	wrap_regcomp("^      ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+(([[:alnum:]]| )+)", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, field);
		debug("\t\t\t%d\t%s\t%s", bytes, type, field);
		profile();
		debug("\n");
		return ITEM_ITEM_FIELD;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_item_item_header()
{
	wrap_regcomp("^      For each (([[:lower:]]| )+):", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("\t\t\tFor each %s", g_buf);
		profile();
		debug("\n");
		return ITEM_ITEM_ITEM_HEADER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_item_item_item_field()
{
	wrap_regcomp("^        ([[:digit:]]+)[[:space:]]+([[:lower:]]+)[[:space:]]+(([[:alnum:]]| )+)", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%d%s%s", &bytes, type, field);
		debug("\t\t\t\t%d\t%s\t%s", bytes, type, field);
		profile();
		debug("\n");
		return ITEM_ITEM_ITEM_FIELD;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_comment()
{
	wrap_regcomp("^((//.*|[[:space:]]*)*)\n", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("//%s", g_buf);
		profile();
		debug("\n");
		return COMMENT;
	}

	return UNKNOWN_TOKEN;
}
enum token_line parse_unknown()
{
	wrap_regcomp("^(([^/]{2}.*|[[:space:]]*)*)\n", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, 0)) {
		wrap_regmatch("%s", g_buf);
		debug("//%s", g_buf);
		profile();
		debug("\n");
		return OTHERS;
	}

	return UNKNOWN_TOKEN;
}
parse_line_func_t *parse_line_funcs[] = {
	parse_item_field,
	parse_comment,
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
	
	debug("//UNKNOWN:%s\n", linebuf);
	return (looknext = UNKNOWN_TOKEN);
}

void match_line(enum token_line t)
{
    if (looknext == t)
        looknext = parse_line();
    else
        report_error(SYNTAX);
}

int parse_item_item_item()
{
	match_line(ITEM_ITEM_ITEM_HEADER);
	match_line(ITEM_ITEM_ITEM_FIELD);
	while(1) {
		switch (looknext) {
		case ITEM_ITEM_ITEM_FIELD:
			match_line(ITEM_ITEM_ITEM_FIELD);
			break;
		default:
			goto _end;
		}
	}
_end:
	return 0;
}

int parse_item_item()
{
	match_line(ITEM_ITEM_HEADER);
	match_line(ITEM_ITEM_FIELD);
	while(1) {
		switch (looknext) {
		case ITEM_ITEM_FIELD:
			match_line(ITEM_ITEM_FIELD);
			break;
		case ITEM_ITEM_ITEM_HEADER:
			parse_item_item_item();
		default:
			goto _end;
		}
	}
_end:
	return 0;
}

int parse_item()
{
	match_line(ITEM_HEADER);
	match_line(ITEM_FIELD);
	while(1) {
		switch (looknext) {
		case ITEM_FIELD:
			match_line(ITEM_FIELD);
			break;
		case ITEM_ITEM_HEADER:
			parse_item_item();
			break;
		default:
			goto _end;
		}
	}
_end:
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