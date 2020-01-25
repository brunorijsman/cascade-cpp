CC= clang
CXX= clang
CCFLAGS= -Wall -g -Ofast
CXXFLAGS= -Wall -g -std=c++14 -Ofast
LDFLAGS= -lc++
TEST_OBJECTS= test_key.o
CASCADE_OBJECTS= key.o

all: cascade

cascade: $(CASCADE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o cascade $(CASCADE_OBJECTS) $(LDFLAGS)

test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	$(CXX) $(CXXFLAGS) -lgtest -o test $(TEST_OBJECTS) $(CASCADE_OBJECTS) $(LDFLAGS)
	./test

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

get-dependencies:
	# Gtest
	sudo apt-get update -y
	sudo apt-get install -y libgtest-dev
	sudo apt-get install -y cmake
	sudo apt-get install -y clang
	cd /usr/src/gtest && \
	sudo cmake CMakeLists.txt && \
	sudo make && \
	sudo make install

.PHONY: clean get-dependencies

clean:
	rm -f *.o
	rm -rf *.dSYM
	rm -f cascade 
	rm -f test
