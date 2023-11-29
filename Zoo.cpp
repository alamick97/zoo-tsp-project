//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include "Zoo.h"

Zoo::Zoo(int argc, char** argv) : _argc(argc), _argv(argv) {
    _mode_flag = false;
	_mode = Mode::Unspecified;
	_num_vertices = 0;
	_arbitrary_root_id = 0;
	_mst_tot_dist = 0;

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
	primsLinear();
	printMST();
} 

void Zoo::runFASTTSP() {
	//TODO: IMPLEMENT (Part B)
	//Cristofedes Alg
	christofidesAlg();
}

void Zoo::runOPTTSP() {
	//TODO: IMPLEMENT (Part C)
}

void Zoo::christofidesAlg() {
	//step 1: find odd degree vertices in MST
	std::unordered_set<uint32_t> odd_vertices = getOddVertices();

	//step 2: find MWPM amongst odd deg vertices
	findMWPM(odd_vertices);

	//step 3: combine MST and MWPM

	//step 4: find eulerian circuit

	//step 5: convert to hamiltonian circuit 

}

//============WIP ZONE START===========================================
void Zoo::findMWPM(std::unordered_set<uint32_t> odd_vertices) {
	std::vector<Edge> potentialEdges; //set size to (v^2)/2?

	//step 1: Create a list of all possible pairs of odd-degree vertices with their corr. edge weights
	for (uint32_t u : odd_vertices) {
		for (uint32_t v : odd_vertices) {
			if (u < v) {
				potentialEdges.emplace_back(u, v, getDistance(_vertices[u], _vertices[v]));
			}
		}
	}

	//step 2: sort the list based on edge weights in asc. order
	/*pseudo:
		- use a PQ
	*/


	//
}
//============WIP ZONE END=============================================

std::unordered_set<uint32_t> Zoo::getOddVertices() {
	primsLinear(); //generates MST	

	std::vector<uint32_t> count(_num_vertices, 1);
	count[0] = 0; //MST root 

	for (uint32_t id = 1; id < _num_vertices; ++id) {
		uint32_t pv = _primsTable[id].pv;
		count[pv]++;
	}	

	std::unordered_set<uint32_t> odd_vertices;

	for (uint32_t id = 0; id < _num_vertices; ++id) {
		if (count[id] % 2 != 0) {
			odd_vertices.insert(id);
		}
	}

	return odd_vertices;
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

void Zoo::primsLinear() {
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

void Zoo::initPrimsTable() {
	_primsTable.resize(_num_vertices);
	_primsTable[_arbitrary_root_id].dv = 0;
}

//helper func
Category Zoo::getCategory(int x, int y) {
	if (x < 0 && y < 0) {
		return Category::Wild;
	} else if ((y == 0 && x <= 0) || (x == 0 && y <= 0)) {
		return Category::WallCage;
	}
	return Category::Safe;
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