CC=gcc
CFLAGS=-Wall -Wextra -std=c11
TARGET=vingc
SOURCE=vingc.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Статическая сборка (больший размер, но не требует библиотек)
static: $(SOURCE)
	$(CC) $(CFLAGS) -static -o $(TARGET) $(SOURCE)

# Сборка для Windows (требует MinGW)
windows:
	@if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then \
		x86_64-w64-mingw32-gcc $(CFLAGS) -o vingc.exe $(SOURCE); \
		echo "✅ Создан vingc.exe для Windows"; \
	elif command -v i686-w64-mingw32-gcc >/dev/null 2>&1; then \
		i686-w64-mingw32-gcc $(CFLAGS) -o vingc.exe $(SOURCE); \
		echo "✅ Создан vingc.exe для Windows"; \
	else \
		echo "❌ MinGW не найден. Установите: sudo pacman -S mingw-w64-gcc"; \
		exit 1; \
	fi

clean:
	rm -f $(TARGET) vingc.exe output.html

test: $(TARGET)
	./$(TARGET) example.ving example.html

.PHONY: all static windows clean test

