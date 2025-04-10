/*
    Copyright (C) 2025  Hardunx

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PACMAKE_PACMAKE_H
#define PACMAKE_PACMAKE_H

#include <stdio.h>

struct repository {
  char *name;
  char *url;
  char *color;
};

struct package {
  char *name;
  char *version;
  struct repository *repository;
};

int pkgclone(struct repository repository, char *pkgname, char *destpath);
int download(struct repository repository, char *pkgname, char *destpath, int verbose);
int build(char *pkgdir, int verbose, ...);
struct package install(char *pkgdir, int verbose, ...);
struct package parse_pacmake(FILE *file, int verbose);
char** parse_depends(FILE* pkgbuildf, int verbose);

void rmrdir(const char *path);

#define NONEPKG (struct package){0, 0, 0}
#define NONEREPOS (struct repository){0, 0, 0}
#define PACMAKE "\033[34;1mpac\033[31;1mmake\033[m"

#endif // PACMAKE_PACMAKE_H
