# Variables
CC = gcc
CFLAGS = -Wall -I./switches -I./roll
LDFLAGS = -lncurses
TARGET = puzzle
SRC = puzzle.c switches/switches.c roll/roll.c
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Link the object file to create the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compile the source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Rebuild everything
rebuild: clean all