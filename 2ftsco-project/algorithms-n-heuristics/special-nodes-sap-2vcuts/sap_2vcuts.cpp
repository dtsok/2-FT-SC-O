#include "sap_2vcuts.h"

sap_vcuts::sap_vcuts()
{
}

sap_vcuts::sap_vcuts(cndp::common::DimacsGraph *G)
{
	points = new bool[G->vertex_size + 1]();
}

sap_vcuts::~sap_vcuts()
{
	if (points != nullptr) {
		delete[] points;
	}
}

void sap_vcuts::findAllPoints(cndp::common::DimacsGraph *G)
{
	subroutine(G);
	for (int i = 1; i <= G->vertex_size; i++) {
		if (points[i]) {
			// printf("%d ",i);
			sap_points++;
		}
	}

	for (int x = 1; x <= G->vertex_size; x++) {
		// printf("x = %d\n",x);
		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(G, x); // find every SCC for the subgraph G\{x} (>=1)
		for (int scc_index = 0; scc_index < sccObj.t_cc; scc_index++) {
			cndp::common::ConnectedComponent *scc = sccObj.cc_array[scc_index];
			if (scc->GetTotalVertices() < 2) {
				continue;
			}
			int *indecies_in_scc = createIndexOfVertexInCC(scc, G->vertex_size);
			cndp::common::DimacsGraph *subgraph = createSubgraph(G, scc, indecies_in_scc);

			sap_vcuts subgraphSAP = sap_vcuts(subgraph);
			subgraphSAP.subroutine(subgraph);
			// verifyResults(subgraph, subgraphSAP.points, scc);
			// printf("\n");
			for (int i = 1; i <= scc->GetTotalVertices(); i++) {
				int vertex = scc->GetVertex(i);
				if (subgraphSAP.points[i] && !points[vertex]) {
					points[vertex] = true;
					points[x] = true;
				}
			}
			// if(sccObj.t_cc == 1) points[x] = true;
			delete[] indecies_in_scc;
			delete subgraph;
		}
	}

	for (int i = 1; i <= G->vertex_size; i++) {
		if (points[i]) {
			// printf("%d ",i);
			n_points++;
		}
	}
}

void sap_vcuts::subroutine(cndp::common::DimacsGraph *G)
{
	init_roots(G);
	isRootSAP(G);
	findStrongArticulationPoints(G, s1);
}

void sap_vcuts::init_roots(cndp::common::DimacsGraph *G)
{
	srand(time(NULL));					  // initialize the random seed
	s1 = rand() % G->vertex_size + 1;	  // select root s
	while (true) {
		s2 = rand() % G->vertex_size + 1; // select root s'
		if (s2 != s1) {
			break;
		}
	}
	// printf("%d %d /",s1,s2);
}

void sap_vcuts::isRootSAP(cndp::common::DimacsGraph *G)
{
	// mark s1 as deleted and do 2 DFS traversals from/to s2. if all (n-1) nodes are visited in both DFS runs then
	// s1 is not SAP

	// initial graph
	G->deleted_vertices[s1] = true;
	int first_run = dfs(G, s2);

	// reverse graph
	G->SetReverseGraph();
	int second_run = dfs(G, s2);
	G->deleted_vertices[s1] = false;
	G->SetReverseGraph();

	if ((first_run == second_run) && (first_run == G->vertex_size - 1)) {
		points[s1] = false; // s1 is not SAP
	}
	else {
		points[s1] = true;
	}
}

int sap_vcuts::dfs(cndp::common::DimacsGraph *G, int source)
{
	bool *visited = new bool[G->vertex_size + 1]();
	dfs_sub(G, source, visited);
	int n_reached = 0;
	for (int i = 1; i <= G->vertex_size; i++) {
		if (visited[i]) {
			n_reached++;
		}
	}
	delete[] visited;
	return n_reached;
}

void sap_vcuts::dfs_sub(cndp::common::DimacsGraph *G, int source, bool *visited)
{
	visited[source] = true;
	for (int i = G->out_edges_start_index[source]; i <= G->out_edges_end_index[source]; i++) {
		int v = G->out_edges[i];
		if (G->deleted_vertices[v]) {
			continue;
		}
		if (!visited[v]) {
			dfs_sub(G, v, visited);
		}
	}
}

void sap_vcuts::findStrongArticulationPoints(cndp::common::DimacsGraph *G, int source)
{
	// find SAP for initial graph
	findStrongArticulationPointsOneWay(G, source);

	// find SAP for the reverse graph
	G->SetReverseGraph();
	findStrongArticulationPointsOneWay(G, source);
	G->SetReverseGraph();
}

