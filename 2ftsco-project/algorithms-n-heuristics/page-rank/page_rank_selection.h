#ifndef PAGE_RANK_NODE_SELECTION_ALGORITHM_H_
#define PAGE_RANK_NODE_SELECTION_ALGORITHM_H_

#include "../../common/frequently_used_functions.h"
#include "../../sccs-tarjan/sccs.h"
#include "page_rank.h"

int PR(cndp::common::DimacsGraph *graph)
{
	int n = graph->vertex_size;
	scc_tarjan *scc_obj = new scc_tarjan();
	scc_obj->findSCCs(graph);

	if (scc_obj->t_cc != 1) {
		cout << "\n\nERROR\nGRAPH IS NOT STRONGLY CONNECTED\n\n" << scc_obj->t_cc << endl;
		exit(1);
	}
	cndp::common::ConnectedComponent *cc = scc_obj->cc_array[0];

	int *indecies_cc = createIndexOfVertexInCC(cc, n);
	cndp::algorithms::page_rank::PageRank *cn_obj = new cndp::algorithms::page_rank::PageRank(graph, indecies_cc);
	cn_obj->SetCriticalNode(cc);

	int cn = cc->GetCriticalNode();

	delete scc_obj;
	delete cn_obj;
	delete[] indecies_cc;
	return cn;
}

#endif