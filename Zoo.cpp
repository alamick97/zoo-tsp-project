//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include "Zoo.h"

Zoo::Zoo(int argc, char** argv) : _argc(argc), _argv(argv) {
    _mode_flag = false;
	_mode = Mode::Unspecified;
	_num_vertices = 0;
	_arbitrary_root_id = 0;
	_mst_tot_dist = 0;
	_fast_tot = 0;
	_cur_tot = 0;
	_best_tot = 0;

    int opt;
	int opt_idx;

	static struct option long_opts[] = {
		{"mode", required_argument, 0, 'm'},
		{"help", no_argument, 0, 'h'},
	};

	while ((opt = getopt_long(_argc, _argv, "m:h", long_opts, &opt_idx)) != -1) {
		switch(opt) {
			case 'm':
                if (strcmp(optarg, "MST") == 0) { _mode = Mode::MST; } 
                else if (strcmp(optarg, "FASTTSP") == 0) { _mode = Mode::FASTTSP; }
                else if (strcmp(optarg, "OPTTSP") == 0) { _mode = Mode::OPTTSP; }
				break;
			case 'h':
				std::cout << "HELP:\n";
				std::cout <<"---------------\n";
				std::cout << "To use the program, do the following:\n";
				std::cout << "--------------------------------------\n";
				std::cout << "The following options are available:\n";
				std::cout << "-m to specify a mode (MST, FASTTSP, OPTTSP)\n"; 
                std::cout << "-h to display help message.\n";
                std::exit(0);
		}
    }

    if (_mode == Mode::Unspecified) {
        std::cerr << "Must provide a -m/--mode option, which requires an argument (MST, FASTTSP, or OPTTSP).\n";
		std::cerr << "See -h/--help for more details.\n";
        exit(1); 
    }
}
    
	
//reads in vertex info via cin, stores them.
void Zoo::readInput() {
	int x, y;

	std::cin >> _num_vertices;
	_vertices.reserve(_num_vertices);

	for (uint32_t i = 0; i < _num_vertices; ++i) { //creates vector of vertices.
		std::cin >> x >> y;
		Category cat = getCategory(x, y);
		_vertices.emplace_back(Vertex{x, y, cat}); //no need to store id. vector index is id.
	}
}

void Zoo::runSpecifiedMode() {
	if (_mode == Mode::MST) {
		runMST();
	} else if (_mode == Mode::FASTTSP) {
		runFASTTSP();
	} else if (_mode == Mode::OPTTSP) {
		runOPTTSP();
	}
}

void Zoo::runMST() {
	primsLinearPartA(); //consider cage category
	printMST();
} 

void Zoo::runFASTTSP() {
	randInsTSP();
	_fast_path.pop_back(); //removes trailing 0-root
	printTSP(Mode::FASTTSP);
}

void Zoo::runOPTTSP() {
	randInsTSP(); //gen initial upper bound (part B total).
	_fast_path.pop_back(); //removes trailing 0 from Part B path.

	_best_tot = _fast_tot; //init to Part B total.
	_cur_tot = 0; //safeguard init
	_best_path = _fast_path; //init to Part B path.
	_cur_path = _best_path; //init to Part B path.

	genPerms(1); //updates _best_path & _best_tot to approp. vals.

	printTSP(Mode::OPTTSP);
}

//function: updates _best_path & _best_tot
void Zoo::genPerms(uint32_t pLen) {
	if (pLen == _cur_path.size()) { //base case, for (n-1) Complete Paths
		_cur_tot += getAppendCost(_cur_path[pLen], _cur_path[0]); //pre-cond: consider cycle-closing edge

		if (promising(false)) {  //updates best-so-far if better
			_best_path = _cur_path;
			_best_tot = _cur_tot;
		}

		_cur_tot -= getAppendCost(_cur_path[pLen], _cur_path[0]); //post-cond: remove cycle-closing edge
		return;
	}

	//if (!promising(pLen)) { return; } //pruning
	if (!promising(true)) { return; } //pruning

	for (uint32_t i = pLen; i < _cur_path.size(); ++i) { //continue DFS
		std::swap(_cur_path[pLen], _cur_path[i]);
		_cur_tot += getAppendCost(_cur_path[pLen], _cur_path[pLen + 1]); //pre-cond (trav. to children)
		genPerms(pLen + 1);
		_cur_tot -= getAppendCost(_cur_path[pLen], _cur_path[pLen + 1]); //post-cond (return to parent)
		std::swap(_cur_path[pLen], _cur_path[i]);
	}
}

//bool Zoo::promising(uint32_t pLen) {
bool Zoo::promising(bool isPartial) {
	//first check lower bound (mst)
	if (isPartial) {
		double lowerbound = getLowerBound();
		if (lowerbound >= _best_tot) { return false; }
	}

	//then check upper bound (best-so-far)
	if (_cur_tot >= _best_tot) { return false; }

	return true;
}

