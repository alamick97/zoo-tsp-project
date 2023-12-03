//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <limits> //numeric_limits<double>
#include <algorithm> //swap
#include <cstring>
#include <cstdint> //UINT32_MAX
#include <cmath> //sqrt, round
#include <cstddef> //size_t
#include <unordered_set>
#include <algorithm> //sort, min
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

/*P4C: Paoletti vid notes:
    [x] "you want to get the function to have 1 parameter (or 0)"
            - "it's possible to make it have 0 params. this is a little harder to do."
    [] "add a bunch of member vars to your class"
        [] coordinates that you read in (vertices)
        [x] path that you're building 
        [x] length of path you're building 
        [x] best path you've seen
            - first use _fast_path from part B.
        [] length of best path you've seen 
    [] 
    [] 
    [] 
*/

class Zoo {
    int _argc;
    char** _argv;
    bool _mode_flag;
    Mode _mode;
    uint32_t _num_vertices;
    uint32_t _arbitrary_root_id;
    double _mst_tot_dist; //sum of prim's table dists.
    double _fast_tot;
    double _best_tot; //upper bound. best so far.
    double _tot; //current total.

    std::vector<Vertex> _vertices;
    std::vector<primsTable> _table; //prims table
    std::vector<uint32_t> _fast_path; //completed path includes trailing 0.
    std::vector<uint32_t> _best_path; //best COMPLETE path so far. 
    std::vector<uint32_t> _path; //current path.
    
public:
    Zoo(int argc, char** argv);

    double getDistance(Vertex v1, Vertex v2); //get euclid dist
    double getInsCost(uint32_t i, uint32_t k, uint32_t j); //{i, k, j} = vertex IDs
    double getAppendCost(uint32_t i, uint32_t j);
    Category getCategory(int x, int y); //calculates & determines category
    void printMST();
    void primsLinearPartA();
    double primsLinearPartC(uint32_t pLen);
    double getLowerBound(uint32_t pLen); //lowerbound = cur_path + underestimate_of_remaining(mst)
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