CC= g++
CPPFLAGS= -Wall -g -std=c++14 -Ofast
LDFLAGS= 
TEST_OBJECTS= test_key.o
CASCADE_OBJECTS= key.o

all: cascade

cascade: $(CASCADE_OBJECTS)
	$(CC) $(CPPFLAGS) -o cascade $(CASCADE_OBJECTS) $(LDFLAGS)

test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	$(CC) $(CPPFLAGS) -lgtest -o test $(TEST_OBJECTS) $(CASCADE_OBJECTS) $(LDFLAGS)
	./test

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: clean 

clean:
	rm -f *.o
	rm -rf *.dSYM
	rm -f cascade 
	rm -f test
