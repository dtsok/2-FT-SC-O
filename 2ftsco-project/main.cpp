#include "main_headers.h"

bool TwoFaultTolerantStrongConnectivityQuery(TreeNode *node, int x, int y, int failed_1, int failed_2, int &q_depth, int &ch_calls,
											 int &onef_calls)
{
	if (x == 0 || y == 0) {
		return false;
	}

	int Px = 0, Py = 0;		// x,y are in same SCC (root node)
	int P_f1 = 0, P_f2 = 0; // same for failed_1, failed_2
	while (Px == Py) {

		if (((P_f1 != Px) && (P_f2 != Px)) || ((failed_1 == 0) && (failed_2 == 0)) || (P_f1 == 0 && failed_1 == 0 && (P_f2 != Px)) ||
			(P_f2 == 0 && failed_2 == 0 && (P_f1 != Px))) {
			// both failed vertices ended up in different SCCs than x and y
			return true;
		}
		else if (failed_1 == 0 || P_f1 != Px) {
			// failed_1 is absent
			failed_1 = failed_2;
			P_f1 = P_f2;
		}
		else if (failed_2 == 0 || P_f2 != Px) {
			// failed_2 is absent
			failed_2 = failed_1;
			P_f2 = P_f1;
		}

		int t = node->chObj->r; // node that was removed

		if (t == failed_1 || t == failed_2) {
			q_depth++; // query will be answered in the next node
			int f;
			(t == failed_1) ? f = failed_2 : f = failed_1;

			if (failed_1 == failed_2) {
				// simply check if after the removal of f, x and y belongs to same SCC
				if (node->n_chil == 0) {
					return false;
				}
				Px = node->vertex2children[x];
				Py = node->vertex2children[y];

				// new label of x and y in the child node
				x = node->vertexMap[x];
				y = node->vertexMap[y];

				if ((x == 0 || y == 0) || (Px != Py)) {
					return false;
				}
				else {
					return true;
				}
			}
			else {
				// failed_1 != failed_2
				if (node->n_chil == 0) {
					return false;
				}
				Px = node->vertex2children[x];
				Py = node->vertex2children[y];

				// new label of x and y in the child node
				x = node->vertexMap[x];
				y = node->vertexMap[y];

				if ((x == 0 || y == 0) || (Px != Py)) {
					return false;
				}
				else {
					// x, y still in same SCC Px(i+1) == Py(i+1)
					int P_fsingle = node->vertex2children[f];
					f = node->vertexMap[f];
					if (f == 0 || (P_fsingle != Px)) {
						return true;
					}
					else {
						onef_calls++;
						node = node->children[Px]; // move to children node in order to answer the query
						return node->oneFaultOracle->query(x, y, f);
					}
				}
			}
		}
		else {

			bool q1 = node->chObj->reachabilityQuery(x, failed_1, failed_2);
			bool q2 = node->chObj->reachabilityQuery(y, failed_1, failed_2);
			ch_calls += 2;
			if (q1 != q2) {
				return false;
			}
			bool q3 = node->chObj->objReverseReach->reachabilityQuery(x, failed_1, failed_2);
			bool q4 = node->chObj->objReverseReach->reachabilityQuery(y, failed_1, failed_2);
			ch_calls += 2;
			if (q3 != q4) {
				return false;
			}
			if (q1 && q2 && q3 && q4) {
				return true;
			}
			else {
				q_depth++;
				if (node->n_chil == 0) {
					// x, y ended up in different sccs (trivial 1-node sccs)
					return false;
				}

				Px = node->vertex2children[x];
				Py = node->vertex2children[y];

				// new label of x and y in the child node
				x = node->vertexMap[x];
				y = node->vertexMap[y];
				if (x == 0 || y == 0) {
					// if a vertex has label 0, it means it doesn't belong to a child node thus it ended up in a
					// trivial SCC even though we can have Px == Py (== 0)
					return false;
				}

				P_f1 = node->vertex2children[failed_1];
				P_f2 = node->vertex2children[failed_2];

				failed_1 = node->vertexMap[failed_1];
				failed_2 = node->vertexMap[failed_2];
				node = node->children[Px];
			}
		}
	}
	return false;
}

