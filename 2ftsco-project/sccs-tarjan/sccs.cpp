#include "sccs.h"

scc_tarjan::scc_tarjan()
{
}

scc_tarjan::~scc_tarjan()
{
	delete[] buffer;
	delete[] isInStack;
	for (int i = 0; i < t_cc; i++) {
		delete cc_array[i];
	}
	delete[] cc_array;
}

void scc_tarjan::init(int n)
{
	bsize = n + 1;
	buffer = new int[8 * bsize + 1]; // +1 because SCCs_first must be n+2 long
	lowLink = &buffer[0];
	label2pre = &buffer[bsize];
	stack = &buffer[2 * bsize];
	parent = &buffer[3 * bsize];
	SCCs = &buffer[4 * bsize];
	SCC_id = &buffer[5 * bsize];
	SCC_size = &buffer[6 * bsize];
	SCCs_first = &buffer[7 * bsize]; // n+2 long (if n+1 then it can segfault when there are n SCCs)
	isInStack = new bool[bsize]();
}

void scc_tarjan::strongConnect(cndp::common::DimacsGraph *G, int k, int *N, int *currentSccID, bool *separator)
{
	lowLink[k] = label2pre[k] = ++(*N);
	stack[++stack_top] = k;
	isInStack[k] = true;

	for (int i = G->out_edges_start_index[k]; i <= G->out_edges_end_index[k]; i++) {
		int v = G->out_edges[i];
		if (separator[v]) {
			continue;
		}
		if (!label2pre[v]) { // tree arc
			parent[v] = k;
			strongConnect(G, v, N, currentSccID, separator);
			lowLink[k] = std::min(lowLink[k], lowLink[v]);
		}
		else if (label2pre[v] < label2pre[k]) { // front or cross arc
			if (isInStack[v]) {
				lowLink[k] = std::min(lowLink[k], label2pre[v]);
			}
		}
	}

	if (lowLink[k] == label2pre[k]) { // k is the root of a component
		SCC_num++;
		int w;
		while ((stack_top > 0) && (label2pre[w = stack[stack_top]] >= label2pre[k])) {
			isInStack[w] = false;
			SCCs[SCC_next_pos++] = w;
			SCC_id[w] = *currentSccID;
			SCC_size[*currentSccID]++;
			stack_top--;
		}
		SCCs_first[++(*currentSccID)] = SCC_next_pos;
	}
}

void scc_tarjan::strongConnect(cndp::common::DimacsGraph *G, int k, int *N, int *currentSccID, int ignoreVertex)
{
	lowLink[k] = label2pre[k] = ++(*N);
	stack[++stack_top] = k;
	isInStack[k] = true;

	for (int i = G->out_edges_start_index[k]; i <= G->out_edges_end_index[k]; i++) {
		int v = G->out_edges[i];
		if (v == ignoreVertex) {
			continue;
		}
		if (!label2pre[v]) { // tree arc
			parent[v] = k;
			strongConnect(G, v, N, currentSccID, ignoreVertex);
			lowLink[k] = std::min(lowLink[k], lowLink[v]);
		}
		else if (label2pre[v] < label2pre[k]) { // front or cross arc
			if (isInStack[v]) {
				lowLink[k] = std::min(lowLink[k], label2pre[v]);
			}
		}
	}

	if (lowLink[k] == label2pre[k]) { // k is the root of a component
		SCC_num++;
		int w;
		while ((stack_top > 0) && (label2pre[w = stack[stack_top]] >= label2pre[k])) {
			isInStack[w] = false;
			SCCs[SCC_next_pos++] = w;
			SCC_id[w] = *currentSccID;
			SCC_size[*currentSccID]++;
			stack_top--;
		}
		SCCs_first[++(*currentSccID)] = SCC_next_pos;
	}
}

void scc_tarjan::strongConnect(cndp::common::DimacsGraph *G, int k, int *N, int *currentSccID)
{
	lowLink[k] = label2pre[k] = ++(*N);
	stack[++stack_top] = k;
	isInStack[k] = true;

	for (int i = G->out_edges_start_index[k]; i <= G->out_edges_end_index[k]; i++) {
		int v = G->out_edges[i];
		if (!label2pre[v]) { // tree arc
			parent[v] = k;
			strongConnect(G, v, N, currentSccID);
			lowLink[k] = std::min(lowLink[k], lowLink[v]);
		}
		else if (label2pre[v] < label2pre[k]) { // front or cross arc
			if (isInStack[v]) {
				lowLink[k] = std::min(lowLink[k], label2pre[v]);
			}
		}
	}

	if (lowLink[k] == label2pre[k]) { // k is the root of a component
		SCC_num++;
		int w;
		while ((stack_top > 0) && (label2pre[w = stack[stack_top]] >= label2pre[k])) {
			isInStack[w] = false;
			SCCs[SCC_next_pos++] = w;
			SCC_id[w] = *currentSccID;
			SCC_size[*currentSccID]++;
			stack_top--;
		}
		SCCs_first[++(*currentSccID)] = SCC_next_pos;
	}
}

