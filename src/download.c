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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "pacmake.h"

int pkgclone(struct repository repository, char* pkgname, char* destpath) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "git clone %s/%s.git %s", repository.url, pkgname, destpath);
    return system(cmd);
}

int download(struct repository repository, char* pkgname, char* destpath, int verbose) {
    char pkgdir[256];
    char pkgbuildf[264];
    char pkgmkfp[264];

    snprintf(pkgdir, sizeof(pkgdir), "%s/%s", destpath, pkgname);
    snprintf(pkgbuildf, sizeof(pkgbuildf), "%s/PKGBUILD", pkgdir);
    snprintf(pkgmkfp, sizeof(pkgmkfp), "%s/.pacmake", pkgdir);

    if (access(pkgbuildf, F_OK) == 0 && access(pkgdir, F_OK) == 0) {
        if (verbose) printf("%s: repository already cloned. Skipping.\n", PACMAKE);
    }

    if (access(pkgdir, F_OK) != 0) {
        if (pkgclone(repository, pkgname, pkgdir) != 0) {
            if (verbose) fprintf(stderr, "%s: error on cloning repository.\n", PACMAKE);
            exit(EXIT_FAILURE);
        }
    }
    if (access(pkgbuildf, F_OK) != 0) {
        rmrdir(pkgdir);
        if (pkgclone(repository, pkgname, pkgdir) != 0) {
            if (verbose) fprintf(stderr, "%s: error on cloning repository.\n", PACMAKE);
            exit(EXIT_FAILURE);
        }
    }

    FILE* pkgmkf;
    if (access(pkgmkfp, F_OK) != 0) {
        if (verbose) printf("%s: download warning: %s file not found. Creating file.\n", PACMAKE, pkgmkfp);
        pkgmkf = fopen(pkgmkfp, "w+");
        if  (pkgmkf == NULL) {
            if (verbose) fprintf(stderr, "%s download error: error when creating %s file.\n", PACMAKE, pkgmkfp);
            exit(EXIT_FAILURE);
        }
        fprintf(pkgmkf, "reposname=%s\nrepository=%s\nreposcolor=%s\npkgname=%s\n", repository.name, repository.url, repository.color, pkgname);
    }
    return 0;
}
