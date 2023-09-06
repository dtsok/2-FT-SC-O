#include "cndp_linear_time.h"
#include "../../common/maximum_child_tree.h"
#include "../../sccs-tarjan/sccs.h"
#include "../special-nodes-sap-2vcuts/dominator_only.h"
#include <chrono>
#include <random>

namespace cndp {
namespace algorithms {
namespace linear_time {

std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count()); // provide seed
int randomInteger(int low, int high)
{
	std::uniform_int_distribution<int> uid {low, high}; // generate ints from low to high (lim included);
	return uid(dre);
}

LinearTime::LinearTime()
{
}
LinearTime::~LinearTime()
{
	if (common_descendants_size != nullptr) {
		delete[] common_descendants_size;
	}
	if (ncr_common_descendants_sum != nullptr) {
		delete[] ncr_common_descendants_sum;
	}
	if (ptr_lntdom != nullptr) {
		delete ptr_lntdom;
	}
}
void LinearTime::InitComputation(cndp::common::DimacsGraph *input_graph, int *index_of_v_in_cc, int total_vertices)
{

	this->input_graph = input_graph;
	this->index_of_v_in_cc = index_of_v_in_cc;

	ptr_lntdom = new LnfAndDominator();
	ptr_lntdom->InitComputation(input_graph);
	ptr_lntdom->SetVertexIndexInCC(index_of_v_in_cc);

	this->common_descendants_size = new int[total_vertices + 1];
	this->ncr_common_descendants_sum = new long[total_vertices + 1];
	for (int i = 0; i <= total_vertices; i++) {
		this->common_descendants_size[i] = this->ncr_common_descendants_sum[i] = 0;
	}
}

bool LinearTime::FindCriticalNode(cndp::common::ConnectedComponent *cc)
{

	int total_vertices = cc->GetTotalVertices();

	if (total_vertices == 1) {
		cc->SetCriticalNodeLong(1, 1);
		return true;
	}

	int source = 1;

	//	Compute Tree in Forward Direction
	//	cout << "Forward Flow Graph" << endl;
	Trees *forward_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);

	//    forward_tree->PrintTrees();
	//	forward_tree->ExportStepsForReport();

	//	cout << "Reverse Flow Graph" << endl;

	//	Compute Tree in Reverse Direction
	this->input_graph->SetReverseGraph();
	Trees *reverse_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);
	this->input_graph->SetReverseGraph(); // Set graph to original

	//	cout << "Reverse Tree" << endl;
	//	reverse_tree->PrintTrees();
	//	reverse_tree->ExportStepsForReport();

	// Select only non trivial dominators
	bool *is_non_trivial_dom = new bool[total_vertices + 1];
	bool is_exist_non_trivial_dom = false;

	for (int i = 1; i <= total_vertices; i++) {
		is_non_trivial_dom[i] = false;
	}

	// check if source is non trivial dominator or not
	if (!cc->IsConnectedWithoutVertex(this->input_graph, this->index_of_v_in_cc, source)) {
		is_non_trivial_dom[source] = is_exist_non_trivial_dom = true;
	}

