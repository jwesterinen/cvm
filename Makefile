TARGET = cvm

DEPS = $(TARGET).h codegen.h cvmparse.h
OBJ =  $(TARGET).o codegen.o cvmparse.o

CPP=g++
CPPFLAGS = -I. -O0 -g -Wall -c


%.o: %.c $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS)

$(TARGET): $(OBJ)
	$(CPP) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET) *.o






