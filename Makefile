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
