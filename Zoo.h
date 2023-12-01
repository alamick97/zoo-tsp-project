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
    double _cur_tot; //"running total"
    double _best_tot; //upper bound. best so far.
    //maybe: add overly-optimistic lower bound. MST (but forego safe/wild cats)

    std::vector<Vertex> _vertices;
    std::vector<primsTable> _primsTable;
    std::vector<uint32_t> _fast_path;
    std::vector<uint32_t> _cur_path;
    std::vector<uint32_t> _best_path; //best so far.
    
public:
    Zoo(int argc, char** argv);

    double getDistance(Vertex v1, Vertex v2); //get euclid dist
    double getInsCost(uint32_t i, uint32_t k, uint32_t j); //{i, k, j} = vertex IDs
    double getAppendCost(uint32_t i, uint32_t j);
    Category getCategory(int x, int y); //calculates & determines category
    void initPrimsTable(); //inits via root id
    void printMST();
    void primsLinear();
    void randInsTSP();
    void printFastTSP();
    //void christofidesAlg(); //run Christofide's Algorithm
    //std::unordered_set<uint32_t> getOddVertices(); //count # of odd degree vertices
    //std::vector<Edge> findMWPM(std::unordered_set<uint32_t> odd_vertices); //Minimum-Weight Perfect Matching (christofides step 2)
    
    void readInput(); //reads input redirection via cin. 
    void runSpecifiedMode();
    void runMST();
    void runFASTTSP();
    void runOPTTSP();

    //part C
    void genPerms(uint32_t pLen); //i think uint32_t is sufficient (not size_t). waiting for piazza answer. 
    bool promising(uint32_t pLen);
    void branchBoundTSP();
};

#endif //ZOO_H