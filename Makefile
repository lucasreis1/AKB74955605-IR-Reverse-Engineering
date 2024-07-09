CXX=g++

CXXFLAGS=-std=c++20
LIBS=-lboost_program_options

EXE=acsender

all: $(EXE)

$(EXE): acsender.cpp
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LIBS)

clean:
	rm $(EXE)
	
