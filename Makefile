CC=gcc

DIST_DIR=src/dist

all: pre_build main display

$(shell mkdir -p $(DIST_DIR))

pre_build:ß
	@echo "Checking and removing /tmp/foodsec_pip if it exists..."
	@rm -f /tmp/foodsec_pip

main: src/main.o
	$(CC) $(CFLAGS) -o $(DIST_DIR)/main src/main.o -lpthread

display: src/display.o
	$(CC) $(CFLAGS) -o $(DIST_DIR)/display src/display.o

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o && rm -rf $(DIST_DIR)
