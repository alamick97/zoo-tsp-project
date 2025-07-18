//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <limits> //numeric_limits<double>
#include <algorithm> //swap, sort, min
#include <cstring>
#include <cstdint> //UINT32_MAX
#include <cmath> //sqrt, round
#include <cstddef> //size_t
#include <unordered_set>
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
    Category cat;
};

struct primsTable {
    bool kv;
    double dv;
    uint32_t pv;

    primsTable() : kv(false), dv(std::numeric_limits<double>::infinity()),
                    pv(UINT32_MAX) {} 
};

struct Edge {
    uint32_t u, v;
    double dist;
    Edge(uint32_t u, uint32_t v, double dist) : u(u), v(v), dist(dist) {}
};

inline bool compareEdges(const Edge& a, const Edge& b) {
    return a.dist < b.dist;
}

class Zoo {
    int _argc;
    char** _argv;
    bool _mode_flag;
    Mode _mode;
    uint32_t _num_vert;
    uint32_t _arbitrary_root_id;
    double _mst_total_dist; //sum of prim's table dists.
    double _fast_total;
    double _best_total; //upper bound. best so far.
    double _total; //current total.

    std::vector<Vertex> _vert; //all vertices from input file. idx = vertex id.
    std::vector<primsTable> _table; //prims table for all vertices read in.
    std::vector<uint32_t> _fast_path; //completed path includes trailing 0.
    std::vector<uint32_t> _best_path; //best COMPLETE path so far. 
    std::vector<uint32_t> _path; //current path.
    
public:
    Zoo(int argc, char** argv);

    double dist(Vertex v1, Vertex v2); //get euclid dist
    double insCost(uint32_t i, uint32_t k, uint32_t j); //{i, k, j} = vertex IDs
    double appendCost(uint32_t i, uint32_t j);
    double armCost(uint32_t pLen, uint32_t idx); //connects vertex id at _path[idx] to closest unvisited vertex.
    double remMST(uint32_t pLen); //gets remaining MST weight
    Category category(int x, int y); //calculates & determines category
    void printMST();
    void partAMST();
    void randInsTSP();
    void printTSP(Mode tsp);
    
    void readInput(); //reads input redirection via cin. 
    void runSpecifiedMode();
    void runMST();
    void runFASTTSP();
    void runOPTTSP();

    //part C
    void genPerms(uint32_t pLen); //i think uint32_t is sufficient (not size_t). waiting for piazza answer. 
    bool promising(uint32_t pLen);
};

#endif //ZOO_H