# Define the compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Define the target executable
TARGET = kilo

# Define the source files
SRCS = kilo.c rawMode.c errorHandler.c input.c output.c editor.c appendBuffer.c

# Define the object files (by replacing .c with .o for each source file)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

# Default target
all: $(TARGET)

# Default target: compile the program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Header dependencies (Now correctly tied to the pattern rule below)
kilo.o: kilo.c rawMode.h editor.h input.h output.h
editor.o: editor.c editor.h rawMode.h errorHandler.h
rawMode.o: rawMode.c rawMode.h errorHandler.h
errorHandler.o: errorHandler.c errorHandler.h
input.o: input.c input.h editor.h errorHandler.h
output.o: output.c output.h editor.h appendBuffer.h
appendBuffer.o: appendBuffer.c appendBuffer.h errorHandler.h

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
