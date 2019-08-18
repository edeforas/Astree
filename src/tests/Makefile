CXXFLAGS = -g -Wall -Wfatal-errors -std=c++17

ALL = test_newton test_perfect_surface test_scaling

all: $(ALL)

test_newton: test_newton.cpp Makefile
	$(CXX) $(CXXFLAGS) test_newton.cpp -I.. ../*.cpp -o test_newton -lm

test_perfect_surface: test_perfect_surface.cpp Makefile
	$(CXX) $(CXXFLAGS) test_perfect_surface.cpp -I.. ../*.cpp -o test_perfect_surface -lm

test_scaling: test_scaling.cpp Makefile
	$(CXX) $(CXXFLAGS) test_scaling.cpp -I.. ../*.cpp -o test_scaling -lm

clean:
	$(RM) $(ALL) *.o

test: all
	./test_newton && ./test_perfect_surface && ./test_scaling
