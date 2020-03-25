[![Build Status](https://travis-ci.org/brunorijsman/cascade-cpp.svg?branch=master)](https://travis-ci.org/brunorijsman/cascade-cpp)    [![Code Coverage](https://codecov.io/gh/brunorijsman/cascade-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/brunorijsman/cascade-cpp)

# cascade-cpp

C++ implementation of Cascade information reconciliation protocol for Quantum Key Distribution.

Modeled after the [Python implementation](https://github.com/brunorijsman/cascade-python) of Cascade.

# Getting started

To install on Ubuntu 18.04:

    git clone https://github.com/brunorijsman/cascade-cpp.git
    cd cascade-cpp
    make ubuntu-get-dependencies

To build and run unit tests:

    make test

To build and run unit tests, and produce code coverage report in coverage/coverage-test.html:

    make test-coverage
    
