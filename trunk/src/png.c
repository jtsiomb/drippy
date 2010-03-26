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
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>	/* for htons/htonl */
#include "common.h"

struct chunk {
	uint32_t len;
	char type[4];
	unsigned char data[1];
} __attribute__((packed));

struct png_hdr {
	uint32_t width, height;
	unsigned char depth, ctype;
	unsigned char cmp, filt, ilace;
} __attribute__((packed));

static int read_chunk_header(FILE *fp, struct chunk *c);
static int skip_chunk(FILE *fp, struct chunk *c);
static struct chunk *read_chunk(FILE *fp);
static void free_chunk(struct chunk *c);

int rip_png(FILE *fp)
{
	struct chunk *chunk, chdr;
	struct png_hdr *ihdr;
	long pos, end;
	int found_iend = 0;

	pos = ftell(fp) - 8;

	if(!(chunk = read_chunk(fp))) {
		return -1;
	}

	if(memcmp(chunk->type, "IHDR", 4) != 0) {
		free_chunk(chunk);
		return -1;
	}
	ihdr = (struct png_hdr*)chunk->data;

	if(verbose) {
		printf("found PNG image file at offset: %lxh\n", pos);
		printf("  size: %dx%d\n", ntohl(ihdr->width), ntohl(ihdr->height));
	}
	free_chunk(chunk);

	while(read_chunk_header(fp, &chdr) != -1) {
		if(skip_chunk(fp, &chdr) == -1) {
			return -1;
		}
		if(memcmp(chdr.type, "IEND", 4) == 0) {
			found_iend = 1;
			break;
		}
	}
	if(!found_iend) {
		return -1;
	}

	end = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return dump(fp, end - pos, "png", "PNG image file");
}

static int read_chunk_header(FILE *fp, struct chunk *c)
{
	if(fread(c, sizeof *c - 1, 1, fp) < 1) {
		return -1;
	}
	c->len = ntohl(c->len);

	return 0;
}

static int skip_chunk(FILE *fp, struct chunk *c)
{
	return fseek(fp, c->len + 4, SEEK_CUR);
}

static struct chunk *read_chunk(FILE *fp)
{
	struct chunk hdr;
	struct chunk *c;

	if(read_chunk_header(fp, &hdr) == -1) {
		return 0;
	}
	if(!(c = malloc(sizeof hdr - 1 + hdr.len))) {
		return 0;
	}
	memcpy(c, &hdr, sizeof hdr - 1);

	if(fread(c->data, 1, hdr.len, fp) < hdr.len) {
		free(c);
		return 0;
	}
	fseek(fp, 4, SEEK_CUR);	/* skip CRC */
	return c;
}

static void free_chunk(struct chunk *c)
{
	free(c);
}
