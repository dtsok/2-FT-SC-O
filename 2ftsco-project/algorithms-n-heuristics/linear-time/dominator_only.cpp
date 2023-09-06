#include "lnf_and_dominator.h"

namespace cndp {
namespace algorithms {
namespace linear_time {

void LnfAndDominator::FindDominators(int component_size)
{

	for (int i = 0; i <= component_size; i++) {
		idom[i] = ubucket[i] = 0;
		label[i] = semi[i] = i;
	}

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

			int v = label2pre[vertex_index]; // v is the source_root of the arc
			int u;

			if (v <= i) {
				u = v;
			} // v is an ancestor of i
			else {
				RCompress(v, i);
				u = label[v];
			}

			if (semi[u] < semi[i]) {
				semi[i] = semi[u];
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

void LnfAndDominator::RCompress(int v, int c)
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
void LnfAndDominator::TranslateDom()
{
	for (int i = 2; i <= next_preorder_no; i++) {
		idom[pre2label[i]] = pre2label[dom[i]];
	}
}
void LnfAndDominator::PrintIdom(int component_size)
{
	cout << "Printing Dominator Tree" << endl;
	for (int i = 1; i <= component_size; i++) {
		cout << "idom[" << connected_component->GetVertex(i) << "]=" << connected_component->GetVertex(idom[i])
			 << endl;
	}
}
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
