CC = gcc

INCLUDES = -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

LD_FLAGS = -lT-Rex -lglib-2.0 -lm

COMMON_CFLAGS = $(INCLUDES) -march=native -O2 -pedantic -pedantic-errors -Wall -Wextra -Werror
SHARED_LIBRARY_CFLAGS = $(COMMON_CFLAGS) -fPIC -shared 
RUN_MINICHESS_CFLAGS = $(COMMON_CFLAGS)

SHARED_LIBRARY_SOURCE = $(wildcard src/data_tier/*.c) \
	$(wildcard src/logic_tier/*.c) \
	$(wildcard src/presentation_tier/*.c) 

RUN_MINICHESS_SOURCE = $(wildcard src/data_tier/*.c) \
	$(wildcard src/logic_tier/*.c) \
	$(wildcard src/presentation_tier/*.c) \
	$(wildcard src/*.c)
	      
SHARED_LIBRARY_OBJECTS = $(SHARED_LIBRARY_SOURCE:.c=.o)
RUN_MINICHESS_OBJECTS = $(RUN_MINICHESS_SOURCE:.c=.o)

SHARED_LIBRARY_TARGET = libMinichess.so
RUN_MINICHESS_TARGET = Minichess

ifeq ($(library),true)
	CFLAGS = $(SHARED_LIBRARY_CFLAGS)
	OBJECTS = $(SHARED_LIBRARY_OBJECTS)
	TARGET = $(SHARED_LIBRARY_TARGET)
else
	CFLAGS = $(RUN_MINICHESS_CFLAGS)
	OBJECTS = $(RUN_MINICHESS_OBJECTS)
	TARGET = $(RUN_MINICHESS_TARGET)
endif

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

clean:
	$(RM) $(SHARED_LIBRARY_TARGET) $(RUN_MINICHESS_TARGET) $(SHARED_LIBRARY_OBJECTS) $(RUN_MINICHESS_OBJECTS)
	