void sap_vcuts::findStrongArticulationPointsOneWay(cndp::common::DimacsGraph *G, int source)
{
	dominatorTree *domObj = new dominatorTree(G, source);
	bool *has_children = new bool[G->vertex_size + 1]();
	findNonTrivialDominators(domObj->idom, has_children, G->vertex_size);
	for (int v = 1; v <= G->vertex_size; v++) {
		if (v != source && has_children[v]) {
			points[v] = true;
		}
	}
	// for(int v = 1; v <= G -> vertex_size; v++) if(points[v]) printf("%d is non triv dom\n", v);

	delete domObj;
	delete[] has_children;
}

void sap_vcuts::findNonTrivialDominators(int *tree, bool *has_children, int n)
{
	bool *visited = new bool[n + 1]();
	for (int i = 1; i <= n; i++) {
		if (!visited[i]) {
			recursionInTree(tree, has_children, visited, i);
		}
	}
	delete[] visited;
}

void sap_vcuts::recursionInTree(int *tree, bool *has_children, bool *visited, int current)
{
	visited[current] = true;
	if (current != tree[current]) {	 // if current is not root continue the traversal
		int parent = tree[current];	 // move to its parent
		has_children[parent] = true; // current (parent of prev) has at least one child -> non trivial dominator
		if (!visited[parent]) {
			recursionInTree(tree, has_children, visited, parent);
		}
	}
}

int *sap_vcuts::createIndexOfVertexInCC(cndp::common::ConnectedComponent *cc, int total_nodes)
{
	int *indecies_in_cc = new int[total_nodes + 1];

	for (int i = 0; i <= total_nodes; i++) {
		indecies_in_cc[i] = 0;
	}

	for (int i = 1; i <= cc->GetTotalVertices(); i++) {
		indecies_in_cc[cc->GetVertex(i)] = i;
		// cout<<cc -> GetVertex(i)<<" "<<i<<endl;
	}

	return indecies_in_cc;
}

cndp::common::DimacsGraph *sap_vcuts::createSubgraph(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *scc, int *ind)
{
	cndp::common::DimacsGraph *sub1 = new cndp::common::DimacsGraph();
	int m = 0;
	for (int i = 1; i <= scc->GetTotalVertices(); i++) {

		int v = scc->GetVertex(i);

		for (int j = graph->out_edges_start_index[v]; j <= graph->out_edges_end_index[v]; j++) {

			int w = graph->out_edges[j];
			if (graph->deleted_vertices[w] || ind[w] == 0) {
				continue;
			}

			// std::cout << v << "-->" << w << std::endl;
			m++;
		}
	}
	// cout<<"edges "<<m<<endl;
	sub1->vertex_size = scc->GetTotalVertices();
	sub1->edge_size = m;
	sub1->input_file = new int[2 * m];
	int p = 0;
	for (int i = 1; i <= scc->GetTotalVertices(); i++) {

		int x = scc->GetVertex(i);

		for (int j = graph->out_edges_start_index[x]; j <= graph->out_edges_end_index[x]; j++) {

			int y = graph->out_edges[j];

			if (graph->deleted_vertices[y] || ind[y] == 0) {
				continue;
			}
			if (x == y) {
				continue;
			}

			sub1->input_file[p++] = ind[x];
			sub1->input_file[p++] = ind[y];

			// cout << x << "-->" << y << std::endl;
			// cout << this -> vertexMap[x] << "-->" << this -> vertexMap[y] << std::endl;
			// cout<<endl;
		}
	}

	if (p > 2 * m) {
		fprintf(stderr, "Error! Graph has >%d arcs.\n", m);
		exit(-1);
	}
	// for(int i = 0; i < 2*m-1; i=i+2) cout<<sub1->input_file[i]<<" -> "<<sub1->input_file[i+1]<<" /
	// "<<scc->GetVertex(sub1->input_file[i])<<"-"<<scc->GetVertex(sub1->input_file[i+1])<<endl;

	sub1->starting_vertex = 1;
	sub1->CreateAdjencyList();

	return sub1;
}

void sap_vcuts::verifyResults(cndp::common::DimacsGraph *G, bool *results, cndp::common::ConnectedComponent *scc)
{
	// printf("\nInside verify\n");
	// scc->PrintVertices();
	if (scc->GetTotalVertices() < 2) {
		return;
	}
	bool *tempResults = new bool[G->vertex_size + 1]();
	for (int i = 1; i <= G->vertex_size; i++) {
		// printf("remove i = %d\n",i);
		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(G, i);
		if (sccObj.t_cc > 1) {
			// printf("sap = %d/%d\n", i, scc->GetVertex(i));
			tempResults[i] = true;
		}
	}
	for (int i = 1; i <= G->vertex_size; i++) {
		if (tempResults[i] != results[i]) {
			printf("\nERROR @ results\n");
			printf("i = %d %d %d\n %d\n", i, tempResults[i], results[i], scc->GetVertex(i));
			scc->PrintVertices();
			exit(1);
		}
	}
	// printf("\n");
}
