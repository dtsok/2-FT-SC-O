#include "dominator_only.h"

dominatorTree::dominatorTree()
{
}

dominatorTree::~dominatorTree()
{
	delete[] this->idom;
}

dominatorTree::dominatorTree(cndp::common::DimacsGraph *G, int root)
{
	computeDominatorTree(G, root);
}

inline void dominatorTree::incc()
{
	count++;
}

void dominatorTree::initComputation(int N)
{

	next = count = 0;
	label2pre = new int[N];
	pre2label = new int[N];
	idom = new int[N];
	parent = new int[N];
	semi = new int[N];
	label = new int[N];
	dom = new int[N];
	ubucket = new int[N];

	for (int i = 0; i < N; i++) {
		label2pre[i] = pre2label[i] = idom[i] = dom[i] = parent[i] = ubucket[i] = semi[i] = label[i] = 0;
	}
	nr = 1;
}

// path compression
inline void dominatorTree::rcompress(int v, int c)
{
	int p;
	incc();
	if ((p = parent[v]) > c) {
		rcompress(p, c);
		incc();
		if (semi[label[p]] < semi[label[v]]) {
			label[v] = label[p];
		}
		parent[v] = parent[p];
	}
}

void dominatorTree::dfsForConstruction(cndp::common::DimacsGraph *graph, int k)
{
	label2pre[k] = (++next);
	pre2label[next] = k;
	for (int j = graph->out_edges_start_index[k]; j <= graph->out_edges_end_index[k]; j++) {
		int v = graph->out_edges[j];
		if (graph->deleted_vertices[v]) {
			continue;
		}
		incc();
		if (!label2pre[v]) {
			dfsForConstruction(graph, v);
			parent[label2pre[v]] = label2pre[k];
		}
	}
}

void dominatorTree::computeDominatorTree(cndp::common::DimacsGraph *graph, int root)
{
	r = root;
	initComputation(graph->vertex_size + 1);

	int i, j;

	// pre-dfs
	next = 0;
	dfsForConstruction(graph, r);
	nr = next;

	for (i = 0; i <= nr; i++) {
		label[i] = semi[i] = i;
	}

	// process the vertices in reverse preorder
	for (i = nr; i > 1; i--) {
		// printf("i %d ",pre2label[i]);

		/*---------------------
		 | process i-th bucket
		 *--------------------*/
		for (int v = ubucket[i]; v; v = ubucket[v]) {
			// fprintf(stdout, "processing bucket %d\n", i);
			rcompress(v, i);
			int u = label[v]; // u is a vertex with min sdom in (sdom[v],v]
			incc();
			dom[v] = (semi[u] < semi[v]) ? u : i;
			// u is a vertex with min sdom in (sdom[v],v]
		}
		// no need to empty the bucket

		/*---------------------------------------------
		 | check incoming arcs, update semi-dominators
		 *--------------------------------------------*/
		int k = pre2label[i];
		// fprintf(stdout, "processing vertex=%d, preorder=%d\n", k, i);
		// int t;
		for (j = graph->in_edges_start_index[k]; j <= graph->in_edges_end_index[k]; j++) {
			int v = label2pre[graph->in_edges[j]];
			// fprintf(stdout, "\t arc from vertex=%d, preorder=%d\n", graph -> in_edges[j], v);
			incc();

			if (v) {
				int u;
				incc();
				if (v <= i) {
					u = v;
				} // v is an ancestor of i
				else {
					rcompress(v, i);
					u = label[v];
				}
				incc();

				/* make B and R arc-disjoint */
				// if ( (semi[u] == semi[i]) && (blue2[i] == parent[i]) ) blue2[i] = v;

				if (semi[u] < semi[i]) {
					semi[i] = semi[u];
				}
			}
		}
		/*---------------------------
		 | process candidate semidom
		 *--------------------------*/
		int s = semi[i];
		incc();
		if (s != parent[i]) { // if semidominator n not parent: add i to s's bucket
			ubucket[i] = ubucket[s];
			ubucket[s] = i;
			// printf("enter %d into bucket[%d]\n",i,s);
		}
		else {
			dom[i] = s; // semidominator is parent: s is a candidate dominator
		}
	}

	/*------------------
	 | process bucket 1
	 *-----------------*/
	for (int v = ubucket[1]; v; v = ubucket[v]) {
		dom[v] = 1;
	}

	/*---------------
	 | recover idoms
	 *--------------*/

	dom[1] = 1;
	idom[r] = r;
	for (i = 2; i <= nr; i++) {
		incc();
		if (dom[i] != semi[i]) {
			dom[i] = dom[dom[i]]; // make relative absolute
		}
		idom[pre2label[i]] = pre2label[dom[i]];
	}

	// printTree(idom, graph -> vertex_size, 'd');

	// delete [] idom;

	delete[] dom;
	delete[] parent;
	delete[] semi;
	delete[] label;
	delete[] ubucket;
	delete[] pre2label;
	delete[] label2pre;
}

void dominatorTree::printTree(int *p, int n, char c)
{
	fprintf(stdout, "\n");
	for (int i = 1; i <= n; i++) {
		fprintf(stdout, "%c %d %d\n", c, p[i], i);
	}
	printf("\n");
}
