#include "page_rank.h"

namespace cndp {
namespace algorithms {
namespace page_rank {

PageRank::PageRank()
{
	/* It compute the page rank algorithm on unweighted directed graph */
}
PageRank::PageRank(cndp::common::DimacsGraph *dimacs_graph, int *cc_vertex_indexer)
{
	this->input_graph = dimacs_graph;
	this->index_of_vertex_in_cc = cc_vertex_indexer;
}
void PageRank::ResetTable()
{
	outgoing_edges_column.clear();
	edge_matrix.clear();
	pagerank_table.clear();
}
void PageRank::SetTotalVertices(size_t num_rows)
{
	outgoing_edges_column.resize(num_rows);
	edge_matrix.resize(num_rows);
}

template <class Vector> bool PageRank::Insert(Vector &edge_matrix, int &to_vertex)
{

	typename Vector::iterator it = lower_bound(edge_matrix.begin(), edge_matrix.end(), to_vertex);

	if (it == edge_matrix.end() || to_vertex < *it) {
		edge_matrix.insert(it, to_vertex);
		return true;
	}
	else {
		return false;
	}
}
void PageRank::InsertEdge(int from_vertex, int to_vertex)
{

	int max_vertex = this->GetMaximum(from_vertex, to_vertex);

	if ((int)edge_matrix.size() <= max_vertex) {
		max_vertex = max_vertex + 1;

		edge_matrix.resize(max_vertex);
		if ((int)outgoing_edges_column.size() <= max_vertex) {
			outgoing_edges_column.resize(max_vertex);
		}
	}
	if (Insert(edge_matrix[to_vertex], from_vertex)) {
		outgoing_edges_column[from_vertex]++;
	}
}
void PageRank::ComputePageRank()
{

	long double difference = 1;
	long double dangling_elements;
	long double sum_current_pagerank;

	int next_iteration_number = 0;

	vector<int>::iterator current_incoming;
	vector<long double> old_pagerank_table;

	size_t table_index;
	size_t num_rows = edge_matrix.size();

	if (num_rows == 0) {
		return;
	}

	pagerank_table.resize(num_rows);

	pagerank_table[0] = 1;

	while (difference > CONVERGENCE && next_iteration_number < MAX_ITERATIONS) {

		// reset counters
		sum_current_pagerank = 0;
		dangling_elements = 0;

		// sum the page rank value of each nodes and
		// find the the dangling elements
		for (size_t k = 0; k < pagerank_table.size(); k++) {

			long double current_page_rank = pagerank_table[k];
			sum_current_pagerank += current_page_rank;

			if (outgoing_edges_column[k] == 0) {
				dangling_elements += current_page_rank;
			}
		}

		if (next_iteration_number == 0) {

			// at the starting point  both page rank table are same
			old_pagerank_table = pagerank_table;
		}
		else {

			// calculate the page rank value of each node and for that round
			// and store it in old page rank table
			for (table_index = 0; table_index < pagerank_table.size(); table_index++) {

				old_pagerank_table[table_index] = pagerank_table[table_index] / sum_current_pagerank;
			}
		}

		sum_current_pagerank = 1;

		// All elements are identical of 1 x I vector
		long double value_of_vector_1xI = (1 - ALPHA) * sum_current_pagerank / num_rows;

		// All elements are identical of A x I vector
		long double value_of_vector_AxI = ALPHA * dangling_elements / num_rows;

		// We have to check the difference for the convergance
		difference = 0;
		for (table_index = 0; table_index < num_rows; table_index++) {

			// The corresponding element of the N multiplication
			long double element_n_multiplication = 0.0;

			// Let count the element multiplication vector
			for (current_incoming = edge_matrix[table_index].begin();
				 current_incoming != edge_matrix[table_index].end(); current_incoming++) {

				// The current element of the N vector
				long double element_n_vector = (outgoing_edges_column[*current_incoming])
												   ? 1.0 / outgoing_edges_column[*current_incoming]
												   : 0.0;

				element_n_multiplication += element_n_vector * old_pagerank_table[*current_incoming];
			}

			element_n_multiplication *= ALPHA;

			pagerank_table[table_index] = element_n_multiplication + value_of_vector_1xI + value_of_vector_AxI;

			difference += fabs(pagerank_table[table_index] - old_pagerank_table[table_index]);
		}
		next_iteration_number++;
	}
}
void PageRank::SetCriticalNode(cndp::common::ConnectedComponent *connected_comp)
{

	this->ResetTable();
	int total_vertices = connected_comp->GetTotalVertices();
	this->SetTotalVertices(total_vertices);

	// let suppose v-->w
	int vertex = 0, adj_vertex = 0, adj_vertex_index = 0;
	for (int vertex_index = 1; vertex_index <= total_vertices; vertex_index++) {

		vertex = connected_comp->GetVertex(vertex_index);

		for (int edge_index = input_graph->out_edges_start_index[vertex];
			 edge_index <= input_graph->out_edges_end_index[vertex]; edge_index++) {

			adj_vertex = input_graph->out_edges[edge_index];

			// if adj_vertex is obsolete(=0) OR already deleted  then just continue
			if (adj_vertex == 0 || input_graph->deleted_vertices[adj_vertex]) {
				continue;
			}

			adj_vertex_index = index_of_vertex_in_cc[adj_vertex];

			// if adj_vertex vertex is not in this component then remove the edge betwen them.
			if (adj_vertex_index == 0) {

				if (input_graph->RemoveEdgesBetweenVertices(vertex, adj_vertex,
															cndp::common::DimacsGraph::EdgeType::FORWARD) &&
					(input_graph->out_edges[edge_index] != 0)) {
					edge_index--;
				}

				continue;
			}

			if (input_graph->out_edges[edge_index] != 0) {
				this->InsertEdge(vertex, adj_vertex);
			}
		}

	} // end for

	this->ComputePageRank();

	int max_node = connected_comp->GetVertex(1), temp_node = 0;
	long double max_value = this->pagerank_table[1], temp_value = 0;

	// start from index 2, index 1 is already set as max value
	for (int vertex_index = 2; vertex_index <= total_vertices; vertex_index++) {

		temp_node = connected_comp->GetVertex(vertex_index);
		temp_value = this->pagerank_table[vertex_index];

		if (max_value < temp_value) {

			max_node = temp_node;
			max_value = temp_value;
		}
		else if (max_value == temp_value && max_node < temp_node) { // choose the smallar vertex
			max_node = temp_node;
		}
	}
	connected_comp->SetCriticalNodeLDouble(max_node, max_value);

	this->ResetTable();
}
} // namespace page_rank
} // namespace algorithms
} // namespace cndp
