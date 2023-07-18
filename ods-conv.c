 /* libc */
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <stdarg.h>
#include <string.h>
 /* unix */
#include <getopt.h>
#include <sysexits.h>
#include <err.h>
 /* libs */
#include <zip.h>
#include <libxml/xmlreader.h>

enum { FMT_TSV };

static noreturn void
usage(void)
{
	fputs("usage: ods-conv [-T fmt] file\n", stderr);
	exit(EX_USAGE);
}

static noreturn void
err_zip(int status, zip_error_t *error, const char *fmt, ...)
{
	static char buf[256];
	va_list ap;

	if (!fmt)
		errx(status, "%s", zip_error_strerror(error));
	else {
		va_start(ap, fmt);
		vsnprintf(buf, sizeof(buf), fmt, ap);
		va_end(ap);
		errx(status, "%s: %s", buf, zip_error_strerror(error));
	}
}

static void
on_xml_error(void *ctx, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vwarnx(fmt, ap);
	va_end(ap);
}

static int
xml_zip_read(void *ctx, char *buf, int len)
{
	return (int)zip_fread(ctx, buf, (zip_uint64_t)len);
}

static int
xml_zip_close(void *ctx)
{
	return zip_fclose(ctx);
}

static void
ods_conv(const char *path, int fmt)
{
	zip_t *zip;
	zip_file_t *content_file;
	zip_error_t error;
	const char *name, *text;
	xmlTextReaderPtr reader;
	int code, first_row=1, new_row=0;

	if (!(zip = zip_open(path, ZIP_RDONLY, &code))) {
		zip_error_init_with_code(&error, code);
		err_zip(1, &error, "%s", path);
	}

	if (!(content_file = zip_fopen(zip, "content.xml", 0)))
		err_zip(1, zip_get_error(zip), "content.xml");

	reader = xmlReaderForIO(xml_zip_read, xml_zip_close, content_file,
	    "content.xml", "content.xml", XML_PARSE_NONET);
	if (!reader)
		errx(1, "content.xml: failed to create XML reader");

	while (xmlTextReaderRead(reader)) {
		name = (const char *)xmlTextReaderConstName(reader);

		switch (xmlTextReaderNodeType(reader)) {
		case XML_READER_TYPE_ELEMENT:
			if (!strcmp(name, "table:table-row"))
				new_row = 1;
			else if (!strcmp(name, "table:table-cell")) {
				if (!new_row)
					printf("\t");
				else if (!first_row)
					printf("\n");
				new_row = 0;
				first_row = 0;
			}
			break;

		case XML_READER_TYPE_TEXT:
			text = (const char *)xmlTextReaderConstValue(reader);
			printf("%s", text);
			break;
		}
	}

	xmlFreeTextReader(reader);
	/* zip_fclose(content_file) -- done by XML reader */
	zip_close(zip);
}

int
main(int argc, char **argv)
{
	int fmt=FMT_TSV, c;

	while ((c = getopt(argc, argv, "hT:")) != -1)
		switch (c) {
		case 'T':
			if (!strcmp(optarg, "tsv"))
				fmt = FMT_TSV;
			else
				errx(EX_USAGE, "bad format (-T)");
			break;

		default:
			usage();
		}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		errx(EX_USAGE, "no file given (-h for help)");
	else if (argc > 1)
		errx(EX_USAGE, "too many arguments (-h for help)");

	LIBXML_TEST_VERSION

	xmlSetGenericErrorFunc(NULL, on_xml_error);

	ods_conv(argv[0], fmt);

	return 0;
}
