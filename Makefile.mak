CC = gcc
CXX = g++

glfw = C:/Users/Rich/Documents/Libraries/glfw
glfw_inc = $(glfw)/include
glfw_lib = $(glfw)/lib-mingw-w64

glad = C:/Users/Rich/Documents/Libraries/glad
glad_inc = $(glad)/include
glad_src = $(glad)/src

glm_inc = C:/Users/Rich/Documents/Libraries/glm

inc = C:/Dev/OpenGL/inc

INCLUDES = -I$(inc) -I$(glfw_inc) -I$(glad_inc) -I$(glm_inc)
LIBRARIES = -L$(glfw_lib)

CFLAGS = -Wall -ggdb -O3 $(INCLUDES)
CXXFLAGS = -Wall -ggdb -O3 $(INCLUDES)
LDFLAGS = $(LIBRARIES) -lglfw3 -lopengl32 -lglu32 -lgdi32

TARGET = app.exe
obj_dir = obj
src = src

rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
cpp_files = $(call rwildcard,$(src)/,*.cpp)
#cpp_files = src/app.cpp src/core/window.cpp src/core/vendor.cpp src/core/input.cpp src/renderer/shader.cpp src/renderer/shaderProgram.cpp
objects = $(cpp_files:.cpp=.o) src/glad.o
headers =

all: $(TARGET)

$(TARGET): $(objects)
		$(CXX) -o bin/app.exe $^ $(LDFLAGS)

.PHONY : clean
clean :
		-rm $(TARGET) $(objects)
