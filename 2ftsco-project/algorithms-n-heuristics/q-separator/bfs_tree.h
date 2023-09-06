#ifndef QSEPARATOR_WITH_BFS_H_
#define QSEPARATOR_WITH_BFS_H_

#include "../../common/dimacs_graph.h"

class qSeparator {
	public:
	qSeparator();
	qSeparator(cndp::common::DimacsGraph *G, int delta);
	virtual ~qSeparator();
	bool *sep;
	bool has_small_diameter = false;
	bool found = false;
	int diam = 0;
	int sep_size = 0;

	private:
	void BFS(cndp::common::DimacsGraph *G);
	double maxSize;
	int root;
	int sqrt_n;
	int tree_depth = 0;
	std::queue<int> q;
	bool *visited;
	int *depths;
	int *layer_size;
	int *parents;
	bool findSeparators(cndp::common::DimacsGraph *G, int R);
	void copyResults(int n, bool *separators);
	void printInfo(int size);
};

#endif