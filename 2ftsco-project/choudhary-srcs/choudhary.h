#ifndef CHOUDHARY_SINGLE_SOURCE_REACHABILITY_DS_H_
#define CHOUDHARY_SINGLE_SOURCE_REACHABILITY_DS_H_

#include "../common/dimacs_graph.h"
#include "../rmq-srcs/RMQ.h"

using namespace rmq;

class Choudhary {
	public:
	/* public constructor */
	Choudhary();

	/* public constructor - initialize whole data structure */
	Choudhary(cndp::common::DimacsGraph *graph, int root);

	/* constructor used for writing directly to file the required arrays */
	Choudhary(cndp::common::DimacsGraph *graph, int root, string dir, int ID);

	/* load/initialize the arrays from the corresponding files */
	void initializeFromFile(string dir, int ID);

	/* public destructor */
	virtual ~Choudhary();

	/* method for answering - in optimal time - reachability queries from root to u. root -> u */
	bool reachabilityQuery(int u, int failed_1, int failed_2);

	/* method for answering he query in a naive way - used for comparison/ debugging only */
	bool reachabilityQueryNaive(int u, int failed_1, int failed_2);

	/* method for answering strong connectivity queries between u and root. root <-> u */
	bool stronglyConnected(int u, int failed_1, int failed_2);

	/* Choudhary object to store info for the reverse graph - reachability towards the root */
	Choudhary *objReverseReach;

	/* root vertex for the disjoint spanning trees */
	int r;

	private:
	/* auxiliary private methods and variables for the "writing" constructor and destructor */
	void initializeDataStructureForWriting(cndp::common::DimacsGraph *graph, int root);
	void writeToFile(string dir, int ID, int graph_size);

	bool init_from_file = false;
	bool write_to_file = false;

	/* initialize Choudhary's data structure for current (sub)graph */
	void initializeDataStructure(cndp::common::DimacsGraph *graph, int root);

	/* initialize Choudhary's data structure for the reverse graph */
	void initializeReverseDataStructure(cndp::common::DimacsGraph *graph, int root);

	/* arrays for the construction of the disjoint spanning trees */
	int *blue;		// blue spanning tree
	int *red;		// red spanning tree
	int *tempBlue;	// red spanning tree
	int *tempRed;	// red spanning tree
	int *idom;		// idom[v]=immediate dominator of v

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
	void compute_dst(cndp::common::DimacsGraph *graph, int r);

	/* store depth for the nodes in blue/red tree */
	int *arrayDepthsBlue;
	int *arrayDepthsRed;
	void findDepthOfBlueRed(cndp::common::DimacsGraph *graph);
	void recursionForDepth(int *array, int *arrayDepths, bool *traversed, int j);

	/* for metric alpha() - store depths of idoms */
	int *arrayIdomDepthsBlue;
	int *arrayIdomDepthsRed;
	void findIdomDepths(cndp::common::DimacsGraph *graph);

	/* for metric beta() - find first vertices of detours and corresponding depths */
	int *depthOfDetoursRED;	 // for metric β()
	int *depthOfDetoursBLUE; // for metric β()
	int *firstVerticesRED;	 // for metric β()
	int *firstVerticesBLUE;	 // for metric β()

	/* for metric gamma() - find first vertices of ancestor detours and corresponding depths */
	int *depthOfAD_bb;
	int *depthOfAD_br;
	int *depthOfAD_rb;
	int *depthOfAD_rr;
	int *firstVerticesAD_bb;
	int *firstVerticesAD_br;
	int *firstVerticesAD_rb;
	int *firstVerticesAD_rr;
	int *pseudoParentsBlue;
	int *pseudoParentsRed;

	/* main methods used for the detours/ ancestor detours */
	void findDetours(cndp::common::DimacsGraph *graph);
	void findPseudoParents(int total_nodes);
	void findAncestorsDetours(cndp::common::DimacsGraph *graph);
	void dfsForDetour(cndp::common::DimacsGraph *graph, bool *visited, int fromVertex, int startingVertex);
	void dfsForDetourPB(cndp::common::DimacsGraph *graph, bool *visited, int fromVertex, int startingVertex);
	void dfsForDetourPR(cndp::common::DimacsGraph *graph, bool *visited, int fromVertex, int startingVertex);

	/* objects of various classes used for searching the requried vertices in optimal time - range minima queries
	 * classes ~> implemented with sparse tables */
	NCA *ncaObj;					// for finding the nearest common ancestor in the dominator tree (idom(v))
	ChildQueries *nextDominatorObj; // for finding the next dominator (v) in the dominator tree
	ChildQueries
		*nextVertexBlue; // for finding the child vertex of a node in the tree, used for finding the child of f_i
	ChildQueries *nextVertexRed;

	TreeRMQ2 *obj_x1; // for finding vertex x_i in path P(~fi,v)
	TreeRMQ2 *obj_x2;

	TreeRMQ *obj_w1; // for finding vertex w in path P(~fi,~xi)
	TreeRMQ *obj_w2;

	TreeRMQ2 *obj_wbb; // for finding vertex w in path P(~xj,v)
	TreeRMQ2 *obj_wbr;
	TreeRMQ2 *obj_wrb;
	TreeRMQ2 *obj_wrr;

	/* method used for the initialization of the above objects */
	void initializeRangeMinima(cndp::common::DimacsGraph *graph);

	/* main methods used for the searching of the requried vertices in an optimal way */
	int findX(char tree, int v, int child_f);
	int findWcase2(char tree, int v, int child_f);
	int findWcase3(char i, char j, int v, int xj, int child_xj);

	/* check if vertex x is ancestor of y in blue/red tree */
	bool isAncestor(char tree, int x, int y);

	/* method used for deleting/freeing up memory allocated for aux arrays */
	void deleteAuxArrays();

	/* AUX METHODS USED ONLY FOR DEBUGGING / VALIDATING RESULTS */

	/* for debugging - find detours using non-recursice DFS - some large graphs leaded to segmentation fault due to
	 * RAM - stack limitations */
	void nonRecursiveDetours(cndp::common::DimacsGraph *graph);

	/* for debugging - finding detours - non-optimal way*/
	void findDetoursNO(cndp::common::DimacsGraph *graph);
	void dfsForDetourNO(cndp::common::DimacsGraph *graph, bool *ancestors2avoid, bool *visited, int fromVertex);
	void findDepthInDominatorTree(cndp::common::DimacsGraph *graph, int *storeDepths);
	/* for debugging - find ancestor detours non-optimal way*/
	void findAncestorsDetoursNO(cndp::common::DimacsGraph *graph);

	/* for debugging - naive methods for searching the required vertices - case 3 of choudharys algorithm ~> O(n)
	 * worst case*/
	int findXNaive(int *tree, int *depth, int fromVertex, int upToVertex, int total_nodes);
	int findWNaive(int *tree, int *depth, int fromVertex, int upToVertex, int total_nodes);
	int findADNaive(int *tree, int *depth, int *ad_depth, int fromVertex, int upToVertex, int total_nodes);
	int lcaNaive(int *parent, int *depth, int x, int y);	  // lowest-common-ancestor
	int nextDominatorNaive(int u, int idomV, int f1, int f2); // level-ancestor problem
};

#endif
