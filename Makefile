CC= g++
CPPFLAGS= -Wall -g -std=c++14 -Ofast
LDFLAGS= 
CASCADE_OBJECTS= key.o

all: cascade

cascade: $(CASCADE_OBJECTS)
	$(CC) $(CPPFLAGS) -o cascade $(CASCADE_OBJECTS) $(LDFLAGS)

test: test_key

test_key: test_key.cpp key.cpp
	$(CC) $(CPPFLAGS) -o test_key test_key.o key.o $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o
	rm -f cascade 
	rm -f test_key
	rm -rf test_key.dSYM