	// check other vertices (source = 1)
	for (int i = 2; i <= total_vertices; i++) {

		if (forward_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[forward_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}

		if (reverse_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[reverse_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}
	}

	// If there are no trivial dominator then graph is two connected.
	if (!is_exist_non_trivial_dom) {

		// If graph is 2 connected then we choose a node as a critical node by using
		// LNT heuristic
		cndp::common::MaxChildTree lnt;
		int total_children = 0;
		int cnode = lnt.GetMaxChildVertex(forward_tree->lnt_header, source, cc, total_children);

		// Graph will still be connected after removing the vertex.
		// But the vertex size will be n-1.
		total_vertices--;
		long cvalue = (long)total_vertices * ((long)total_vertices - 1) / 2;

		cc->SetCriticalNodeLong(cnode, cvalue);

		delete[] is_non_trivial_dom;
		delete forward_tree;
		delete reverse_tree;

		return true;
	}

	// Calculate the SCCs in Common Descendants
	this->ComputeSCCsInCommonDescendants(forward_tree, reverse_tree);

	int cnode = 0, temp_cnode = 0;
	long cvalue = 0, temp_cvalue = 0;

	// If more than one node have the same critical value then
	// we have to choose one of them in random
	vector<int> L;

	for (int v = 1; v <= total_vertices; v++) {

		if (!is_non_trivial_dom[v]) {
			continue;
		}

		// Compute the SCC size in Ancestor of v
		// totalAv = |V| - |D(v)| - |DR(v)| + |C|
		// Where D(v), DR(v) are the proper descendents of v in D and DR.
		// C is the total common descendant of v.

		int proper_forward_dscnts = forward_tree->dominator_tree->GetDescendantSize(v) - 1;

		int proper_reverse_dscnts = reverse_tree->dominator_tree->GetDescendantSize(v) - 1;

		long proper_common_ancestors = total_vertices - proper_forward_dscnts - proper_reverse_dscnts +
									   common_descendants_size[v] - 1; // -1 for the proper ancestor.

		long ncr_ancestors = 0;
		if (proper_common_ancestors > 1) {
			ncr_ancestors = (proper_common_ancestors * (proper_common_ancestors - 1)) / 2;
		}

		// calcualte the critical value of a node  in cc
		temp_cnode = cc->GetVertex(v);
		temp_cvalue =
			forward_tree->dsum[v] + reverse_tree->dsum[v] - ncr_common_descendants_sum[v] + ncr_ancestors;

		// cout << temp_cnode << ":" << temp_cvalue << endl;

		// vertex with smallar value is the highest critical node
		if (cnode == 0 || temp_cvalue < cvalue) {
			cnode = temp_cnode;
			cvalue = temp_cvalue;

			L.clear(); // remove if any node are already set as a critical nodes
			L.push_back(temp_cnode);
		}
		else if (cvalue == temp_cvalue) {
			// these nodes have the equal critical values, so let add it in stack.
			// later, we will choose one of them randomly.
			L.push_back(temp_cnode);
		}
	}

	// Choose a single critical node from the set of candidates
	int max_index = (int)L.size() - 1;
	int random_index = cndp::common::Utilities::GetRandomNumber(0, max_index);
	cnode = L.at(random_index);
	
	/* comment above lines and uncomment bellow in order to remove the randomness */
	// cnode = L.at(0);

	L.clear();

	cc->SetCriticalNodeLong(cnode, cvalue);

	delete[] is_non_trivial_dom;
	delete forward_tree;
	delete reverse_tree;
	return false;
}

bool LinearTime::is3Connected()
{
	int n_nodes = input_graph->vertex_size;
	for (int i = 1; i <= n_nodes; i++) {
		int r = randomInteger(1,n_nodes);
		while (r == i) {
			r = randomInteger(1,n_nodes);
		}
		if (r > n_nodes || r < 1) {
			printf("\nERROR @ is3Connected()\n");
			exit(15);
		}

		scc_tarjan *sccs_obj = new scc_tarjan();
		bool *separator = new bool[n_nodes + 1]();
		separator[i] = true;
		separator[r] = true;
		sccs_obj->decomposeSCC(input_graph, separator);
		if ((sccs_obj->t_cc == 1) && (sccs_obj->size_of_biggest_scc == n_nodes - 2)) {

			delete[] separator;
			delete sccs_obj;

			input_graph->deleted_vertices[i] = true; // mark i as deleted and find the idoms wrt root r
			dominatorTree *dom_obj = new dominatorTree(input_graph, r);
			for (int q = 1; q <= n_nodes; q++) {
				if (q == i || q == r) {
					continue;
				}
				if (dom_obj->idom[q] != r){
					input_graph->deleted_vertices[i] = false;
					delete dom_obj;
					return false;
				}
			}
			delete dom_obj;
			// same for the reverse graph
			input_graph->SetReverseGraph();
			dom_obj = new dominatorTree(input_graph, r);
			for (int q = 1; q <= n_nodes; q++) {
				if (q == i || q == r) {
					continue;
				}
				if (dom_obj->idom[q] != r){
					input_graph->SetReverseGraph();
					input_graph->deleted_vertices[i] = false;
					delete dom_obj;
					return false;
				}
			}
			delete dom_obj;

			//reset state and continue with the next node
			input_graph->SetReverseGraph();
			input_graph->deleted_vertices[i] = false;
		}
		else {

			delete[] separator;
			delete sccs_obj;

			return false;
		}
	}
	return true;
}

bool LinearTime::FindCriticalNode3Connected(cndp::common::ConnectedComponent *cc)
{
	int total_vertices = cc->GetTotalVertices();

	if (total_vertices == 1) {
		cc->SetCriticalNodeLong(1, 1);
		return true;
	}

	int source = 1;

	//	Compute Tree in Forward Direction
	//	cout << "Forward Flow Graph" << endl;
	Trees *forward_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);

	//    forward_tree->PrintTrees();
	//	forward_tree->ExportStepsForReport();

	//	cout << "Reverse Flow Graph" << endl;

	//	Compute Tree in Reverse Direction
	this->input_graph->SetReverseGraph();
	Trees *reverse_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);
	this->input_graph->SetReverseGraph(); // Set graph to original

	//	cout << "Reverse Tree" << endl;
	//	reverse_tree->PrintTrees();
	//	reverse_tree->ExportStepsForReport();

	// Select only non trivial dominators
	bool *is_non_trivial_dom = new bool[total_vertices + 1];
	bool is_exist_non_trivial_dom = false;

	for (int i = 1; i <= total_vertices; i++) {
		is_non_trivial_dom[i] = false;
	}

	// check if source is non trivial dominator or not
	if (!cc->IsConnectedWithoutVertex(this->input_graph, this->index_of_v_in_cc, source)) {
		is_non_trivial_dom[source] = is_exist_non_trivial_dom = true;
	}

