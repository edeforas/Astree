CXXFLAGS = -g -Wall -Wfatal-errors -std=c++17

ALL = test_newton

all: $(ALL)

test_newton: test_newton.cpp Makefile
	$(CXX) $(CXXFLAGS) test_newton.cpp -I.. ../*.cpp -o test_newton -lm

clean:
	$(RM) $(ALL) *.o

test: all
	./test_newton
