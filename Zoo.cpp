//Project Identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include "Zoo.h"

Zoo::Zoo(int argc, char** argv) : _argc(argc), _argv(argv) {
    _mode_flag = false;
    _mode = Mode::Unspecified;

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
} //Zoo()
    
	
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
} //readInput()

void Zoo::runMST() {
	//TODO: Implement.
	primsLinear();
	printMST();
} //runMST()

void Zoo::printMST() {
	//TODO: Implement.
	std::cout << _dv_sum << "\n";
}

void Zoo::primsLinear() {
	initPrimsTable(3);
	uint32_t true_count = 0;
	uint32_t prev_id;

	while (true_count < _num_vertices) {
		double minDist = std::numeric_limits<double>::infinity();
		uint32_t id;

//std::cout << "round count: " << true_count << "\n";
		//find smallest dist
		for (uint32_t i = 0; i < _num_vertices; i++) {
//std::cout << "i: " << i << "\n";
			if (_primsTable[i].kv == false) {
//std::cout << "prims is false\n";
//std::cout << "dv: " << _primsTable[i].dv << "\n";
				if (_primsTable[i].dv < minDist) {
//std::cout << "dv is less than minDist\n";
					minDist = _primsTable[i].dv;
					id = i;
				}
			}
//std::cout << "minDist: " << minDist << "\n";
		}


//std::cout << "id: " << id << "\n"; //DEBUGGER

		//update table (set smallest to T). update k
		_primsTable[id].kv = true;
		true_count++;
		_dv_sum += _primsTable[id].dv;

		//update dv,pv (for id, if find smaller dist, overwrite dv * pv)
		for (uint32_t i = 0; i < _num_vertices; i++) {
			if (_primsTable[i].kv == false) {
				Vertex v1 = _vertices[i];
				Vertex v2 = _vertices[id];
				if (!((v1.cat == Category::Safe && v2.cat == Category::Wild) ||
					(v1.cat == Category::Wild && v2.cat == Category::Safe))) {
						double oldDist = _primsTable[i].dv;
std::cout << "oldDist: " << oldDist << "\n";
						double newDist = getDistance(v1, v2);
std::cout << "newDist: " << newDist << "\n";
						if (newDist < oldDist) { 
							_primsTable[i].dv = newDist; 
							if (id != _arbitrary_root_id) { _primsTable[i].pv = id; }
						}
				}
			}
		}
	}
}

void Zoo::initPrimsTable(uint32_t root_id) {
	//set arbitrary root kv to True (& dv to 0?)
	_dv_sum = 0;
	_arbitrary_root_id = root_id;
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
	int a = v1.x;
	int b = v2.y;

	return std::sqrt(static_cast<double>((a * a) + (b * b)));
}