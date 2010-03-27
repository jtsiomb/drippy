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
#include "common.h"

struct ogg_page_hdr {
	char id[4];
	unsigned char version;
	unsigned char type;
	uint16_t gpos0, gpos1, gpos2, gpos3;
	uint32_t bitstr_serial_num;
	uint32_t page_num;
	uint32_t checksum;
	unsigned char nsegm;
	unsigned char segm_table[1];
} __attribute__((packed));

#define PG_CONT		1
#define PG_FIRST	2
#define PG_LAST		4

static struct ogg_page_hdr *read_page_header(FILE *fp);
static int skip_page(FILE *fp, struct ogg_page_hdr *hdr);


int rip_ogg(FILE *fp)
{
	long start, end;
	struct ogg_page_hdr *hdr;
	int pgnest = 0;
	int pgnum = 0;

	fseek(fp, -4, SEEK_CUR);
	start = ftell(fp);

	while((hdr = read_page_header(fp))) {
		if(!pgnum++) {
			/* a valid first page must have PG_FIRST set and PG_CONT unset */
			if(hdr->type & PG_CONT || !(hdr->type & PG_FIRST)) {
				return -1;
			}
		}

		if(skip_page(fp, hdr) == -1) {
			return -1;
		}

		if(hdr->type & PG_FIRST) {
			pgnest++;
		}

		if(hdr->type & PG_LAST && !--pgnest) {
			break;
		}
	}

	end = ftell(fp);
	fseek(fp, start, SEEK_SET);

	return dump(fp, end - start, "ogg", "OGG media file");
}

static struct ogg_page_hdr *read_page_header(FILE *fp)
{
	static char buf[sizeof(struct ogg_page_hdr) + 255];
	struct ogg_page_hdr *hdr;

	if(fread(buf, sizeof *hdr - 1, 1, fp) < 1) {
		return 0;
	}
	hdr = (struct ogg_page_hdr*)buf;

	if(fread(hdr->segm_table, 1, hdr->nsegm, fp) < hdr->nsegm) {
		return 0;
	}
	return hdr;
}

static int skip_page(FILE *fp, struct ogg_page_hdr *hdr)
{
	int i;

	for(i=0; i<hdr->nsegm; i++) {
		if(fseek(fp, hdr->segm_table[i], SEEK_CUR) == -1) {
			return -1;
		}
	}
	return 0;
}
