//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <getopt.h>

#ifndef ZOO_H
#define ZOO_H

enum class Mode {
    Unspecified,
    MST,
    FASTTSP,
    OPTTSP
};

enum class Category {
    Wild,
    Domestic,
    WallCage
};

struct Vertex {
    int x;
    int y;
    uint32_t id;
    Category cat;
};

class Zoo {
    int _argc;
    char** _argv;
    bool _mode_flag;
    Mode _mode;

public:
    Zoo(int argc, char** argv);

    double calcDist(); //distance helper function
    Category getCategory(int x, int y); //calculates & determines category
    
    void readInput(); //reads input redirection via cin. 
    void runMST();
};


#endif //ZOO_H