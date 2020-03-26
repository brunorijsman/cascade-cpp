INCLUDE_DIRS = -Iinclude

CC := clang
CCFLAGS := -Wall -Wextra -Werror -g -Ofast $(INCLUDE_DIRS)

CXX := clang++
CXXFLAGS := -Wall -Wextra -Weffc++ -Werror -g -Ofast -std=c++14 $(INCLUDE_DIRS)
CXXCOVFLAGS := -fprofile-instr-generate -fcoverage-mapping

LDFLAGS :=

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LLVM_PROFDATA := /Library/Developer/CommandLineTools/usr/bin/llvm-profdata
	LLVM_COV := /Library/Developer/CommandLineTools/usr/bin/llvm-cov
	OPEN := open
else
	LLVM_PROFDATA := llvm-profdata
	LLVM_COV := llvm-cov
	OPEN := true
endif

CASCADE_SRCS := $(shell find cascade -name "*.cpp")
CASCADE_OBJECTS := $(patsubst cascade/%.cpp, obj/%.o, $(CASCADE_SRCS))
CASCADE_OBJECTS_COV := $(patsubst cascade/%.cpp, obj-cov/%.o, $(CASCADE_SRCS))
CASCADE_DEPS := $(CASCADE_SRCS:.cpp=.d)

STUDY_SRCS := $(shell find study -name "*.cpp")
STUDY_OBJECTS := $(patsubst study/%.cpp, obj/%.o, $(STUDY_SRCS))
STUDY_OBJECTS_COV := $(patsubst study/%.cpp, obj-cov/%.o, $(STUDY_SRCS))
STUDY_DEPS := $(STUDY_SRCS:.cpp=.d)

TEST_SRCS := $(shell find tests -name "*.cpp")
TEST_OBJECTS := $(patsubst tests/%.cpp, obj/%.o, $(TEST_SRCS))
TEST_OBJECTS_COV := $(patsubst tests/%.cpp, obj-cov/%.o, $(TEST_SRCS))
TEST_DEPS := $(TEST_SRCS:.cpp=.d)

NODEPS := clean default ubuntu-get-dependencies

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	-include $(CASCADE_DEPS)
	-include $(STUDY_DEPS)
	-include $(TEST_DEPS)
endif

default:
	@echo Please specify a make target

test: build-test run-test

build-test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	mkdir -p bin && \
	$(CXX) $(CXXFLAGS) -o bin/test $(TEST_OBJECTS) $(CASCADE_OBJECTS) -lgtest -lpthread $(LDFLAGS)

run-test:
	bin/test

build-study:  $(STUDY_OBJECTS) $(CASCADE_OBJECTS)
	mkdir -p bin && \
	$(CXX) $(CXXFLAGS) -o bin/run_experiments $(STUDY_OBJECTS) $(CASCADE_OBJECTS) \
	-lboost_program_options -lboost_filesystem $(LDFLAGS)

test-coverage: build-test-coverage run-test-coverage

build-test-coverage: $(TEST_OBJECTS_COV) $(CASCADE_OBJECTS_COV)
	mkdir -p bin && \
	$(CXX) $(CXXFLAGS) $(CXXCOVFLAGS) -o bin/test_coverage $(TEST_OBJECTS_COV) \
		$(CASCADE_OBJECTS_COV) -lgtest -lpthread $(LDFLAGS)

run-test-coverage:
	mkdir -p coverage && \
	LLVM_PROFILE_FILE="coverage/coverage-test.profraw" bin/test_coverage && \
	$(LLVM_PROFDATA) merge -sparse coverage/coverage-test.profraw \
		-o coverage/coverage-test.profdata && \
	$(LLVM_COV) show bin/test_coverage -instr-profile=coverage/coverage-test.profdata \
		coverage/coverage-test.profdata -format=text > coverage/coverage-test.txt && \
	$(LLVM_COV) show bin/test_coverage -instr-profile=coverage/coverage-test.profdata \
		coverage/coverage-test.profdata -format=html > coverage/coverage-test.html && \
	$(OPEN) coverage/coverage-test.html

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

obj-cov/%.o: cascade/%.cpp cascade/%.d
	mkdir -p obj-cov && \
	$(CXX) $(CXXFLAGS) $(CXXCOVFLAGS) -c $< -o $@

obj-cov/%.o: study/%.cpp study/%.d
	mkdir -p obj-cov && \
	$(CXX) $(CXXFLAGS) $(CXXCOVFLAGS) -c $< -o $@

obj-cov/%.o: tests/%.cpp tests/%.d
	mkdir -p obj-cov && \
	$(CXX) $(CXXFLAGS) $(CXXCOVFLAGS) -c $< -o $@

data: data-performance

data-performance:
	mkdir -p study/data/performance
	rm -f study/data/performance/data__*
	bin/run_experiments study/experiments_performance.json --output-dir study/data/performance

ubuntu-get-dependencies:
	# Gtest
	sudo apt-get update -y
	sudo apt-get install -y libgtest-dev
	sudo apt-get install -y cmake
	sudo apt-get install -y clang
	sudo apt-get install -y llvm
	sudo apt-get install -y libboost-all-dev
	cd /usr/src/gtest && \
	sudo cmake CMakeLists.txt && \
	sudo make && \
	sudo make install

clean:
	rm -rf obj
	rm -rf obj-cov
	rm -rf bin
	rm -rf profile
	rm -rf coverage
	rm -f cascade/*.d
	rm -f study/*.d
	rm -f tests/*.d
	rm -rf *.dSYM

.PHONY: clean get-dependencies
