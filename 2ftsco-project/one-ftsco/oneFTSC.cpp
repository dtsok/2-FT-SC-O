#include "oneFTSC.h"
#include <cstdlib>
namespace rmq {

int find(int *p, int x)
{
	int r = x;
	while (p[r] != r) {
		r = p[r];
	}
	while (p[x] != r) {
		int next = p[x];
		p[x] = r;
		x = next;
	}
	return r;
}

void unite(int *p, int *size, int x, int y)
{
	int r1 = find(p, x);
	int r2 = find(p, y);
	if (r1 == r2) {
		return;
	}
	if (size[r1] < size[r2]) {
		p[r1] = r2;
		size[r2] += size[r1];
	}
	else {
		p[r2] = r1;
		size[r1] += size[r2];
	}
}

void get_h(int n, int *gOut, int *firstOut, int *dfs, int *idfs, int *p, int **_h)
{
	int *ND = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		ND[i] = 1;
	}
	for (int i = n - 1; i > 0; i--) {
		int v = idfs[i];
		ND[p[v]] += ND[v];
	}
	NCA *nca = new NCA();
	nca->initialize(n, p, idfs[0]);
	int m = firstOut[n + 1];
	int *edgeFirst = (int *)malloc(sizeof(int) * (n + 1));
	int *edgeNext = (int *)malloc(sizeof(int) * m);
	int *edgeStack = (int *)malloc(sizeof(int) * 2 * m);
	for (int i = 1; i <= n; i++) {
		edgeFirst[i] = -1;
	}
	int SP = 0;
	// store the crossing edges (x,y) such that nca(x,y)=v
	for (int x = 1; x <= n; x++) {
		for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
			int y = gOut[i];
			if (dfs[y] < dfs[x] && dfs[x] >= dfs[y] + ND[y]) {
				int v = nca->query(x, y);
				edgeNext[SP] = edgeFirst[v];
				edgeFirst[v] = SP;
				edgeStack[2 * SP] = x;
				edgeStack[2 * SP + 1] = y;
				SP++;
			}
		}
	}
	char *found = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		found[i] = 0;
	}
	int *Q = (int *)malloc(sizeof(int) * n);

	int *incomingFirst = (int *)malloc(sizeof(int) * (n + 1));
	int *incomingNext = (int *)malloc(sizeof(int) * m);
	int *incomingStack = (int *)malloc(sizeof(int) * m);
	int incSP = 0;
	for (int i = 1; i <= n; i++) {
		incomingFirst[i] = -1;
	}

	int *ufparent = (int *)malloc(sizeof(int) * (n + 1));
	int *ufsize = (int *)malloc(sizeof(int) * (n + 1));
	int *root = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		ufparent[i] = i;
		ufsize[i] = 1;
		root[i] = i;
	}
	int *h = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		h[i] = -1;
	}
	int *gOutR;
	int *firstOutR;
	get_reverse(n, gOut, firstOut, &gOutR, &firstOutR);

	for (int d = n - 2; d >= 0; d--) {
		int v = idfs[d];
		// store all crossing (x,y) such that nca(x,y)=v, as incoming to y
		for (int i = edgeFirst[v]; i != -1; i = edgeNext[i]) {
			int x = edgeStack[2 * i];
			int y = edgeStack[2 * i + 1];
			int z = root[find(ufparent, y)];
			incomingNext[incSP] = incomingFirst[z];
			incomingFirst[z] = incSP;
			incomingStack[incSP++] = x;
		}

		// find all u such that h[u]=v
		int first = 0;
		int last = 0;
		// first, get all the incoming back-edges to v
		for (int i = firstOutR[v]; i < firstOutR[v + 1]; i++) {
			int u = gOutR[i];
			if (dfs[v] < dfs[u] && dfs[u] < dfs[v] + ND[v]) {
				u = root[find(ufparent, u)];
				if (!found[u]) {
					Q[last++] = u;
					found[u] = 1;
				}
			}
		}
		while (first != last) {
			int u = Q[first++];
			if (h[u] == -1) {
				h[u] = v;
			}
			// first, get the incoming edges to u
			for (int i = incomingFirst[u]; i != -1; i = incomingNext[i]) {
				int z = root[find(ufparent, incomingStack[i])];
				if (!found[z]) {
					Q[last++] = z;
					found[z] = 1;
				}
			}
			// now go to the parent of u
			int next = root[find(ufparent, p[u])];
			if (next != v && !found[next]) {
				Q[last++] = next;
				found[next] = 1;
			}
		}
		for (int i = 0; i < last; i++) {
			unite(ufparent, ufsize, Q[i], v);
		}
		root[find(ufparent, v)] = v;
		for (int i = 0; i < last; i++) {
			found[Q[i]] = 0;
		}
	}
	free(ND);
	free(edgeFirst);
	free(edgeNext);
	free(edgeStack);
	free(ufparent);
	free(ufsize);
	free(root);
	delete nca;
	free(incomingFirst);
	free(incomingNext);
	free(incomingStack);
	free(found);
	free(Q);
	free(gOutR);
	free(firstOutR);
	*_h = h;
}

