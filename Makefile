CC = gcc
CFLAGS = -I. -Wall
DEBUG_FLAGS = -g
SOURCES = main.c util.c image_util.c argparser.c
HEADERS = util.h image_util.h argparser.h
OUTPUT = dip
LIBS = -lm

# Default target
all: $(OUTPUT)

# Compile with optional debug flag
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(OUTPUT)

# Link the executable, including additional libraries
$(OUTPUT): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT) $(LIBS)

# Clean target to remove output files
clean:
	rm -f $(OUTPUT)

# Phony targets
.PHONY: all debug clean
