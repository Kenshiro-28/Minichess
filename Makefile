CC = gcc

INCLUDES = -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

LD_FLAGS = -lT-Rex -lglib-2.0

CFLAGS = $(INCLUDES) -O3 -pedantic -pedantic-errors -Wall -Wextra -Werror

SOURCE = $(wildcard src/data_tier/*.c) \
	$(wildcard src/logic_tier/*.c) \
	$(wildcard src/presentation_tier/*.c) \
	$(wildcard src/*.c)

OBJECTS = $(SOURCE:.c=.o)

TARGET = Minichess

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

clean:
	$(RM) $(TARGET) $(OBJECTS)
