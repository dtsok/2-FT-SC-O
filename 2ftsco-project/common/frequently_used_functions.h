#ifndef FREQUENTLY_USED_FUNCTIONS_IN_DIFF_SRCS_FILES_H_
#define FREQUENTLY_USED_FUNCTIONS_IN_DIFF_SRCS_FILES_H_

#include "connected_component.h"
#include <chrono>
#include <random>

class randomInteger {
	private:
	int low = 0;
	int high = 0;
	std::mt19937 mt;

	public:
	randomInteger(int low, int high, int seed)
	{
		this->low = low;
		this->high = high;
		mt.seed(seed);
	}

	int generate()
	{
		std::uniform_int_distribution<int> uint(low, high);
		return (uint(mt));
	}
};

// std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count()); // provide seed
// int randomInteger(int low, int high)
// {
// 	std::uniform_int_distribution<int> uid {low, high}; // generate ints from low to high (lim included);
// 	return uid(dre);
// }

int *createIndexOfVertexInCC(cndp::common::ConnectedComponent *cc, int n_nodes)
{
	int *indecies_in_cc = new int[n_nodes + 1];

	for (int i = 0; i <= n_nodes; i++) {
		indecies_in_cc[i] = 0;
	}

	for (int i = 1; i <= cc->GetTotalVertices(); i++) {
		indecies_in_cc[cc->GetVertex(i)] = i;
	}

	return indecies_in_cc;
}

cndp::common::DimacsGraph *createSubgraph(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *scc,
										  int *ind)
{
	cndp::common::DimacsGraph *sub1 = new cndp::common::DimacsGraph();
	int m = 0;
	for (int i = 1; i <= scc->GetTotalVertices(); i++) {

		int v = scc->GetVertex(i);

		for (int j = graph->out_edges_start_index[v]; j <= graph->out_edges_end_index[v]; j++) {

			int w = graph->out_edges[j];
			if (graph->deleted_vertices[w] || ind[w] == 0) {
				continue;
			}

			// std::cout << v << "-->" << w << std::endl;
			m++;
		}
	}
	// cout<<"edges "<<m<<endl;
	sub1->vertex_size = scc->GetTotalVertices();
	sub1->edge_size = m;
	sub1->input_file = new int[2 * m];
	int p = 0;
	for (int i = 1; i <= scc->GetTotalVertices(); i++) {

		int x = scc->GetVertex(i);

		for (int j = graph->out_edges_start_index[x]; j <= graph->out_edges_end_index[x]; j++) {

			int y = graph->out_edges[j];

			if (graph->deleted_vertices[y] || ind[y] == 0) {
				continue;
			}
			if (x == y) {
				continue;
			}

			sub1->input_file[p++] = ind[x];
			sub1->input_file[p++] = ind[y];

			// cout << x << "-->" << y << std::endl;
			// cout << this -> vertexMap[x] << "-->" << this -> vertexMap[y] <<
			// std::endl; cout<<endl;
		}
	}

	if (p > 2 * m) {
		fprintf(stderr, "Error! Graph has >%d arcs.\n", m);
		exit(-1);
	}
	// for(int i = 0; i < 2*m-1; i=i+2) cout<<sub1->input_file[i]<<" ->
	// "<<sub1->input_file[i+1]<<" /
	// "<<scc->GetVertex(sub1->input_file[i])<<"-"<<scc->GetVertex(sub1->input_file[i+1])<<endl;

	sub1->starting_vertex = 1;
	sub1->CreateAdjencyList();

	return sub1;
}

#endif