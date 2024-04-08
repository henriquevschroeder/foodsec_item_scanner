CC=gcc

DIST_DIR=src/dist

all: main display

$(shell mkdir -p $(DIST_DIR))

main: src/main.o
	$(CC) $(CFLAGS) -o $(DIST_DIR)/main src/main.o -lpthread

display: src/display.o
	$(CC) $(CFLAGS) -o $(DIST_DIR)/display src/display.o

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o && rm -rf $(DIST_DIR)
