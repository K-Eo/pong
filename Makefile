CC = gcc

FLAGS 			= -std=gnu99 -Iinclude
CFLAGS 			= -pedantic -Wall -Wextra -march=native -ggdb3
DEBUGFLAGS 		= -O0 -D _DEBUG
RELEASEFLAGS 	= -O2 -D NDEBUG
POSTFLAGS 		= -lncurses -lSDL2 -lm

TARGET = pong
SOURCES = $(shell echo src/*.c)
OBJECTS = $(SOURCES:.c=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS) $(POSTFLAGS)

release: $(SOURCES)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(POSTFLAGS)

install: release
	install -D $(TARGET) $(BINDIR)/$(TARGET)

install-strip: release
	install -D -s $(TARGET) $(BINDIR)/$(TARGET)

uninstall:
	-rm $(BINDIR)/$(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

%.o: %.c
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $<
