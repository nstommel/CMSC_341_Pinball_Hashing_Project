# Project 5 Makefile Nick Stommel

CCFLAGS = -Wall -ansi -g
DRIVER = myDriver
OBJ_FILES = $(DRIVER).o Pinball.o
SUBMIT_DIR = /afs/umbc.edu/users/s/l/slupoli/pub/cs341/stommel1/proj5/src

Driver.out: $(OBJ_FILES)
	g++ $(CCFLAGS) $(OBJ_FILES) -o Driver.out

$(DRIVER).o: $(DRIVER).cpp Pinball.h words.h
	g++ $(CCFLAGS) -c $(DRIVER).cpp

Pinball.o: Pinball.cpp Pinball.h
	g++ $(CCFLAGS) -c Pinball.cpp

.PHONY: clean
clean:
	rm -f *.o *~ *.out

.PHONY: run
run: Driver.out
	./Driver.out

.PHONY: mem-check
mem-check: Driver.out
	valgrind ./Driver.out
	
.PHONY: output
output: Driver.out
	./Driver.out > output.txt

.PHONY: submit
submit:
	cp Pinball.h Pinball.cpp myDriver.cpp output.txt $(SUBMIT_DIR)

