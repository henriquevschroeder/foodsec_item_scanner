# Compiler
CC=gcc

CFLAGS=-Wall

# Executable name
TARGET=program

# Object files
OBJS=src/main.o src/belt.o src/display.o

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lpthread

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJS)

# Dependencies
main.o: src/main.c src/belt.h src/display.h
belt.o: src/belt.c src/belt.h
display.o: src/display.c src/display.h
