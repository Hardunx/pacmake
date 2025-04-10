#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "pacmake.h"

struct package install(char* pkgdir, int verbose, ...) {
    va_list args;
    va_start(args, verbose);

    char cmd[1024];
    char cargs[512] = "";
    char vercmd[512];
    char pkgver[256];
    char pkgbuildpath[512];
    char pacmakepath[512];

    char* arg;
    while ((arg = va_arg(args, char*)) != NULL) {
        strncat(cargs, arg, sizeof(cargs) - strlen(cargs) - 1);
        strncat(cargs, " ", sizeof(cargs) - strlen(cargs) - 1);
    }
    snprintf(cmd, sizeof(cmd), "makepkg -i %s", cargs);
    snprintf(pkgbuildpath, sizeof(pkgbuildpath), "%s/PKGBUILD", pkgdir);

    chdir(pkgdir);
    int cmdret = system(cmd);

    if (cmdret != 0) {
        if (verbose) fprintf(stderr, "%s: install error: makepkg error.\n", PACMAKE);
        return NONEPKG;
    }

    FILE* pkgbuildf = fopen(pkgbuildpath, "r");
    if (pkgbuildf == NULL) {
        if (verbose) fprintf(stderr, "%s: install error: failure on opening pacmake file.\n", PACMAKE);
        return NONEPKG;
    }

    struct package pkg = parse_pacmake(pkgbuildf, 1);
    snprintf(vercmd, sizeof(vercmd), "pacman -Q %s", pkg.name);

    FILE* versionp = popen(vercmd, "r");
    if (versionp == NULL) {
        if (verbose) fprintf(stderr, "%s: install error: failure on getting version.\n", PACMAKE);
        return NONEPKG;
    }

    char dummy[256];
    fscanf(versionp, "%255s %255s", dummy, pkgver);
    pkg.version = pkgver;

    snprintf(pacmakepath, sizeof(pacmakepath), "%s/.pacmake", pkgdir);

    FILE* pacmakef = fopen(pacmakepath, "r+");
    if (pacmakef == NULL) {
        if (verbose) fprintf(stderr, "%s: install error: failure on opening pacmake file.\n", PACMAKE);
        return NONEPKG;
    }

    char pkgmkcontent[1536];
    char line[256];

    int pkgver_found = 0;
    size_t offset = 0;

    while (fgets(line, sizeof(line), pacmakef)) {
        size_t len = strlen(line);
        if (offset + len < sizeof(pkgmkcontent)) {
            memcpy(pkgmkcontent + offset, line, len);
            offset += len;
            pkgmkcontent[offset] = '\0';
        }
        if (strstr(line, "pkgver=") != NULL) {
            pkgver_found = 1;
        }
    }

    if (!pkgver_found) {
        snprintf(pkgmkcontent + offset, sizeof(pkgmkcontent) - offset, "pkgver=%s\n", pkgver);
        fprintf(pacmakef, "%s", pkgmkcontent);
    }

    fclose(pacmakef);

    return pkg;
}
