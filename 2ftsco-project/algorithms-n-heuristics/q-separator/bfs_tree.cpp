#include "bfs_tree.h"
#include "../../common/connected_component.h"

qSeparator::qSeparator()
{
}

qSeparator::qSeparator(cndp::common::DimacsGraph *G, int delta)
{
	maxSize = sqrt(G->vertex_size);
	// sqrt_n = std::ceil(sqrt(G->vertex_size)) * delta;
	sqrt_n = maxSize;
	sep = new bool[G->vertex_size + 1]();

	// int count = 0;
	/*for (int currentRoot = 1; currentRoot <= G->vertex_size; currentRoot++) {
		// count++;
		if (findSeparators(G, currentRoot)) {
			// printf("root = %d\n", currentRoot);
			break;
		}
	}*/
	int currentRoot = rand()%G->vertex_size + 1;
	// printf("\nr = %d\n",currentRoot);
	if(currentRoot > G->vertex_size || currentRoot <= 0){
		printf("\nERROR\n");
		exit(2);
	}
	if(!findSeparators(G, currentRoot)){
		G->SetReverseGraph();
		findSeparators(G, currentRoot);
		G->SetReverseGraph();
	}
}

qSeparator::~qSeparator()
{
	delete[] sep;
}

void qSeparator::BFS(cndp::common::DimacsGraph *G)
{
	layer_size[1] = 1;
	depths[root] = 1;
	parents[root] = root;

	q = std::queue<int>();
	q.push(root);
	visited[root] = true;
	while (!q.empty()) {
		int u = q.front();
		q.pop();
		for (int i = G->out_edges_start_index[u]; i <= G->out_edges_end_index[u]; i++) {
			int adj = G->out_edges[i];
			if (!visited[adj]) {
				q.push(adj);
				visited[adj] = true;
				depths[adj] = depths[u] + 1;
				if (depths[adj] > tree_depth) {
					tree_depth = depths[adj];
				}
				parents[adj] = u;
				layer_size[depths[adj]]++;
			}
		}
	}
}

bool qSeparator::findSeparators(cndp::common::DimacsGraph *g, int R)
{
	root = R;
	tree_depth = 0;
	has_small_diameter = false;

	visited = new bool[g->vertex_size + 1]();
	depths = new int[g->vertex_size + 1]();
	layer_size = new int[g->vertex_size + 1]();
	parents = new int[g->vertex_size + 1]();

	BFS(g);
	// printf("root = %d\ttree depth = %d\n", root, tree_depth);
	if (tree_depth > diam) {
		diam = tree_depth;
	}
	if (tree_depth - 1 < sqrt_n) {
		has_small_diameter = true;
		// test
	}
	else {
		for (int i = 2; i < tree_depth; i++) {
			if (layer_size[i] <= maxSize) {
				int aboveLayers = 0;
				int belowLayers = 0;
				for (int j = 1; j < i; j++) {
					aboveLayers += layer_size[j];
				}
				for (int j = i + 1; j <= tree_depth; j++) {
					belowLayers += layer_size[j];
				}
				int bigger = std::max(aboveLayers, belowLayers);
				int smaller = std::min(aboveLayers, belowLayers);
				if (smaller / (double)bigger >= 0.75) {
					// printf("smaller/bigger %f lay size %d maxS
					// %f\n",(double)smaller/bigger, layer_size[i], maxSize);
					// printf("%d %d\n", smaller, bigger);
					if (!(bigger > g->vertex_size - layer_size[i] * maxSize)) {
						continue;
					}
					int tempCount = 0;
					for (int v = 1; v <= g->vertex_size; v++) {
						if (depths[v] == i) {
							sep[v] = true;
							tempCount++;
						}
						if (tempCount == layer_size[i]) {
							break;
						}
					}
					sep_size = layer_size[i];
					found = true;
					break;
				}
			}
		}
	}

	delete[] visited;
	delete[] depths;
	delete[] layer_size;
	delete[] parents;

	return found;
}

void qSeparator::copyResults(int n, bool *separators)
{
	for (int i = 1; i <= n; i++) {
		separators[i] = sep[i];
	}
}

void qSeparator::printInfo(int n)
{
	printf("root %d -  sqrt_n %d - max_depth %d\n", root, sqrt_n, tree_depth);
	for (int i = 1; i <= n; i++) {
		printf("%d %d\n", i, parents[i]);
	}
	for (int i = 1; i <= n; i++) {
		printf("%d %d %d\n", i, layer_size[i], depths[i]);
	}
}