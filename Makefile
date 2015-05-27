ifneq (,$(findstring /cygdrive/,$(PATH)))
    uname_S := Cygwin
else
ifneq (,$(findstring WINDOWS,$(PATH)))
    uname_S := Windows
else
    uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif
endif

EXTERN_DIRECTORY=ext
SOURCE_DIRECTORY=src
OBJECT_DIRECTORY=obj

CC=g++
# CFLAGS=-w -Wall
# CFLAGS=-w
# CFLAGS=-Wdeprecated-declarations
CFLAGS=-std=c++11




# # GLUT - Ubuntu
# ifeq ($(uname_S),Linux)
# 	INCLUDE_PATHS=-I$(EXTERN_DIRECTORY) -I/usr/include -I/usr/include/freetype2
# 	LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib
# 	LINKER_FLAGS=-lGL -lGLU -lglut -lGLEW -lfreetype -lglfw -lboost_graph
# endif

# # GLUT - OS X
# ifeq ($(uname_S),Darwin)
# 	INCLUDE_PATHS=-I$(EXTERN_DIRECTORY) -I/opt/local/include -I/opt/local/include/freetype2
# 	LIBRARY_PATHS=-L/opt/local/lib
# 	LINKER_FLAGS=-lglfw -lGLEW -framework OpenGL -lboost_graph-mt -lfreetype
# endif


# GLUT - Ubuntu
ifeq ($(uname_S),Linux)
	INCLUDE_PATHS=-I$(EXTERN_DIRECTORY) -I/usr/include
	LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib
	LINKER_FLAGS=-lGL -lGLU -lglut -lGLEW -lglfw -lboost_graph
endif

# GLUT - OS X
ifeq ($(uname_S),Darwin)
	INCLUDE_PATHS=-I$(EXTERN_DIRECTORY) -I/opt/local/include
	LIBRARY_PATHS=-L/opt/local/lib
	LINKER_FLAGS=-lglfw -lGLEW -framework OpenGL -lboost_graph-mt
endif

ifeq ($(uname_S),Windows)
	# Don't know yet
endif

ifeq ($(uname_S),Cygwin)
	# Don't know yet
endif






# # LINKER_FLAGS=-lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
# `freetype-config --libs`
# LINKER_FLAGS=-lglfw

# GLUT - OS X
# INCLUDE_PATHS=-I$(EXTERN_DIRECTORY) -I/opt/local/include -I/usr/local/include -I/opt/X11/include
# LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib -L/opt/X11/lib
# LINKER_FLAGS=-framework OpenGL -lGLUT -lGLEW
# # LINKER_FLAGS=-framework OpenGL -framework GLUT -lGLEW

# GLFW
# INCLUDE_PATHS=-I$(EXTERN_DIRECTORY) -I/opt/local/include -I/usr/local/include
# LIBRARY_PATHS=-L/usr/local/lib -L/opt/local/lib
# LINKER_FLAGS=-lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo







_DEPS = Graph/Graph.hpp Graph/DR_Plan.hpp \
	Graph/Pebbled_Graph.hpp Graph/Isostatic_Graph_Realizer.hpp \
	Graph/Tree.hpp Graph/Min_Priority_Queue.hpp \
	GUI/Main_GUI_Manager.hpp GUI/Vision_Handler_2D.hpp GUI/Program.hpp GUI/gl_objects.hpp GUI/easy_font.hpp GUI/utils.h \
	Application/App_Window_2D.hpp Application/Graph_Display_Window.hpp Application/DRP_Display_Window.hpp
DEPS = $(patsubst %,$(SOURCE_DIRECTORY)/%,$(_DEPS))


_OBJ = main.o \
	Graph/Graph.o Graph/DR_Plan.o Graph/Pebbled_Graph.o Graph/Isostatic_Graph_Realizer.o \
	GUI/Main_GUI_Manager.o GUI/Vision_Handler_2D.o GUI/Program.o GUI/utils.o
OBJ = $(patsubst %,$(OBJECT_DIRECTORY)/%,$(_OBJ))

EXEC = drp




$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

.PHONY: clean

clean:
	rm -f $(OBJECT_DIRECTORY)/*.o $(OBJECT_DIRECTORY)/GUI/*.o $(OBJECT_DIRECTORY)/Graph/*.o *~ core $(EXTERN_DIRECTORY)/*~ $(EXEC)
