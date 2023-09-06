#include "trees.h"

namespace cndp {
namespace algorithms {
namespace linear_time {
Trees::Trees()
{

	/*-------------------------------------------------------------------------------------*
	 | A single graph has two tree graph; one is the dominator, and another is loop nesting.|
	 | So when we compute the graph in the forward direction, we will have one tree which   |
	 | contains the two tree graph (dominator, and loop nesting).The Same for the reverse   |
	 | graph.  So In the computation, we will have two trees (forward and reversed) and	    |
	 | four tree graph.																	    |
	 *--------------------------------------------------------------------------------------*/
}
Trees::Trees(int *idom, int *header, int root, int cc_size, cndp::common::ConnectedComponent *cc)
{

	lnt_header = header;
	immediate_dominator = idom;
	source = root;
	total_vertices = cc_size;

	dominator_tree = new TreeGraph(idom, root, cc_size, cc);
	loop_nesting_tree = new TreeGraph(lnt_header, root, cc_size, cc);
	CalculateDescendantsValueByDOMandLNT();
}

//	public Destructor
Trees::~Trees()
{
	delete dominator_tree;
	delete loop_nesting_tree;
	delete[] dsum;
}

void Trees::CalculateDescendantsValueByDOMandLNT()
{

	// Lemma: if h(v) is the lnt_header of v and u is the dominator of h(v) then
	//  u also dominates v.
	// cout << "Calculating bundle By DOM and LNT" << endl;

	dsum = new long[total_vertices + 1];
	for (int i = 0; i <= total_vertices; i++) {
		dsum[i] = 0;
	}

	for (int v = 1; v <= total_vertices; v++) {

		if (v == source) {
			continue;
		}

		int dv = immediate_dominator[v];
		int hv = lnt_header[v];
		int dHv = immediate_dominator[hv];

		// if v and hv are cibling (dv = dHv)  then bundle of v = 0.
		if (dv == dHv) {
			continue;
		}

		long nCr = 0;
		long hvSize = (long)loop_nesting_tree->GetDescendantSize(v);
		if (hvSize > 1) {
			nCr = (hvSize * (hvSize - 1)) / 2;
		}

		dsum[dv] += nCr;

		// if hv is source then it's parent in D is dummy and nothing to do.
		if (hv != source) {
			dsum[dHv] -= nCr;
		}
	}

	// Sum up the bundle value in bottom up fashion
	// cout << "Vertice by Preorder number" << endl;
	int total_preorder_no = dominator_tree->GetTotalPreOrderNo();

	for (int pre_order_no = total_preorder_no; pre_order_no >= 1; pre_order_no--) {

		int v = dominator_tree->GetPreOrderToVertex(pre_order_no);
		// cout << v << ",";

		for (int j = dominator_tree->GetOutEdgeStartIndex(v); j < dominator_tree->GetOutEdgeStartIndex(v + 1);
			 j++) {

			dsum[v] += dsum[dominator_tree->GetOutEdge(j)];
		}
	}
}

// Method to export the report for paper and thesis

void Trees::ExportStepsForReport()
{

	// header
	cout << "v,d(v)";
	cout << ",h(v),d(h(v))";
	cout << ",Bundle[v--d(h(v))]";
	cout << ",|H(v)|,DSum(v)";
	cout << endl;

	for (int v = 1; v <= total_vertices; v++) {

		int dv = immediate_dominator[v];
		int hv = lnt_header[v];
		int dhv = immediate_dominator[lnt_header[v]];
		string bundle = "[" + std::to_string(v) + "--" + std::to_string(dhv) + "]";
		int sizeHv = loop_nesting_tree->GetDescendantSize(v);
		long dSumV = dsum[v];

		//		if (dv != dhv) {
		cout << v << "," << dv;
		cout << "," << hv << "," << dhv;
		cout << "," << bundle;
		cout << "," << sizeHv << "," << dSumV << endl;
		//		}
	}
}
// Method to Debug
void Trees::PrintTrees()
{
	cout << "----- Dominator Tree ----" << endl;
	dominator_tree->PrintTree();
	cout << "----- Loop Nesting Tree ----" << endl;
	loop_nesting_tree->PrintTree();
}
void Trees::PrintDsum()
{
	cout << "----- bundle  ----" << endl;
	for (int i = 1; i <= total_vertices; i++) {
		cout << "bundle[" << i << "]:" << dsum[i] << endl;
	}
}
void Trees::ComputeDescedents()
{

	if (!dominator_tree->descendants_computed) {
		dominator_tree->ComputeDescedents();
	}

	if (!loop_nesting_tree->descendants_computed) {
		loop_nesting_tree->ComputeDescedents();
	}
}
void Trees::PrintDescedents()
{

	/*-------------------------------------------------*
	 * Before print, you have to call the method
	 * ComputeDescendants(), because these are
	 * the debugging methods.
	 --------------------------------------------------*/

	/*If descendants are not computed yet, then let compute it*/
	this->ComputeDescedents();

	int start, end, i, j;
	cout << "----- Descedent in Dominator Tree  ----" << endl;

	for (i = 1; i <= total_vertices; i++) {

		start = dominator_tree->GetDescendantStartIndex(i);
		end = dominator_tree->GetDescendantEndIndex(i);

		cout << "Descdent[" << i << "](size =" << (end - start) + 1 << "):";
		for (j = start; j <= end; j++) {
			cout << dominator_tree->GetDescendant(j) << ",";
		}
		cout << endl;
	}
	cout << "----- Descedent in Loop Nesting Tree  ----" << endl;

	for (i = 1; i <= total_vertices; i++) {

		start = loop_nesting_tree->GetDescendantStartIndex(i);
		end = loop_nesting_tree->GetDescendantEndIndex(i);

		cout << "Descdent[" << i << "](size =" << (end - start) + 1 << "):";
		for (j = start; j <= end; j++) {
			cout << loop_nesting_tree->GetDescendant(j) << ",";
		}
		cout << endl;
	}
}
vector<int> Trees::GetCommonDominator(int *tree1, int *tree2, int source, int total_vertices)
{

	/* This Method is not used by our project, I just write it at beginning */
	vector<int> v1, v2, v_intersection;
	int i = 0;

	// for non trivial dominator in D
	for (i = 1; i <= total_vertices; i++) {
		if (tree1[i] != source && tree1[i] != i && tree1[i] != 0) {
			v1.push_back(tree1[i]);
		}
	}

	// for non trivial dominator in Dr
	for (i = 1; i <= total_vertices; i++) {
		if (tree2[i] != source && tree2[i] != i && tree2[i] != 0) {
			v2.push_back(tree2[i]);
		}
	}

	// sort both vector
	sort(v1.begin(), v1.end());
	sort(v2.begin(), v2.end());

	// find the intersection
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v_intersection));

