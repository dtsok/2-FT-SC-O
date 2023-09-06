#ifndef CNDP_COMMON_STRONGLY_CONNECTED_COMPONENT_H_
#define CNDP_COMMON_STRONGLY_CONNECTED_COMPONENT_H_

#include "connected_component.h"

namespace cndp {
namespace common {

class StronglyConnectedComponent final {

	/*----------------------------------------------------------------------------------*
	 | It will be used to calculate the strongly connected components of a given graph  |
	 | by removing the provided vertex. It helps from two different perspectives.		|
	 | 		1. After having the critical node, to decompose the graph and return the	|
	 |			vertex potential.														|
	 |		2. For non-trivial dominator algorithm, return the vertex potential of the 	|
	 |			provided vertex without decomposing the graph.							|
	 *-----------------------------------------------------------------------------------*/
	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	StronglyConnectedComponent();
	StronglyConnectedComponent(int initializer_value);

	//	public Destructor
	virtual ~StronglyConnectedComponent();

	//	public Methods, including static methods
	long ComputeSCCinComponent(ConnectedComponent *cc, int exclude_vertex, bool is_decompose);

	// Setter
	void SetConnectedComponentArray(ConnectedComponent **cc_array);
	void SetInputGraph(DimacsGraph *graph);
	void SetVertexIndexInCC(int *cc_vertex_indexer);
	void SetTotalConnectedCC(int *total_cc);
	void SetDeletedCCCounter(int *total_deleted_cc);

	private:
	//	private Typedefs and Enums /* not defined */
	//	private static const data members /* not defined */

	//	private Methods, including static methods
	void InitSCC(int n);
	void ResetSCC();
	void FindSCCinComponent(int vertex_index, int vertex, int exclude_vertex, bool is_decompose);

	void PopUpStacks(int total_vertex_of_new_cc, int from_index);
	void CreateNewSCC(int total_vertex_of_new_cc, int from_index);

	// For Setter variables
	ConnectedComponent **connected_comp_array = 0;
	DimacsGraph *input_graph = 0;
	int *index_of_vertex_in_cc = 0, *total_connected_cc = 0;
	int *total_deleted_comp = 0;

	// share variables for methods
	ConnectedComponent *connected_comp = 0;
	int *stack_s = 0, *stack_b = 0, *vertex_label_index = 0;
	int stack_s_top = 0, stack_b_top = 0;
	int next_constant = 0, total_vertex = 0;
	long critical_value = 0;
	int counter_deleted_cc = 0;
};
} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_STRONGLY_CONNECTED_COMPONENT_H_ */
