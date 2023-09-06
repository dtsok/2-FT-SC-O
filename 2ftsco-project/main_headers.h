#ifndef CONTAINS_MAIN_HEADERS_AND_AUX_FUNCTIONS_H_
#define CONTAINS_MAIN_HEADERS_AND_AUX_FUNCTIONS_H_

#include "algorithms-n-heuristics/heuristics-with-bfs/heuristics_bfs.h"
#include "algorithms-n-heuristics/label-propagation/label_propagation.h"
#include "algorithms-n-heuristics/linear-time/critical_node_selection.h"
#include "algorithms-n-heuristics/page-rank/page_rank_selection.h"
#include "algorithms-n-heuristics/q-separator/bfs_tree.h"
#include "algorithms-n-heuristics/special-nodes-sap-2vcuts/sap_2vcuts.h"
#include "common/timer_cndp.h"
#include "tree-node-srcs/tree_node.h"

int cndp::common::Utilities::TOTAL_SLEEP_MICROSECONDS = 0; // used for the random number generator in utilities.h

/* global variables */
/* for the decomposition tree */
const char *filename; // substring of input file name
cndp::common::DimacsGraph *inputGraph = nullptr;
std::string graph_name;
int sccs_ID = 0;		// used for the mapping of the tree nodes to the files - also max{sccs_ID} = number of nodes in dec. tree
int algo2use = 1;		// used for the selection of the algorithm/ heuristic method - defaut 1 = critical node;
int tree_depth = 0;		// depth of decomposition tree
int total_nodes = 0;	// number of nodes for the input graph
bool initialize_ds = 0; // initialize aux data structures [2-FT-SSR-O, 1-FT-SC-O] and answer 1M random queries (avg. case)
/* store queries as 2-d array - {x,y,f1,f2} = {queries[i][0],...,queries[i][3]} */
int num_of_queries = 0;
int **queries = nullptr;

/* declaration of main algorithm used for answering the queries */
bool TwoFaultTolerantStrongConnectivityQuery(TreeNode *node, int x, int y, int failed_1, int failed_2, int &q_depth, int &ch_calls,
											 int &onef_calls);

void checkStartingParameters(int argc, char *argv[])
{
	std::string algos[6] = {"RANDOM", "MCN", "LABEL_PROPAGATION", "PAGE_RANK", "Q-SEPARATOR+MCN", "MCN + 3-CONNECTED"};
	if (argc != 4) {
		printf("\nUsage: %s <input file> [algorithm] <int> [initialize-data-structures] <bool>\n\n", argv[0]);
		printf("int: select algorithm\nRANDOM = 0\nMCN = 1\nLABEL "
			   "PROP = 2\nPAGE RANK = 3\nQ-SEPARATOR+MCN = 4\n\n"
			   "bool: initialize data structures for the queries [2-FT-SSR-O, 1-FT-SC-O]\n[default = 0]\n\n");
		printf("\n\n---- END ----\n\n");
		exit(-1);
	}
	else {
		filename = argv[1];
		algo2use = atoi(argv[2]);
		initialize_ds = atoi(argv[3]);
		if (algo2use == 4 && initialize_ds == true) {
			printf("Q-SEP + MCN doesn't support answering queries.\nQ-SEP + MCN is used only for the tree height experiments.");
			printf("\n\n---- END ----\n\n");
			exit(-1);
		}
	}
	graph_name = std::string(filename);
	int len = graph_name.length() - 4 - 7;
	graph_name = graph_name.substr(7, len);
	if (algo2use < 0 || algo2use > 4) {
		printf("\nUsage: %s <input file> [algorithm] <int> [initialize-data-structures] <bool>\n\n", argv[0]);
		printf("int: select algorithm\nRANDOM = 0\nMCN = 1\nLABEL "
			   "PROP = 2\nPAGE RANK = 3\nQ-SEPARATOR+MCN = 4\n\n"
			   "bool: initialize data structures for the queries [2-FT-SSR-O, 1-FT-SC-O]\n[default = 0]\n\n");
		printf("\n\n---- END ----\n\n");
		exit(-1);
	}
	printf("Algorithm used for the decomposition: %s\n", algos[algo2use].c_str());
	if (initialize_ds) {
		printf("Initialize 2-FT-SSR-O and 1-FT-SC-O for every node of the tree.\n");
	}
	srand(time(NULL)); /* comment this line if you want fixed seed for RANDOM algorithm */
}

