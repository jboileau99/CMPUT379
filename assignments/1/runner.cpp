#include <iostream>

int main() {
    int i = 0;
    int j = 0;
    int count_to = 1000000000; // Takes like 1/4 second
    int multiplier = 5; // for multiplier*(1/4) seconds total

    while (j < multiplier) {
        i = 0;
        while (i < count_to) {
            i++;
        }
        j++;
    }

    std::cout << "Done" << std::endl;

    return 1;
}