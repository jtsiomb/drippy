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

#define NPREV	8
static char prevc[NPREV];

int nextc(FILE *fp)
{
	int c, i;

	if((c = fgetc(fp)) == EOF) {
		return -1;
	}

	for(i=0; i<NPREV-1; i++) {
		prevc[i] = prevc[i + 1];
	}
	prevc[NPREV-1] = c;

	return c;
}

int lastchars(const char *key, int num)
{
	if(memcmp(key, prevc + sizeof prevc - num, num) == 0) {
		return 1;
	}
	return 0;
}

void reset_prev(void)
{
	memset(prevc, 0, sizeof prevc);
}
