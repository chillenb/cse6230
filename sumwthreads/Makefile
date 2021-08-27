CC = gcc
CXX = g++

CFLAGS = -g -Wall -fopt-info-all
CXXFLAGS = -std=c++20
OPTFLAGS = -O3 -march=native -mtune=native -ffast-math
LIBS = -lpthread

% : %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@ $(LIBS)

% : %.cpp
	$(CXX) $(CFLAGS) $(OPTFLAGS) $(CXXFLAGS) $< -o $@ $(LIBS)

ALL=sum

all: $(ALL)

clean:
	rm -f $(ALL)

