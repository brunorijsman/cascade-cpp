INCLUDE_DIRS = -Iinclude

CC := clang
CCFLAGS := -Wall -Wextra -Werror -g -Ofast $(INCLUDE_DIRS)

CXX := clang++
CXX_FLAGS := -Wall -Wextra -Weffc++ -Werror -g -std=c++14 -pthread $(INCLUDE_DIRS)
CXX_FLAGS_PROD := -Ofast
CXX_FLAGS_COV := -fprofile-instr-generate -fcoverage-mapping
CXX_FLAGS_DEBUG := -O0 -DENABLE_DEBUG

LDFLAGS :=

CASCADE_PYTHON_DIR=$(HOME)/cascade-python

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
CASCADE_OBJECTS_DEBUG := $(patsubst cascade/%.cpp, obj-debug/%.o, $(CASCADE_SRCS))
CASCADE_DEPS := $(CASCADE_SRCS:.cpp=.d)

TEST_SRCS := $(shell find tests -name "*.cpp")
TEST_OBJECTS := $(patsubst tests/%.cpp, obj/%.o, $(TEST_SRCS))
TEST_OBJECTS_COV := $(patsubst tests/%.cpp, obj-cov/%.o, $(TEST_SRCS))
TEST_OBJECTS_DEBUG := $(patsubst tests/%.cpp, obj-debug/%.o, $(TEST_SRCS))
TEST_DEPS := $(TEST_SRCS:.cpp=.d)

RUNEXP_SRCS := $(shell find study -name "*.cpp")
RUNEXP_OBJECTS := $(patsubst study/%.cpp, obj/%.o, $(RUNEXP_SRCS))
RUNEXP_OBJECTS_COV := $(patsubst study/%.cpp, obj-cov/%.o, $(RUNEXP_SRCS))
RUNEXP_OBJECTS_DEBUG := $(patsubst study/%.cpp, obj-debug/%.o, $(RUNEXP_SRCS))
RUNEXP_DEPS := $(RUNEXP_SRCS:.cpp=.d)

NODEPS := clean default ubuntu-get-dependencies

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	-include $(CASCADE_DEPS)
	-include $(RUNEXP_DEPS)
	-include $(TEST_DEPS)
endif

default:
	@echo Please specify a make target

bin/test: $(TEST_OBJECTS) $(CASCADE_OBJECTS)
	mkdir -p bin && \
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -o bin/test $(TEST_OBJECTS) $(CASCADE_OBJECTS) \
		-lgtest -lpthread $(LDFLAGS)

test: bin/test
	bin/test

bin/test_coverage: $(TEST_OBJECTS_COV) $(CASCADE_OBJECTS_COV) bin/test_coverage
	mkdir -p bin && \
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) $(CXX_FLAGS_COV) -o bin/test_coverage \
		$(TEST_OBJECTS_COV) $(CASCADE_OBJECTS_COV) -lgtest -lpthread $(LDFLAGS)

test-coverage: bin/test_coverage
	mkdir -p coverage
	LLVM_PROFILE_FILE="coverage/coverage-test.profraw" bin/test_coverage
	$(LLVM_PROFDATA) merge -sparse coverage/coverage-test.profraw \
		-o coverage/coverage-test.profdata
	$(LLVM_COV) show bin/test_coverage -instr-profile=coverage/coverage-test.profdata \
		coverage/coverage-test.profdata -format=text > coverage/coverage-test.txt
	$(LLVM_COV) show bin/test_coverage -instr-profile=coverage/coverage-test.profdata \
		coverage/coverage-test.profdata -format=html > coverage/coverage-test.html
	$(OPEN) coverage/coverage-test.html

bin/run_experiments: $(RUNEXP_OBJECTS) $(CASCADE_OBJECTS)
	@echo YEAH!
	mkdir -p bin
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -o bin/run_experiments $(RUNEXP_OBJECTS) \
		$(CASCADE_OBJECTS) -lboost_program_options -lboost_filesystem -lboost_system $(LDFLAGS)

bin/run_experiments_debug: $(RUNEXP_OBJECTS_DEBUG) $(CASCADE_OBJECTS_DEBUG)
	mkdir -p bin
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) -o bin/run_experiments_debug \
		$(RUNEXP_OBJECTS_DEBUG) $(CASCADE_OBJECTS_DEBUG) -lboost_program_options \
		-lboost_filesystem -lboost_system $(LDFLAGS)

