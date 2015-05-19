CXXFLAGS = -Wall -O2 -std=c++11
CXX = g++

CXX_FILES = $(shell echo *.cpp **/*.cpp | tr \  \\n | grep -v ^main.cpp\$)
H_FILES = *.h **/*.h
OBJ_FILES = $(CXX_FILES:.cpp=.o)

.PHONY: all clean remake

all: main

clean:
	rm -f main $(OBJ_FILES)

remake: clean all

main: main.cpp $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o main main.cpp $(OBJ_FILES)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $*.o $*.cpp