void get_dominators(int n, int *gOut, int *firstOut, int r, int **_dom)
{
	int *dom = (int *)malloc(sizeof(int) * (n + 1));
	int *dfs;
	int *idfs;
	int *p;
	DFS(n, gOut, firstOut, r, &dfs, &idfs, &p);
	int *parent = (int *)malloc(sizeof(int) * (n + 1));
	int *sdom = (int *)malloc(sizeof(int) * (n + 1));
	int *stack = (int *)malloc(sizeof(int) * (n + 1));
	int *first = (int *)malloc(sizeof(int) * (n + 1));
	int *next = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		first[i] = -1;
	}
	int SP = 0;
	int *gOutR;
	int *firstOutR;
	get_reverse(n, gOut, firstOut, &gOutR, &firstOutR);
	int *label = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		parent[i] = -1;
		label[i] = i;
		sdom[i] = i;
	}
	dom[r] = -1;
	for (int d = n - 1; d > 0; d--) {
		int w = idfs[d];
		for (int i = firstOutR[w]; i < firstOutR[w + 1]; i++) {
			int v = gOutR[i];

			// u=EVAL(v)
			int u;
			if (parent[v] == -1) {
				u = v;
			}
			else {
				COMPRESS(v, parent, label, sdom, dfs);
				u = label[v];
			}

			if (dfs[sdom[u]] < dfs[sdom[w]]) {
				sdom[w] = sdom[u];
			}
		}

		next[SP] = first[sdom[w]];
		first[sdom[w]] = SP;
		stack[SP++] = w;
		parent[w] = p[w];

		for (int i = first[parent[w]]; i != -1; i = next[i]) {
			int v = stack[i];

			// u=EVAL(v)
			int u;
			if (parent[v] == -1) {
				u = v;
			}
			else {
				COMPRESS(v, parent, label, sdom, dfs);
				u = label[v];
			}

			if (dfs[sdom[u]] < dfs[sdom[v]]) {
				dom[v] = u;
			}
			else {
				dom[v] = parent[w];
			}
		}
	}

	for (int d = 1; d < n; d++) {
		int w = idfs[d];
		if (dom[w] != sdom[w]) {
			dom[w] = dom[dom[w]];
		}
	}

	free(dfs);
	free(idfs);
	free(p);
	free(parent);
	free(sdom);
	free(label);
	free(stack);
	free(first);
	free(next);
	free(gOutR);
	free(firstOutR);
	*_dom = dom;
}

void COMPRESS(int v, int *parent, int *label, int *sdom, int *dfs)
{
	if (parent[parent[v]] != -1) {
		COMPRESS(parent[v], parent, label, sdom, dfs);
		if (dfs[sdom[label[parent[v]]]] < dfs[sdom[label[v]]]) {
			label[v] = label[parent[v]];
		}
		parent[v] = parent[parent[v]];
	}
}

void DFS(int n, int *gOut, int *firstOut, int r, int **_dfs, int **_idfs, int **_p)
{
	int *dfs = (int *)malloc(sizeof(int) * (n + 1));
	int *idfs = (int *)malloc(sizeof(int) * (n + 1));
	int *p = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		dfs[i] = -1;
	}
	int *temp_vertex = (int *)malloc(sizeof(int) * n);
	int *temp_out = (int *)malloc(sizeof(int) * n);
	int Nr = 0;
	dfs[r] = Nr;
	idfs[Nr++] = r;
	p[r] = -1;
	temp_vertex[0] = r;
	temp_out[0] = firstOut[r];
	int SP = 0;
	while (SP != -1) {
		int v = temp_vertex[SP];
		char descend = 0;
		for (int i = temp_out[SP]; i < firstOut[v + 1]; i++) {
			int u = gOut[i];
			if (dfs[u] == -1) {
				dfs[u] = Nr;
				idfs[Nr++] = u;
				p[u] = v;
				temp_vertex[SP + 1] = u;
				temp_out[SP + 1] = firstOut[u];
				temp_out[SP] = i + 1;
				descend = 1;
				break;
			}
		}
		if (descend) {
			SP++;
			continue;
		}
		SP--;
	}

	*_dfs = dfs;
	*_idfs = idfs;
	*_p = p;
	free(temp_vertex);
	free(temp_out);
}

