BINARY=subd
CFLAGS=-c -Wall -I../include/ --std=c++0x -I/usr/include/GL -I/usr/include -O2
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
UNAME=$(shell uname)

ifeq ($(UNAME), Darwin)
CC=clang++
CFLAGS+=-arch x86_64
LDFLAGS=-lm -framework OpenGL -framework GLUT
else
CC=g++
LDFLAGS=-L/usr/local/lib -L/usr/X11/lib -lm -lglut -lGL -lGLU
endif

all: $(SOURCES) $(BINARY)
	
$(BINARY): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f ${OBJECTS} ${BINARY}
