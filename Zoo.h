//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
#include <iostream>
#include <string>
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

class Zoo {
    int _argc;
    char** _argv;
    bool _mode_flag;
    Mode _mode;

public:
    Zoo(int argc, char** argv);

    void readInput(); //reads input redirection via cin. 
    void findMST();
    void findFASTTSP();
    void findOPTTSP();
};


#endif //ZOO_H