IDIR=ext
SOURCE_DIRECTORY=src
OBJECT_DIRECTORY=obj

CC=g++
# CFLAGS=-w -Wall
# CFLAGS=-w
CFLAGS=-Wdeprecated-declarations

# # GLUT - Ubuntu
# INCLUDE_PATHS=-I$(IDIR)
# LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib -L/opt/X11/lib
# LINKER_FLAGS=-lGL -lGLU -lglut -lGLEW

# # GLUT - OS X
# INCLUDE_PATHS=-I$(IDIR) -I/opt/local/include -I/usr/local/include -I/opt/X11/include
# LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib -L/opt/X11/lib
# LINKER_FLAGS=-framework OpenGL -lGLUT -lGLEW
# # LINKER_FLAGS=-framework OpenGL -framework GLUT -lGLEW

# GLFW
# INCLUDE_PATHS=-I$(IDIR) -I/opt/local/include -I/usr/local/include
# LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib
# LINKER_FLAGS=-lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

INCLUDE_PATHS=-I$(IDIR) -I/opt/local/include
LIBRARY_PATHS=-L/opt/local/lib
# LINKER_FLAGS=-lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
LINKER_FLAGS=-lglfw -lGLEW -framework OpenGL
# LINKER_FLAGS=-lglfw


_DEPS = Graph/Graph.hpp \
	GUI/MainGuiManager.hpp GUI/program.hpp GUI/Circle.hpp GUI/utils.h
DEPS = $(patsubst %,$(SOURCE_DIRECTORY)/%,$(_DEPS))

_OBJ = main.o \
	Graph/Graph.o \
	GUI/MainGuiManager.o GUI/program.o GUI/utils.o
OBJ = $(patsubst %,$(OBJECT_DIRECTORY)/%,$(_OBJ))

EXEC = drp


$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

.PHONY: clean

clean:
	rm -f $(OBJECT_DIRECTORY)/*.o $(OBJECT_DIRECTORY)/GUI/*.o $(OBJECT_DIRECTORY)/Graph/*.o *~ core $(IDIR)/*~ $(EXEC)
