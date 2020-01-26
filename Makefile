CC= clang
CXX= clang++
CCFLAGS= -Wall -g -Ofast
CXXFLAGS= -Wall -Wextra -Weffc++ -Werror -g -std=c++14 -Ofast
LDFLAGS=
TEST_OBJECTS= test_key.o
CASCADE_OBJECTS= key.o

all: cascade

cascade: $(CASCADE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o cascade $(CASCADE_OBJECTS) $(LDFLAGS)

test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o test $(TEST_OBJECTS) $(CASCADE_OBJECTS) -lgtest -lpthread $(LDFLAGS)
	./test

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

ubuntu-get-dependencies:
	# Gtest
	sudo apt-get update -y
	sudo apt-get install -y libgtest-dev
	sudo apt-get install -y cmake
	sudo apt-get install -y clang
	cd /usr/src/gtest && \
	sudo cmake CMakeLists.txt && \
	sudo make && \
	sudo make install

clean:
	rm -f *.o
	rm -rf *.dSYM
	rm -f cascade 
	rm -f test

.PHONY: clean get-dependencies
