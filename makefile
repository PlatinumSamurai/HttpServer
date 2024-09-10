# Name of the target executable
TARGET := build.out

# Name of the target dynamic library
DLTARGET := http

# Source files
SOURCES := $(wildcard ./source/*.c)

# Main file
MAIN := test_http.c

# Object files
OBJECTS := $(SOURCES:.c=.o)

# Compiler
CXX := gcc

# Compiler flags
CXXFLAGS := -Wall

# Linker flags
LDFLAGS :=

# Dynamic library flags
DLFLAGS := -shared -fPIC

# Valgrind options
VALGRIND_OPTS := --leak-check=full --show-leak-kinds=all --track-origins=yes

# Default make target
all: build

# Compile step for source files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	$(CXX) -o lib$(DLTARGET).so $(DLFLAGS) $(SOURCES)
	export LD_LIBRARY_PATH=$(pwd)
	$(CXX) $(MAIN) -L. -l$(DLTARGET) -o $(TARGET)

memory: build
	valgrind $(VALGRIND_OPTS) ./$(TARGET)

run: build
	./$(TARGET)
	
clean:
	rm *.out

.PHONY: all build memory run clean
