CXX = g++
INCLUDES = -I./include -I./src -I./src/imgui

# Notice: No cimgui here, and main is now .cpp
SOURCES = src/main.cpp \
          src/imgui/imgui.cpp \
          src/imgui/imgui_draw.cpp \
          src/imgui/imgui_widgets.cpp \
          src/imgui/imgui_tables.cpp \
          src/imgui/imgui_impl_glfw.cpp \
          src/imgui/imgui_impl_opengl3.cpp

LIBS = -L./lib -lglfw3 -lgdi32 -lopengl32 -lwinmm -mwindows -static

all: school_bell.exe

school_bell.exe:
	$(CXX) $(INCLUDES) $(SOURCES) -o $@ $(LIBS)