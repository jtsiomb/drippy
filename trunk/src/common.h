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
#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <inttypes.h>

#define FOURCC(a, b, c, d)	\
	(((uint32_t)a) | (((uint32_t)b) << 8) | \
	 (((uint32_t)c) << 16) | (((uint32_t)d) << 24))

long filesize;
int verbose;

int nextc(FILE *fp);
int lastchars(const char *key, int num);
void reset_prev(void);

/* in drip.c */
int dump(FILE *in, size_t len, const char *suffix, const char *desc);

/* format rippers */
int rip_avi(FILE *fp);
int rip_wav(FILE *fp);
int rip_ogg(FILE *fp);
int rip_bink(FILE *fp);
int rip_smacker(FILE *fp);
int rip_jpeg(FILE *fp);
int rip_png(FILE *fp);

#endif	/* COMMON_H_ */
