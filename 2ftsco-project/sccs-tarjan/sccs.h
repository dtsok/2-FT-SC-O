// #pragma once
#ifndef SCC_WITH_TARJAN_SCCS_H_
#define SCC_WITH_TARJAN_SCCS_H_

#include "../common/connected_component.h"
#include <algorithm>

class scc_tarjan {
	public:
	scc_tarjan();
	virtual ~scc_tarjan();

	cndp::common::ConnectedComponent **cc_array;
	int t_cc = 0;
	int size_of_biggest_scc = 0;
	void decomposeSCC(cndp::common::DimacsGraph *G, int ignoreVertex);
	void decomposeSCC(cndp::common::DimacsGraph *G, bool *separator);
	void findSCCs(cndp::common::DimacsGraph *G);

	private:
	int *buffer;
	int bsize;
	int *lowLink;
	int *label2pre;
	int *parent;
	int *stack;

	int SCC_next_pos;
	int stack_top;

	bool *isInStack;

	int *SCCs;
	int *SCCs_first;
	int *SCC_id;
	int *SCC_size;
	int SCC_num;

	void init(int n);
	void strongConnect(cndp::common::DimacsGraph *G, int k, int *N, int *currentSccID, bool *separator);
	void strongConnect(cndp::common::DimacsGraph *G, int k, int *N, int *currentSccID, int ignoreVertex);
	void strongConnect(cndp::common::DimacsGraph *G, int k, int *N, int *currentSccID);
};

#endif