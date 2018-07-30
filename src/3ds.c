/*
This file is part of "drippy", a datafile ripper.
Copyright (C) 2018 John Tsiombikas <nuclear@member.fsf.org>

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

enum {
	CID_MAIN3DS = 0x4d4d,
	CID_EDIT3DS = 0x3d3d,
	CID_KEYF3DS = 0xb000
};

struct chunk {
	uint16_t id;
	uint32_t size;
};

static int read_chunk_header(FILE *fp, struct chunk *c)
{
	if(fread(c, sizeof *c, 1, fp) < 1) {
		return -1;
	}
	return 0;
}

int rip_3ds(FILE *fp)
{
	struct chunk c;
	uint32_t size;
	long pos;

	pos = ftell(fp) - 2;

	if(read_chunk_header(fp, &c) == -1 || c.id != CID_MAIN3DS) {
		return -1;
	}
	size = c.size;

	if(pos + size > filesize) {
		return -1;
	}

	if(read_chunk_header(fp, &c) == -1) {
		return -1;
		if(c.id != CID_EDIT3DS || c.id != CID_KEYF3DS) {
			return -1;
		}
	}

	fseek(fp, pos, SEEK_SET);

	return dump(fp, size, "3ds", "3DS scene file");
}
