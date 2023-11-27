//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include <iostream>
#include <iomanip> //setprecision(), fixed()
#include "Zoo.h"

int main (int argc, char** argv) {
    std::ios_base::sync_with_stdio(false); //Speeds up project's I/O
    std::cout << std::setprecision(2); //Always show 2 decimal places.
    std::cout << std::fixed; //Disable scientific notation for large numbers.

    Zoo zoo(argc, argv);
    zoo.readInput();
    zoo.runSpecifiedMode();

    return 0;
}