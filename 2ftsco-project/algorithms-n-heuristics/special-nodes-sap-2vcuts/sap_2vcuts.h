#ifndef SPECIAL_NODES_SAP_2VCUTS_H_
#define SPECIAL_NODES_SAP_2VCUTS_H_

#include "../../sccs-tarjan/sccs.h"
#include "dominator_only.h"
#include <vector>

class sap_vcuts {
	public:
	sap_vcuts();
	sap_vcuts(cndp::common::DimacsGraph *G);
	virtual ~sap_vcuts();

	void findAllPoints(cndp::common::DimacsGraph *G);
	bool *points = nullptr;
	int n_points = 0;
	int sap_points = 0;
	int s1, s2;
	void verifyResults(cndp::common::DimacsGraph *G, bool *results, cndp::common::ConnectedComponent *scc);

	protected:
	void subroutine(cndp::common::DimacsGraph *G);
	void init_roots(cndp::common::DimacsGraph *G);
	int dfs(cndp::common::DimacsGraph *G, int source);
	void dfs_sub(cndp::common::DimacsGraph *G, int source, bool *visited);
	void isRootSAP(cndp::common::DimacsGraph *G);
	void findStrongArticulationPoints(cndp::common::DimacsGraph *G, int source);
	void findStrongArticulationPointsOneWay(cndp::common::DimacsGraph *G, int source);
	void findNonTrivialDominators(int *tree, bool *has, int n);
	void recursionInTree(int *tree, bool *has, bool *visited, int current);
	int *createIndexOfVertexInCC(cndp::common::ConnectedComponent *cc, int total_nodes);
	cndp::common::DimacsGraph *createSubgraph(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *scc, int *ind);
};

#endif