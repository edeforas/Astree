CXXFLAGS = -g -Wall -Wfatal-errors -std=c++17

ALL = tests

all: $(ALL)

clean:
	$(RM) $(ALL) *.o

tests:
	cd ./src/tests && make
	
test: all
	cd ./src/tests && make test
