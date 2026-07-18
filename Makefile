CC := gcc
CFLAGS := -Wall -O2

# Dynamically detect if we are inside the KernelDriver monorepo or standalone
ifneq ($(wildcard ../../src/main.c),)
    # Monorepo mode
    ROOT_DIR := $(abspath ../..)
    RELEASE_DIR := $(ROOT_DIR)/release
else
    # Standalone submodule mode
    RELEASE_DIR := release
endif
SRC_DIR := src
BUILD_DIR := build

TARGET := $(RELEASE_DIR)/AcerBattery
SRC := $(SRC_DIR)/main.c
OBJ := $(BUILD_DIR)/main.o

all: prep $(TARGET)

prep:
	@mkdir -p $(BUILD_DIR) $(RELEASE_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built AcerBattery utility at $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all prep clean
