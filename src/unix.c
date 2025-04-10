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
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

void rmrdir(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    char fullpath[1024];
    struct stat statbuf;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                rmrdir(fullpath);
            } else {
                if (remove(fullpath) != 0) {
                    perror("remove");
                }
            }
        }
    }

    closedir(dir);
    
    if (rmdir(path) != 0) {
        perror("rmdir");
    }
}