	// check other vertices (source = 1)
	for (int i = 2; i <= total_vertices; i++) {

		if (forward_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[forward_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}

		if (reverse_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[reverse_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}
	}

	// If there are no trivial dominator then graph is two connected.
	if (!is_exist_non_trivial_dom) {

		// If graph is 2 connected then check if graph is 3 connected if true then stop the decomposition,
		// otherwise select the node from LNT heuristic

		if (is3Connected()) {
			//printf("\nh\n");
			int cnode = -1;
			int cvalue = -1;
			cc->SetCriticalNodeLong(cnode, cvalue);
			return true;
		}

		cndp::common::MaxChildTree lnt;
		int total_children = 0;
		int cnode = lnt.GetMaxChildVertex(forward_tree->lnt_header, source, cc, total_children);

		// Graph will still be connected after removing the vertex.
		// But the vertex size will be n-1.
		total_vertices--;
		long cvalue = (long)total_vertices * ((long)total_vertices - 1) / 2;

		cc->SetCriticalNodeLong(cnode, cvalue);

		delete[] is_non_trivial_dom;
		delete forward_tree;
		delete reverse_tree;

		return true;
	}

	// Calculate the SCCs in Common Descendants
	this->ComputeSCCsInCommonDescendants(forward_tree, reverse_tree);

	int cnode = 0, temp_cnode = 0;
	long cvalue = 0, temp_cvalue = 0;

	// If more than one node have the same critical value then
	// we have to choose one of them in random
	vector<int> L;

	for (int v = 1; v <= total_vertices; v++) {

		if (!is_non_trivial_dom[v]) {
			continue;
		}

		// Compute the SCC size in Ancestor of v
		// totalAv = |V| - |D(v)| - |DR(v)| + |C|
		// Where D(v), DR(v) are the proper descendents of v in D and DR.
		// C is the total common descendant of v.

		int proper_forward_dscnts = forward_tree->dominator_tree->GetDescendantSize(v) - 1;

		int proper_reverse_dscnts = reverse_tree->dominator_tree->GetDescendantSize(v) - 1;

		long proper_common_ancestors = total_vertices - proper_forward_dscnts - proper_reverse_dscnts +
									   common_descendants_size[v] - 1; // -1 for the proper ancestor.

		long ncr_ancestors = 0;
		if (proper_common_ancestors > 1) {
			ncr_ancestors = (proper_common_ancestors * (proper_common_ancestors - 1)) / 2;
		}

		// calcualte the critical value of a node  in cc
		temp_cnode = cc->GetVertex(v);
		temp_cvalue =
			forward_tree->dsum[v] + reverse_tree->dsum[v] - ncr_common_descendants_sum[v] + ncr_ancestors;

		// cout << temp_cnode << ":" << temp_cvalue << endl;

		// vertex with smallar value is the highest critical node
		if (cnode == 0 || temp_cvalue < cvalue) {
			cnode = temp_cnode;
			cvalue = temp_cvalue;

			L.clear(); // remove if any node are already set as a critical nodes
			L.push_back(temp_cnode);
		}
		else if (cvalue == temp_cvalue) {
			// these nodes have the equal critical values, so let add it in stack.
			// later, we will choose one of them randomly.
			L.push_back(temp_cnode);
		}
	}

	// Choose a single critical node from the set of candidates
	// int max_index = (int)L.size() - 1;
	// int random_index = cndp::common::Utilities::GetRandomNumber(0, max_index);
	// cnode = L.at(random_index);
	cnode = L.at(0);
	L.clear();

	cc->SetCriticalNodeLong(cnode, cvalue);

	delete[] is_non_trivial_dom;
	delete forward_tree;
	delete reverse_tree;
	return false;
}

bool LinearTime::FindCriticalNodeHeuristic1(cndp::common::ConnectedComponent *cc)
{

	int total_vertices = cc->GetTotalVertices();

	if (total_vertices == 1) {
		cc->SetCriticalNodeLong(1, 1);
		return true;
	}

	int source = 1;

	//	Compute Tree in Forward Direction
	//	cout << "Forward Flow Graph" << endl;
	Trees *forward_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);

	//    forward_tree->PrintTrees();
	//	forward_tree->ExportStepsForReport();

	//	cout << "Reverse Flow Graph" << endl;

	//	Compute Tree in Reverse Direction
	this->input_graph->SetReverseGraph();
	Trees *reverse_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);
	this->input_graph->SetReverseGraph(); // Set graph to original

	//	cout << "Reverse Tree" << endl;
	//	reverse_tree->PrintTrees();
	//	reverse_tree->ExportStepsForReport();

	// Select only non trivial dominators
	bool *is_non_trivial_dom = new bool[total_vertices + 1];
	bool is_exist_non_trivial_dom = false;

	for (int i = 1; i <= total_vertices; i++) {
		is_non_trivial_dom[i] = false;
	}

