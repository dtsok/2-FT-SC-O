#ifndef CNDP_ALGORITHM_LINEARTIME_TREES_H_
#define CNDP_ALGORITHM_LINEARTIME_TREES_H_
#include "tree_graph.h"

namespace cndp {
namespace algorithms {
namespace linear_time {
class Trees {

	/*-------------------------------------------------------------------------------------*
	 | A single graph has two tree graph; one is the dominator, and another is loop nesting.|
	 | So when we compute the graph in the forward direction, we will have one tree which   |
	 | contains the two tree graph (dominator, and loop nesting).The Same for the reverse   |
	 | graph.  So In the computation, we will have two trees (forward and reversed) and	    |
	 | four tree graph.																	    |
	 *--------------------------------------------------------------------------------------*/

	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	Trees();
	Trees(int *idom, int *header, int root, int cc_size, cndp::common::ConnectedComponent *cc);

	//	public Destructor
	virtual ~Trees();

	//	public Methods, including static methods
	// Method to Debug
	void PrintTrees();
	void PrintDsum();
	void PrintDescedents();
	void ExportStepsForReport();

	/* This Method is not used by our project, I just write it at beginning */
	vector<int> GetCommonDominator(int *tree1, int *tree2, int source, int total_vertices);
	/* This Method is not used by our project, I just write it at beginning */
	vector<int> GetCommonDescedents(TreeGraph *forward_tree, TreeGraph *reverse_tree, int vertex);

	int source = 0, total_vertices = 0;
	int *lnt_header = 0, *immediate_dominator = 0;
	TreeGraph *loop_nesting_tree = 0, *dominator_tree = 0;
	long *dsum = 0;

	private:
	void CalculateDescendantsValueByDOMandLNT();
	void ComputeDescedents();
};
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
#endif /* CNDP_ALGORITHM_LINEARTIME_TREES_H_ */
