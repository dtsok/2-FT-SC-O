#ifndef AUX_CLASS_ONLY_DOMINATOR_TREE_H_
#define AUX_CLASS_ONLY_DOMINATOR_TREE_H_

#include "../../common/dimacs_graph.h"

class dominatorTree {
	public:
	int r;	   // root
	int *idom; // idom[v]=immediate dominator of v
	dominatorTree();
	dominatorTree(cndp::common::DimacsGraph *graph, int root);
	virtual ~dominatorTree();

	private:
	int *dom;		// dominator tree
	int *label2pre; // mapping node id -> preoder number
	int *pre2label; // mapping preoder number -> node id
	int *parent;	// parents in the link-eval tree
	int next;		// next preorder number
	int *semi;
	int *label;
	int *ubucket;
	int count; // comparison counter
	int nr;	   // number of vertices reachable from r

	void printTree(int *p, int n, char c);
	inline void incc();
	inline void rcompress(int v, int c);
	void initComputation(int N);
	void dfsForConstruction(cndp::common::DimacsGraph *graph, int k);
	void computeDominatorTree(cndp::common::DimacsGraph *graph, int root);
};

#endif