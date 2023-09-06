#ifndef CNDP_COMMON_DOMINATOR_H_
#define CNDP_COMMON_DOMINATOR_H_

#include "connected_component.h"

namespace cndp {
namespace common {

class Dominator {

	/* --------------------------------------------------------------------------*
	 | It computes the dominator tree of unweighted directed graph in 			 |
	 | O(m_log_n) time by using the algorithm simple lengauer tarjan.	 		 |
	 | For the details of the algorithm please refer the following paper,		 |
	 |	Thomas Lengauer and Robert Endre Tarjan									 |
	 |	A fast algorithm for finding dominators in a flowgraph					 |
	 |	ACM Transactions on Programming Language and Systems, 1(1):121-141, 1979.|
	 *---------------------------------------------------------------------------*/

	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	Dominator();

	//	public Destructor
	virtual ~Dominator();

	//	public Methods, including static methods
	void InitComputation(DimacsGraph *G);
	void SetVertexIndexInCC(int *cc_vertex_indexer);
	int *ComputeDominatorTree(ConnectedComponent *cc, int root);
	void PrintIdom(int total_vertices);

	private:
	//	private Typedefs and Enums /* not defined */
	//	private static const data members /* not defined */

	//	private Methods, including static methods
	void RCompress(int v, int c);

	// Translate dominators from DFS numbers to original vertex labels
	void TranslateDom();
	void FindDominators();
	void DFSinComponent(int vertex_index, int vertex);

	// private variables
	// input Graph
	DimacsGraph *input_graph = 0;
	ConnectedComponent *connected_component = 0;
	int source_root = 0;
	int *index_of_vertex_in_cc = 0;

	// For DFS
	int next_preorder_no = 0, total_vertices = 0;
	int *label2pre = 0, *pre2label = 0, *preparent = 0;

	// Dominator Tree
	int *idom = 0, *semi = 0, *label = 0, *dom = 0, *ubucket = 0;
};
} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_DOMINATOR_H_ */
