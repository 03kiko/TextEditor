# Define the compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Define the target executable
TARGET = kilo

# Define the source files
SRCS = kilo.c rawMode.c errorHandler.c

# Define the object files (by replacing .c with .o for each source file)
OBJS = $(SRCS:.c=.o)

# Default target: compile the program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
