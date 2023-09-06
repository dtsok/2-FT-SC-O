#include "connected_component.h"

namespace cndp {
namespace common {
//	public Constructors and assignment operators
ConnectedComponent::ConnectedComponent()
{

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
}
ConnectedComponent::ConnectedComponent(int cc_size)
{
	vertices = new int[cc_size + 1]; // to start the index from 1
	vertices[counter++] = 0;
	component_size = cc_size;
	component_potential = ((long)cc_size * (cc_size - 1)) / 2;
}

//	public Destructor
ConnectedComponent::~ConnectedComponent()
{
	delete[] vertices;
}

//	public Methods, including static methods
bool ConnectedComponent::IsConnectedWithoutVertex(DimacsGraph *dimacs_graph, int *index_of_vertex_in_cc,
												  int exclude_index)
{

	// Assign the parameter
	this->input_graph = dimacs_graph;
	this->index_of_vertex_in_cc = index_of_vertex_in_cc;

	int cc_size = this->GetTotalVertices();
	label = new bool[cc_size + 1];

	int vertex_index = exclude_index == 1 ? 2 : 1;
	int root_vertex = this->GetVertex(vertex_index);
	int exclude_vertex = this->GetVertex(exclude_index);

	// Check whether all the vertices are reachable from the root or not
	// By excluding the excluded vertex.
	for (int i = 0; i <= cc_size; i++) {
		label[i] = false;
	}
	this->total_connected_counter = 0;
	DFSinComponentByOutGoingEdges(vertex_index, root_vertex, exclude_vertex);
	bool reahable_from_root = ((cc_size - 1) == total_connected_counter);

	// Check whether all the vertices are reachable to the root or not
	// By excluding the excluded vertex.
	for (int i = 0; i <= cc_size; i++) {
		label[i] = false;
	}
	this->total_connected_counter = 0;
	DFSinComponentByInComingEdges(vertex_index, root_vertex, exclude_vertex);
	bool reahable_to_root = ((cc_size - 1) == total_connected_counter);

	bool is_connected_without_excluded_vertex = (reahable_from_root && reahable_to_root);

	delete[] label;

	return is_connected_without_excluded_vertex;
}

void ConnectedComponent::AddVertex(int v)
{

	if (counter > component_size) {
		std::cerr << "Sorry, component is already full with vertices." << std::endl;
		exit(-1);
	}
	vertices[counter++] = v;
}

// setter
void ConnectedComponent::SetCriticalNodeLong(int node, long value)
{
	highest_critical_node = node;
	highest_critical_value_long = value;
	is_calculated = true;
}
void ConnectedComponent::SetCriticalNodeLDouble(int node, long double value)
{
	highest_critical_node = node;
	highest_critical_value_double = value;
	is_calculated = true;
}

// getter
int ConnectedComponent::GetTotalVertices()
{
	return (counter - 1);
}
int ConnectedComponent::GetVertex(int index)
{
	return this->vertices[index];
}
int ConnectedComponent::GetCriticalNode()
{
	// cout << "CN:" << highest_critical_node << endl;
	return highest_critical_node;
}
long ConnectedComponent::GetCriticalNodeValueLong()
{
	return this->highest_critical_value_long;
}
long double ConnectedComponent::GetCriticalNodeValueDouble()
{
	return this->highest_critical_value_double;
}

long ConnectedComponent::GetHighestVertexPotential(long current_potential)
{
	return (current_potential - component_potential + highest_critical_value_long);
}

long ConnectedComponent::GetComponentPotential()
{
	return this->component_potential;
}

bool ConnectedComponent::IsCalculated()
{

	/* The critical node of a component can calculate and store in any round.
	 If the critical node was not selected from this component in that round,
	 then the component will exist for the next round and keep its critical
	 node. But we don't have to recalculate its critical node again.*/

	return this->is_calculated;
}
// print edges
void ConnectedComponent::PrintEdges(DimacsGraph *input_graph, int *vertex_index_in_cc)
{
	for (int i = 1; i < counter; i++) {

		int v = this->GetVertex(i);

		for (int j = input_graph->out_edges_start_index[v]; j <= input_graph->out_edges_end_index[v]; j++) {

			int w = input_graph->out_edges[j];
			if (input_graph->deleted_vertices[w] || vertex_index_in_cc[w] == 0) {
				continue;
			}

			std::cout << v << "-->" << w << std::endl;
		}
	}
}
void ConnectedComponent::PrintVertices()
{
	std::cout << "Component vertices:";
	for (int i = 1; i < (counter - 1); i++) {
		std::cout << vertices[i] << ",";
	}
	std::cout << vertices[counter - 1] << std::endl;
}
void ConnectedComponent::ExportVertices(std::string export_file_name)
{
	//	//Export vertices in a file
	std::ofstream out_v;
	out_v.open(export_file_name.c_str());
	for (int i = 1; i < (counter - 1); i++) {
		out_v << vertices[i] << ",";
	}
	out_v << vertices[counter - 1] << std::endl;
	out_v << "\n" << std::endl;
	out_v.close();
}
void ConnectedComponent::DFSinComponentByOutGoingEdges(int vertex_index, int vertex, int exclude_vertex)
{

	total_connected_counter++;
	label[vertex_index] = true;

	// scan the outgoing edges from realV
	for (int i = this->input_graph->out_edges_start_index[vertex];
		 i <= this->input_graph->out_edges_end_index[vertex]; i++) {

		int adj_vertex = this->input_graph->out_edges[i];

		// if adj_vertex is obsolete(=0) OR already deleted  then just continue
		if (adj_vertex == 0 || this->input_graph->deleted_vertices[adj_vertex] || adj_vertex == exclude_vertex) {
			continue;
		}

		int adj_vertex_index = this->index_of_vertex_in_cc[adj_vertex];
		// if adj_vertex is not in this component then just continue
		if (adj_vertex_index == 0) {
			continue;
		}

		// if  adj_vertex was not discovered yet.
		if (!label[adj_vertex_index]) {
			DFSinComponentByOutGoingEdges(adj_vertex_index, adj_vertex, exclude_vertex);
		}
	}
}

void ConnectedComponent::DFSinComponentByInComingEdges(int vertex_index, int vertex, int exclude_vertex)
{

	total_connected_counter++;
	label[vertex_index] = true;

	// scan the outgoing edges from realV
	for (int i = this->input_graph->in_edges_start_index[vertex];
		 i <= this->input_graph->in_edges_end_index[vertex]; i++) {

		int adj_vertex = this->input_graph->in_edges[i];

		// if adj_vertex is obsolete(=0) OR already deleted  then just continue
		if (adj_vertex == 0 || this->input_graph->deleted_vertices[adj_vertex] || adj_vertex == exclude_vertex) {
			continue;
		}

		int adj_vertex_index = this->index_of_vertex_in_cc[adj_vertex];
		// if adj_vertex is not in this component then just continue
		if (adj_vertex_index == 0) {
			continue;
		}

		// if  adj_vertex was not discovered yet.
		if (!label[adj_vertex_index]) {
			DFSinComponentByInComingEdges(adj_vertex_index, adj_vertex, exclude_vertex);
		}
	}
}

} // namespace common
} // namespace cndp
