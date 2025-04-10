#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "pacmake.h"

int build(char* pkgdir, int verbose, ...) {
    va_list args;
    va_start(args, verbose);
    char cmd[1024];
    char cargs[512];
    char pkgbuildf[256];

    cargs[0] = '\0';

    snprintf(pkgbuildf, sizeof(pkgbuildf), "%s/PKGBUILD", pkgdir);

    char* arg;
    while ((arg = va_arg(args, char*)) != NULL) {
        if (strlen(cargs) + strlen(arg) + 2 > sizeof(cargs)) {
            if (verbose) fprintf(stderr, "%s: build error: arguments exceed buffer size.\n", PACMAKE);
            va_end(args);
            return -1;
        }
        strcat(cargs, " ");
        strcat(cargs, arg);
    }
    va_end(args);

    snprintf(cmd, sizeof(cmd), "makepkg %s", cargs);

    if (access(pkgbuildf, F_OK) != 0) {
        if (verbose) fprintf(stderr, "%s: build error: %s not found.\n", PACMAKE, pkgbuildf);
        return -1;
    }
    if (chdir(pkgdir) != 0) {
        if (verbose) fprintf(stderr, "%s: build error: failed to change directory to %s.\n", PACMAKE, pkgdir);
        return -1;
    }
    return system(cmd);
}