void scc_tarjan::decomposeSCC(cndp::common::DimacsGraph *G, int ignoreVertex)
{
	init(G->vertex_size);
	memset(buffer, 0, (7 * bsize + 1) * sizeof(int));

	SCC_num = 0;
	stack_top = 0;
	SCC_next_pos = 1;
	int currentSccID = 1;
	SCCs_first[1] = SCC_next_pos;
	for (int i = 1; i <= G->vertex_size; i++) {
		if (!label2pre[i] && i != ignoreVertex) {
			int N = 0;
			strongConnect(G, i, &N, &currentSccID, ignoreVertex);
		}
	}

	// printf("Number of strongly connected components : %d\n", currentSccID-1);
	int i = 1;
	cc_array = new cndp::common::ConnectedComponent *[currentSccID - 1];
	while (i < currentSccID) {

		int x = SCCs_first[i + 1] - SCCs_first[i];
		if (x > 1) {
			if(x > size_of_biggest_scc){
				size_of_biggest_scc = x;
			}
			// fprintf(stdout,"In SCC with ID = %d :", i);
			cndp::common::ConnectedComponent *_scc = new cndp::common::ConnectedComponent(x);
			for (int j = SCCs_first[i]; j < SCCs_first[i + 1]; j++) {
				_scc->AddVertex(SCCs[j]);
				// fprintf(stdout, " %d", SCCs[j]);
			}
			cc_array[t_cc++] = _scc;
			// fprintf(stdout,"\n");
		}
		i++;
	}
}

void scc_tarjan::decomposeSCC(cndp::common::DimacsGraph *G, bool *separator)
{
	init(G->vertex_size);
	memset(buffer, 0, (7 * bsize + 1) * sizeof(int));

	SCC_num = 0;
	stack_top = 0;
	SCC_next_pos = 1;
	int currentSccID = 1;
	SCCs_first[1] = SCC_next_pos;
	for (int i = 1; i <= G->vertex_size; i++) {
		if (!label2pre[i] && !separator[i]) {
			int N = 0;
			strongConnect(G, i, &N, &currentSccID, separator);
		}
	}

	// printf("Number of strongly connected components : %d\n", currentSccID-1);
	int i = 1;
	cc_array = new cndp::common::ConnectedComponent *[currentSccID - 1];
	while (i < currentSccID) {

		int x = SCCs_first[i + 1] - SCCs_first[i];
		if (x > 1) {
			if(x > size_of_biggest_scc){
				size_of_biggest_scc = x;
			}
			// fprintf(stdout,"In SCC with ID = %d :", i);
			cndp::common::ConnectedComponent *_scc = new cndp::common::ConnectedComponent(x);
			for (int j = SCCs_first[i]; j < SCCs_first[i + 1]; j++) {
				_scc->AddVertex(SCCs[j]);
				// fprintf(stdout, " %d", SCCs[j]);
			}
			cc_array[t_cc++] = _scc;
			// fprintf(stdout,"\n");
		}
		i++;
	}
}

void scc_tarjan::findSCCs(cndp::common::DimacsGraph *G)
{
	init(G->vertex_size);
	memset(buffer, 0, (7 * bsize + 1) * sizeof(int));

	SCC_num = 0;
	stack_top = 0;
	SCC_next_pos = 1;
	int currentSccID = 1;
	SCCs_first[1] = SCC_next_pos;
	for (int i = 1; i <= G->vertex_size; i++) {
		if (!label2pre[i]) {
			int N = 0;
			strongConnect(G, i, &N, &currentSccID);
		}
	}

	// printf("Number of strongly connected components : %d\n", currentSccID-1);
	int i = 1;
	cc_array = new cndp::common::ConnectedComponent *[currentSccID - 1];
	while (i < currentSccID) {

		int x = SCCs_first[i + 1] - SCCs_first[i];
		if (x > 1) {
			// fprintf(stdout,"In SCC with ID = %d :", i);
			cndp::common::ConnectedComponent *_scc = new cndp::common::ConnectedComponent(x);
			for (int j = SCCs_first[i]; j < SCCs_first[i + 1]; j++) {
				_scc->AddVertex(SCCs[j]);
				// fprintf(stdout, " %d", SCCs[j]);
			}
			cc_array[t_cc++] = _scc;
			// fprintf(stdout,"\n");
		}
		i++;
	}
}