double Zoo::getLowerBound() {
	//lowerbound = cur_path_len + MST_of_remaining = estimate of totality of current path
	//lowerbound estimates (beyond the) best case scenario for what can result if we continue down the current path.
		//thus, if our partial solution total 
	/*plan: Use _best_path & _cur_path to get _rem_path. 
	*/
	uint32_t first = _cur_path[0];
	uint32_t last = _cur_path[_cur_path.size() - 1];

	std::vector<uint32_t> rem_path; //remaining path = _best_path (complete) - _cur_path (usually partial)
	uint32_t num_rem = _num_vertices - _cur_path.size();
	rem_path.reserve(num_rem);
	
	//build vector of remaining vertex IDs (+ first & last of visited)
	for (const uint32_t& visited : _cur_path) {
		for (const uint32_t& unvisited: _best_path) {
			if (visited != unvisited || first == unvisited || last == unvisited) { 
				rem_path.push_back(unvisited); 
			}
		}
	}

	//calc MST len for remaining path
	double rem_tot = primsLinearPartC(rem_path, num_rem);
	//still need to "connect visited vertices to the unvisited". so visited[0] and visited[last]

	double lowerbound = _cur_tot + rem_tot;

	return lowerbound;
}

//TODO: Implement for part C. Takes in a pointer to a vector of vertices, to find MST underestimate cost of remaining path. 
double Zoo::primsLinearPartC(const std::vector<uint32_t>& rem_path, uint32_t num_rem) {
	//idea: O(V^2): Identify unvisited nodes:
		//Double for-loop for current vector path & 
	double rem_tot = 0;
	std::vector<primsTable> primsTable;
	primsTable.resize(num_rem);
	primsTable[0].dv = 0;

	for (uint32_t i = 0; i < num_rem; ++i) {
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t id_min = 0; //AG warning forced me to init to val.

		//get shortest dist (find id_min)
		for (uint32_t i = 0; i < num_rem; i++) {
			if (primsTable[i].kv == false) {
				if (primsTable[i].dv < minDist) {
					minDist = primsTable[i].dv;
					id_min = i;
				}
			}
		}

		//update kv (for id_min)
		primsTable[id_min].kv = true;

		//update total dist
		rem_tot += primsTable[id_min].dv;

		Vertex v1 = _vertices[id_min];
		//update dv,pv (for id_min)
		for (uint32_t i = 0; i < num_rem; i++) {
			if (primsTable[i].kv == false) {
				Vertex v2 = _vertices[i];
				double oldDist = primsTable[i].dv;
				double newDist = getDistance(v1, v2);
				if (newDist < oldDist) { 
					primsTable[i].dv = newDist; 
					primsTable[i].pv = id_min;
				}
			}
		}
	}

	return rem_tot;
}

void Zoo::printTSP(Mode tsp) {
	const std::vector<uint32_t>* path;
	double* tot;
	bool b = false; //for warning flags

	if (tsp == Mode::FASTTSP) {
		path = &_fast_path;
		tot = &_fast_tot;
		b = true;
	} else if (tsp == Mode::OPTTSP) {
		path = &_best_path;
		tot = &_best_tot;	
		b = true;
	}

	if (b) {
		std::cout << *tot << "\n";
		for (uint32_t id : *path) { std::cout << id << " "; }
		std::cout << "\n";
	}
}


void Zoo::randInsTSP() {
	/*
	algorithm ref (random insertion heuristic): 
		- https://ocw.mit.edu/courses/1-203j-logistical-and-transportation-planning-methods-fall-2006/03634d989704c2607e6f48a182d455a0_lec16.pdf

	P says:
	 	[x] make a vec of IDX/ID's (NOT a vec of coord.!)
		[x] for ins heur:
			[x] init path as {0, 0}
				("bc then, however u pick the nxt one, u have a plc to insrt it")
	*/
	_fast_path.reserve(_num_vertices + 1);	
	_fast_path.push_back(0); //root id (start)
	_fast_path.push_back(0); //root id (end)

	for (uint32_t k = 1; k < _num_vertices; ++k) { //rand vert id
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t ins_idx = UINT32_MAX;

		for (uint32_t i_idx = 0; i_idx < _fast_path.size() - 1; ++i_idx) {
			uint32_t j_idx = i_idx + 1;
			uint32_t i = _fast_path[i_idx]; //idx -> id
			uint32_t j = _fast_path[j_idx]; //idx -> id
			double cost = getInsCost(i, k, j);

			if (cost < minDist) {
				minDist = cost;
				ins_idx = j_idx; //idx to insert
			}
		}

		_fast_tot += minDist;	
		_fast_path.insert(_fast_path.begin() + ins_idx, k);
	}
}

