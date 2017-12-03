/*
 * gpio-watch, a tool for running scripts in response to gpio events
 * Copyright (C) 2014 Lars Kellogg-Stedman <lars@oddbit.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fileutil.h"

// Return 1 if path is a regular file, 0 otherwise.
int is_file (const char *path) {
	struct stat buf;
	int err;

	err = stat(path, &buf);
	if (-1 == err) {
		return 0;
	}

	return S_ISREG(buf.st_mode);
}

// Return 1 if path is a directory, 0 otherwise.
int is_dir (const char *path) {
	struct stat buf;
	int err;

	err = stat(path, &buf);
	if (-1 == err) {
		return 0;
	}

	return S_ISDIR(buf.st_mode);
}

