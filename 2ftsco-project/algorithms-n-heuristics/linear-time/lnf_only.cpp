#include "lnf_and_dominator.h"

namespace cndp {
namespace algorithms {
namespace linear_time {
void LnfAndDominator::FindLNFheaders(int component_size, int source_root)
{

	// Initialize the values
	c_current_pos = 1;
	next_preorder_no = 0;

	// calculate the loop nesting tree
	int vertex_index = source_root;
	int vertex = connected_component->GetVertex(vertex_index);
	DFSinComponent(vertex_index, vertex);
	if (next_preorder_no != component_size) {
		cerr << "Sorry component is not connected" << endl;
	}
}

void LnfAndDominator::DfsParentCompress(int vertex)
{
	int vertex_parent = ufparent[vertex];
	if (vertex_parent > 0) {
		DfsParentCompress(vertex_parent);
		if (ufparent[vertex_parent] > 0) {
			ufparent[vertex] = ufparent[vertex_parent];
		}
	}
}

int LnfAndDominator::C_pop(int vertex)
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

void LnfAndDominator::C_merge(int vertex_1, int vertex_2)
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
int LnfAndDominator::Find(int vertex)
{
	DfsParentCompress(vertex);
	if (ufparent[vertex] > 0) {
		return ufparent[vertex];
	}
	else {
		return vertex; // v is root
	}
}
void LnfAndDominator::Unite(int vertex, int parent)
{
	ufparent[vertex] = parent;
}

void LnfAndDominator::CloseVertex(int vertex_x)
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
void LnfAndDominator::C_push(int vertex, int source)
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
void LnfAndDominator::PrintHeader(int component_size)
{
	cout << "Printing Loop nesting Tree" << endl;
	for (int i = 1; i <= component_size; i++) {
		cout << "header[" << connected_component->GetVertex(i) << "]=" << connected_component->GetVertex(header[i])
			 << endl;
	}
}
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
