/*
This file is part of "drippy", a datafile ripper.
Copyright (C) 2010-2018 John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "common.h"

struct {
	char *id;
	char *name;
	int (*rip)(FILE*);
} fmt[] = {
	{"RIFF", "avi", rip_avi},
	/*{"RIFF", "wav", rip_wav},*/
	{"OggS", "ogg", rip_ogg},
	{"BIK", "bink", rip_bink},
	{"SMK", "smacker", rip_smacker},
	{"\xff\xd8\xff\xe0", "jpeg", rip_jpeg},
	{"\x89PNG\r\n\x1a\n", "png", rip_png},
	{"\x4d\x4d", "3ds", rip_3ds},
	{0, 0, 0}
};

int rip(const char *fname, int type);
const char *fsize_str(size_t sz);

int main(int argc, char **argv)
{
	int i, stopopt = 0;
	int num_files = 0;
	const char *only_type = 0;

	for(i=1; i<argc; i++) {
		if(!stopopt && argv[i][0] == '-') {
			if(argv[i][2] != 0) {
				fprintf(stderr, "unrecognized option: %s\n", argv[i]);
				return 1;
			}
			switch(argv[i][1]) {
			case 't':
				only_type = argv[++i];
				break;

			case '-':
				stopopt = 1;
				break;

			case 'v':
				verbose = 1;
				break;

			case 'h':
				printf("usage:\n");
				printf("%s [opt] <file1> [<file2> ... <file-n>]\n", argv[0]);
				printf("options:\n");
				printf("  -h            this usage help message\n");
				printf("  -t <type>     rip only files of that type\n");
				printf("  --            stop processing options\n");
				printf("\nknown filetypes: ");

				for(i=0; fmt[i].rip; i++) {
					printf("%s ", fmt[i].name);
				}
				putchar('\n');
				return 0;

			default:
				fprintf(stderr, "unrecognized option: %s\n", argv[i]);
				return 1;
			}

		} else {
			int type = -1;

			if(only_type) {
				for(i=0; fmt[i].rip; i++) {
					if(strcmp(only_type, fmt[i].name) == 0) {
						type = i;
						break;
					}
				}

				if(type == -1) {
					fprintf(stderr, "unknown filetype: %s\n", only_type);
					return 1;
				}
			}

			if(rip(argv[i], type) == -1) {
				return 1;
			}
			num_files++;
		}
	}

	if(!num_files) {
		fprintf(stderr, "please specify one or more files to rip data from\n");
		return 1;
	}
	return 0;
}

int rip(const char *fname, int type)
{
	FILE *fp;
	struct stat st;

	printf("ripping %s known file types from: %s\n", type == -1 ? "all" : fmt[type].name, fname);

	if(!(fp = fopen(fname, "rb"))) {
		fprintf(stderr, "failed to open: %s: %s\n", fname, strerror(errno));
		return -1;
	}
	fstat(fileno(fp), &st);
	filesize = st.st_size;

	while(nextc(fp) != EOF) {
		int i;
		long pos = ftell(fp);

		if(type != -1) {
			if(lastchars(fmt[type].id, strlen(fmt[type].id))) {
				if(fmt[type].rip(fp) == 0) {
					reset_prev();
					continue;
				}
			}
		} else {

			for(i=0; fmt[i].rip; i++) {
				if(i > 0) {
					fseek(fp, pos, SEEK_SET);
				}

				if(lastchars(fmt[i].id, strlen(fmt[i].id))) {
					if(fmt[i].rip(fp) == 0) {
						reset_prev();
						break;
					}
				}
			}
		}
	}

	fclose(fp);
	return 0;
}

#define MIN(a, b)	((a) < (b) ? (a) : (b))

int dump(FILE *in, size_t len, const char *suffix, const char *desc)
{
	static int num;
	char fname[64];
	char buf[4096];
	FILE *out;

	sprintf(fname, "rip%04d.%s", num++, suffix);

	printf("dumping: %s [%s] (%s)\n", fname, fsize_str(len), desc);

	if(!(out = fopen(fname, "wb"))) {
		fprintf(stderr, "failed to open file: %s for writing: %s\n", fname, strerror(errno));
		return -1;
	}

	while(len > 0) {
		size_t res, rd = MIN(4096, len);

		if(!(res = fread(buf, 1, rd, in))) {
			fprintf(stderr, "unexpected EOF\n");
			fclose(out);
			return -1;
		}
		fwrite(buf, 1, res, out);
		len -= res;
	}

	fclose(out);
	return 0;
}

const char *fsize_str(size_t sz)
{
	static char str[128];
	static const char *units[] = {"bytes", "kb", "mb", "gb", "tb", "pb", 0};
	int i = 0;
	float fsz = (float)sz;

	while(units[i + 1] && fsz > 1024.0) {
		i++;
		fsz /= 1024.0;
	}

	sprintf(str, "%.2f %s", fsz, units[i]);
	return str;
}