	// check if source is non trivial dominator or not
	if (!cc->IsConnectedWithoutVertex(this->input_graph, this->index_of_v_in_cc, source)) {
		is_non_trivial_dom[source] = is_exist_non_trivial_dom = true;
	}

	// check other vertices (source = 1)
	for (int i = 2; i <= total_vertices; i++) {

		if (forward_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[forward_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}

		if (reverse_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[reverse_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}
	}

	// If there are no trivial dominator then graph is two connected.
	if (!is_exist_non_trivial_dom) {

		// graph is 2 connected find the node that minimizes the biggest SCC
		int minMaxSCC = cc->GetTotalVertices();
		int cnode = 1;
		for (int v = 1; v <= cc->GetTotalVertices(); v++) {
			scc_tarjan sccObj = scc_tarjan();
			sccObj.decomposeSCC(input_graph, v);
			if (sccObj.size_of_biggest_scc < minMaxSCC) {
				minMaxSCC = sccObj.size_of_biggest_scc;
				cnode = v;
			}
		}

		// Graph will still be connected after removing the vertex.
		// But the vertex size will be n-1.
		total_vertices--;
		long cvalue = (long)total_vertices * ((long)total_vertices - 1) / 2;

		cc->SetCriticalNodeLong(cnode, cvalue);

		delete[] is_non_trivial_dom;
		delete forward_tree;
		delete reverse_tree;

		return true;
	}

	// Calculate the SCCs in Common Descendants
	this->ComputeSCCsInCommonDescendants(forward_tree, reverse_tree);

	int cnode = 0, temp_cnode = 0;
	long cvalue = 0, temp_cvalue = 0;

	// If more than one node have the same critical value then
	// we have to choose one of them in random
	vector<int> L;

	for (int v = 1; v <= total_vertices; v++) {

		if (!is_non_trivial_dom[v]) {
			continue;
		}

		// Compute the SCC size in Ancestor of v
		// totalAv = |V| - |D(v)| - |DR(v)| + |C|
		// Where D(v), DR(v) are the proper descendents of v in D and DR.
		// C is the total common descendant of v.

		int proper_forward_dscnts = forward_tree->dominator_tree->GetDescendantSize(v) - 1;

		int proper_reverse_dscnts = reverse_tree->dominator_tree->GetDescendantSize(v) - 1;

		long proper_common_ancestors = total_vertices - proper_forward_dscnts - proper_reverse_dscnts +
									   common_descendants_size[v] - 1; // -1 for the proper ancestor.

		long ncr_ancestors = 0;
		if (proper_common_ancestors > 1) {
			ncr_ancestors = (proper_common_ancestors * (proper_common_ancestors - 1)) / 2;
		}

		// calcualte the critical value of a node  in cc
		temp_cnode = cc->GetVertex(v);
		temp_cvalue =
			forward_tree->dsum[v] + reverse_tree->dsum[v] - ncr_common_descendants_sum[v] + ncr_ancestors;

		// cout << temp_cnode << ":" << temp_cvalue << endl;

		// vertex with smallar value is the highest critical node
		if (cnode == 0 || temp_cvalue < cvalue) {
			cnode = temp_cnode;
			cvalue = temp_cvalue;

			L.clear(); // remove if any node are already set as a critical nodes
			L.push_back(temp_cnode);
		}
		else if (cvalue == temp_cvalue) {
			// these nodes have the equal critical values, so let add it in stack.
			// later, we will choose one of them randomly.
			L.push_back(temp_cnode);
		}
	}

	// select the cn that minimizes the biggest SCC	wrt the marked nodess
	int minMaxSCC = cc->GetTotalVertices();
	for (int v : L) {
		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(input_graph, v);
		if (sccObj.size_of_biggest_scc < minMaxSCC) {
			minMaxSCC = sccObj.size_of_biggest_scc;
			cnode = v;
		}
	}

	L.clear();

	cc->SetCriticalNodeLong(cnode, cvalue);

	delete[] is_non_trivial_dom;
	delete forward_tree;
	delete reverse_tree;
	return false;
}

bool LinearTime::FindCriticalNodeHeuristic2(cndp::common::ConnectedComponent *cc, bool *marked, int *reverseMap)
{

	int total_vertices = cc->GetTotalVertices();

	if (total_vertices == 1) {
		cc->SetCriticalNodeLong(1, 1);
		return true;
	}

	int source = 1;

	//	Compute Tree in Forward Direction
	//	cout << "Forward Flow Graph" << endl;
	Trees *forward_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);

	//    forward_tree->PrintTrees();
	//	forward_tree->ExportStepsForReport();

	//	cout << "Reverse Flow Graph" << endl;

	//	Compute Tree in Reverse Direction
	this->input_graph->SetReverseGraph();
	Trees *reverse_tree = ptr_lntdom->ComputeLNTandDominators(cc, source);
	this->input_graph->SetReverseGraph(); // Set graph to original

	//	cout << "Reverse Tree" << endl;
	//	reverse_tree->PrintTrees();
	//	reverse_tree->ExportStepsForReport();

	// Select only non trivial dominators
	bool *is_non_trivial_dom = new bool[total_vertices + 1];
	bool is_exist_non_trivial_dom = false;

	for (int i = 1; i <= total_vertices; i++) {
		is_non_trivial_dom[i] = false;
	}

	// check if source is non trivial dominator or not
	if (!cc->IsConnectedWithoutVertex(this->input_graph, this->index_of_v_in_cc, source)) {
		is_non_trivial_dom[source] = is_exist_non_trivial_dom = true;
	}

	// check other vertices (source = 1)
	for (int i = 2; i <= total_vertices; i++) {

		if (forward_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[forward_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}

		if (reverse_tree->immediate_dominator[i] != source) {
			is_non_trivial_dom[reverse_tree->immediate_dominator[i]] = is_exist_non_trivial_dom = true;
		}
	}

	// If there are no trivial dominator then graph is two connected.
	if (!is_exist_non_trivial_dom) {

		// If graph is 2 connected then we choose a node as a critical node by using
		// LNT heuristic
		/*
				cndp::common::MaxChildTree lnt;
				int total_children = 0;
				int cnode = lnt.GetMaxChildVertex(forward_tree->lnt_header, source, cc, total_children);
		*/
		// graph is 2 connected find the node that minimizes the biggest SCC
		int minMaxSCC = cc->GetTotalVertices();
		int cnode = 1;
		for (int v = 1; v <= cc->GetTotalVertices(); v++) {
			scc_tarjan sccObj = scc_tarjan();
			sccObj.decomposeSCC(input_graph, v);
			int nodesInBiggestSCC = 0;
			for (int i = 0; i < sccObj.t_cc; i++) {
				int count = 0;
				cndp::common::ConnectedComponent *scc = sccObj.cc_array[i];
				for (int j = 1; j <= scc->GetTotalVertices(); j++) {
					if (marked[reverseMap[scc->GetVertex(j)]]) {
						count++;
					}
				}
				if (count > nodesInBiggestSCC) {
					nodesInBiggestSCC = count;
				}
			}
			if (nodesInBiggestSCC < minMaxSCC) {
				minMaxSCC = nodesInBiggestSCC;
				cnode = v;
			}
		}
		// Graph will still be connected after removing the vertex.
		// But the vertex size will be n-1.
		total_vertices--;
		long cvalue = (long)total_vertices * ((long)total_vertices - 1) / 2;

		cc->SetCriticalNodeLong(cnode, cvalue);

		delete[] is_non_trivial_dom;
		delete forward_tree;
		delete reverse_tree;

		return true;
	}

	// Calculate the SCCs in Common Descendants
	this->ComputeSCCsInCommonDescendants(forward_tree, reverse_tree);

	int cnode = 0, temp_cnode = 0;
	long cvalue = 0, temp_cvalue = 0;

	// If more than one node have the same critical value then
	// we have to choose one of them in random
	vector<int> L;

	for (int v = 1; v <= total_vertices; v++) {

		if (!is_non_trivial_dom[v]) {
			continue;
		}

		// Compute the SCC size in Ancestor of v
		// totalAv = |V| - |D(v)| - |DR(v)| + |C|
		// Where D(v), DR(v) are the proper descendents of v in D and DR.
		// C is the total common descendant of v.

		int proper_forward_dscnts = forward_tree->dominator_tree->GetDescendantSize(v) - 1;

		int proper_reverse_dscnts = reverse_tree->dominator_tree->GetDescendantSize(v) - 1;

		long proper_common_ancestors = total_vertices - proper_forward_dscnts - proper_reverse_dscnts +
									   common_descendants_size[v] - 1; // -1 for the proper ancestor.

		long ncr_ancestors = 0;
		if (proper_common_ancestors > 1) {
			ncr_ancestors = (proper_common_ancestors * (proper_common_ancestors - 1)) / 2;
		}

		// calcualte the critical value of a node  in cc
		temp_cnode = cc->GetVertex(v);
		temp_cvalue =
			forward_tree->dsum[v] + reverse_tree->dsum[v] - ncr_common_descendants_sum[v] + ncr_ancestors;

		// cout << temp_cnode << ":" << temp_cvalue << endl;

		// vertex with smallar value is the highest critical node
		if (cnode == 0 || temp_cvalue < cvalue) {
			cnode = temp_cnode;
			cvalue = temp_cvalue;

			L.clear(); // remove if any node are already set as a critical nodes
			L.push_back(temp_cnode);
		}
		else if (cvalue == temp_cvalue) {
			// these nodes have the equal critical values, so let add it in stack.
			// later, we will choose one of them randomly.
			L.push_back(temp_cnode);
		}
	}

	// select the cn that minimizes the biggest SCC	wrt the marked nodess
	int minMaxSCC = cc->GetTotalVertices();
	for (int v : L) {
		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(input_graph, v);
		int nodesInBiggestSCC = 0;
		for (int i = 0; i < sccObj.t_cc; i++) {
			int count = 0;
			cndp::common::ConnectedComponent *cc = sccObj.cc_array[i];
			for (int j = 1; j <= cc->GetTotalVertices(); j++) {
				if (marked[reverseMap[cc->GetVertex(j)]]) {
					count++;
				}
			}
			if (count > nodesInBiggestSCC) {
				nodesInBiggestSCC = count;
			}
		}
		if (nodesInBiggestSCC < minMaxSCC) {
			minMaxSCC = nodesInBiggestSCC;
			cnode = v;
		}
	}

	L.clear();

	cc->SetCriticalNodeLong(cnode, cvalue);

	delete[] is_non_trivial_dom;
	delete forward_tree;
	delete reverse_tree;
	return false;
}

void LinearTime::ComputeSCCsInCommonDescendants(Trees *gTree, Trees *rTree)
{

	int total_vertices = gTree->total_vertices;

	// initial Q = D
	// set start(v) = end(v) = 0 for all v in Q
	int *start_size = new int[total_vertices + 1];
	int *end_size = new int[total_vertices + 1];

	long *start_sum = new long[total_vertices + 1];
	long *end_sum = new long[total_vertices + 1];

	int *vertex_tree_id = new int[total_vertices + 1];
	int *w_of_y = new int[total_vertices + 1];

	int total_yw = 0;

	for (int i = 0; i <= total_vertices; i++) {
		start_size[i] = end_size[i] = vertex_tree_id[i] = w_of_y[i] = common_descendants_size[i] = 0;
		start_sum[i] = end_sum[i] = ncr_common_descendants_sum[i] = 0;
	}

	// calculate the List L
	vector<std::pair<int, int>> L; // <x,y>
	for (int v = 1; v <= total_vertices; v++) {

		if (v == gTree->source) {
			continue;
		}

		// For L: if d(h(v)) != d(v) (i.e., h(v) not a sibling of v in D
		// then we have to make a pair of <h(v),v> and insert into L.
		int hv = gTree->lnt_header[v];
		if (gTree->immediate_dominator[hv] != gTree->immediate_dominator[v]) {

			std::pair<int, int> l;
			l.first = hv;
			l.second = v;
			L.push_back(l); // L (union) <h(x),x>

							// cout << v << "," << hv << endl;
		}
	} // end of List L

	int list_size = L.size();
	int row_size = list_size + total_vertices;
	int column_size = 3;

	int total_elements = row_size * column_size;
	int *tuples = new int[total_elements];

	for (int i = 0; i < total_elements; i++) {
		tuples[i] = 0;
	}

	// First Let refill listA = <x,y> => <pre(d(x)),pre(y)>
	for (int i = 0; i < list_size; i++) {
		std::pair<int, int> la = L.at(i);
		tuples[i * 3] = gTree->dominator_tree->GetVertexToPreOrderNo(gTree->immediate_dominator[la.first]);

		tuples[i * 3 + 1] = gTree->dominator_tree->GetVertexToPreOrderNo(la.second);
		tuples[i * 3 + 2] = 1; // indication for List A
	}

	// Now we don't need the value of List L, let clear it.
	L.clear();

	// Second Let refill listB = <v> => <pre(d(v)), pre(v)>
	// cout << "List-B" << endl;
	for (int i = list_size; i < row_size; i++) {
		int v = i - list_size + 1;
		tuples[i * 3] = gTree->dominator_tree->GetVertexToPreOrderNo(gTree->immediate_dominator[v]);

		tuples[i * 3 + 1] = gTree->dominator_tree->GetVertexToPreOrderNo(v);
		tuples[i * 3 + 2] = 0; // indication for List B
	}

	// Sort Tuple
	cndp::common::Utilities::SortTuples(row_size, column_size, tuples);

	/* ===== For Debugging ===== */
	//	cout << "Sorted Tuples:" << endl;
	//	for (int i = 0; i < (total_vertices + list_size); i++) {
	//		cout << "<" << tuples[i * 3] << "," << tuples[i * 3 + 1] << ","
	//				<< tuples[i * 3 + 2] << ">" << endl;
	//	}
	// find the w for <x,y> in L
	for (int row = row_size - 1; row >= 0; row--) { // Label: for loop row

		// if row is belongs to List-B then just continue;
		if (tuples[row * 3 + 2] == 0) { // 0 is the indication of List-B
			continue;
		}

		// then it belongs to List-A, where y extract as follows.
		int y = gTree->dominator_tree->GetPreOrderToVertex(tuples[row * 3 + 1]);
		int w = 0;

		// for loop j
		for (int j = (row - 1); j >= 0; j--) {

			// w be the child of d(x) that is an ancestor of y in D
			// nearest list B contain the w
			if (tuples[j * 3 + 2] == 0) // 0 is the indication of List-B
			{
				w = gTree->dominator_tree->GetPreOrderToVertex(tuples[j * 3 + 1]);
				break;
			}
		} // end of for loop j

		// cout << y << "," << w << endl;

		// cout << w << " is an ancestor of " << y << " in D " << endl;

		//			 Now, we have to check if w is an ancestor of y in DR OR not
		//			 reference [Tarjan: Finding Dominators in Directed graphs.
		//			 SIAM J. Comput. 3, 1 (1974), 62 - 89], w is an ancester of y if and only if
		//			 pre(w) <= pre(y) <pre(w) + size(w)
		//			 OR w is not an ancestor of y if and only if
		//			 pre(w) > pre(y) || pre(y) >= (pre(w) + size(w))

		int preorder_y = rTree->dominator_tree->GetVertexToPreOrderNo(y);
		int preorder_w = rTree->dominator_tree->GetVertexToPreOrderNo(w);
		int descendants_size_w = rTree->dominator_tree->GetDescendantSize(w);

		//		if (preorder_w <= preorder_y
		//				&& preorder_y < (preorder_w + descendants_size_w)) {
		//			cout << w << " is an ancestor of " << y << " in DR " << endl;
		//		} else
		//			cout << w << " is not an ancestor of " << y << " in DR " << endl;

		// change the condition from && to || for easiness.
		if (preorder_w > preorder_y || preorder_y >= (preorder_w + descendants_size_w)) {
			// cout << w << " is not an ancestor of " << y << " in DR " << endl;
			// cout << "No" << endl;
			continue;
		} // end if

		// cout << "Yes" << endl;

		// cout << w << " is an ancestor of " << y << " in DR " << endl;

		// execution come to this line means we found w.
		w_of_y[y] = w;
		total_yw++;

	} // end of for loop row.

	// we don't need tuples so let delete it.
	delete[] tuples;

	// Now, we find the find the z for each y as follows.
	// z is the nearest ancestor of y in D with z ∈ Q(w)

	// Definition of Q: if an edge x-->y in D and y-->x in Dr => x-->y exists in Q.
	// It means to be an edge x-->y in the common forest,Q, the condition is:
	// if x-->y in D then y-->x in Dr, i.e. if (d(v),v) is an edge in D then
	// (v,d(v)) should be an edge in Dr.In other words, if d(v) dominates v in D
	// then v should dominates d(v) in Dr.If the condition doesnot hold then
	// we should remove the edge x-->y from Q.In programming :
	// if (v == rTree->D[gTree->D[v]]) then an edge (d(v),v) is in Q otherwise no.

	// Let make the trees in Forest Q.
	int tree_id_no = 0;							// treeID for each vertex.
	for (int i = total_vertices; i >= 2; i--) { // traverse in reverse pre-order.

		int v = gTree->dominator_tree->GetPreOrderToVertex(i);
		int dv = gTree->immediate_dominator[v];

		// if d(v) = source in D then (v,d(v)) never exist in Dr.
		if (dv == gTree->source) {

			// v may already has a tree id from its children
			if (vertex_tree_id[v] == 0) { // if v does not have the treeID then
				vertex_tree_id[v] = ++tree_id_no;
			}

			continue;
		}

		// if d(v) != source in D then (v,d(v)) may exist in Dr
		if (v == rTree->immediate_dominator[dv]) {

			// v may already assign a tree id
			if (vertex_tree_id[v] == 0) { // if v does not have the treeID then
				vertex_tree_id[v] = ++tree_id_no;
			}

			// dv should have a same tree id as v
			vertex_tree_id[dv] = vertex_tree_id[v];
			continue;
		}

		// if an edge (v,d(v)) doesnot exist in Dr then the will have different tree ID
		if (vertex_tree_id[v] == 0) { // if v does not have the treeID then
			vertex_tree_id[v] = ++tree_id_no;
		}

		if (vertex_tree_id[dv] == 0) { // if dv does not have the treeID then
			vertex_tree_id[dv] = ++tree_id_no;
		}
	}
	// At last we should assign the tree id for source
	vertex_tree_id[gTree->source] = ++tree_id_no;

	/* ===== For Debugging ===== */
	//	//Let print the tree id of each vertex.
	//	for (int i = 1; i <= total_vertices; i++)
	//		cout << "TreeID[" << i << "]=" << vertex_tree_id[i] << endl;
	//
	/* =====  For Debugging Let print the trees ===== */
	//	for (int i = 1; i <= tree_id_no; i++) {
	//		cout << i << " {";
	//		for (int j = 1; j <= total_vertices; j++) {
	//			if (i == vertex_tree_id[j])
	//				cout << j << " ";
	//		}
	//		cout << "}" << endl;
	//	}
	/* =====  For Report, Let print the trees ===== */
	//	//header
	//	cout << "Tree-IDs,";
	//	for (int i = 1; i < tree_id_no; i++)
	//		cout << i << ",";
	//	// last tree id
	//	cout << tree_id_no << endl;
	//
	//	//print elements
	//	cout << "Elements,";
	//	for (int i = 1; i < tree_id_no; i++) {
	//		cout << "{";
	//		for (int j = 1; j <= total_vertices; j++) {
	//			if (i == vertex_tree_id[j])
	//				cout << j << " ";
	//		}
	//		cout << "},";
	//	}
	//
	//	// elements of last tree
	//	cout << "{";
	//	for (int j = 1; j <= total_vertices; j++) {
	//		if (tree_id_no == vertex_tree_id[j])
	//			cout << j;
	//	}
	//	cout << "}" << endl;
	// Now we have to make the bucket of the tree and yw as follows
	// <tree-id(v), pre(v),0>, 0 for the indication of tree bucket
	// <tree-id(w),pre(y),1>, 1 for the indication of the yw bucket
	row_size = total_yw + total_vertices;
	column_size = 3;
	total_yw = 0;

	total_elements = row_size * column_size;
	tuples = new int[total_elements];

	// First Let refill tree Bucket (<tree-id(v), pre(v),0>)
	for (int i = 0; i < total_vertices; i++) {
		tuples[i * 3] = vertex_tree_id[i + 1];
		tuples[i * 3 + 1] = gTree->dominator_tree->GetVertexToPreOrderNo(i + 1);
		tuples[i * 3 + 2] = 0; // indication for Tree Bucket
	}

	// Second Let refill yw Bucket (<tree-id(w),pre(y),1>)
	total_yw = total_vertices;
	for (int y = 1; y <= total_vertices; y++) {
		int w = w_of_y[y];
		if (w != 0) {
			// cout << "y:" << y << " and w:" << w << endl;
			tuples[total_yw * 3] = vertex_tree_id[w];									// tree-id(w)
			tuples[total_yw * 3 + 1] = gTree->dominator_tree->GetVertexToPreOrderNo(y); // preY
			tuples[total_yw * 3 + 2] = 1;												// indication for YW Bucket
			total_yw++;
		}
	}

	cndp::common::Utilities::SortTuples(row_size, column_size, tuples);

	// Now, we have to find the z as follow.
	for (int row = row_size - 1; row >= 0; row--) { // Label: for loop row

		// if row is belongs to tree bucket then just continue;
		if (tuples[row * 3 + 2] == 0) { // 0 is the indication of tree bucket
			continue;
		}

		// then it belongs to yw, where extract the y as follows.
		int y = gTree->dominator_tree->GetPreOrderToVertex(tuples[row * 3 + 1]);
		int w = w_of_y[y];
		int z = 0;

		// for loop j
		for (int j = (row - 1); j >= 0; j--) {

			// z is the nearest ancestor of y in D with z ∈ Q(w)
			if (tuples[j * 3 + 2] == 0) { // 0 is the indication of tree bucket
				z = gTree->dominator_tree->GetPreOrderToVertex(tuples[j * 3 + 1]);
				break;
			}
		} // end of for loop j

		long hy = gTree->loop_nesting_tree->GetDescendantSize(y);

		long n_choose_k = 0;
		if (hy > 1) {
			n_choose_k = (hy * (hy - 1)) / 2;
		}

		// Calculation of SCC
		end_sum[z] += n_choose_k;
		start_sum[w] += n_choose_k;

		// size of common descendants
		end_size[z] += (int)hy;
		start_size[w] += (int)hy;

		/* =====  For Report, Let print the values ===== */
		// header
		//		cout << "v,y,w,z, startSum(w), endSum(z), cdSum(v)" << endl;
		//
		//		cout << "H(y):" << hy << endl;
	}

	// Now let sum up the number of SCC in bottom UP Fashion.
	for (int i = total_vertices; i >= 1; i--) { // traverse in reverse pre-order.

		int v = gTree->dominator_tree->GetPreOrderToVertex(i);

		common_descendants_size[v] = end_size[v];
		ncr_common_descendants_sum[v] = end_sum[v];

		// let say w = child of v
		for (int j = gTree->dominator_tree->GetOutEdgeStartIndex(v);
			 j < gTree->dominator_tree->GetOutEdgeStartIndex(v + 1); j++) {

			int w = gTree->dominator_tree->GetOutEdge(j);
			// if w = child of v, and v are in same tree.
			if (vertex_tree_id[v] == vertex_tree_id[w]) {

				common_descendants_size[v] =
					common_descendants_size[v] + common_descendants_size[w] - start_size[w];

				ncr_common_descendants_sum[v] =
					ncr_common_descendants_sum[v] + ncr_common_descendants_sum[w] - start_sum[w];
			}
		}
	}

	delete[] start_size;
	delete[] end_size;

	delete[] start_sum;
	delete[] end_sum;

	delete[] vertex_tree_id;
	delete[] w_of_y;

	delete[] tuples;
}
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
