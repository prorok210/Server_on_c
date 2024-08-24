MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -I./include

# Директории
SRC_DIR := $(MAKEFILE_DIR)src
BUILD_DIR := $(MAKEFILE_DIR)build
INCLUDE_DIR := $(MAKEFILE_DIR)include

# Исходные файлы
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Объектные файлы
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Имя исполняемого файла
TARGET = server

# Правило по умолчанию
all: $(BUILD_DIR) $(TARGET)

# Создание директории для объектных файлов
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Компиляция исполняемого файла
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

# Компиляция объектных файлов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Пересборка
rebuild: clean all

.PHONY: all clean rebuild