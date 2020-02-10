INCLUDE_DIRS = -Iinclude

CC := clang
CCFLAGS := -Wall -Wextra -Werror -g -Ofast $(INCLUDE_DIRS)

CXX := clang++
CXXFLAGS := -Wall -Wextra -Weffc++ -Werror -g -Ofast -std=c++14 $(INCLUDE_DIRS)

LDFLAGS :=

CASCADE_SRCS := $(shell find cascade -name "*.cpp")
CASCADE_OBJECTS := $(patsubst cascade/%.cpp, obj/%.o, $(CASCADE_SRCS))
CASCADE_DEPS := $(CASCADE_SRCS:.cpp=.d)

STUDY_SRCS := $(shell find study -name "*.cpp")
STUDY_OBJECTS := $(patsubst study/%.cpp, obj/%.o, $(STUDY_SRCS))
STUDY_DEPS := $(STUDY_SRCS:.cpp=.d)

TEST_SRCS := $(shell find tests -name "*.cpp")
TEST_OBJECTS := $(patsubst tests/%.cpp, obj/%.o, $(TEST_SRCS))
TEST_DEPS := $(TEST_SRCS:.cpp=.d)

NODEPS := clean

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	-include $(CASCADE_DEPS)
	-include $(STUDY_DEPS)
	-include $(TEST_DEPS)
endif

default: build-study

test: build-test run-test

build-test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	mkdir -p bin && \
	$(CXX) $(CXXFLAGS) -o bin/test $(TEST_OBJECTS) $(CASCADE_OBJECTS) -lgtest -lpthread $(LDFLAGS)

run-test:
	bin/test

build-study:  $(STUDY_OBJECTS) $(CASCADE_OBJECTS)
	mkdir -p bin && \
	$(CXX) $(CXXFLAGS) -o bin/run_experiments $(STUDY_OBJECTS) $(CASCADE_OBJECTS) \
	-lboost_program_options $(LDFLAGS)

test-coverage: build-test-coverage run-test-coverage

build-test-coverage: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	mkdir -p bin && \
	$(CXX) $(CXXFLAGS) -o bin/test-coverage $(TEST_OBJECTS) $(CASCADE_OBJECTS) -lgtest -lpthread \
		-coverage $(LDFLAGS)

run-test-coverage:
	bin/test-coverage && \
	llvm-cov gcov $(CASCADE_SRCS)

cascade/%.d: cascade/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst cascade/%.cpp,obj/%.o,$<)' $< -MF $@

study/%.d: study/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst study/%.cpp,obj/%.o,$<)' $< -MF $@

tests/%.d: tests/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst tests/%.cpp,obj/%.o,$<)' $< -MF $@

obj/%.o: cascade/%.cpp cascade/%.d
	mkdir -p obj && \
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: study/%.cpp study/%.d
	mkdir -p obj && \
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: tests/%.cpp tests/%.d
	mkdir -p obj && \
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
	rm -rf obj
	rm -rf bin
	rm -f cascade/*.d
	rm -f study/*.d
	rm -f tests/*.d
	rm -rf *.dSYM
	rm -f *.gcov
	rm -f *.gcda
	rm -f *.gcno	

.PHONY: clean get-dependencies
