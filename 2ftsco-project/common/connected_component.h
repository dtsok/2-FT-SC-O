#ifndef CNDP_COMMON_CONNECTED_COMPONENT_H_
#define CNDP_COMMON_CONNECTED_COMPONENT_H_

#include "dimacs_graph.h" // dimacs graph

namespace cndp {
namespace common {

class ConnectedComponent {

	/*----------------------------------------------------------------------------------*
	 | It contain the information of a strongly connected component (scc) as following  |
	 |	1. It's size and vertices														|
	 | |	2. It stores the vertices by indexing											|
	 | |	3. Component potential															| |	4. Highest critical
	 node of a component											| |	5. Vertex Potential
	 | | In addition, It also helps to find the connectivity of the component without its	| | particular vertex.
	 | | Remark: The vertex potential, the component potential is calculated either		| | during the creation of
	 its new object or will set after the creation.			| | The calculation depends on the algorithm or
	 heuristic							|
	 *----------------------------------------------------------------------------------*/

	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */

	//	public Constructors and assignment operators
	ConnectedComponent();

	/* potential of the component will find later */
	ConnectedComponent(int cc_size);

	/* if we already have the potential of the component */
	// ConnectedComponent(int cc_size, long potential);

	//	public Destructor
	virtual ~ConnectedComponent();

	//	public Methods, including static methods

	// Method to compute
	bool IsConnectedWithoutVertex(DimacsGraph *dimacs_graph, int *index_of_vertex_in_cc, int exclude_index);
	void AddVertex(int v);

	// setter
	void SetCriticalNodeLong(int node, long value);
	void SetCriticalNodeLDouble(int node, long double value);

	// getter
	//  give the total vertex of the component.
	int GetTotalVertices();

	// get the vertex at a given index
	int GetVertex(int index);

	// get the critical node of the component
	int GetCriticalNode();

	// get the critical node value of a component.
	long GetCriticalNodeValueLong();

	// get the critical node value of the component.
	long double GetCriticalNodeValueDouble();

	// get the highest vertex potential of the component
	long GetHighestVertexPotential(long current_potential);

	// get the component potential
	long GetComponentPotential();

	// It will cotain the information that whether the critical node of a component is calculated or not.
	bool IsCalculated();

	// Helper to debug
	// print the edges between the vertices of the component on console
	void PrintEdges(DimacsGraph *input_graph, int *vertex_index_in_cc);

	// print the vertices of the component on console
	void PrintVertices();

	// Export the vertices of the component on a text file
	void ExportVertices(string export_file_name);

	private:
	//	private Typedefs and Enums /* not defined */
	//	private static const data members /* not defined */

	//	private Methods, including static methods

	/*
	 * To check the connectivity of a component without its single vertex (exclude_vertex)
	 */
	void DFSinComponentByOutGoingEdges(int vertex_index, int vertex, int exclude_vertex);
	void DFSinComponentByInComingEdges(int vertex_index, int vertex, int exclude_vertex);

	// private variables

	// For the computation
	int *vertices = 0;
	int component_size = 0, counter = 0;

	int highest_critical_node = 0;
	long highest_critical_value_long = 0;
	long double highest_critical_value_double = 0;

	long component_potential = 0;
	bool is_calculated = false;

	// For the DFS
	DimacsGraph *input_graph = 0;
	int total_connected_counter = 0;
	int *index_of_vertex_in_cc = 0;
	bool *label = 0;
};
} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_CONNECTED_COMPONENT_H_ */
