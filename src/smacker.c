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
#include "common.h"

struct smk_hdr {
	uint32_t sig;
	uint32_t width, height;
	uint32_t frames;
	uint32_t framerate;
	uint32_t flags;
	uint32_t audio_size[7];
	uint32_t trees_size;
	uint32_t mmap_size;
	uint32_t mclr_size;
	uint32_t full_size;
	uint32_t type_size;
	uint32_t audio_rate[7];
	uint32_t dummy;
} __attribute__((packed));

/* TODO 0 for big endian */
#define VER_SHIFT	24

int rip_smacker(FILE *fp)
{
	int i;
	long pos;
	uint32_t *frmsz;
	size_t datasz;
	struct smk_hdr hdr;

	fseek(fp, -3, SEEK_CUR);
	pos = ftell(fp);

	if(fread(&hdr, sizeof hdr, 1, fp) < 1) {
		return -1;
	}

	if(((hdr.sig >> VER_SHIFT) & 0xff) != '2' && ((hdr.sig >> VER_SHIFT) & 0xff) != '4') {
		return -1;
	}

	if(verbose) {
		printf("found RAD Game Tools Smacker video file at offset: %lxh\n", pos);
		printf("  size: %dx%d\n", hdr.width, hdr.height);
	}

	/* populate the frame sizes array */
	if(!(frmsz = malloc(hdr.frames * sizeof *frmsz))) {
		return -1;
	}
	if(fread(frmsz, sizeof *frmsz, hdr.frames, fp) < hdr.frames) {
		free(frmsz);
		return -1;
	}

	/* header + huffman trees + frame-size data + frame-types data to start with */
	datasz = sizeof hdr + hdr.trees_size + hdr.frames * 4 + hdr.frames;
	for(i=0; i<hdr.frames; i++) {
		datasz += frmsz[i] & ~1;
	}
	free(frmsz);

	fseek(fp, pos, SEEK_SET);
	if(dump(fp, datasz, "smk", "RAD Game Tools Smacker video file") == -1) {
		return -1;
	}
	return 0;
}
