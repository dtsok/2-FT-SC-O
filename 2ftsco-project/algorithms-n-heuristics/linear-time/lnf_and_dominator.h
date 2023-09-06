#ifndef CNDP_ALGORITHM_LINEARTIME_COMBINE_LNF_AND_DOMINATOR
#define CNDP_ALGORITHM_LINEARTIME_COMBINE_LNF_AND_DOMINATOR

#include "trees.h"

namespace cndp {
namespace algorithms {
namespace linear_time {
class LnfAndDominator {

	/* It compute the header of loop nesting tree and immediate dominator of dominator tree */

	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	public:
	LnfAndDominator();

	//	public Destructor
	virtual ~LnfAndDominator();

	//	public Methods, including static methods
	void InitComputation(cndp::common::DimacsGraph *dimacs_graph);
	void SetVertexIndexInCC(int *cc_vertex_indexer);
	Trees *ComputeLNTandDominators(cndp::common::ConnectedComponent *cc, int root);

	private:
	// added below variable
	int myBuffer_size;
	//	private Typedefs and Enums /* not defined */
	//	private static const data members /* not defined */

	//	private Methods, including static methods

	// DFS
	void DFSinComponent(int vertex_index, int vertex);

	// To find dominators
	void FindDominators(int component_size);
	void RCompress(int v, int c);
	// Translate dominators from DFS numbers to original vertex labels
	void TranslateDom();
	void PrintIdom(int component_size);

	// To find lnf
	void FindLNFheaders(int component_size, int source_root);
	void DfsParentCompress(int vertex);
	int C_pop(int vertex);
	void C_merge(int vertex_1, int vertex_2);
	int Find(int vertex);
	void Unite(int vertex, int parent);
	void CloseVertex(int vertex_x);
	void C_push(int vertex, int source);
	void PrintHeader(int component_size);

	//	private Data Members (except static const data members)

	// input Graph
	cndp::common::DimacsGraph *input_graph = 0;
	cndp::common::ConnectedComponent *connected_component = 0;
	int *index_of_vertex_in_cc = 0;

	// For DFS
	int next_preorder_no = 0;
	int *label2pre = 0, *pre2label = 0, *preparent = 0;

	// Dominator Tree
	int *idom = 0, *semi = 0, *label = 0, *dom = 0, *ubucket = 0;

	// For Loop Nesting Tree
	int c_current_pos = 0;
	int *header = 0, *c_buffer = 0, *c_last = 0, *c_target = 0, *c_next = 0, *ufparent = 0;
};
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
#endif /* CNDP_ALGORITHM_LINEARTIME_COMBINE_LNF_AND_DOMINATOR */
