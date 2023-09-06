#ifndef CNDP_ALGORITHM_NON_TRIVIAL_DOM_H_
#define CNDP_ALGORITHM_NON_TRIVIAL_DOM_H_

#include "../../common/dominator.h"
#include "../../common/strongly_connected_component.h"

namespace cndp {
namespace algorithms {
namespace non_trivial_dom {

class NonTrivialDom {

	/*------------------------------------------------------------------------------*
	 | It collects all the non-trivial dominators									|
	 | so called strong articulation points (SAPs) of the given component.			|
	 | After that compare the potential of each SAP by removing them one by one.	|
	 | In the end, it will select one strong articulation point which has the		|
	 | minimum potential. Indeed, it's very slow process and computation will take	|
	 | too much time for the computation.											|
	 *------------------------------------------------------------------------------*/

	public:
	NonTrivialDom()
	{

		/*------------------------------------------------------------------------------*
		 | It collects all the non-trivial dominators									|
		 | so called strong articulation points (SAPs) of the given component.			|
		 | After that compare the potential of each SAP by removing them one by one.	|
		 | In the end, it will select one strong articulation point which has the		|
		 | minimum potential. Indeed, it's very slow process and computation will take	|
		 | too much time for the computation.											|
		 *------------------------------------------------------------------------------*/
	}
	virtual ~NonTrivialDom()
	{

		delete[] is_dominator;
		delete[] dominators;
		delete ptr_dominator;
	}
	void InitComputation(cndp::common::DimacsGraph *input_graph, cndp::common::StronglyConnectedComponent *ptr_scc,
						 int *index_of_v_in_cc, int total_vertices)
	{

		is_dominator = new bool[total_vertices + 1];
		dominators = new int[total_vertices + 1];
		for (int i = 0; i <= total_vertices; i++) {
			is_dominator[i] = false;
			dominators[i] = 0;
		}

		ptr_dominator = new cndp::common::Dominator();
		ptr_dominator->InitComputation(input_graph);
		ptr_dominator->SetVertexIndexInCC(index_of_v_in_cc);

		this->ptr_scc = ptr_scc;
	}
	bool FindCriticalNode(cndp::common::ConnectedComponent *cc)
	{

		int source = 1;
		int total_vertices = cc->GetTotalVertices();
		int *immediate_dominators = ptr_dominator->ComputeDominatorTree(cc, source);
		int dom_top = 0, idom_i = 0;
		for (int i = 1; i <= total_vertices; i++) {

			if (immediate_dominators[i] == source || immediate_dominators[i] == i ||
				immediate_dominators[i] == 0) {
				continue;
			}

			idom_i = immediate_dominators[i];
			if (!is_dominator[idom_i]) {

				dominators[dom_top++] = idom_i;
				is_dominator[idom_i] = true;
			}
		}
		// cout << endl;
		delete[] immediate_dominators;

		int cnode = 0, temp_cnode = 0;
		long cvalue = 0, temp_cvalue = 0;

		// If more than one node have the same critical value then
		// we have to choose one of them in random
		vector<int> L;

		// calculate the highest critical from the non trivial dominator of G
		for (int i = 0; i < dom_top; i++) {

			idom_i = dominators[i];
			temp_cnode = cc->GetVertex(idom_i);

			// this time we just have to compute the vertex potential of
			// non trivial dominator vertex
			temp_cvalue = ptr_scc->ComputeSCCinComponent(cc, temp_cnode, false);

			// clean container
			dominators[i] = 0;
			is_dominator[idom_i] = false;

			// vertex with smallar value is the highest critical node
			if (cnode == 0 || temp_cvalue < cvalue) {
				cnode = temp_cnode;
				cvalue = temp_cvalue;

				L.clear(); // remove if any node are already set as a critical nodes
				L.push_back(temp_cnode);
			}
			else if (cvalue == temp_cvalue && cnode > temp_cnode) {
				// these nodes have the equal critical values, so let add it in stack.
				// later, we will choose one of them randomly.
				L.push_back(temp_cnode);
			}
		}

		// If there are no non trivial dominators then graph is two vertex connected.
		if (dom_top <= 0) {
			return true;
		}

		// Otherwise, choose a single critical node from the set of candidates
		int max_index = (int)L.size() - 1;
		// cout << "Maximum Index:" << max_index << endl;
		int random_index = cndp::common::Utilities::GetRandomNumber(0, max_index);
		cnode = L.at(random_index);
		L.clear();

		cc->SetCriticalNodeLong(cnode, cvalue);

		// graph is not two vertex connected
		return false;
	}

	private:
	bool *is_dominator = nullptr;
	int *dominators = nullptr;

	cndp::common::Dominator *ptr_dominator = nullptr;
	cndp::common::StronglyConnectedComponent *ptr_scc = nullptr;
};
} // namespace non_trivial_dom
} // namespace algorithms
} // namespace cndp
#endif /* CNDP_ALGORITHM_NON_TRIVIAL_DOM_H_ */
