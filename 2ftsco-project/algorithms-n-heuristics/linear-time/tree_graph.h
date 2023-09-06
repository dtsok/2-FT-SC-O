#ifndef CNDP_ALGORITHM_LINEARTIME_TREE_GRAPH_H_
#define CNDP_ALGORITHM_LINEARTIME_TREE_GRAPH_H_

#include "../../common/connected_component.h"

namespace cndp {
namespace algorithms {
namespace linear_time {

class TreeGraph {

	/*--------------------------------------------------------------------------*
	 | When we computed the immediate dominator of dominator tree or immediate  |
	 | header of loop nesting tree, then we have to process those value to get  |
	 | the descendants, bundles of each vertex by creating the tree graph.		|
	 *--------------------------------------------------------------------------*/
	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	TreeGraph();
	TreeGraph(int *tree, int root, int vertex_size, cndp::common::ConnectedComponent *cc);

	//	public Destructor
	virtual ~TreeGraph();

	// Outgoing edges and decendant size of a vertex.
	int GetOutEdgeStartIndex(int vertex);
	int GetOutEdge(int index);
	int GetDescendantSize(int vertex);

	// Preorder number
	int GetTotalPreOrderNo();
	int GetPreOrderToVertex(int preorder_no);
	int GetVertexToPreOrderNo(int vertex);

	// Helper Methods to Debug
	void PrintInputTree(string tree_name);
	void PrintTree();

	/* Descendants Debugging Methods */
	void ComputeDescedents();
	int GetDescendantStartIndex(int vertex);
	int GetDescendantEndIndex(int vertex);
	int GetDescendant(int index);
	bool descendants_computed = false;

	private:
	//	private Typedefs and Enums /* not defined */
	//	private static const data members /* not defined */

	//	private Methods, including static methods
	void CreateTreeGraph();
	void DFSinTree(int vertex);

	//	private Data Members (except static const data members)
	int total_vertices = 0, source_root = 0;
	int *vertex_tree = 0;
	int total_preorder_no = 0;
	int *label2pre = 0, *pre2label = 0;

	// To calculate the descedants
	int *start_index = 0, *end_index = 0, *descendants = 0;
	int *out_edges_start_index = 0, *out_edges = 0;
	int *descendant_size = 0;

	// Class objects and pointers
	cndp::common::ConnectedComponent *connected_component = 0;
};
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
#endif /* CNDP_ALGORITHM_LINEARTIME_TREE_GRAPH_H_ */
