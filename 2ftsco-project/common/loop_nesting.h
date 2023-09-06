#ifndef CNDP_ALGORITHM_LOOPNESTING_TREE_
#define CNDP_ALGORITHM_LOOPNESTING_TREE_

#include "connected_component.h"

namespace cndp {
namespace common {

class LoopNesting {

	/* --------------------------------------------------------------------------*
	 | It computes the loop nesting tree of unweighted directed graph in 		 |
	 | O(m+n) time by using the algorithm purpose by R. E. Tarjan.	 		 	 |
	 | For the details of the algorithm please refer the following paper,		 |
	 | R. E. Tarjan. Edge-disjoint spanning trees and depth-first search.		 |
	 | Acta Informatica, 6(2):171–85, 1976.										 |
	 *---------------------------------------------------------------------------*/

	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	LoopNesting();

	//	public Destructor
	virtual ~LoopNesting();

	void InitComputation(DimacsGraph *dimacs_graph);
	void SetVertexIndexInCC(int *cc_vertex_indexer);
	int *ComputeLoopNestingTree(ConnectedComponent *cc, int root);
	void PrintHeader(int component_size);

	private:
	// Method to compute the loop nesting tree
	void DfsParentCompress(int vertex);
	int C_pop(int vertex);
	void C_merge(int vertex_1, int vertex_2);
	int Find(int vertex);
	void Unite(int vertex, int parent);
	void CloseVertex(int vertex_x);
	void C_push(int vertex, int source);

	// Method to create the dfs tree
	void DFSinComponent(int vertex_index, int vertex);

	// input parameter
	DimacsGraph *input_graph = 0;
	ConnectedComponent *connected_component = 0;
	int *index_of_vertex_in_cc = 0;

	// For Loop Nesting Tree
	int c_current_pos = 0;
	int *header = 0, *c_buffer = 0, *c_last = 0, *c_target = 0, *c_next = 0, *ufparent = 0;

	// For DFS
	int next_preorder_no = 0;
	int *label2pre = 0, *pre2label = 0;
};
} // namespace common
} // namespace cndp
#endif /* CNDP_ALGORITHM_LOOPNESTING_TREE_ */
