#include "dominator.h"

namespace cndp {
namespace common {

Dominator::Dominator()
{
	/* --------------------------------------------------------------------------*
	 | It computes the dominator tree of unweighted directed graph in 			 |
	 | O(m_log_n) time by using the algorithm simple lengauer tarjan	 		 |
	 | For the details of the algorithm please refer the following paper,		 |
	 |	Thomas Lengauer and Robert Endre Tarjan									 |
	 |	A fast algorithm for finding dominators in a flowgraph					 |
	 |	ACM Transactions on Programming Language and Systems, 1(1):121-141, 1979.|
	 *---------------------------------------------------------------------------*/
}
//	public Destructor
Dominator::~Dominator()
{

	delete[] semi;
	delete[] dom;
	delete[] ubucket;
	delete[] label2pre;
	delete[] preparent;

	delete[] pre2label;
	delete[] label;
}

//	public Methods, including static methods
void Dominator::InitComputation(cndp::common::DimacsGraph *G)
{

	int total_vertices = G->vertex_size;
	input_graph = G;

	// for DFS
	pre2label = new int[total_vertices + 1];
	label2pre = new int[total_vertices + 1];
	preparent = new int[total_vertices + 1];

	// For DOM
	semi = new int[total_vertices + 1];
	label = new int[total_vertices + 1];
	dom = new int[total_vertices + 1];
	ubucket = new int[total_vertices + 1];
}
void Dominator::SetVertexIndexInCC(int *cc_vertex_indexer)
{
	this->index_of_vertex_in_cc = cc_vertex_indexer;
}
int *Dominator::ComputeDominatorTree(ConnectedComponent *cc, int root)
{

	// Assign parameteres
	connected_component = cc;
	total_vertices = cc->GetTotalVertices();
	source_root = root;

	idom = new int[total_vertices + 1];

	FindDominators(); // calculate dominators

	// PrintIdom(component_size);

	// clean container
	for (int i = 0; i <= total_vertices; i++) {
		pre2label[i] = label2pre[i] = preparent[i] = 0;
	}
	return idom;
}
void Dominator::PrintIdom(int total_vertices)
{
	cout << "Printing Dominator Tree" << endl;
	for (int i = 1; i <= total_vertices; i++) {
		cout << "idom[" << connected_component->GetVertex(i) << "]=" << connected_component->GetVertex(idom[i])
			 << endl;
	}
}

void Dominator::RCompress(int v, int c)
{
	int p;
	if ((p = preparent[v]) > c) {
		RCompress(p, c);

		if (semi[label[p]] < semi[label[v]]) {
			label[v] = label[p];
		}
		preparent[v] = preparent[p];
	}
}
// Translate dominators from DFS numbers to original vertex labels
void Dominator::TranslateDom()
{
	idom[source_root] = source_root;
	for (int i = 2; i <= next_preorder_no; i++) {
		idom[pre2label[i]] = pre2label[dom[i]];
	}
}
void Dominator::FindDominators()
{

	for (int i = 0; i <= total_vertices; i++) {
		idom[i] = ubucket[i] = label2pre[i] = preparent[i] = 0;
		label[i] = semi[i] = i;
	}

	// Initialize the values
	next_preorder_no = 0;

	int vertex_index = source_root;
	int vertex = connected_component->GetVertex(vertex_index);
	DFSinComponent(vertex_index, vertex);

	int i, j, k;

	// process the vertices in reverse preorder
	for (i = next_preorder_no; i > 1; i--) {
		/*---------------------
		 | process i-th bucket
		 *--------------------*/
		for (int v = ubucket[i]; v; v = ubucket[v]) {
			RCompress(v, i);
			int u = label[v];

			dom[v] = (semi[u] < semi[v]) ? u : i;
		}

		/*---------------------------------------------
		 | check incoming arcs, update semi-dominators
		 *--------------------------------------------*/
		k = connected_component->GetVertex(pre2label[i]);
		// cout << "nr:" << nr << ", i:" << i << ", k:" << k << endl;
		for (j = input_graph->in_edges_start_index[k]; j <= input_graph->in_edges_end_index[k]; j++) {

			int vertex = input_graph->in_edges[j];
			int vertex_index = this->index_of_vertex_in_cc[vertex];

			// if real v is  deleted or not in this CC
			if (vertex == 0 || input_graph->deleted_vertices[vertex] || vertex_index == 0) {
				continue;
			}

			int preorder_v = label2pre[vertex_index]; // v is the source_root of the arc

			if (preorder_v) {

				int eval_v;

				if (preorder_v <= i) {
					eval_v = preorder_v;
				} // v is an ancestor of i
				else {
					RCompress(preorder_v, i);
					eval_v = label[preorder_v];
				}

				if (semi[eval_v] < semi[i]) {
					semi[i] = semi[eval_v];
				}
			}
		}

		/*---------------------------
		 | process candidate semidom
		 *--------------------------*/
		int s = semi[i];

		if (s != preparent[i]) { // if semidominator n not parent: add i to s's bucket

			ubucket[i] = ubucket[s];
			ubucket[s] = i;
		}
		else {
			dom[i] = s; // semidominator is parent: s is a candidate dominator
		}
	}

	/*------------------
	 | process bucket 1
	 *-----------------*/
	for (int v = ubucket[1]; v; v = ubucket[v]) {
		dom[v] = 1;
	}

	/*---------------
	 | recover idoms
	 *--------------*/

	dom[1] = 1;
	for (i = 2; i <= next_preorder_no; i++) {

		if (dom[i] != semi[i]) {
			dom[i] = dom[dom[i]]; // make relative absolute
		}
	}

	TranslateDom();
}
/* depth-first search from vertex k
 stores parents and labels        */

void Dominator::DFSinComponent(int vertex_index, int vertex)
{
	label2pre[vertex_index] = ++next_preorder_no;
	pre2label[next_preorder_no] = vertex_index;

	// scan the outgoing edges from realV
	for (int i = this->input_graph->out_edges_start_index[vertex];
		 i <= this->input_graph->out_edges_end_index[vertex]; i++) {

		int adj_vertex = this->input_graph->out_edges[i];

		// if realW is obsolete(=0) or it's already deleted then just continue
		if (adj_vertex == 0 || input_graph->deleted_vertices[adj_vertex]) {
			continue;
		}

		int adj_vertex_index = this->index_of_vertex_in_cc[adj_vertex];
		// if realW is not in this component then remove the edge between realV and realW
		if (adj_vertex_index == 0) {

			if (input_graph->RemoveEdgesBetweenVertices(vertex, adj_vertex, DimacsGraph::EdgeType::FORWARD) &&
				(input_graph->out_edges[i] != 0)) {
				i--;
			}

			continue;
		}

		//			if (connected_component->indexNo == 3)
		//				cout << "Process:" << realW << ", indexW:" << indexW << endl;

		if (this->label2pre[adj_vertex_index] == 0) {
			this->DFSinComponent(adj_vertex_index, adj_vertex);
			preparent[label2pre[adj_vertex_index]] = label2pre[vertex_index];
		}
	}
}
} // namespace common
} // namespace cndp
