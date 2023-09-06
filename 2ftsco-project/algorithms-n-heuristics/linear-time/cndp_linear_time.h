#ifndef CNDP_ALGORITHM_LINEAR_TIME_H_
#define CNDP_ALGORITHM_LINEAR_TIME_H_

/* the common header file are already included in critical node header file */
#include "lnf_and_dominator.h"

namespace cndp {
namespace algorithms {
namespace linear_time {

class LinearTime {

	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	LinearTime();
	virtual ~LinearTime();

	//	public Methods, including static methods
	void InitComputation(cndp::common::DimacsGraph *input_graph, int *index_of_v_in_cc, int total_vertices);
	bool FindCriticalNode(cndp::common::ConnectedComponent *cc);
	bool FindCriticalNodeHeuristic1(cndp::common::ConnectedComponent *cc);
	bool FindCriticalNodeHeuristic2(cndp::common::ConnectedComponent *cc, bool *points, int *reverseMap);
	bool FindCriticalNode3Connected(cndp::common::ConnectedComponent *cc);

	private:
	bool is3Connected();
	//	private Typedefs and Enums. /* not defined */
	//	private static const data members /* not defined */

	// private variables

	cndp::common::DimacsGraph *input_graph = nullptr;
	LnfAndDominator *ptr_lntdom = nullptr;
	int *index_of_v_in_cc = nullptr;
	int *common_descendants_size = nullptr;
	long *ncr_common_descendants_sum = nullptr;
	void ComputeSCCsInCommonDescendants(Trees *forward_tree, Trees *reverse_tree);
};
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
#endif /* CNDP_ALGORITHM_LINEAR_TIME_H_ */
