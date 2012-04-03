#include "stdafx.h"
#include "global.h"

///////////////////////////////////////////////////////////
char *g_buf;
char g_tag[128];
int g_errorcode;
long offset;
///////////////////////////////////////////////////////////

void report_error(char *error)
{
	printf("error: %s\n", error);
//	g_errorcode = -1;
}

int match_tag(char* tag)
{
	long len = strlen(tag);
	int rt;

	fread(g_tag, 1, len, fp_src);
	g_tag[len] = 0;

	rt = strcmp(g_tag, tag);
	if (rt)
		report_error(tag);

	return rt;
}

int load_section()
{
	long len = 0;
	long number, length;
	long number2; 
	long dbg_offset;

	dbg_offset = ftell(fp_src);

	fread(g_tag, 1, 4, fp_src);
	g_tag[4] = 0;
	debug("load section: %s @ %08X\n", g_tag, dbg_offset);
	printf("load section: %s @ %08X\n", g_tag, dbg_offset);

	if (dbg_offset == 0x334C8) {
		fseek(fp_src, 0x3459b, 0);
		return 0;
	}

	if (!strcmp(g_tag, "FLAV")) {
		fread(&number, sizeof(long), 1, fp_src);
		fread(&number2, sizeof(long), 1, fp_src);
		offset = number2 * 0x124;
//	} else if (!strcmp(g_tag, "GAME")) {
	} else if (!strcmp(g_tag, "WRLD")) {
		fseek(fp_src, 0x91a15, 0);
		return 0;
	} else if (!strcmp(g_tag, "LEAD")) {
		fseek(fp_src, 0x91a15, 0);
		return 0;
	} else {
		long i;
		fread(&number, sizeof(long), 1, fp_src);

		for (i = 0; i < number; i++) {
			fread(&length, sizeof(long), 1, fp_src);
			fseek(fp_src, length, 1);
		}

		return len;
	}

	fseek(fp_src, offset, 1);

	return len;
}

int load_file()
{
	g_buf = (char *)malloc(MAXBUF);

	if (match_tag("CIV3"))
		printf("file is not CIV3 save file.\n");

	fseek(fp_src, 0x1e, 0);

	if (match_tag("BIC "))
		printf("file is not CIV3 save file.\n");

	fread(&offset, sizeof(long), 1, fp_src);
	fseek(fp_src, offset, 1);

	if (match_tag("BICQ"))
		printf("this version is not support.\n");

	while (!feof(fp_src)) {
		load_section();
	}

	free(g_buf);
	g_buf = NULL;
	return 0;
}

