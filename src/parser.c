#include "pacmake.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 65535
#define BUFFER_SIZE 255

static char *substr(const char *str, int start, int len, int verbose) {
    char *result = malloc(len + 1);
    if (!result) {
        if (verbose )fprintf(stderr, "%s: parser error: allocation error for substr function.\n", PACMAKE);
        exit(EXIT_FAILURE);
    }
    strncpy(result, str + start, len);
    result[len] = '\0';
    return result;
}

struct package parse_pacmake(FILE *file, int verbose) {
    if (file == NULL) {
        if (verbose) fprintf(stderr, "%s: parser error: failure to open pacmake file.\n", PACMAKE);
        return NONEPKG;
    }

    struct repository *out_repos = malloc(sizeof(struct repository));
    if (!out_repos) {
        if (verbose) fprintf(stderr, "%s: parser error: allocation error for repository.\n", PACMAKE);
        exit(EXIT_FAILURE);
    }
    out_repos->name = NULL;
    out_repos->url = NULL;
    out_repos->color = NULL;

    struct package out_pkg;
    out_pkg.name = NULL;
    out_pkg.repository = out_repos;
    out_pkg.version = NULL;

    char line[MAX_LINE_SIZE];

    regex_t regex_reposname, regex_repository, regex_reposcolor,
            regex_pkgname, regex_pkgver;

    regcomp(&regex_reposname, "^reposname=(.*)$", REG_EXTENDED);
    regcomp(&regex_repository, "^repository=(.*)$", REG_EXTENDED);
    regcomp(&regex_reposcolor, "^reposcolor=(.*)$", REG_EXTENDED);
    regcomp(&regex_pkgname, "^pkgname=(.*)$", REG_EXTENDED);
    regcomp(&regex_pkgver, "^pkgver=(.*)$", REG_EXTENDED);

    while (fgets(line, sizeof(line), file)) {
        regmatch_t matches[2];

        if (regexec(&regex_reposname, line, 2, matches, 0) == 0) {
            free(out_repos->name);
            out_repos->name = substr(line, matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so, verbose);
        }
        if (regexec(&regex_repository, line, 2, matches, 0) == 0) {
            free(out_repos->url);
            out_repos->url = substr(line, matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so, verbose);
        }
        if (regexec(&regex_reposcolor, line, 2, matches, 0) == 0) {
            free(out_repos->color);
            out_repos->color = substr(line, matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so, verbose);
        }
        if (regexec(&regex_pkgname, line, 2, matches, 0) == 0) {
            free(out_pkg.name);
            out_pkg.name = substr(line, matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so, verbose);
        }
        if (regexec(&regex_pkgver, line, 2, matches, 0) == 0) {
            free(out_pkg.version);
            out_pkg.version = substr(line, matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so, verbose);
        }
    }

    regfree(&regex_reposname);
    regfree(&regex_repository);
    regfree(&regex_reposcolor);
    regfree(&regex_pkgname);
    regfree(&regex_pkgver);

    return out_pkg;
}

char *strip_version(const char *dep, int verbose) {
    char *result = NULL;
    char *pos = strstr(dep, ">=");
    if (pos != NULL) {
        size_t len = pos - dep;
        result = malloc(len + 1);
        if (!result) {
            if (verbose) fprintf(stderr, "%s: parser error: allocation failure in strip_version.\n", PACMAKE);
            exit(EXIT_FAILURE);
        }
        strncpy(result, dep, len);
        result[len] = '\0';
        return result;
    }
    pos = strstr(dep, "<");
    if (pos != NULL) {
        size_t len = pos - dep;
        result = malloc(len + 1);
        if (!result) {
            if (verbose) fprintf(stderr, "%s: parser error: allocation failure in strip_version.\n", PACMAKE);
            exit(EXIT_FAILURE);
        }
        strncpy(result, dep, len);
        result[len] = '\0';
        return result;
    }
    return strdup(dep);
}

char **parse_depends(FILE *pkgbuildf, int verbose) {
    if (pkgbuildf == NULL) {
        if (verbose)
            fprintf(stderr, "pacmake: parser error: failure on opening PKGBUILD file.\n");
        return NULL;
    }

    char **depends = malloc(sizeof(char *) * 256);
    if (!depends) {
        if (verbose)
            fprintf(stderr, "%s: parser error: allocation failure for depends array.\n", PACMAKE);
        exit(EXIT_FAILURE);
    }
    int count = 0;
    int in_dep_block = 0;
    char line[BUFFER_SIZE];

    while (fgets(line, sizeof(line), pkgbuildf)) {
        if (!in_dep_block && (strstr(line, "depends=(") || strstr(line, "makedepends=("))) {
            in_dep_block = 1;
            continue;
        }

        if (in_dep_block) {
            char *ptr = line;
            if (strchr(line, ')') != NULL) {
                in_dep_block = 0;
                continue;
            }

            while (*ptr) {
                while (*ptr == ' ' || *ptr == '\t')
                    ptr++;
                if (*ptr == '\'' || *ptr == '\"') {
                    char quote = *ptr++;
                    char buffer[BUFFER_SIZE];
                    int i = 0;
                    while (*ptr && *ptr != quote && i < BUFFER_SIZE - 1)
                        buffer[i++] = *ptr++;
                    buffer[i] = '\0';
                    if (*ptr == quote)
                        ptr++;
                    
                    char *dep_clean = strip_version(buffer, verbose);

                    int is_duplicate = 0;
                    for (int j = 0; j < count; j++) {
                        if (strcmp(depends[j], dep_clean) == 0) {
                            is_duplicate = 1;
                            free(dep_clean);
                            break;
                        }
                    }
                    if (!is_duplicate)
                        depends[count++] = dep_clean;
                } else {
                    ptr++;
                }
            }
        }
    }
    depends[count] = NULL;
    return depends;
}