void dfs_sub(cndp::common::DimacsGraph *G, int source, int destination, bool *visited)
{
	visited[source] = true;
	if (visited[destination]) {
		return;
	}
	for (int i = G->out_edges_start_index[source]; i <= G->out_edges_end_index[source]; i++) {
		int v = G->out_edges[i];
		if (G->deleted_vertices[v]) {
			continue;
		}
		if (!visited[v]) {
			dfs_sub(G, v, destination, visited);
		}
	}
}

bool reachabilityQueryDFS(cndp::common::DimacsGraph *G, int source, int destination)
{
	bool *visited = new bool[G->vertex_size + 1];
	for (int i = 0; i <= G->vertex_size; i++) {
		visited[i] = false;
	}

	dfs_sub(G, source, destination, visited);
	bool ans = visited[destination];

	delete[] visited;
	return ans;
}

bool areStronglyConnectedDFS(cndp::common::DimacsGraph *G, int source, int destination, int failed_1, int failed_2)
{
	G->deleted_vertices[failed_1] = true;
	G->deleted_vertices[failed_2] = true;

	bool ans = reachabilityQueryDFS(G, source, destination) && reachabilityQueryDFS(G, destination, source);

	G->deleted_vertices[failed_1] = false;
	G->deleted_vertices[failed_2] = false;

	return ans;
}

void dfs(cndp::common::DimacsGraph *G, int source, bool *visited, int &maxDistVertex)
{
	visited[source] = true;
	maxDistVertex = source;
	for (int i = G->out_edges_start_index[source]; i <= G->out_edges_end_index[source]; i++) {
		int v = G->out_edges[i];
		if (!visited[v]) {
			dfs(G, v, visited, maxDistVertex);
		}
	}
}

/* find farthest vertex from source wrt the adjacency list
	i.e. same graph with different edge order will produce different result */
int far(cndp::common::DimacsGraph *G, int source)
{
	bool *visited = new bool[G->vertex_size + 1];
	for (int i = 0; i <= G->vertex_size; i++) {
		visited[i] = false;
	}
	int k = 0;
	dfs(G, source, visited, k);

	delete[] visited;
	return k;
}

/* for generating random queries */
int *all_vertices = nullptr;

void get_query(int n, int *x, int *y, int *f1, int *f2)
{
	int i;
	int temp;
	i = rand() % n;
	*x = all_vertices[i];
	temp = all_vertices[0];
	all_vertices[0] = all_vertices[i];
	all_vertices[i] = temp;
	i = 1 + rand() % (n - 1);
	*y = all_vertices[i];
	temp = all_vertices[1];
	all_vertices[1] = all_vertices[i];
	all_vertices[i] = temp;
	i = 2 + rand() % (n - 2);
	*f1 = all_vertices[i];
	temp = all_vertices[2];
	all_vertices[2] = all_vertices[i];
	all_vertices[i] = temp;
	i = 3 + rand() % (n - 3);
	*f2 = all_vertices[i];
	temp = all_vertices[3];
	all_vertices[3] = all_vertices[i];
	all_vertices[i] = temp;
}

void initializeQ(int n)
{
	all_vertices = (int *)malloc(sizeof(int) * n);
	for (int i = 0; i < n; i++) {
		all_vertices[i] = i + 1;
	}
}

int load_queries(std::string file_name)
{
	/* queries in the file should be of the form x y f1 f2 */
	std::ifstream inp(file_name);
	std::string line, token;
	int number_of_lines = 0;
	while (getline(inp, line)) {
		++number_of_lines;
	}
	inp.close();
	queries = new int *[number_of_lines];
	num_of_queries = number_of_lines;
	std::ifstream inp2(file_name);
	int ind = 0;
	while (getline(inp2, line)) {
		queries[ind] = new int[4];
		std::istringstream iss(line);
		iss >> token;
		for (int i = 0; i < 4; i++) {
			// cout << token << " ";
			queries[ind][i] = std::stoi(token);
			iss >> token;
		}
		// cout << "\n";
		ind++;
	}
	inp2.close();
	return number_of_lines;
}