//gets cost (could be pos. or neg.) of inserting k between i and j ({i, k, j} = Vertex id's).  
double Zoo::getInsCost(uint32_t i, uint32_t k, uint32_t j) {
	Vertex v1 = _vertices[i];
	Vertex v2 = _vertices[k];
	Vertex v3 = _vertices[j];
	double cost = getDistance(v1, v2) + getDistance(v2, v3) - getDistance(v1, v3);

	return cost;
}

//gets cost (always positive) of adding an edge, given two Vertex id's. 
double Zoo::getAppendCost(uint32_t i, uint32_t j) {
	Vertex v1 = _vertices[i];
	Vertex v2 = _vertices[j];
	double cost = getDistance(v1, v2);

	return cost;
}

double Zoo::getDistance(Vertex v1, Vertex v2) {
	double x1 = v1.x;
	double x2 = v2.x;
	double y1 = v1.y;
	double y2 = v2.y;
	double a = y2 - y1;
	double b = x2 - x1;

	return std::sqrt(static_cast<double>((a * a) + (b * b)));	
}

Category Zoo::getCategory(int x, int y) {
	if (x < 0 && y < 0) {
		return Category::Wild;
	} else if ((y == 0 && x <= 0) || (x == 0 && y <= 0)) {
		return Category::WallCage;
	}
	return Category::Safe;
}

void Zoo::printMST() {
	std::cout << _mst_tot_dist << "\n";

	for (uint32_t id = 0; id < _num_vertices; ++id) {
		uint32_t pv = _primsTable[id].pv;

		if (id != _arbitrary_root_id) { 
			std::cout << id << " " << pv << "\n";
		}
	}
}

void Zoo::primsLinearPartA() {
	_primsTable.resize(_num_vertices);
	_primsTable[_arbitrary_root_id].dv = 0;

	for (uint32_t i = 0; i < _num_vertices; ++i) {
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t id_min = _arbitrary_root_id; //AG warning forced me to init to val.

		//get shortest dist (find id_min)
		for (uint32_t i = 0; i < _num_vertices; i++) {
			if (_primsTable[i].kv == false) {
				if (_primsTable[i].dv < minDist) {
					minDist = _primsTable[i].dv;
					id_min = i;
				}
			}
		}

		//update kv (for id_min)
		_primsTable[id_min].kv = true;

		//update total dist
		_mst_tot_dist += _primsTable[id_min].dv;

		Vertex v1 = _vertices[id_min];
		//update dv,pv (for id_min)
		for (uint32_t i = 0; i < _num_vertices; i++) {
			if (_primsTable[i].kv == false) {
				Vertex v2 = _vertices[i];
				if (!((v1.cat == Category::Safe && v2.cat == Category::Wild) ||
					  (v1.cat == Category::Wild && v2.cat == Category::Safe))) {
						double oldDist = _primsTable[i].dv;
						double newDist = getDistance(v1, v2);
						if (newDist < oldDist) { 
							_primsTable[i].dv = newDist; 
							_primsTable[i].pv = id_min;
						}
				}
			}
		}
	}
}


/*
void Zoo::primsLinear(bool considerCat) {
	initPrimsTable();

	for (uint32_t i = 0; i < _num_vertices; ++i) {
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t id_min = _arbitrary_root_id; //AG warning forced me to init to val.

		//get shortest dist (find id_min)
		for (uint32_t i = 0; i < _num_vertices; i++) {
			if (_primsTable[i].kv == false) {
				if (_primsTable[i].dv < minDist) {
					minDist = _primsTable[i].dv;
					id_min = i;
				}
			}
		}

		//update kv (for id_min)
		_primsTable[id_min].kv = true;

		//update total dist
		_mst_tot_dist += _primsTable[id_min].dv;

		Vertex v1 = _vertices[id_min];
		//update dv,pv (for id_min)
		for (uint32_t i = 0; i < _num_vertices; i++) {
			if (_primsTable[i].kv == false) {
				Vertex v2 = _vertices[i];
				if (!considerCat ||
					!((v1.cat == Category::Safe && v2.cat == Category::Wild) ||
					(v1.cat == Category::Wild && v2.cat == Category::Safe))) {
						double oldDist = _primsTable[i].dv;
						double newDist = getDistance(v1, v2);
						if (newDist < oldDist) { 
							_primsTable[i].dv = newDist; 
							_primsTable[i].pv = id_min;
						}
				}
			}
		}
	}
}

void Zoo::initPrimsTable() {
	_primsTable.resize(_num_vertices);
	_primsTable[_arbitrary_root_id].dv = 0;
}
*/