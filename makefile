# Имя исполняемого файла
TARGET = server

# Файлы исходного кода для сервера
SRC = server.c handle_app.c processing_req.c register_views.c views.c http_pars.c

# Файлы объектных файлов
OBJ = $(SRC:.c=.o)

# Флаги компилятора
CFLAGS = -Wall -g

# Правило для сборки исполняемого файла
$(TARGET): $(OBJ)
	gcc $(CFLAGS) -o $(TARGET) $(OBJ)

# Правило для компиляции каждого .c файла в .o файл
%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# Очистка объектных файлов и исполняемого файла
clean:
	rm -f $(OBJ) $(TARGET)