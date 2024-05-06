CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lGL -lGLU -lglut
SOURCES=pool.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=pool

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
        $(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
        $(CC) $(CFLAGS) $< -o $@

clean:
        rm -rf $(OBJECTS) $(EXECUTABLE)