	return v_intersection;
}

vector<int> Trees::GetCommonDescedents(TreeGraph *forward_tree, TreeGraph *reverse_tree, int vertex)
{

	/* This Method is not used by our project, I just wrote it at beginning */

	/*-------------------------------------------------*
	 * Before use this method, you have to call the method
	 * ComputeDescendants() of both tree, because these are
	 * the debugging methods.
	 --------------------------------------------------*/

	vector<int> descandants_in_forward_tree, descandants_in_reverse_tree, v_intersection;

	int start = 0, end = 0, i = 0;
	// descedants of Dv
	start = forward_tree->GetDescendantStartIndex(vertex);
	end = forward_tree->GetDescendantEndIndex(vertex);
	for (i = start; i < end; i++) {
		descandants_in_forward_tree.push_back(forward_tree->GetDescendant(i));
	}

	// descedants of revDv
	start = reverse_tree->GetDescendantStartIndex(vertex);
	end = reverse_tree->GetDescendantEndIndex(vertex);
	for (i = start; i < end; i++) {
		descandants_in_reverse_tree.push_back(reverse_tree->GetDescendant(i));
	}

	// sort both vector
	sort(descandants_in_forward_tree.begin(), descandants_in_forward_tree.end());

	sort(descandants_in_reverse_tree.begin(), descandants_in_reverse_tree.end());

	// find the intersection
	std::set_intersection(descandants_in_forward_tree.begin(), descandants_in_forward_tree.end(),
						  descandants_in_reverse_tree.begin(), descandants_in_reverse_tree.end(),
						  std::back_inserter(v_intersection));

	return v_intersection;
}
} // namespace linear_time
} // namespace algorithms
} // namespace cndp
