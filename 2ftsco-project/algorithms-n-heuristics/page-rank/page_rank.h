#ifndef CNDP_ALGORITHM_PAGE_RANK_H_
#define CNDP_ALGORITHM_PAGE_RANK_H_

#include "../../common/connected_component.h"
#include "../../enviromental_variable.h"

namespace cndp {
namespace algorithms {
namespace page_rank {

class PageRank {
	/* It compute the page rank algorithm on unweighted directed graph */
	public:
	PageRank();
	PageRank(cndp::common::DimacsGraph *dimacs_graph, int *cc_vertex_indexer);
	void SetCriticalNode(cndp::common::ConnectedComponent *connected_comp);

	private:
	inline static int GetMaximum(int a, int b)
	{
		if (a < b) {
			return b;
		}
		return a;
	}

	void ResetTable();
	void SetTotalVertices(size_t num_rows);

	template <class Vector> bool Insert(Vector &edge_matrix, int &to_vertex);
	void InsertEdge(int from_vertex, int to_vertex);

	void ComputePageRank();

	/*store the outgoing edges per column*/
	vector<int> outgoing_edges_column;

	/*number of rows of the matrix*/
	vector<vector<int>> edge_matrix;

	/*page rank table*/
	vector<long double> pagerank_table;

	/*input graph*/
	cndp::common::DimacsGraph *input_graph = nullptr;

	int *index_of_vertex_in_cc = nullptr;

	// Parameter
	const long double ALPHA = EnvironmentalSetting::PAGE_RANK_ALPHA;
	const long double CONVERGENCE = EnvironmentalSetting::PAGE_RANK_CONVERGENCE;
	const int MAX_ITERATIONS = EnvironmentalSetting::PAGE_RANK_MAX_ITERATIONS;
};
} // namespace page_rank
} // namespace algorithms
} // namespace cndp
#endif
