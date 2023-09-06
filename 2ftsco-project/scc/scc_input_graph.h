#ifndef CNDP_COMMON_SCC_INPUT_GRAPH_H_
#define CNDP_COMMON_SCC_INPUT_GRAPH_H_

#include "../common/connected_component.h"
#include "../common/dimacs_graph.h"

namespace cndp {
namespace common {

class SCC final {

	// public Typedefs and Enums -- not defined.
	// public static const data members -- not defined.

	// public Constructors and assignment operators
	public:
	SCC();

	// Destructor
	virtual ~SCC();

	// public Methods, including static methods
	//  Find SCC in Dimacs Graph
	void InitSCC(DimacsGraph *dimacs_graph);
	int ComputeSCCs();
	void CopySCCs(cndp::common::ConnectedComponent **sccs);

	private:
	// private Typedefs and Enums -- not defined.
	// private static const data members -- not defined.

	// private Methods, including static methods
	void SetVerticesIndexOfNextSCC();
	void SetIndexForSCC(int component_number);

	// Find SCC in Dimacs Graph
	void FindSCCinDimacsGraph(int v_i);
	void PrintSCCVertices(int scc_index);
	void DestroySCC();

	// private Data Members

	DimacsGraph *dimacs_graph = 0;
	int next_constant = 0;
	int *stack_s = 0, *stack_b = 0;
	int stack_s_top = 0, stack_b_top = 0;
	int *stack_i = 0;

	int *component_vertices = 0;
	int total_scc_vertices = 0;

	int *components = 0;
	int *vertex_scc_id = 0;
	int total_component = 0;
	int start_index = 0, end_index = 0, looper = 0;
};

} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_SCC_INPUT_GRAPH_H_ */
