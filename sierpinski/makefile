CC=g++
GCC_OPTIONS=-c -Wall -pedantic
GL_OPTIONS=-lGLEW -lglut -lGL  -lXmu -lX11  -lm
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)
SOURCES=sierpinski.cpp initShader.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sierpinski

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(GL_OPTIONS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(GCC_OPTIONS) $< -o $@

clean:
	rm *.o sierpinski
