CXXFLAGS_BASE = -Wall -std=c++11
ifdef DEBUG
	CXXFLAGS = $(CXXFLAGS_BASE) -g
else
	CXXFLAGS = $(CXXFLAGS_BASE) -O2
endif
CXX = g++

CXX_FILES = $(wildcard *.cpp **/*.cpp)
H_FILES = $(wildcard *.h **/*.h)
OBJ_FILES = $(CXX_FILES:.cpp=.o)

.PHONY: all clean remake

all: main

clean:
	rm -f main $(OBJ_FILES)

remake: clean all

main: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o main $(OBJ_FILES)
	strip main

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $*.o $*.cpp
