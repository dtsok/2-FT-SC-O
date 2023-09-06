#ifndef BASIC_TREE_NODE_CLASS_H_
#define BASIC_TREE_NODE_CLASS_H_

#include "../choudhary-srcs/choudhary.h"
#include "../common/connected_component.h"
#include "../one-ftsco/oneFTSC.h"

class TreeNode {
	public:
	TreeNode();
	TreeNode(cndp::common::DimacsGraph *graph);
	virtual ~TreeNode();

	cndp::common::DimacsGraph *graph;
	int node_depth;
	int n_nodes; // num of nodes in cc/graph
	int n_edges;
	int n_chil;
	TreeNode **children = nullptr;
	int *vertex2children = nullptr;
	int *vertexMap = nullptr;
	int *vertexMapReverse = nullptr; // reverse map of vertex i in {1,...,k} to
									 // parent {1,...,N} s.t. k < N
	Choudhary *chObj = nullptr;
	int global_ID = -1;
	void decomposeNode(int removeVertex);
	void decomposeNode(int removeVertex, int ID, std::string graph_name);

	void decomposeNode(bool *separator);
	void decomposeNode(int removeVertex, bool init_aux_ds);
	void decomposeNode(bool *separator, int sep_size, int ID, std::string graph_name);
	Choudhary **store_ch_for_sep = nullptr;
	bool removed_separator = false;
	int size_of_separator = 0; // num of nodes in separator

	oneFTSC *oneFaultOracle = nullptr;

	int n_bfs;
	int e_bfs;
	int *g_out = nullptr;
	int *g_first_out = nullptr;

	private:
	int *createIndexOfVertexInCC(cndp::common::ConnectedComponent *cc);
	int *createIndexOfVertexInCC(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *cc);
	void addChild(cndp::common::ConnectedComponent *cc, int c_id);
	void shrinkChildrenArray();
	cndp::common::DimacsGraph *createSubgraph(cndp::common::ConnectedComponent *cc, int *ind);
	void printGraph(cndp::common::DimacsGraph *G);
	void constructReverseMap(TreeNode *node, cndp::common::ConnectedComponent *parent_scc);
	
	void copyArraysBFS(int vertex_size, int edge_size, int *out_edges, int *out_edges_start_index);
};

#endif
