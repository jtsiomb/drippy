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

struct bink_hdr {
	char sig[3], codec;
	uint32_t size;
	uint32_t num_frames, max_frame_sz;
	uint32_t pad1;
	uint32_t width, height;
	uint32_t fps_num, fps_den;
	uint32_t flags;
	uint32_t num_audio_tracks;
} __attribute__((packed));

int rip_bink(FILE *fp)
{
	struct bink_hdr hdr;
	long pos;

	fseek(fp, -3, SEEK_CUR);
	pos = ftell(fp);

	if(fread(&hdr, sizeof hdr, 1, fp) < 1) {
		return -1;
	}

	if(hdr.codec < 'b' || hdr.codec > 'z') {
		return -1;
	}
	if(hdr.size + 4 + pos > filesize) {
		return -1;
	}
	if(hdr.width > 32767 || hdr.height > 32767) {
		return -1;
	}

	if(verbose) {
		printf("found RAD Game Tools Bink video file at offset: %lxh\n", pos);
		printf("  size: %dx%d\n", hdr.width, hdr.height);
		printf("  framerate: %.2f\n", (float)hdr.fps_num / (float)hdr.fps_den);
	}

	fseek(fp, pos, SEEK_SET);
	if(dump(fp, hdr.size + 8, "bik", "RAD Game Tools Bink video file") == -1) {
		return -1;
	}
	return 0;
}