int selectVertex(TreeNode *node)
{
	switch (algo2use) {
		case 0:
			{
				// random node
				return rand() % node->n_nodes + 1;
			}
		case 1:
			{
				// critical node
				return criticalNode(node->graph);
			}
		case 2:
			{
				// label propagation
				return Label_Propagation(node->graph);
			}
		case 3:
			{
				// page rank
				return PR(node->graph);
			}
		case 5:
			{
				// critical node but if graph is 3-connected stop the decomposition of the current node
				return criticalNode3Connected(node->graph);
			}
	}
	printf("\nERROR! @ selectVertex\n");
	exit(1);
}

void constructTree(TreeNode *node)
{

	if (algo2use == 4) {
		// heuristic using q-separator
		qSeparator qs = qSeparator(node->graph, 1);
		if (!qs.has_small_diameter && qs.found) {
			// remove whole separator
			int prev_depth = node->node_depth;
			node->node_depth += qs.sep_size - 1;
			node->decomposeNode(qs.sep);
			node->node_depth = prev_depth;
		}
		else {
			// remove only a critical node
			node->decomposeNode(criticalNode(node->graph), false); // do not init aux ds because qsep+mcn tree doesn't support answering queries.
																   // it's only used for the tree height experiments
		}
	}
	else {
		node->decomposeNode(selectVertex(node), initialize_ds);
	}

	for (int i = 0; i < node->n_chil; i++) {
		constructTree(node->children[i]);
	}
}

void recursionInTreeDepth(TreeNode *node, int &depth)
{
	if (node->n_chil > 0) {
		for (int i = 0; i < node->n_chil; i++) {
			recursionInTreeDepth(node->children[i], depth);
		}
	}
	if (node->node_depth > depth) {
		depth = node->node_depth;
	}
}

void findTreeDepth(TreeNode *node)
{
	tree_depth = 0;
	recursionInTreeDepth(node, tree_depth);
	cout << "max depth = " << tree_depth << "\n";
}

void recursionInDeleteTree(TreeNode *node)
{
	if (node->n_chil > 0) {
		for (int i = 0; i < node->n_chil; i++) {
			recursionInDeleteTree(node->children[i]);
		}
	}
	delete node;
}

void deleteTree(TreeNode *root)
{
	recursionInDeleteTree(root);
}

void cleanPointers(TreeNode *node)
{
	printf("\nCleaning pointers...\n");

	/* delete decomposition tree */
	deleteTree(node);

	/* delete aux arrays and pointers */
	if (vertex_to_last_scc != nullptr) {
		delete[] vertex_to_last_scc;
		vertex_to_last_scc = nullptr;
	}

	if (dec_tree != nullptr) {
		delete[] dec_tree;
		dec_tree = nullptr;
	}

	if (id_to_nodes != nullptr) {
		delete[] id_to_nodes;
		id_to_nodes = nullptr;
		// pointers stored in id_to_nodes were freed when d.t. was deleted
	}

	if (ncaObj != nullptr) {
		delete ncaObj;
		ncaObj = nullptr;
	}

	if (chqObj != nullptr) {
		delete chqObj;
		chqObj = nullptr;
	}

	if (queries != nullptr) {
		for (int i = 0; i < num_of_queries; i++) {
			delete[] queries[i];
		}
		delete[] queries;
		queries = nullptr;
	}

	if (all_vertices != nullptr) {
		free(all_vertices);
	}
}

void generateRandomQueries(int n_nodes, int q_number)
{
	initializeQ(n_nodes);
	queries = new int *[q_number];
	num_of_queries = q_number;
	for (int i = 0; i < q_number; i++) {
		queries[i] = new int[4];
		get_query(n_nodes, &queries[i][0], &queries[i][1], &queries[i][2], &queries[i][3]);
	}
}

