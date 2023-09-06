#include "lnf_and_dominator.h"

namespace cndp {
namespace algorithms {
namespace linear_time {
LnfAndDominator::LnfAndDominator()
{

	/* It compute the header of loop nesting tree and immediate dominator of dominator tree */
}
//	public Destructor
LnfAndDominator::~LnfAndDominator()
{

	// delete from dfs
	delete[] label2pre;
	delete[] pre2label;

	// delete lnt
	delete[] c_buffer;
	delete[] ufparent;

	// delete dominator tree
	delete[] semi;
	delete[] dom;
	delete[] ubucket;
	delete[] preparent;
	delete[] label;
}
void LnfAndDominator::InitComputation(cndp::common::DimacsGraph *dimacs_graph)
{

	int total_vertices = dimacs_graph->vertex_size;
	int edges_minus_vertices = dimacs_graph->edge_size - dimacs_graph->vertex_size;

	input_graph = dimacs_graph;

	// for DFS
	label2pre = new int[total_vertices + 1];
	pre2label = new int[total_vertices + 1];
	preparent = new int[total_vertices + 1];

	// for LNF
	ufparent = new int[total_vertices + 1];

	int bsize = total_vertices + 1;
	int msize = edges_minus_vertices + 2;
	c_buffer = new int[bsize + 2 * msize];
	this->myBuffer_size = bsize + 2 * msize;
	// printf("\nbsize = %d\nbuffer =%d", bsize, myBuffer_size);
	c_last = &c_buffer[0];
	c_target = &c_buffer[bsize];
	c_next = &c_buffer[bsize + msize];

	// For DOM
	semi = new int[total_vertices + 1];
	label = new int[total_vertices + 1];
	dom = new int[total_vertices + 1];
	ubucket = new int[total_vertices + 1];

	// initialize
	for (int i = 0; i <= total_vertices; i++) {
		ufparent[i] = c_last[i] = label2pre[i] = pre2label[i] = preparent[i] = 0;
	}

	//		for (int i = 0; i <= total_vertices; i++)
	//			label2pre[i] = ufparent[i] = c_last[i] = 0;
}
void LnfAndDominator::SetVertexIndexInCC(int *cc_vertex_indexer)
{
	this->index_of_vertex_in_cc = cc_vertex_indexer;
}

Trees *LnfAndDominator::ComputeLNTandDominators(cndp::common::ConnectedComponent *cc, int root)
{

	// Assign parameteres
	connected_component = cc;
	int total_vertices = cc->GetTotalVertices();

	// Initialize the values

	header = new int[total_vertices + 1];
	idom = new int[total_vertices + 1];

	FindLNFheaders(total_vertices, root);
	FindDominators(total_vertices);

	// // the header and idom of root is same
	header[root] = root;
	idom[root] = root;

	// clean container ~~~~~~~~~~~~MODIFIED BELOW LOOP - COMMENTED 1 LINE AND ADDED ANOTHER + ADDED 2nd for
	// ~~~~~~~~~~~~~~~~~~~~~
	for (int i = 0; i <= total_vertices; i++) {
		// pre2label[i] = label2pre[i] = preparent[i] = ufparent[i] = c_last[i] = c_target[i] = 0;
		pre2label[i] = label2pre[i] = preparent[i] = ufparent[i] = c_last[i] = 0;
	}
	// printf("\ntotal_nodes + 1 = %d\nbuffer =%d", total_vertices+1, myBuffer_size);
	for (int i = 0; i < this->myBuffer_size - total_vertices - 1; i++) {
		c_target[i] = 0;
	}

	Trees *t = new Trees(idom, header, root, cc->GetTotalVertices(), cc);
	return t;
}

/* depth-first search from vertex k stores parents and labels  */
void LnfAndDominator::DFSinComponent(int vertex_index, int vertex)
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

			// Print Flow Graph
			// cout << vertex << "-->" << adj_vertex << endl;

			// use negative for vertices that have not been processed
			ufparent[adj_vertex_index] = -vertex_index;
			this->DFSinComponent(adj_vertex_index, adj_vertex);
			preparent[label2pre[adj_vertex_index]] = label2pre[vertex_index];
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
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
