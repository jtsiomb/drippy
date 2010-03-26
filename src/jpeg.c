/*
This file is part of "drippy", a datafile ripper.
Copyright (C) 2010 John Tsiombikas <nuclear@member.fsf.org>

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
#include "common.h"

int rip_jpeg(FILE *fp)
{
	char ident[5];
	long end, pos = ftell(fp) - 4;
	int nestimg = 1;

	fseek(fp, 2, SEEK_CUR);
	if(fread(ident, 1, 5, fp) < 5) {
		return -1;
	}
	if(memcmp(ident, "JFIF\0", 5) != 0) {
		return -1;
	}

	if(verbose) {
		printf("found JPEG image file at offset: %lxh\n", pos);
	}

	while(nextc(fp) != EOF) {
		if(lastchars("\xff\xd9", 2)) {
			if(!--nestimg) {
				end = ftell(fp);
				break;
			}
		}
		if(lastchars("\xff\xd8", 2)) {
			nestimg++;
		}
	}
	if(feof(fp)) {
		return -1;
	}

	fseek(fp, pos, SEEK_SET);
	dump(fp, end - pos, "jpg", "JPEG image file");
	return 0;
}
