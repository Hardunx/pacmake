#    Copyright (C) 2025  Hardunx
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC      = ccache gcc
LD      = $(CC)
CFLAGS  = -Wall -Wextra -O3 -march=native -flto=auto -pipe -msse -msse2 -msse3 -msse4.1 -fsanitize=address -g
LDFLAGS = -flto=auto

SRC     = $(shell find . -name '*.c')
OBJ     = $(SRC:.c=.o)
TARGET  = pacmake

all: $(TARGET)
run: runner

$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJ)
%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@
runner: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
