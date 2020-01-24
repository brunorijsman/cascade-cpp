CC= g++
CPPFLAGS= -Wall -g -std=c++14 -Ofast
LDFLAGS= 
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(OBJECTS)
	$(CC) $(CPPFLAGS) -o cascade $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

.PHONY: clean

clean:
	rm *.o cascade