void test_queries_from_file(TreeNode *obj, std::string queries_file_name)
{
	int ll = load_queries(queries_file_name);

	int min_d = obj->n_nodes, max_d = 0;
	int depth_sum = 0;
	long int ch_sum = 0, onef_sum = 0;
	int positive = 0, negative = 0;
	cndp::common::RFWTimer timer;
	timer.start();
	for (int i = 0; i < ll; i++) {
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

	system("mkdir -p queries_results/");
	std::ofstream outp("queries_results/results_" + graph_name + ".txt");
	outp << "~~~~ " + graph_name + " ~~~~~\n";
	outp << "#total queries: " << ll << "\n";

	outp << "time\t" << t << "\n";
	outp << "avg time\t" << t / (double)ll << "\n";
	outp << "min depth\t" << min_d << "\n";
	outp << "max depth\t" << max_d << "\n";
	outp << "avg depth\t" << depth_sum / (double)ll << "\n";
	outp << "positive\t" << positive << "\n";
	outp << "negative\t" << negative << "\n";
	outp << "ch calls\t" << ch_sum << "\n";
	outp << "avg ch calls\t" << ch_sum / (double)ll << "\n";
	outp << "one fault calls\t" << onef_sum << "\n";
	outp << "avg one f\t" << onef_sum / (double)ll << "\n";

	outp.close();
}

void generate_export_avg_case_queries(int nodes, int q_num)
{
	initializeQ(nodes);
	queries = new int *[q_num];
	std::ofstream qw("avg_case_queries_" + graph_name + ".txt");
	for (int i = 0; i < q_num; i++) {
		queries[i] = new int[4];
		get_query(nodes, &queries[i][0], &queries[i][1], &queries[i][2], &queries[i][3]);
	}
	for (int i = 0; i < q_num; i++) {
		qw << queries[i][0] << " " << queries[i][1] << " " << queries[i][2] << " " << queries[i][3] << "\n";
	}
	qw.close();
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*	aux functions and variables used for searching bad/worst case queries
	following variables should be used only if IDs are assigned to the nodes of the tree, otherwise seg fault will occur  */
int *vertex_to_last_scc = nullptr;	 // store last scc (id) of every vertex
int *dec_tree = nullptr;			 // decomposition tree as array with sccs ids
TreeNode **id_to_nodes = nullptr;	 // store pointers to TreeNode objects (nodes of d.t.) for finding/accessing corresponding nodes
rmq::NCA *ncaObj = nullptr;			 // used for fill_dec_tree function -> nca queries
rmq::ChildQueries *chqObj = nullptr; // same as ncaObj

void traverseTree(TreeNode *node, int &index, int parent_index)
{
	dec_tree[index] = parent_index;
	parent_index = index;
	id_to_nodes[node->global_ID + 1] = node;
	for (int i = 0; i < node->n_chil; i++) {
		traverseTree(node->children[i], ++index, parent_index);
	}
}

void traverseTree(TreeNode *node)
{
	// printf("id %d size %d depth %d\n", node->global_ID, node->n_nodes, node->node_depth);
	if (node->n_chil == 0) {
		for (int v = 1; v <= node->n_nodes; v++) {
			vertex_to_last_scc[node->vertexMapReverse[v]] = node->global_ID + 1;
		}
		return;
	}

	for (int i = 1; i <= node->n_nodes; i++) {
		if (node->vertexMap[i] == 0) {
			vertex_to_last_scc[node->vertexMapReverse[i]] = node->global_ID + 1;
		}
	}

	for (int i = 0; i < node->n_chil; i++) {
		traverseTree(node->children[i]);
	}
}

/* store dec. tree as array with ids as labels. initialize nca/is_ancestor queries. used for finding bad/worst case queries only */
void fill_dec_tree(TreeNode *root)
{
	// printf("TOTAL IDS %d\n",sccs_ID);
	vertex_to_last_scc = new int[inputGraph->vertex_size + 1]();
	dec_tree = new int[sccs_ID + 1]();
	id_to_nodes = new TreeNode *[sccs_ID + 1];
	int index = 1;
	traverseTree(root, index, index);
	traverseTree(root);
	ncaObj = new rmq::NCA();
	ncaObj->initialize(sccs_ID, dec_tree, 1);
	chqObj = new rmq::ChildQueries();
	chqObj->initialize(sccs_ID, dec_tree, 1);
}

#endif
