//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <limits>
#include <cstring>
#include <cmath> //sqrt, round
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
    Safe,
    WallCage
};

struct Vertex {
    int x;
    int y;
    //uint32_t id; //no need to store id. vector index is id.
    Category cat;
};

struct primsTable {
    bool kv;
    double dv;
    uint32_t pv;

    primsTable() : kv(false), dv(std::numeric_limits<double>::infinity()) {} 
};

class Zoo {
    int _argc;
    char** _argv;
    bool _mode_flag;
    Mode _mode;
    uint32_t _num_vertices;
    uint32_t _arbitrary_root_id;
    double _dv_sum; //prim's table dist sum.

    std::vector<Vertex> _vertices;
    std::vector<primsTable> _primsTable;
public:
    Zoo(int argc, char** argv);

    double getDistance(Vertex v1, Vertex v2); //get euclid dist
    Category getCategory(int x, int y); //calculates & determines category
    void initPrimsTable(uint32_t root_id); //inits via root id
    
    void readInput(); //reads input redirection via cin. 
    void runMST();
    void printMST();
    void primsLinear(); //runs prim's linear
};

double roundToHundredths(double n);

#endif //ZOO_H