[![Build Status](https://travis-ci.org/brunorijsman/cascade-cpp.svg?branch=master)](https://travis-ci.org/brunorijsman/cascade-cpp)    [![Code Coverage](https://codecov.io/gh/brunorijsman/cascade-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/brunorijsman/cascade-cpp)

# Cascade-CPP

This repository contains a C++ implementation of Cascade information reconciliation protocol for
Quantum  Key Distribution (QKD).

The C++ code is modeled after the Python implementation of the Cascade algorithm.

The Python code is in GitHub repository [Cascade-Python](https://github.com/brunorijsman/cascade-python)
and is documented on [ReadTheDocs](https://cascade-python.readthedocs.io/en/latest/)
(the documentation also includes an extensive description of the Cascade algorithm).

# Comparison between Cascade-Python and Cascade-CPP

There is one major difference between the Cascade-Python code and the Cascade-CPP code.

The Cascade-Python code contains data structures and logic to avoid repeatedly computing the current
parity for the same block. It keeps track of the current parity for each block, and each time a
single bit error is corrected, the current parity of all affected blocks is flipped.

We implemented similar logic in Cascade-CPP (actually in a more efficient way than it was done in
the Cascade-Python code). However, performance measurements showed that the extra data structures
and the extra logic to keep track of which blocks needed to be parity-flipped after a single bit
correction actual *decreased* the performance instead of *increasing* it. Simply recomputing the
current parity of a block whenever it was needed turned out to be *more* efficient. The code
for avoiding recomputing block current parities is still stored in branch
"reduce-current-parity-recompute" if you want to check for yourself.

# How to install, compile and run the Cascade-CPP code

To install on Ubuntu 18.04:

    git clone https://github.com/brunorijsman/cascade-cpp.git
    cd cascade-cpp
    make ubuntu-get-dependencies

To build and run unit tests:

    make test

To build and run unit tests, and produce code coverage report in coverage/coverage-test.html (on a
Mac this also opens the coverage report in the browser):

    make test-coverage

To produce the data for all experiments:

    make data

To produce the data for specific set of experiments:

    # Choose one of these
    make data-papers
    make data-papers-subset
    make data-performance
    make data-zero-handling

To produce graphs, you need to have [Cascade-Python](https://github.com/brunorijsman/cascade-python)
installed in the $HOME/cascade-python directory.

The produce the graphs for specific set of experiments (this assumes that that the data has already
been produced):

    make graphs

To produce the graphs for specific set of experiments:

    # Choose one of these
    make graphs-papers
    make graphs-performance
    make graphs-zero-handling

To run a small reconciliation with full debug prints:

    make debug