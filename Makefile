.PHONY:all clean run debug build
all:build
build:tchess
NAMES=$(patsubst %.cpp,%,$(wildcard *.cpp))
tchess:$(NAMES:=.o)
	g++ -std=c++11 -g -Wall -o $@ $^
%.o:%.cpp $(wildcard *.h)
	g++ -std=c++11 -g -Wall -c $< -o $@
clean:
	rm -f tchess *.o
run:tchess
	./tchess
debug:tchess
	gdb tchess
