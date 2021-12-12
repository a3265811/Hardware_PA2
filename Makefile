cc:=g++
executable:=M11007428.o
cflag=-g
sources=*.cpp
objects=$(sources:.c=.o)

all: $(sources) $(executable)
	
$(executable): $(objects)
	$(cc) $(cflag) $(objects) -o $(executable)

%.o: %.c
	$(cc) $< -c -o $@


.PHONY: clean
clean:
	rm -rf *.o *.bench
