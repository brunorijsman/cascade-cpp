CC= clang
CXX= clang++
CCFLAGS= -Wall -g -Ofast -coverage
CXXFLAGS= -Wall -Wextra -Weffc++ -Werror -g -std=c++14 -Ofast -coverage
LDFLAGS=
TEST_OBJECTS= test_algorithm.o test_key.o
CASCADE_OBJECTS= algorithm.o block.o iteration.o key.o reconciliation.o
CASCADE_SRCS= algorithm.cpp block.cpp iteration.cpp key.cpp reconciliation.cpp

all: cascade

cascade: $(CASCADE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o cascade $(CASCADE_OBJECTS) $(LDFLAGS)

test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o test $(TEST_OBJECTS) $(CASCADE_OBJECTS) -lgtest -lpthread $(LDFLAGS)
	./test && \
	llvm-cov gcov $(CASCADE_SRCS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

ubuntu-get-dependencies:
	# Gtest
	sudo apt-get update -y
	sudo apt-get install -y libgtest-dev
	sudo apt-get install -y cmake
	sudo apt-get install -y clang
	sudo apt-get install -y llvm
	cd /usr/src/gtest && \
	sudo cmake CMakeLists.txt && \
	sudo make && \
	sudo make install

clean:
	rm -f *.o
	rm -rf *.dSYM
	rm -f cascade 
	rm -f test
	rm -f *.gcov
	rm -f *.gcda
	rm -f *.gcno	

.PHONY: clean get-dependencies
