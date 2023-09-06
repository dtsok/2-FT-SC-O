#include "loop_nesting.h"

namespace cndp {
namespace common {

LoopNesting::LoopNesting()
{

	/* --------------------------------------------------------------------------*
	 | It computes the loop nesting tree of unweighted directed graph in 		 |
	 | O(m+n) time by using the algorithm purpose by R. E. Tarjan.	 		 	 |
	 | For the details of the algorithm please refer the following paper,		 |
	 | R. E. Tarjan. Edge-disjoint spanning trees and depth-first search.		 |
	 | Acta Informatica, 6(2):171–85, 1976.										 |
	 *---------------------------------------------------------------------------*/
}
//	public Destructor
LoopNesting::~LoopNesting()
{

	delete[] c_buffer;
	delete[] ufparent;
	delete[] label2pre;
	delete[] pre2label;
}
void LoopNesting::InitComputation(DimacsGraph *dimacs_graph)
{

	int total_vertices = dimacs_graph->vertex_size;
	int edges_minus_vertices = dimacs_graph->edge_size - dimacs_graph->vertex_size;

	input_graph = dimacs_graph;

	// for DFS & LNF
	ufparent = new int[total_vertices + 1];
	label2pre = new int[total_vertices + 1];
	pre2label = new int[total_vertices + 1];

	int bsize = total_vertices + 1;
	int msize = edges_minus_vertices + 2;
	c_buffer = new int[bsize + 2 * msize];
	c_last = &c_buffer[0];
	c_target = &c_buffer[bsize];
	c_next = &c_buffer[bsize + msize];

	for (int i = 0; i <= total_vertices; i++) {
		ufparent[i] = c_last[i] = label2pre[i] = pre2label[i] = 0;
	}
}
void LoopNesting::SetVertexIndexInCC(int *cc_vertex_indexer)
{
	this->index_of_vertex_in_cc = cc_vertex_indexer;
}
int *LoopNesting::ComputeLoopNestingTree(ConnectedComponent *cc, int source_root)
{

	// Assign parameteres
	connected_component = cc;
	int total_vertices = cc->GetTotalVertices();

	// Initialize the values
	c_current_pos = 1;
	this->next_preorder_no = 0;
	header = new int[total_vertices + 1];

	// create dfs
	int vertex_index = source_root;
	int vertex = connected_component->GetVertex(vertex_index);
	DFSinComponent(vertex_index, vertex);

	// the header of the source is zero
	header[source_root] = 0;
	if (next_preorder_no != total_vertices) {
		cerr << "LoopNesting::Sorry component is not connected" << endl;
	}

	// connected_component->PrintVertices();
	//	cout << "header[" << connected_component->GetVertex(6) << "]";
	//	cout << "=" << header[6] << endl;
	// PrintHeader(cc->GetSize());

	// clean container
	for (int i = 0; i <= total_vertices; i++) {
		pre2label[i] = label2pre[i] = ufparent[i] = c_last[i] = c_target[i] = 0;
	}
	for (int i = 0; i < c_current_pos; i++) {
		c_next[i] = c_target[i] = 0;
	}

	return header;
}
void LoopNesting::PrintHeader(int component_size)
{
	cout << "Printing Loop nesting Tree" << endl;
	for (int i = 1; i <= component_size; i++) {
		cout << "header[" << connected_component->GetVertex(i) << "]=" << connected_component->GetVertex(header[i])
			 << endl;
	}
}
void LoopNesting::DfsParentCompress(int vertex)
{
	int vertex_parent = ufparent[vertex];
	if (vertex_parent > 0) {
		DfsParentCompress(vertex_parent);
		if (ufparent[vertex_parent] > 0) {
			ufparent[vertex] = ufparent[vertex_parent];
		}
	}
}

int LoopNesting::C_pop(int vertex)
{

	int vertex_source;
	if (c_next[c_last[vertex]] == c_last[vertex]) { // C-list contains one element
		vertex_source = c_target[c_last[vertex]];
		c_last[vertex] = 0;
	}
	else {
		vertex_source = c_target[c_next[c_last[vertex]]]; // first element in C-list
		c_next[c_last[vertex]] = c_next[c_next[c_last[vertex]]];
	}
	return vertex_source;
}
void LoopNesting::C_merge(int vertex_1, int vertex_2)
{

	if (!c_last[vertex_2]) {
		return; // do nothing if second list is empty
	}

	if (!c_last[vertex_1]) { // replace fist list with second
		c_last[vertex_1] = c_last[vertex_2];
	}
	else { // append second list after last node of first

		int first1 = c_next[c_last[vertex_1]];
		int first2 = c_next[c_last[vertex_2]];

		c_next[c_last[vertex_1]] = first2;
		c_next[c_last[vertex_2]] = first1;
	}
	c_last[vertex_2] = 0;
}

int LoopNesting::Find(int vertex)
{
	DfsParentCompress(vertex);
	if (ufparent[vertex] > 0) {
		return ufparent[vertex];
	}
	else {
		return vertex; // v is root
	}
}
void LoopNesting::Unite(int vertex, int parent)
{
	ufparent[vertex] = parent;
}
void LoopNesting::CloseVertex(int vertex_x)
{

	int vertex_y, parent_y;
	while (c_last[vertex_x]) {

		vertex_y = Find(C_pop(vertex_x));

		while (vertex_y != vertex_x) {

			C_merge(vertex_x, vertex_y);
			parent_y = (ufparent[vertex_y] > 0) ? ufparent[vertex_y] : -ufparent[vertex_y];
			Unite(vertex_y, vertex_x);
			header[vertex_y] = vertex_x;
			vertex_y = Find(parent_y);
		}
	}
}

void LoopNesting::C_push(int vertex, int source)
{

	if (!c_last[vertex]) { // C-list is empty

		c_target[c_current_pos] = source;
		c_last[vertex] = c_current_pos;
		c_next[c_current_pos] = c_current_pos;
		c_current_pos++;
	}
	else {

		int first_pos = c_next[c_last[vertex]];
		c_target[c_current_pos] = source;
		c_next[c_last[vertex]] = c_current_pos;
		c_next[c_current_pos] = first_pos;
		c_last[vertex] = c_current_pos++;
	}
}

/* depth-first search from vertex k stores parents and labels  */
void LoopNesting::DFSinComponent(int vertex_index, int vertex)
{

	label2pre[vertex_index] = ++next_preorder_no;
	pre2label[next_preorder_no] = vertex_index;

	// scan the outgoing edges from realV
	for (int i = this->input_graph->out_edges_start_index[vertex];
		 i <= this->input_graph->out_edges_end_index[vertex]; i++) {

		int adj_vertex = this->input_graph->out_edges[i];

		// if adj_vertex is obsolete(=0) or it's already deleted then just continue
		if (adj_vertex == 0 || input_graph->deleted_vertices[adj_vertex]) {
			continue;
		}

		int adj_vertex_index = this->index_of_vertex_in_cc[adj_vertex];

		// if adj_vertex is not in this component then remove the edge to it.
		if (adj_vertex_index == 0) {

			if (input_graph->RemoveEdgesBetweenVertices(vertex, adj_vertex,
														cndp::common::DimacsGraph::EdgeType::FORWARD) &&
				(input_graph->out_edges[i] != 0)) {
				i--;
			}

			continue;
		}

		if (this->label2pre[adj_vertex_index] == 0) {
			// use negative for vertices that have not been processed
			ufparent[adj_vertex_index] = -vertex_index;
			this->DFSinComponent(adj_vertex_index, adj_vertex);
		}
		else {
			if (label2pre[vertex_index] >= label2pre[adj_vertex_index]) {
				// (k,node) is cross or back arc
				C_push(Find(adj_vertex_index), vertex_index);
			}
		}
	}
	CloseVertex(vertex_index);
}
} // namespace common
} // namespace cndp
