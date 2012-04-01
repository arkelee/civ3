#include "stdafx.h"
#include "Civ3global.h"

///////////////////////////////////////////////////////////
char linebuf[1024];
long begin_line;

enum errorcode g_errorcode;
enum token_line looknext;

char section[50];
char section_comment[128];


///////////////////////////////////////////////////////////
void report_error(enum errorcode code)
{
    g_errorcode = code;
}

void inline next_line()
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
	wrap_regcomp("TOP [[:upper:]]+ SECTION", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, REG_EXTENDED)) {
		wrap_regmatch("%s", section);
		debug("parse_section_name %s\n", section);
//		return SECTION_HEADER;
	}

	if (0 < sscanf(linebuf, "TOP %s SECTION (%s)", section, section_comment)) {
		section[4] = 0;
		return SECTION_HEADER;
	}
	return UNKNOWN_TOKEN;
}
enum token_line parse_section_name()
{
	wrap_regcomp("^  [[:digit:]]+", REG_EXTENDED);
	if (REG_OK == wrap_regexec(linebuf, REG_EXTENDED))
		debug("parse_section_name\n");
	return SECTION_NAME;
}
enum token_line parse_section_number()
{
	return SECTION_NUMBER;
}
enum token_line parse_item_header()
{
	return ITEM_HEADER;
}
enum token_line parse_item_field()
{
	return ITEM_FIELD;
}
enum token_line parse_item_field_binary()
{
	return ITEM_FIELD_BINARY;
}
enum token_line parse_item_field_binary_header()
{
	return ITEM_FIELD_BINARY_HEADER;
}
enum token_line parse_item_field_binary_bitfield()
{
	return ITEM_FIELD_BINARY_BITFIELD;
}
enum token_line parse_item_item_header()
{
	return ITEM_ITEM_HEADER;
}
enum token_line parse_item_item_field()
{
	return ITEM_ITEM_FIELD;
}
enum token_line parse_item_item_item_header()
{
	return ITEM_ITEM_ITEM_HEADER;
}
enum token_line parse_item_item_item_field()
{
	return ITEM_ITEM_ITEM_FIELD;
}
enum token_line parse_comment()
{
	return COMMENT;
}
parse_line_func_t *parse_line_funcs[] = {
	parse_section_header,
	parse_section_name,
	parse_section_number,
	parse_item_header,
	parse_item_field,
	parse_item_field_binary,
	parse_item_field_binary_header,
	parse_item_field_binary_bitfield,
	parse_item_item_header,
	parse_item_item_field,
	parse_item_item_item_header,
	parse_item_item_item_field,
	parse_comment,
	NULL,
};

enum token_line parse_line()
{
	parse_line_func_t **func_ptr;

	next_line();
	for (func_ptr = parse_line_funcs; *func_ptr; ++func_ptr) {
		looknext = (*func_ptr)();
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

int parse_section()
{
#ifdef _DEBUG
	static unsigned int count;
#endif
//	next_line();
//	parse_section_header();
	debug("section%02d %s\n", count++, section);
	match_line(SECTION_HEADER);
	match_line(SECTION_NAME);
	match_line(SECTION_NUMBER);
	return 0;
}


int parse_file()
{

	while (!feof(fp_src)) {

		parse_line();
//		retrect_line();
		switch (looknext) {
		case SECTION_HEADER:
			parse_section();
			break;
		default:
			parse_line();;
		}



//		fputs(linebuf, fp_out);
	}
	return 0;
}

///////////////////////////////////////////////////////////