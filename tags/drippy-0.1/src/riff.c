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

struct riff_chunk_hdr {
	uint32_t id;
	uint32_t size;
} __attribute__((packed));


int rip_avi(FILE *fp)
{
	uint32_t len;
	struct riff_chunk_hdr hdr;

	if(fread(&len, 4, 1, fp) < 1) {
		return -1;
	}

	if(fread(&hdr, sizeof hdr, 1, fp) < 1) {
		return -1;
	}
	if(hdr.id == FOURCC('A', 'V', 'I', ' ')) {
		fseek(fp, -(2 * sizeof hdr), SEEK_CUR);
		if(dump(fp, len + sizeof hdr, "avi", "Microsoft AVI video file") == -1) {
			return -1;
		}
	}
	return 0;
}

int rip_wav(FILE *fp)
{
	return -1;	/* TODO */
}

