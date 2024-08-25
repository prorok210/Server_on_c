MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

include conandeps.mk

# Компилятор
CC = gcc

# Пути к заголовочным файлам: локальные и предоставленные Conan
INCLUDE_PATHS = -I./include -I$(CONAN_INCLUDE_PATH) -mconsole

# Флаги компиляции с добавлением всех путей к заголовочным файлам
CFLAGS = -Wall -Wextra $(INCLUDE_PATHS)

# Флаги для линковки
LDFLAGS = $(shell grep '^CONAN_LDFLAGS=' conandeps.mk | cut -d '=' -f 2)

# Директории
SRC_DIR := $(MAKEFILE_DIR)src
BUILD_DIR := $(MAKEFILE_DIR)build

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

# Компиляция исполняемого файла с добавлением LDFLAGS
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

# Компиляция объектных файлов
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Пересборка
rebuild: clean all

.PHONY: all clean rebuild