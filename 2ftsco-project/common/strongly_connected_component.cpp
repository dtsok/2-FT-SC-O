#include "strongly_connected_component.h"

namespace cndp {
namespace common {
//	public Constructors and assignment operators
StronglyConnectedComponent::StronglyConnectedComponent()
{

	/*----------------------------------------------------------------------------------*
	 | It will be used to calculate the strongly connected components of a given graph  |
	 | by removing the provided vertex. So, it helps from two different perspectives.	|
	 | 		1. After having the critical nodes, to decompose the graph and return the	|
	 |			vertex potential.														|
	 |		2. For non-trivial dominator algorithm, return the vertex potential of the 	|
	 |			provided vertex without decomposing the graph.							|
	 *-----------------------------------------------------------------------------------*/
}
StronglyConnectedComponent::StronglyConnectedComponent(int initializer_value)
{
	this->InitSCC(initializer_value);
}

//	public Destructor
StronglyConnectedComponent::~StronglyConnectedComponent()
{
	delete[] this->stack_s;
	delete[] this->stack_b;
	delete[] this->vertex_label_index;
}

//	public Methods, including static methods
long StronglyConnectedComponent::ComputeSCCinComponent(ConnectedComponent *cc, int exclude_vertex,
													   bool is_decompose)
{

	this->connected_comp = cc;
	this->total_vertex = cc->GetTotalVertices();
	this->next_constant = this->total_vertex;
	this->counter_deleted_cc = 0;
	this->critical_value = 0;

	for (int i = 1; i <= total_vertex; i++) {

		int vertex = this->connected_comp->GetVertex(i);

		if (this->input_graph->deleted_vertices[vertex] || vertex == exclude_vertex) {
			continue;
		}

		if (this->vertex_label_index[i] == -1) {
			this->FindSCCinComponent(i, vertex, exclude_vertex, is_decompose);
		}

	} // end for

	*(this->total_deleted_comp) += this->counter_deleted_cc;
	this->ResetSCC();

	// cout << "critical_value:" << critical_value << endl;
	return critical_value;
}

void StronglyConnectedComponent::SetConnectedComponentArray(ConnectedComponent **cc_array)
{
	this->connected_comp_array = cc_array;
}
void StronglyConnectedComponent::SetInputGraph(DimacsGraph *graph)
{
	this->input_graph = graph;
}
void StronglyConnectedComponent::SetVertexIndexInCC(int *cc_vertex_indexer)
{
	this->index_of_vertex_in_cc = cc_vertex_indexer;
}
void StronglyConnectedComponent::SetTotalConnectedCC(int *total_cc)
{
	this->total_connected_cc = total_cc;
}
void StronglyConnectedComponent::SetDeletedCCCounter(int *total_deleted_cc)
{
	this->total_deleted_comp = total_deleted_cc;
}

//	private Methods, including static methods
void StronglyConnectedComponent::InitSCC(int n)
{
	this->stack_s = new int[n];
	this->stack_b = new int[n];
	this->vertex_label_index = new int[n];
	this->next_constant = n + 2;

	for (int i = 0; i < n; i++) {
		this->vertex_label_index[i] = -1;
	}
	this->stack_s_top = this->stack_b_top = -1;
}
void StronglyConnectedComponent::ResetSCC()
{
	// reset stack_s
	if (this->stack_s == 0) {
		cerr << "Sorry, stack_s is not initialize. SCC::reset()" << endl;
		exit(-1);
	}
	for (int i = 0; i <= this->stack_s_top; i++) {
		this->stack_s[i] = -1;
	}
	this->stack_s_top = -1;

	// reset stack_b
	if (this->stack_b == 0) {
		cerr << "Sorry, stack_b is not initialize. SCC::reset()" << endl;
		exit(-1);
	}
	for (int i = 0; i <= this->stack_b_top; i++) {
		this->stack_b[i] = -1;
	}
	this->stack_b_top = -1;

	// reset vertex_label_index
	if (this->vertex_label_index == 0) {
		cerr << "Sorry, vertex_label_index is not initialize. SCC::reset()" << endl;
		exit(-1);
	}
	for (int i = 0; i <= total_vertex; i++) {
		this->vertex_label_index[i] = -1;
	}
	// reset other variables.
	this->next_constant = 0;
	this->counter_deleted_cc = 0;
}
void StronglyConnectedComponent::FindSCCinComponent(int vertex_index, int vertex, int exclude_vertex,
													bool is_decompose)
{

	this->stack_s[++this->stack_s_top] = vertex_index;
	int from_index = this->stack_s_top;
	this->vertex_label_index[vertex_index] = from_index;
	this->stack_b[++this->stack_b_top] = this->vertex_label_index[vertex_index];

	// scan the outgoing edges from vertex
	for (int i = this->input_graph->out_edges_start_index[vertex];
		 i <= this->input_graph->out_edges_end_index[vertex]; i++) {

		int adj_vertex = this->input_graph->out_edges[i];
		// if adj_vertex is obsolete(=0) OR already deleted  then just continue
		if (adj_vertex == 0 || input_graph->deleted_vertices[adj_vertex] || adj_vertex == exclude_vertex) {
			continue;
		}

		int adj_vertex_index = this->index_of_vertex_in_cc[adj_vertex];

		// if adj_vertex is not in this subgraph then just continue
		if (adj_vertex_index == 0) {
			continue;
		}
		int adjv_label_index = this->vertex_label_index[adj_vertex_index];
		if (adjv_label_index == -1) {
			this->FindSCCinComponent(adj_vertex_index, adj_vertex, exclude_vertex, is_decompose);
		}
		else {
			while ((this->stack_b[this->stack_b_top]) > adjv_label_index) {
				stack_b[this->stack_b_top] = -1;
				this->stack_b_top--;
			}
		}
	}

	if (this->stack_b[this->stack_b_top] != from_index) {
		return;
	}

	// This line means ==> this->stack_b[this->stack_b_top] = iv
	stack_b[this->stack_b_top] = -1;
	this->stack_b_top--;
	this->next_constant++;

	int total_vertex_of_new_cc = this->stack_s_top - from_index + 1;

	// If its not to decompose we just need the nCr,(r =2) value.
	if (!is_decompose) {
		this->PopUpStacks(total_vertex_of_new_cc, from_index);
		return;
	}
	else {
		this->CreateNewSCC(total_vertex_of_new_cc, from_index);
	}
}
void StronglyConnectedComponent::PopUpStacks(int total_vertex_of_new_cc, int from_index)
{

	// For Non Trivial Dom heuristic, even though the vertices are going to
	// pop up from the stacks, we have to calculate the critical node
	// for this particular exclude vertex.
	if (total_vertex_of_new_cc > 1) {
		this->critical_value += ((long)total_vertex_of_new_cc * (total_vertex_of_new_cc - 1) / 2);
	}

	// pop up the vertices from stack
	while (this->stack_s_top >= from_index) {

		vertex_label_index[stack_s[stack_s_top]] = next_constant;

		// Pop up from stack_s
		this->stack_s[stack_s_top] = -1;
		this->stack_s_top--;
	}
}
void StronglyConnectedComponent::CreateNewSCC(int total_vertex_of_new_cc, int from_index)
{

	ConnectedComponent *cc = 0;

	// There should be at least two vertex for new cc
	if (total_vertex_of_new_cc < 2) {
		this->counter_deleted_cc++;
		// cout << "Deleted from SCC" << endl;
	}
	else {
		cc = new ConnectedComponent(total_vertex_of_new_cc);
		this->critical_value += ((long)total_vertex_of_new_cc * (total_vertex_of_new_cc - 1) / 2);
		connected_comp_array[(*total_connected_cc)++] = cc;
	}

	// pop up the vertices from stack and add into component
	int cc_vertex, vertex_index_in_cc;
	while (this->stack_s_top >= from_index) {

		vertex_index_in_cc = this->stack_s[this->stack_s_top];
		this->vertex_label_index[vertex_index_in_cc] = next_constant;

		// Pop(stack_s)
		this->stack_s[stack_s_top] = -1;
		this->stack_s_top--;

		cc_vertex = connected_comp->GetVertex(vertex_index_in_cc);

		if (cc == 0) {
			this->input_graph->deleted_vertices[cc_vertex] = true;
		}
		else {
			cc->AddVertex(cc_vertex);
		}
	}
}
} // namespace common
} // namespace cndp
