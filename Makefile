CC=g++
CFLAGS=-Wall -O2
LDFLAGS=
EXEC=LightSim
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

all: $(EXEC)

LightSim: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS)
%.h:
	touch $@

clean:
	rm -f *.o core
mrproper: clean
	rm -f $(EXEC)
