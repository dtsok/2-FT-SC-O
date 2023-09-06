#ifndef LABEL_PROPAGATION_ALGORITHM_FOR_DIMACS_GRAPH_H_
#define LABEL_PROPAGATION_ALGORITHM_FOR_DIMACS_GRAPH_H_

#include "../../common/connected_component.h"

int Label_Propagation(cndp::common::DimacsGraph *graph);

int Label_Propagation(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *cc, int *index_in_cc);

#endif