void get_reverse(int n, int *gOut, int *firstOut, int **_gOutR, int **_firstOutR)
{
	int *gOutR = (int *)malloc(sizeof(int) * firstOut[n + 1]);
	int *firstOutR = (int *)malloc(sizeof(int) * (n + 2));
	for (int i = 0; i <= n + 1; i++) {
		firstOutR[i] = 0;
	}
	for (int x = 1; x <= n; x++) {
		for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
			int y = gOut[i];
			firstOutR[y + 1]++;
		}
	}
	for (int i = 1; i <= n + 1; i++) {
		firstOutR[i] += firstOutR[i - 1];
	}
	int *currentOut = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		currentOut[i] = firstOutR[i];
	}
	for (int x = 1; x <= n; x++) {
		for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
			int y = gOut[i];
			gOutR[currentOut[y]++] = x;
		}
	}
	free(currentOut);
	*_gOutR = gOutR;
	*_firstOutR = firstOutR;
}

oneFTSC_naive::oneFTSC_naive(int n, int *_gOut, int *_firstOut)
{
	gOut = (int *)malloc(sizeof(int) * _firstOut[n + 1]);
	firstOut = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i < _firstOut[n + 1]; i++) {
		gOut[i] = _gOut[i];
	}
	for (int i = 1; i <= n + 1; i++) {
		firstOut[i] = _firstOut[i];
	}
	Q = (int *)malloc(sizeof(int) * n);
	found = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		found[i] = 0;
	}
}
oneFTSC_naive::~oneFTSC_naive()
{
	free(gOut);
	free(firstOut);
	free(Q);
	free(found);
}
char oneFTSC_naive::query(int x, int y, int f)
{
	if (x == y) {
		return 1;
	}
	if (x == f || y == f) {
		return 0;
	}
	int first = 0;
	int last = 0;
	Q[last++] = x;
	found[x] = 1;
	char foundY = 0;
	while (first != last && !foundY) {
		int v = Q[first++];
		for (int i = firstOut[v]; i < firstOut[v + 1]; i++) {
			int u = gOut[i];
			if (u == f) {
				continue;
			}
			if (u == y) {
				foundY = 1;
				break;
			}
			if (!found[u]) {
				Q[last++] = u;
				found[u] = 1;
			}
		}
	}
	for (int i = 0; i < last; i++) {
		found[Q[i]] = 0;
	}
	if (!foundY) {
		return 0;
	}

	first = 0;
	last = 0;
	Q[last++] = y;
	found[y] = 1;
	char foundX = 0;
	while (first != last && !foundX) {
		int v = Q[first++];
		for (int i = firstOut[v]; i < firstOut[v + 1]; i++) {
			int u = gOut[i];
			if (u == f) {
				continue;
			}
			if (u == x) {
				foundX = 1;
				break;
			}
			if (!found[u]) {
				Q[last++] = u;
				found[u] = 1;
			}
		}
	}
	for (int i = 0; i < last; i++) {
		found[Q[i]] = 0;
	}
	return foundX;
}

oneFTSC::oneFTSC(int n, int *gOut, int *firstOut)
{
	int *gOutR;
	int *firstOutR;
	get_reverse(n, gOut, firstOut, &gOutR, &firstOutR);
	int *dom;
	get_dominators(n, gOut, firstOut, 1, &dom);
	ancD = new ChildQueries();
	ancD->initialize(n, dom, 1);
	free(dom);

	get_dominators(n, gOutR, firstOutR, 1, &dom);
	ancDR = new ChildQueries();
	ancDR->initialize(n, dom, 1);
	free(dom);

	int *h;
	int *dfs;
	int *idfs;
	int *p;
	DFS(n, gOut, firstOut, 1, &dfs, &idfs, &p);
	get_h(n, gOut, firstOut, dfs, idfs, p, &h);
	ncaH = new NCA();
	ncaH->initialize(n, h, 1);
	free(h);
	free(dfs);
	free(idfs);
	free(p);

	DFS(n, gOutR, firstOutR, 1, &dfs, &idfs, &p);
	get_h(n, gOutR, firstOutR, dfs, idfs, p, &h);
	ncaHR = new NCA();
	ncaHR->initialize(n, h, 1);
	free(h);
	free(dfs);
	free(idfs);
	free(p);
	free(gOutR);
	free(firstOutR);
}
oneFTSC::~oneFTSC()
{
	delete ncaH;
	delete ncaHR;
	delete ancD;
	delete ancDR;
}
char oneFTSC::query(int x, int y, int f)
{
	if (x == y) {
		return 1;
	}
	if (x == f || y == f) {
		return 0;
	}
	int w = ncaH->query(x, y);
	if ((w == f || !ancD->is_ancestor(f, w)) && (ancD->is_ancestor(f, x) || ancD->is_ancestor(f, y))) {
		return 0;
	}
	w = ncaHR->query(x, y);
	if ((w == f || !ancDR->is_ancestor(f, w)) && (ancDR->is_ancestor(f, x) || ancDR->is_ancestor(f, y))) {
		return 0;
	}
	return 1;
}

} // namespace rmq