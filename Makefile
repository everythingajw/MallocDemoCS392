CC = gcc
CFLAGS = -Wall -Werror
OUT_DIR = ./build

.PHONY: all build directories run clean

all: directories build

build: directories
	$(CC) $(CFLAGS) main.c -o $(OUT_DIR)/main

run: build
	@$(OUT_DIR)/main

directories:
	@$(shell [ ! -d $(OUT_DIR) ] && mkdir -p -- $(OUT_DIR))

clean:
	find "$(OUT_DIR)" -mindepth 1 -delete
