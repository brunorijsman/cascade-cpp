[![Build Status](https://travis-ci.org/brunorijsman/cascade-cpp.svg?branch=master)](https://travis-ci.org/brunorijsman/cascade-cpp)    [![Code Coverage](https://codecov.io/gh/brunorijsman/cascade-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/brunorijsman/cascade-cpp)

# What is Cascade-CPP?

[This repository](https://github.com/brunorijsman/cascade-cpp) contains a C++ implementation of
Cascade information reconciliation protocol for Quantum Key Distribution (QKD).

[This tutorial](https://cascade-python.readthedocs.io/en/latest/protocol.html) provides a detailed
description of the Cascade information reconciliation protocol and its relation to Quantum Key
Distribution.


# Comparison between Cascade-Python and Cascade-CPP

The C++ code in this repository is modeled after my earlier Python implementation in GitHub 
repository [Cascade-Python](https://github.com/brunorijsman/cascade-python), which
is documented on [ReadTheDocs](https://cascade-python.readthedocs.io/en/latest/).

The C++ code is orders of magnitude faster than the Python code. This is because the C++ code was
carefully optimized based on the insights from profiling the code. In particular, the decision to
cache Shuffle objects made a huge impact on the performance.

The "make data-papers" target in the Python code does 1,000 Cascade iterations per data point and
takes more than 5 days of continuous running on an AWS m5.2xlarge instance (120 hours x US$ 0.40 
per hour = US$ 48 in compute cost).
By contrast, the "make data-papers" target the C++ code does 10,000 Cascade iterations per data
point (10x better accuracy) only takes ten hours to complete (US$ 4).

The C++ was more carefully debugged than the Python code. The C++ code has a "make debug" target
that produces color-coded representations of the blocks at each step in the algorithm (bit errors
are shown in red). This makes it much easier to follow what is going on in the algorithm. Beyond
debugging, this is also a good tool for learning the Cascade algorithm.

As a result of the better debugging in the C++ code, the data produced by the C++ is more consistent
with the results published in
[Demystifying the Information Reconciliation Protocol Cascade](https://arxiv.org/abs/1407.3257)
and [Quantum Key Distribution Post Processing - A study on the Information Reconciliation Cascade Protocol](https://repositorio-aberto.up.pt/bitstream/10216/121965/2/347567.pdf).


# How to install, compile and run the Cascade-CPP code

To install on Ubuntu 18.04:

    git clone https://github.com/brunorijsman/cascade-cpp.git
    cd cascade-cpp
    sudo apt install -y make
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

To produce the graphs for all experiments:

    # Choose one of these
    make graphs-papers
    make graphs-performance
    make graphs-zero-handling

To run a small reconciliation with full debug prints:

    make debug