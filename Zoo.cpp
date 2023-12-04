//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include "Zoo.h"

Zoo::Zoo(int argc, char** argv) : _argc(argc), _argv(argv) {
    _mode_flag = false;
	_mode = Mode::Unspecified;
	_num_vert = 0;
	_arbitrary_root_id = 0;
	_mst_tot_dist = 0;
	_fast_tot = 0;
	_tot = 0;
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

	std::cin >> _num_vert;
	_vert.reserve(_num_vert);

	for (uint32_t i = 0; i < _num_vert; ++i) { //creates vector of vertices.
		std::cin >> x >> y;
		Category cat = getCategory(x, y);
		_vert.emplace_back(Vertex{x, y, cat}); //no need to store id. vector index is id.
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
	_tot = 0; //safeguard init
	_best_path = _fast_path; //init to Part B path.
	_path = _best_path;

//=========GOOD UP TO HERE #1==========================================================
	genPerms(1); //updates _best_path & _best_tot to approp. vals.

	printTSP(Mode::OPTTSP);
}

//function: updates _best_path & _best_tot
void Zoo::genPerms(uint32_t pLen) {
	if (pLen == _path.size()) { //base case, for (n-1) Complete Paths
		_tot += getAppendCost(_path[_path.size() - 1], _path[0]); //pre-cond: consider cycle-closing edge

		if (_tot < _best_tot) {  //updates best-so-far if better
			_best_path = _path;
			_best_tot = _tot;
		}

		_tot -= getAppendCost(_path[_path.size() - 1], _path[0]); //post-cond: remove cycle-closing edge

		return;
	}

//=========GOOD UP TO HERE #2==========================================================
	if (!promising(pLen)) { return; } //pruning

	for (uint32_t i = pLen; i < _path.size(); ++i) { //continue DFS
		std::swap(_path[pLen], _path[i]);
		_tot += getAppendCost(_path[pLen], _path[pLen + 1]); //pre-cond (trav. to children)
		genPerms(pLen + 1);
		_tot -= getAppendCost(_path[pLen], _path[pLen + 1]); //post-cond (return to parent)
		std::swap(_path[pLen], _path[i]);
	}
}

//Note: pLen is always >= 1
bool Zoo::promising(uint32_t pLen) {
	uint32_t k = _path.size() - pLen;

	if (k <= 4) { return true; } //efficiency guard.

	//lower bound check
//=========GOOD UP TO HERE #3==========================================================
	/*pseudo:
	[] check connecting arm 1
		[] get armCost1
		[] if (_tot + armCost1 >= _best_tot) { return false; }
	[] check connecting arm 2
		[] get armCost2
		[] if (_tot + armCost2 >= _best_tot) { return false; }
	[] check est. of remaining
		[] get MST of remaining ONLY
		[] lowerbound = armCost1 + armCost2 + remainingMST	
	*/

//	[] check connecting arm 1
//		[] get armCost1
	double arm1Cost = armCost(pLen, 0);
//		[] if (_tot + armCost1 >= _best_tot) { return false; }
//	[] check connecting arm 2
//		[] get armCost2
	double arm1Cost = armCost(pLen, pLen - 1);
//		[] if (_tot + armCost2 >= _best_tot) { return false; }
//	[] check est. of remaining
//		[] get MST of remaining ONLY
//		[] lowerbound = armCost1 + armCost2 + remainingMST	






 /*pretty much wrong:
	double lowerbound = getLowerBound(pLen);
	if (lowerbound >= _best_tot) { return false; } //if lowerbound >= upperbound
*/

/*may be unneeded:
	//upper bound check
	if (_tot >= _best_tot) { return false; }
*/
	return true;
}

//Note: pLen is always >= 1
double Zoo::getLowerBound(uint32_t pLen) {
	double rem_tot = primsLinearPartC(pLen);//calc MST len for remaining path
	double lowerbound = _tot + rem_tot;
	return lowerbound;
}

double Zoo::primsLinearPartC(uint32_t pLen) {
	uint32_t first = _path[0]; //connecting arm 1 (to root)
	uint32_t last = _path[pLen - 1]; //connecting arm 2 (to _path[pLen - 1])
	_path.push_back(first); //PRE-CONDITION: Root node
	if (first != last) { _path.push_back(last); }//PRE-CONDITION: End of cycle (last node)

	double total = 0;
	_table.clear();
	_table.resize(_num_vert);
	uint32_t _cur_root = _path[pLen];
	_table[_cur_root].dv = 0;

	for (uint32_t i = pLen; i < _path.size(); ++i) {
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t id_min = _cur_root; //AG warning forced me to init to val.
		
		//find id of min dist
		for (uint32_t idx = pLen; idx < _path.size(); ++idx) { //get rem idx's
			uint32_t id = _path[idx]; //get id from idx
			primsTable& row = _table[id];//get row in prim table
			if (row.kv == false) {
				if (row.dv < minDist) {
					minDist = row.dv;
					id_min = id;
				}
			}
		}

		//update the minimum-dv row
		_table[id_min].kv = true;
		total += _table[id_min].dv;

		//update all dv,pv connected to id_min
		for (uint32_t idx = pLen; idx < _path.size(); ++idx) {
			uint32_t id = _path[idx];//get id from idx
			primsTable& row = _table[id];
			if (row.kv == false) {
				Vertex v1 = _vert[id_min];
				Vertex v2 = _vert[id];	

				double oldDist = row.dv;
				double newDist = dist(v1, v2);
				
				if (newDist < oldDist) {
					row.dv = newDist;
					row.pv = id_min;
				}
			}	
		}
	}
	_path.pop_back(); //POST-CONDITION: Root node
	if (first != last) { _path.pop_back(); }//POST-CONDITION: End of cycle (last node)

	return total;
}

//pointer variant
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
	_fast_path.reserve(_num_vert + 1);	
	_fast_path.push_back(0); //root id (start)
	_fast_path.push_back(0); //root id (end)

	for (uint32_t k = 1; k < _num_vert; ++k) { //rand vert id
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
	Vertex v1 = _vert[i];
	Vertex v2 = _vert[k];
	Vertex v3 = _vert[j];
	double cost = dist(v1, v2) + dist(v2, v3) - dist(v1, v3);

	return cost;
}

double Zoo::dist(Vertex v1, Vertex v2) {
	double x1 = v1.x;
	double x2 = v2.x;
	double y1 = v1.y;
	double y2 = v2.y;
	double a = y2 - y1;
	double b = x2 - x1;

	return std::sqrt(static_cast<double>((a * a) + (b * b)));	
}

//gets cost (always positive) of adding an edge, given two Vertex id's. 
double Zoo::getAppendCost(uint32_t i, uint32_t j) {
	Vertex v1 = _vert[i];
	Vertex v2 = _vert[j];
	double cost = dist(v1, v2);

	return cost;
}

double Zoo::armCost(uint32_t pLen, uint32_t idx) {
	//cost = dist(_path[0], closest_remaining)	
	/*pseudo:
	double min;
	Vertex v1 = _vert[_path[idx]]; //vertex we're connecting from

	for (uint32_t i = pLen; i < _path.size(); i++) {
		Vertex v2 = _vert[_path[i]];
		cost = dist(v1, v2);
		min = min(minCost, cost);
	}	

	return min;
	*/

	double min;

	for (uint32_t i = pLen; i < _path.size(); i++) {
		min = std::min(min, dist(_vert[_path[idx]], _vert[_path[i]]));
	}	

	return min;
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

	for (uint32_t id = 0; id < _num_vert; ++id) {
		uint32_t pv = _table[id].pv;

		if (id != _arbitrary_root_id) { 
			std::cout << id << " " << pv << "\n";
		}
	}
}

void Zoo::primsLinearPartA() {
	_table.resize(_num_vert);
	_table[_arbitrary_root_id].dv = 0;

	for (uint32_t i = 0; i < _num_vert; ++i) {
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t id_min = _arbitrary_root_id; //AG warning forced me to init to val.

		//get shortest dist (find id_min)
		for (uint32_t i = 0; i < _num_vert; i++) {
			if (_table[i].kv == false) {
				if (_table[i].dv < minDist) {
					minDist = _table[i].dv;
					id_min = i;
				}
			}
		}

		//update kv (for id_min)
		_table[id_min].kv = true;

		//update total dist
		_mst_tot_dist += _table[id_min].dv;

		//update dv,pv (for id_min)
		for (uint32_t i = 0; i < _num_vert; i++) {
			if (_table[i].kv == false) {
				Vertex v1 = _vert[id_min];
				Vertex v2 = _vert[i];
				if (!((v1.cat == Category::Safe && v2.cat == Category::Wild) ||
					  (v1.cat == Category::Wild && v2.cat == Category::Safe))) {
						double oldDist = _table[i].dv;
						double newDist = dist(v1, v2);
						if (newDist < oldDist) { 
							_table[i].dv = newDist; 
							_table[i].pv = id_min;
						}
				}
			}
		}
	}
}