debug: bin/run_experiments_debug
	mkdir -p study/data/debug
	rm -f study/data/debug/data__*
	bin/run_experiments_debug study/experiments_debug.json --output-dir study/data/debug

data: data-papers data-performance data-zero-handling

data-papers: bin/run_experiments
	mkdir -p study/data/papers
	rm -f study/data/papers/data__*
	bin/run_experiments study/experiments_papers.json --output-dir study/data/papers

data-papers-subset: bin/run_experiments
	mkdir -p study/data/papers
	rm -f study/data/papers/data__*
	bin/run_experiments study/experiments_papers.json --output-dir study/data/papers --max-runs 3

data-performance: bin/run_experiments
	mkdir -p study/data/performance
	rm -f study/data/performance/data__*
	bin/run_experiments study/experiments_performance.json --output-dir study/data/performance

data-zero-handling: bin/run_experiments
	mkdir -p study/data/zero_handling
	rm -f study/data/zero_handling/data__*
	bin/run_experiments study/experiments_zero_handling.json --output-dir study/data/zero_handling

graphs: graphs-papers graphs-performance graphs-zero-handling

graphs-papers:
	mkdir -p study/graphs/papers
	rm -f study/graphs/papers/*.png
	source $(CASCADE_PYTHON_DIR)/env/bin/activate && \
	python $(CASCADE_PYTHON_DIR)/study/make_graphs.py study/graphs_demystifying.json \
		--data-dir study/data/papers
	source $(CASCADE_PYTHON_DIR)/env/bin/activate && \
	python $(CASCADE_PYTHON_DIR)/study/make_graphs.py study/graphs_andre_reis_thesis.json \
		--data-dir study/data/papers

graphs-performance:
	mkdir -p study/graphs/performance
	rm -f study/graphs/performance/*.png
	source $(CASCADE_PYTHON_DIR)/env/bin/activate && \
	python $(CASCADE_PYTHON_DIR)/study/make_graphs.py study/graphs_performance.json \
		--data-dir study/data/performance

graphs-zero-handling:
	mkdir -p study/graphs/zero_handling
	rm -f study/graphs/zero_handling/*.png
	source $(CASCADE_PYTHON_DIR)/env/bin/activate && \
	python $(CASCADE_PYTHON_DIR)/study/make_graphs.py study/graphs_zero_handling.json \
		--data-dir study/data/zero_handling

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
	rm -rf obj-debug
	rm -rf bin
	rm -rf profile
	rm -rf coverage
	rm -f cascade/*.d
	rm -f study/*.d
	rm -f tests/*.d
	rm -rf *.dSYM

cascade/%.d: cascade/%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -MM -MT '$(patsubst cascade/%.cpp,obj/%.o,$<)' $< -MF $@

study/%.d: study/%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -MM -MT '$(patsubst study/%.cpp,obj/%.o,$<)' $< -MF $@

tests/%.d: tests/%.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -MM -MT '$(patsubst tests/%.cpp,obj/%.o,$<)' $< -MF $@

obj/%.o: cascade/%.cpp cascade/%.d
	mkdir -p obj
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -c $< -o $@

obj/%.o: study/%.cpp study/%.d
	mkdir -p obj
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -c $< -o $@

obj/%.o: tests/%.cpp tests/%.d
	mkdir -p obj
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) -c $< -o $@

obj-cov/%.o: cascade/%.cpp cascade/%.d
	mkdir -p obj-cov
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) $(CXX_FLAGS_COV) -c $< -o $@

obj-cov/%.o: study/%.cpp study/%.d
	mkdir -p obj-cov
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) $(CXX_FLAGS_COV) -c $< -o $@

obj-cov/%.o: tests/%.cpp tests/%.d
	mkdir -p obj-cov
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_PROD) $(CXX_FLAGS_COV) -c $< -o $@

obj-debug/%.o: cascade/%.cpp cascade/%.d
	mkdir -p obj-debug
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) -c $< -o $@

obj-debug/%.o: study/%.cpp study/%.d
	mkdir -p obj-debug
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) -c $< -o $@

obj-debug/%.o: tests/%.cpp tests/%.d
	mkdir -p obj-debug
	$(CXX) $(CXX_FLAGS) $(CXX_FLAGS_DEBUG) -c $< -o $@

.PHONY: clean get-dependencies
