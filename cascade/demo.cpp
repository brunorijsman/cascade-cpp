#include <iostream>
#include <random>

int main()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 10000);

    mt.seed(12345);
    for (int i=0; i<10; ++i) {
        std::cout << dist(mt) << std::endl;
    }

    return 0;
}
