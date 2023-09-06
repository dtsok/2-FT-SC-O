#include "tree_graph.h"

namespace cndp {
namespace algorithms {
namespace linear_time {

TreeGraph::TreeGraph()
{
	/*--------------------------------------------------------------------------*
	 | When we computed the immediate dominator of dominator tree or immediate  |
	 | header of loop nesting tree, then we have to process those value to get  |
	 | the descendants, bundles of each vertex by creating the tree graph.		|
	 *--------------------------------------------------------------------------*/
}
TreeGraph::TreeGraph(int *tree, int root, int vertex_size, cndp::common::ConnectedComponent *cc)
{

	// Assign parameter
	vertex_tree = tree;
	source_root = root;
	total_vertices = vertex_size;
	connected_component = cc;

	CreateTreeGraph();
}

//	public Destructor
TreeGraph::~TreeGraph()
{

	if (vertex_tree != 0) {
		delete[] vertex_tree;
		vertex_tree = 0;
	}

	if (start_index != 0) {
		delete[] start_index;
		start_index = 0;
	}

	if (end_index != 0) {
		delete[] end_index;
		end_index = 0;
	}

	if (descendants != 0) {
		delete[] descendants;
		descendants = 0;
	}

	if (out_edges_start_index != 0) {
		delete[] out_edges_start_index;
		out_edges_start_index = 0;
	}

	if (out_edges != 0) {
		delete[] out_edges;
		out_edges = 0;
	}

	if (label2pre != 0) {
		delete[] label2pre;
		label2pre = 0;
	}

	if (pre2label != 0) {
		delete[] pre2label;
		pre2label = 0;
	}

	if (descendant_size != 0) {
		delete[] descendant_size;
		descendant_size = 0;
	}

	descendants_computed = false;
}

// Getter Outgoing edges
int TreeGraph::GetOutEdgeStartIndex(int vertex)
{
	return this->out_edges_start_index[vertex];
}
int TreeGraph::GetOutEdge(int index)
{
	return this->out_edges[index];
}
int TreeGraph::GetDescendantSize(int vertex)
{
	// return (this->end_index[vertex] - this->start_index[vertex] + 1);
	return this->descendant_size[vertex];
}
// Getter preorder number
int TreeGraph::GetTotalPreOrderNo()
{
	return this->total_preorder_no;
}
int TreeGraph::GetPreOrderToVertex(int preorder_no)
{
	return this->pre2label[preorder_no];
}
int TreeGraph::GetVertexToPreOrderNo(int vertex)
{
	return this->label2pre[vertex];
}

// Helper Methods to Debug
void TreeGraph::PrintInputTree(string tree_name)
{

	if (vertex_tree == 0) {
		std::cout << "There is no input Tree" << endl;
	}

	cout << "Printing " << tree_name << " Tree" << endl;

	for (int i = 1; i <= total_vertices; i++) {
		cout << tree_name << "[" << connected_component->GetVertex(i)
			 << "]:" << connected_component->GetVertex(vertex_tree[i]) << endl;
	}
}

void TreeGraph::PrintTree()
{

	// print Tree
	for (int i = 1; i <= total_vertices; i++) {
		for (int j = out_edges_start_index[i]; j < out_edges_start_index[i + 1]; j++) {

			cout << connected_component->GetVertex(i) << "-->" << connected_component->GetVertex(out_edges[j])
				 << endl;
		}
	}
}

void TreeGraph::CreateTreeGraph()
{

	int base_size = total_vertices + 1;

	// TREE GRAPH POINTER
	out_edges = new int[base_size + 1];
	out_edges_start_index = new int[base_size + 1];
	int *temp_next_start = new int[base_size + 1];

	// TREE DFS POINTER
	label2pre = new int[base_size];
	pre2label = new int[base_size];
	descendant_size = new int[base_size];

	// Initialize the pointers
	for (int i = 0; i < base_size; i++) {
		out_edges_start_index[i] = label2pre[i] = pre2label[i] = descendant_size[i] = 0;
	}
	out_edges_start_index[base_size] = temp_next_start[base_size] = 0;

	vertex_tree[source_root] = 0;
	for (int vertex_i = 1; vertex_i <= total_vertices; vertex_i++) {
		if (!vertex_tree[vertex_i]) {
			continue; // skip unreachable vertices
		}
		out_edges_start_index[vertex_tree[vertex_i] + 1]++;
	}

	for (int v_i = 1; v_i <= total_vertices + 1; v_i++) {
		out_edges_start_index[v_i] += out_edges_start_index[v_i - 1];
		temp_next_start[v_i] = out_edges_start_index[v_i];
	}

	for (int v_i = 1; v_i <= total_vertices; v_i++) {
		if (!vertex_tree[v_i]) {
			continue; // skip unreachable vertices
		}
		out_edges[temp_next_start[vertex_tree[v_i]]++] = v_i;
	}

	delete[] temp_next_start;

	total_preorder_no = 0;
	DFSinTree(source_root);
}

void TreeGraph::DFSinTree(int vertex)
{

	label2pre[vertex] = ++total_preorder_no;
	pre2label[total_preorder_no] = vertex;
	descendant_size[vertex] = 1;

	for (int j = out_edges_start_index[vertex]; j < out_edges_start_index[vertex + 1]; j++) {

		int adj_vertex = out_edges[j];

		// we don't have to check, did adj_vertex is already visited or not
		// (adj_vertex's preorder number) because in dominator tree all
		// vertices appear only once.
		DFSinTree(adj_vertex);
		descendant_size[vertex] += descendant_size[adj_vertex];
	}
}

/*All the following methods are for debugging*/
void TreeGraph::ComputeDescedents()
{

	/* This method is used only for debugging*/
	descendants = new int[total_vertices + 1];
	start_index = new int[total_vertices + 1];
	end_index = new int[total_vertices + 1];
	descendants_computed = true;

	int temp = 0, i, j;
	for (i = 0; i <= total_vertices; i++) {
		descendants[i] = start_index[i] = end_index[i] = 0;
		//	childs[i] = 0;
	}

	// Set the descedents of the dominators
	for (int pre_order_no = total_preorder_no; pre_order_no >= 1; pre_order_no--) {

		int vertex = pre2label[pre_order_no];

		// end_index is 'total_vertices' minus it's preorder number.
		//  so vertex is located at
		descendants[total_vertices - pre_order_no] = vertex;
		end_index[vertex] = total_vertices - pre_order_no;

		// so we have to find out it's start index in descendent array.
		start_index[vertex] = end_index[vertex];

		// if vertex is leaf then it's will not execute the following loop
		// otherwise start index = minimum( start index of its childs).
		for (j = out_edges_start_index[vertex]; j < out_edges_start_index[vertex + 1]; j++) {
			temp = start_index[out_edges[j]];
			if (start_index[vertex] > temp) {
				start_index[vertex] = temp;
			}
		}
	}
}
// Getter for descandant
int TreeGraph::GetDescendantStartIndex(int vertex)
{
	/* This method is used only for debugging*/

	// cout<<vertex <<endl;
	return this->start_index[vertex];
}
int TreeGraph::GetDescendantEndIndex(int vertex)
{
	/* This method is used only for debugging*/
	return this->end_index[vertex];
}
int TreeGraph::GetDescendant(int index)
{
	/* This method is used only for debugging*/
	return this->descendants[index];
}
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