void avgCaseQueries(TreeNode *obj, int num_of_q)
{	
	printf("\nAverage case queries:\n\n");
	generateRandomQueries(obj->n_nodes, num_of_q);

	int min_d = obj->n_nodes, max_d = 0;
	int depth_sum = 0;
	long int ch_sum = 0, onef_sum = 0;
	int positive = 0, negative = 0;
	cndp::common::RFWTimer timer;
	timer.start();
	for (int i = 0; i < num_of_q; i++) {
		int dd = 0, ch = 0, of = 0;
		int x = queries[i][0];
		int y = queries[i][1];
		int f1 = queries[i][2];
		int f2 = queries[i][3];
		bool rs = TwoFaultTolerantStrongConnectivityQuery(obj, x, y, f1, f2, dd, ch, of);
		depth_sum += dd;
		ch_sum += ch;
		onef_sum += of;
		if (rs) {
			positive++;
		}
		else {
			negative++;
		}
		if (dd > max_d) {
			max_d = dd;
		}
		else if (dd < min_d) {
			min_d = dd;
		}
		// printf("depth %d\n", dd);
	}
	double t = timer.reset();

	cout << "~~~~~~~ " + graph_name + " ~~~~~~~\n";
	cout << "#total queries: " << num_of_q << "\n";
	cout << "total time (s):\t" << t << "\n";
	cout << "avg. time (s):\t" << t / (double)num_of_q << "\n";
	cout << "min depth:\t" << min_d << "\n";
	cout << "max depth:\t" << max_d << "\n";
	cout << "avg depth:\t" << depth_sum / (double)num_of_q << "\n";
	cout << "positive:\t" << positive << "\n";
	cout << "negative:\t" << negative << "\n";
	cout << "2-FT-SSR-O calls:\t" << ch_sum << "\n";
	cout << "avg. 2-FT-SSR-O calls:\t" << ch_sum / (double)num_of_q << "\n";
	cout << "1-FT-SC-O calls:\t" << onef_sum << "\n";
	cout << "avg. 1-FT-SC-O:\t" << onef_sum / (double)num_of_q << "\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	
}

void checkInputGraphBeginConstruction(cndp::common::DimacsGraph *G)
{
	scc_tarjan *initial_sccs = new scc_tarjan();
	initial_sccs->findSCCs(G);
	TreeNode *root;
	if ((initial_sccs->t_cc == 1) && (initial_sccs->cc_array[0]->GetTotalVertices() == G->vertex_size)) {
		printf("\nInput graph is strongly connected.\n");
		total_nodes = G->vertex_size;
		root = new TreeNode(G);
		root->vertexMapReverse = new int[root->n_nodes + 1];
		for (int i = 0; i <= root->n_nodes; i++) {
			root->vertexMapReverse[i] = i;
		}

		printf("Constructing decomposition tree...\n");
		cndp::common::RFWTimer t;
		t.start();
		constructTree(root);
		printf("time needed: %f\n", t.reset());
		findTreeDepth(root);
		
		if (initialize_ds) {
			num_of_queries = 1000000;
			avgCaseQueries(root, num_of_queries);
		}

		cleanPointers(root);
	}
	else {
		printf("\nGraph is not strongly connected...\nPlease provide a strongly connected graph as input.\n");
		exit(-1);
	}
	delete initial_sccs;
}

int main(int argc, char *argv[])
{
	printf("\n\n---- START ----\n\n");

	checkStartingParameters(argc, argv);
	inputGraph = new cndp::common::DimacsGraph(filename, 1);
	inputGraph->CreateAdjencyList(); // create graph structure from input file
	checkInputGraphBeginConstruction(inputGraph);

	printf("\n\n---- END ----\n\n");
	return 0;
}
