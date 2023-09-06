#ifndef CRITICAL_NODE_SELECTION_ALGORITHM_H_
#define CRITICAL_NODE_SELECTION_ALGORITHM_H_

#include "../../common/frequently_used_functions.h"
#include "../../sccs-tarjan/sccs.h"
#include "cndp_linear_time.h"

int criticalNode(cndp::common::DimacsGraph *graph)
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

	cndp::algorithms::linear_time::LinearTime *cn_obj = new cndp::algorithms::linear_time::LinearTime();
	cn_obj->InitComputation(graph, indecies_cc, n);
	cn_obj->FindCriticalNode(cc);

	int cn = cc->GetCriticalNode();
	delete scc_obj;
	delete cn_obj;
	delete[] indecies_cc;
	return cn;
}

int criticalNodeAndSpecialNodesH1(cndp::common::DimacsGraph *graph)
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

	cndp::algorithms::linear_time::LinearTime *cn_obj = new cndp::algorithms::linear_time::LinearTime();
	cn_obj->InitComputation(graph, indecies_cc, n);
	cn_obj->FindCriticalNodeHeuristic1(cc);

	int cn = cc->GetCriticalNode();
	delete scc_obj;
	delete cn_obj;
	delete[] indecies_cc;
	return cn;
}

int criticalNodeAndSpecialNodesH2(cndp::common::DimacsGraph *graph, bool *points, int *reverseMap)
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

	cndp::algorithms::linear_time::LinearTime *cn_obj = new cndp::algorithms::linear_time::LinearTime();
	cn_obj->InitComputation(graph, indecies_cc, n);
	cn_obj->FindCriticalNodeHeuristic2(cc, points, reverseMap);

	int cn = cc->GetCriticalNode();
	delete scc_obj;
	delete cn_obj;
	delete[] indecies_cc;
	return cn;
}

int criticalNode3Connected(cndp::common::DimacsGraph *graph)
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

	cndp::algorithms::linear_time::LinearTime *cn_obj = new cndp::algorithms::linear_time::LinearTime();
	cn_obj->InitComputation(graph, indecies_cc, n);
	cn_obj->FindCriticalNode3Connected(cc);

	int cn = cc->GetCriticalNode();
	delete scc_obj;
	delete cn_obj;
	delete[] indecies_cc;
	return cn;
}

#endif