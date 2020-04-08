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

# Comparing the data produced by Cascade-CPP with published literature.

In this section we compare the data produced

We used Github version 4b3498837b646d3f7649a13e68d80db0f8008767 (8 April 2020) of Cascade-CPP
to produce the data in these graphs.

## Comparison with Demystifying the Information Reconciliation Protocol Cascade.

In this section we compare the data produced by Cascade-CPP with the results reported in:

[Demystifying the Information Reconciliation Protocol Cascade](https://arxiv.org/abs/1407.3257).<br />
_Jesus Martinez-Mateo, Christoph Pacher, Momtchil Peev, Alex Ciurana, and Vicente Martin._ <br />
arXiv:1407.3257 \[quant-ph\], Jul 2014.

### Figure 1.

Results reported in paper:

![Demystifying Figure 1](docs/figures/demystifying-figure-1-original.png)

### Figure 2.

Results reported in paper:

![Demystifying Figure 2](docs/figures/demystifying-figure-2-original.png)

### Figure 3.

Results reported in paper:

![Demystifying Figure 3](docs/figures/demystifying-figure-3-original.png)

### Figure 4.

Results reported in paper:

![Demystifying Figure 4](docs/figures/demystifying-figure-4-original.png)

### Figure 5.

Results reported in paper:

![Demystifying Figure 5](docs/figures/demystifying-figure-5-original.png)

### Figure 6.

Results reported in paper:

![Demystifying Figure 6](docs/figures/demystifying-figure-6-original.png)

### Figure 7.

Results reported in paper:

![Demystifying Figure 7](docs/figures/demystifying-figure-7-original.png)

### Figure 8.

Results reported in paper:

![Demystifying Figure 8](docs/figures/demystifying-figure-8-original.png)

### Figure 9.

Results reported in paper:

![Demystifying Figure 9](docs/figures/demystifying-figure-9-original.png)

### Figure 10.

Results reported in paper:

![Demystifying Figure 10](docs/figures/demystifying-figure-10-original.png)

### Figure 11.

Results reported in paper:

![Demystifying Figure 11](docs/figures/demystifying-figure-11-original.png)

### Figure 12.

Results reported in paper:

![Demystifying Figure 12](docs/figures/demystifying-figure-12-original.png)

### Figure 13.

Results reported in paper:

![Demystifying Figure 13](docs/figures/demystifying-figure-13-original.png)


## Comparison with Quantum Key Distribution Post Processing.

In this section we compare the data produced by Cascade-CPP with the results reported in:

[Quantum Key Distribution Post Processing - A study on the Information Reconciliation Cascade Protocol.](https://repositorio-aberto.up.pt/bitstream/10216/121965/2/347567.pdf.<br />
_André Reis._<br />
Master’s Thesis, Faculdade de Engenharia da Universidade do Porto, Jul 2019.

### Figure 5.1.

Results reported in thesis:

![Thesis Figure 5.1](docs/figures/andre-reis-thesis-figure-5-1-original.png)

### Figure 5.2.

Results reported in thesis:

![Thesis Figure 5.2](docs/figures/andre-reis-thesis-figure-5-2-original.png)

### Figure 5.3.

Results reported in thesis:

![Thesis Figure 5.3](docs/figures/andre-reis-thesis-figure-5-3-original.png)

### Figure 5.4.

Results reported in thesis:

![Thesis Figure 5.4](docs/figures/andre-reis-thesis-figure-5-4-original.png)

### Figure 5.5.

Results reported in thesis:

![Thesis Figure 5.5](docs/figures/andre-reis-thesis-figure-5-5-original.png)

### Figure 5.6.

Results reported in thesis:

![Thesis Figure 5.6](docs/figures/andre-reis-thesis-figure-5-6-original.png)

### Figure 5.7.

Results reported in thesis:

![Thesis Figure 5.7](docs/figures/andre-reis-thesis-figure-5-7-original.png)

### Figure 5.8.

Results reported in thesis:

![Thesis Figure 5.8](docs/figures/andre-reis-thesis-figure-5-8-original.png)

### Figure 5.9.

Results reported in thesis:

![Thesis Figure 5.9](docs/figures/andre-reis-thesis-figure-5-9-original.png)

### Figure 5.10.

Results reported in thesis:

![Thesis Figure 5.10](docs/figures/andre-reis-thesis-figure-5-10-original.png)
