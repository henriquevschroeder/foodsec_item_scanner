CC=gcc

DIST_DIR=src/dist

# Adds pre_build rule as a dependency for main and display rules
all: pre_build main display

# Creates the DIST_DIR directory if it does not exist
$(shell mkdir -p $(DIST_DIR))

# New rule for build environment preparation
pre_build:
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
