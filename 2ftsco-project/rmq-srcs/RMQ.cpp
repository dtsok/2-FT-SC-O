#include "RMQ.h"
#include <cstdlib>
namespace rmq {

void construct_tree(int n, int *p, int r, int **_child, int **_firstChild)
{
	int *nChildren = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		nChildren[i] = 0;
	}
	for (int i = 1; i <= n; i++) {
		if (i != r) {
			nChildren[p[i]]++;
		}
	}
	int *firstChild = (int *)malloc(sizeof(int) * (n + 2));
	for (int i = 0; i <= n + 1; i++) {
		firstChild[i] = 0;
	}
	for (int i = 2; i <= n + 1; i++) {
		firstChild[i] = firstChild[i - 1] + nChildren[i - 1];
	}
	int *currentChild = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		currentChild[i] = firstChild[i];
	}
	int *child = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		if (i == r) {
			continue;
		}
		int parent = p[i];
		child[currentChild[parent]++] = i;
	}
	free(nChildren);
	free(currentChild);
	*_child = child;
	*_firstChild = firstChild;
}

char ChildQueries::is_ancestor(int a, int d)
{
	return dfs[a] <= dfs[d] && dfs[d] < dfs[a] + ND[a];
}
void ChildQueries::initialize(int n, int *tree, int r)
{
	construct_tree(n, tree, r, &child, &firstChild);
	dfs = (int *)malloc(sizeof(int) * (n + 1));
	int *idfs = (int *)malloc(sizeof(int) * n);
	int *p = (int *)malloc(sizeof(int) * (n + 1));
	int *temp_vertex = (int *)malloc(sizeof(int) * n);
	int *temp_out = (int *)malloc(sizeof(int) * n);
	for (int i = 0; i <= n; i++) {
		dfs[i] = -1;
	}
	int num = 0;
	dfs[r] = num;
	idfs[num++] = r;
	p[r] = 0;
	temp_vertex[0] = r;
	temp_out[0] = firstChild[r];
	int SP = 0;
	while (SP != -1) {
		int v = temp_vertex[SP];
		if (temp_out[SP] < firstChild[v + 1]) {
			int c = child[temp_out[SP]++];
			dfs[c] = num;
			idfs[num++] = c;
			p[c] = v;
			temp_vertex[SP + 1] = c;
			temp_out[SP + 1] = firstChild[c];
			SP++;
			continue;
		}
		SP--;
	}
	ND = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		ND[i] = 1;
	}
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		ND[p[v]] += ND[v];
	}
	free(idfs);
	free(p);
	free(temp_vertex);
	free(temp_out);
}
int ChildQueries::query(int a, int d)
{
	// we assume that a is a proper ancestor of d;
	// otherwise, errors may occur
	return query(d, firstChild[a], firstChild[a + 1] - 1);
}
int ChildQueries::query(int d, int i, int j)
{
	if (i == j) {
		return child[i];
	}
	if (j == i + 1) {
		if (is_ancestor(child[i], d)) {
			return child[i];
		}
		return child[j];
	}
	int m = i + (j - i) / 2;
	if (is_ancestor(child[m], d)) {
		return child[m];
	}
	if (dfs[child[m]] < dfs[d]) {
		return query(d, m + 1, j);
	}
	return query(d, i, m - 1);
}
ChildQueries::~ChildQueries()
{
	free(dfs);
	free(ND);
	free(child);
	free(firstChild);
}

void RMQ::initialize(int _n, int *A)
{
	n = _n;
	minR = (int **)malloc(sizeof(int *) * n);
	minL = (int **)malloc(sizeof(int *) * n);
	minRIndex = (int **)malloc(sizeof(int *) * n);
	minLIndex = (int **)malloc(sizeof(int *) * n);
	for (int x = 0; x < n; x++) {
		int l = 2;
		int log = 1;
		while (x + l - 1 <= n - 1) {
			l *= 2;
			log++;
		}
		minR[x] = (int *)malloc(sizeof(int) * log);
		minR[x][0] = A[x];
		minRIndex[x] = (int *)malloc(sizeof(int) * log);
		minRIndex[x][0] = x;

		l = 2;
		log = 1;
		while (x - l + 1 >= 0) {
			l *= 2;
			log++;
		}
		minL[x] = (int *)malloc(sizeof(int) * log);
		minL[x][0] = A[x];
		minLIndex[x] = (int *)malloc(sizeof(int) * log);
		minLIndex[x][0] = x;
	}
	int log = 1;
	int length = 2;
	while (length - 1 <= n - 1) {
		int x = 0;
		while (x + length - 1 <= n - 1) {
			int min1 = minR[x][log - 1];
			int min2 = minR[x + length / 2][log - 1];
			if (min1 < min2) {
				minR[x][log] = min1;
				minRIndex[x][log] = minRIndex[x][log - 1];
			}
			else {
				minR[x][log] = min2;
				minRIndex[x][log] = minRIndex[x + length / 2][log - 1];
			}
			x++;
		}
		x = n - 1;
		while (x - length + 1 >= 0) {
			int min1 = minL[x][log - 1];
			int min2 = minL[x - length / 2][log - 1];
			if (min1 < min2) {
				minL[x][log] = min1;
				minLIndex[x][log] = minLIndex[x][log - 1];
			}
			else {
				minL[x][log] = min2;
				minLIndex[x][log] = minLIndex[x - length / 2][log - 1];
			}
			x--;
		}
		length *= 2;
		log++;
	}
	_log = (int *)malloc(sizeof(int) * n);
	_log[0] = 0;
	for (int i = 1; i < n; i++) {
		int l = 2;
		_log[i] = 0;
		while (l - 1 <= i) {
			l *= 2;
			_log[i]++;
		}
	}
}
int RMQ::query(int a, int b)
{
	int min1 = minR[a][_log[b - a]];
	int min2 = minL[b][_log[b - a]];
	if (min1 < min2) {
		return min1;
	}
	return min2;
}
int RMQ::queryIndex(int a, int b)
{
	int min1 = minR[a][_log[b - a]];
	int min2 = minL[b][_log[b - a]];
	if (min1 < min2) {
		return minRIndex[a][_log[b - a]];
	}
	return minLIndex[b][_log[b - a]];
}
RMQ::~RMQ()
{
	for (int i = 0; i < n; i++) {
		free(minR[i]);
		free(minL[i]);
		free(minRIndex[i]);
		free(minLIndex[i]);
	}
	free(minR);
	free(minL);
	free(minRIndex);
	free(minLIndex);
	free(_log);
}

void RMQ2::initialize(int _n, int *A1, int *A2)
{
	n = _n;
	minR = (int **)malloc(sizeof(int *) * n);
	minL = (int **)malloc(sizeof(int *) * n);
	minRIndex = (int **)malloc(sizeof(int *) * n);
	minLIndex = (int **)malloc(sizeof(int *) * n);
	for (int x = 0; x < n; x++) {
		int l = 2;
		int log = 1;
		while (x + l - 1 <= n - 1) {
			l *= 2;
			log++;
		}
		minR[x] = (int *)malloc(sizeof(int) * log * 2);
		minR[x][0] = A1[x];
		minR[x][1] = A2[x];
		minRIndex[x] = (int *)malloc(sizeof(int) * log);
		minRIndex[x][0] = x;

		l = 2;
		log = 1;
		while (x - l + 1 >= 0) {
			l *= 2;
			log++;
		}
		minL[x] = (int *)malloc(sizeof(int) * log * 2);
		minL[x][0] = A1[x];
		minL[x][1] = A2[x];
		minLIndex[x] = (int *)malloc(sizeof(int) * log);
		minLIndex[x][0] = x;
	}
	int log = 1;
	int length = 2;
	while (length - 1 <= n - 1) {
		int x = 0;
		while (x + length - 1 <= n - 1) {
			int min11 = minR[x][2 * (log - 1)];
			int min12 = minR[x][2 * (log - 1) + 1];
			int min21 = minR[x + length / 2][2 * (log - 1)];
			int min22 = minR[x + length / 2][2 * (log - 1) + 1];
			if (min11 < min21 || (min11 == min21 && min12 < min22)) {
				minR[x][2 * log] = min11;
				minR[x][2 * log + 1] = min12;
				minRIndex[x][log] = minRIndex[x][log - 1];
			}
			else {
				minR[x][2 * log] = min21;
				minR[x][2 * log + 1] = min22;
				minRIndex[x][log] = minRIndex[x + length / 2][log - 1];
			}
			x++;
		}
		x = n - 1;
		while (x - length + 1 >= 0) {
			int min11 = minL[x][2 * (log - 1)];
			int min12 = minL[x][2 * (log - 1) + 1];
			int min21 = minL[x - length / 2][2 * (log - 1)];
			int min22 = minL[x - length / 2][2 * (log - 1) + 1];
			if (min11 < min21 || (min11 == min21 && min12 < min22)) {
				minL[x][2 * log] = min11;
				minL[x][2 * log + 1] = min12;
				minLIndex[x][log] = minLIndex[x][log - 1];
			}
			else {
				minL[x][2 * log] = min21;
				minL[x][2 * log + 1] = min22;
				minLIndex[x][log] = minLIndex[x - length / 2][log - 1];
			}
			x--;
		}
		length *= 2;
		log++;
	}
	_log = (int *)malloc(sizeof(int) * n);
	_log[0] = 0;
	for (int i = 1; i < n; i++) {
		int l = 2;
		_log[i] = 0;
		while (l - 1 <= i) {
			l *= 2;
			_log[i]++;
		}
	}
}
void RMQ2::query(int a, int b, int *w1, int *w2)
{
	int min11 = minR[a][2 * _log[b - a]];
	int min12 = minR[a][2 * _log[b - a] + 1];
	int min21 = minL[b][2 * _log[b - a]];
	int min22 = minL[b][2 * _log[b - a] + 1];
	if (min11 < min21 || (min11 == min21 && min12 < min22)) {
		*w1 = min11;
		*w2 = min12;
	}
	else {
		*w1 = min21;
		*w2 = min22;
	}
}
int RMQ2::queryIndex(int a, int b)
{
	int min11 = minR[a][2 * _log[b - a]];
	int min12 = minR[a][2 * _log[b - a] + 1];
	int min21 = minL[b][2 * _log[b - a]];
	int min22 = minL[b][2 * _log[b - a] + 1];
	if (min11 < min21 || (min11 == min21 && min12 < min22)) {
		return minRIndex[a][_log[b - a]];
	}
	return minLIndex[b][_log[b - a]];
}
RMQ2::~RMQ2()
{
	for (int i = 0; i < n; i++) {
		free(minR[i]);
		free(minL[i]);
		free(minRIndex[i]);
		free(minLIndex[i]);
	}
	free(minR);
	free(minL);
	free(minRIndex);
	free(minLIndex);
	free(_log);
}

void NCA::initialize(int n, int *tree, int r)
{
	dfs = (int *)malloc(sizeof(int) * (n + 1));
	ET = (int *)malloc(sizeof(int) * (2 * n - 1));
	iET = (int *)malloc(sizeof(int) * (2 * n - 1));
	index = (int *)malloc(sizeof(int) * (n + 1));
	int *height = (int *)malloc(sizeof(int) * (n + 1));
	int *child;
	int *firstChild;
	construct_tree(n, tree, r, &child, &firstChild);
	int *temp_vertex = (int *)malloc(sizeof(int) * n);
	int *temp_out = (int *)malloc(sizeof(int) * n);
	int num = 0;
	dfs[r] = num++;
	height[r] = 0;
	int currentIndex = 0;
	index[r] = 0;
	temp_vertex[0] = r;
	temp_out[0] = firstChild[r];
	int SP = 0;
	while (SP != -1) {
		int v = temp_vertex[SP];
		ET[currentIndex] = height[v];
		iET[currentIndex++] = v;
		if (temp_out[SP] < firstChild[v + 1]) {
			int c = child[temp_out[SP]++];
			dfs[c] = num++;
			height[c] = height[v] + 1;
			index[c] = currentIndex;
			temp_vertex[SP + 1] = c;
			temp_out[SP + 1] = firstChild[c];
			SP++;
			continue;
		}
		SP--;
	}
	rmq = new RMQ();
	rmq->initialize(2 * n - 1, ET);
	free(height);
	free(child);
	free(firstChild);
	free(temp_vertex);
	free(temp_out);
}
int NCA::query(int x, int y)
{
	if (dfs[x] > dfs[y]) {
		return query(y, x);
	}
	return iET[rmq->queryIndex(index[x], index[y])];
}
NCA::~NCA()
{
	free(dfs);
	free(ET);
	free(iET);
	free(index);
	delete rmq;
}

void TreeRMQ::initialize(int n, int *tree, int r, int *w)
{
	treeCopy = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		treeCopy[i] = tree[i];
	}
	wCopy = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		wCopy[i] = w[i];
	}
	int *child;
	int *firstChild;
	construct_tree(n, tree, r, &child, &firstChild);
	int *dfs = (int *)malloc(sizeof(int) * (n + 1));
	int *idfs = (int *)malloc(sizeof(int) * n);
	int *p = (int *)malloc(sizeof(int) * (n + 1));
	int *temp_vertex = (int *)malloc(sizeof(int) * n);
	int *temp_out = (int *)malloc(sizeof(int) * n);
	int num = 0;
	dfs[r] = num;
	idfs[num++] = r;
	p[r] = 0;
	temp_vertex[0] = r;
	temp_out[0] = firstChild[r];
	int SP = 0;
	while (SP != -1) {
		int v = temp_vertex[SP];
		if (temp_out[SP] < firstChild[v + 1]) {
			int c = child[temp_out[SP]++];
			dfs[c] = num;
			idfs[num++] = c;
			p[c] = v;
			temp_vertex[SP + 1] = c;
			temp_out[SP + 1] = firstChild[c];
			SP++;
			continue;
		}
		SP--;
	}
	int *ND = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		ND[i] = 1;
	}
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		ND[p[v]] += ND[v];
	}
	is_in_path = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		is_in_path[i] = 0;
	}
	parentEdgeIsHeavy = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		parentEdgeIsHeavy[i] = 0;
	}
	for (int d = 1; d < n; d++) {
		int v = idfs[d];
		if (ND[v] > ND[p[v]] / 2) {
			is_in_path[v] = 1;
			is_in_path[p[v]] = 1;
			parentEdgeIsHeavy[v] = 1;
		}
	}
	int pathCount = 0;
	char *processed = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		processed[i] = 0;
	}
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		if (processed[v] || !parentEdgeIsHeavy[v]) {
			continue;
		}
		processed[v] = 1;
		int u = v;
		while (parentEdgeIsHeavy[u]) {
			u = treeCopy[u];
			processed[u] = 1;
			if (u == r) {
				break;
			}
		}
		pathCount++;
	}

	_pathCount = pathCount;
	invIndex = (int **)malloc(sizeof(int *) * pathCount);
	pathRMQ = new RMQ *[pathCount];
	for (int i = 0; i < pathCount; i++) {
		pathRMQ[i] = new RMQ();
	}
	pathLow = (int *)malloc(sizeof(int) * pathCount);
	pathHigh = (int *)malloc(sizeof(int) * pathCount);
	for (int i = 1; i <= n; i++) {
		processed[i] = 0;
	}
	pathIndex = (int *)malloc(sizeof(int) * (n + 1));
	index = (int *)malloc(sizeof(int) * (n + 1));
	pathCount = 0;
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		if (processed[v] || !parentEdgeIsHeavy[v]) {
			continue;
		}
		processed[v] = 1;
		pathIndex[v] = pathCount;
		int currentIndex = 0;
		index[v] = currentIndex++;
		pathLow[pathCount] = v;
		int u = v;
		while (parentEdgeIsHeavy[u]) {
			u = treeCopy[u];
			processed[u] = 1;
			pathIndex[u] = pathCount;
			index[u] = currentIndex++;
			if (u == r) {
				break;
			}
		}
		pathHigh[pathCount] = u;
		int *array = (int *)malloc(sizeof(int) * currentIndex);
		for (int i = 0; i < currentIndex; i++) {
			array[i] = wCopy[v];
			v = p[v];
		}
		pathRMQ[pathCount]->initialize(currentIndex, array);
		free(array);
		invIndex[pathCount] = (int *)malloc(sizeof(int) * currentIndex);
		v = idfs[d];
		for (int i = 0; i < currentIndex; i++) {
			invIndex[pathCount][i] = v;
			v = p[v];
		}
		pathCount++;
	}
	free(child);
	free(firstChild);
	free(dfs);
	free(idfs);
	free(p);
	free(ND);
	free(temp_vertex);
	free(temp_out);
	free(processed);
}
int TreeRMQ::query(int a, int d)
{
	// we assume that a is an ancestor of d
	int min = wCopy[d];
	while (1) {
		if (a == d) {
			if (wCopy[d] < min) {
				min = wCopy[d];
			}
			break;
		}
		if (!parentEdgeIsHeavy[d]) {
			if (wCopy[d] < min) {
				min = wCopy[d];
			}
			d = treeCopy[d];
			continue;
		}
		if (!is_in_path[a] || pathIndex[a] != pathIndex[d]) {
			int i = pathIndex[d];
			int res = pathRMQ[i]->query(index[d], index[pathHigh[i]]);
			if (res < min) {
				min = res;
			}
			d = treeCopy[pathHigh[i]];
			continue;
		}
		// if(pathIndex[a]==pathIndex[d])
		{
			int res = pathRMQ[pathIndex[a]]->query(index[d], index[a]);
			if (res < min) {
				min = res;
			}
			break;
		}
	}
	return min;
}
int TreeRMQ::queryVertex(int a, int d)
{
	// we assume that a is an ancestor of d
	int min = wCopy[d];
	int minVertex = d;
	while (1) {
		if (a == d) {
			if (wCopy[d] < min) {
				min = wCopy[d];
				minVertex = d;
			}
			break;
		}
		if (!parentEdgeIsHeavy[d]) {
			if (wCopy[d] < min) {
				min = wCopy[d];
				minVertex = d;
			}
			d = treeCopy[d];
			continue;
		}
		if (!is_in_path[a] || pathIndex[a] != pathIndex[d]) {
			int i = pathIndex[d];
			int res = pathRMQ[i]->query(index[d], index[pathHigh[i]]);
			if (res < min) {
				min = res;
				minVertex = invIndex[i][pathRMQ[i]->queryIndex(index[d], index[pathHigh[i]])];
			}
			d = treeCopy[pathHigh[i]];
			continue;
		}
		// if(pathIndex[a]==pathIndex[d])
		{
			int res = pathRMQ[pathIndex[a]]->query(index[d], index[a]);
			if (res < min) {
				min = res;
				int i = pathIndex[a];
				minVertex = invIndex[i][pathRMQ[i]->queryIndex(index[d], index[a])];
			}
			break;
		}
	}
	return minVertex;
}
TreeRMQ::~TreeRMQ()
{
	for (int i = 0; i < _pathCount; i++) {
		delete pathRMQ[i];
	}
	// free(pathRMQ);
	delete[] pathRMQ;
	free(is_in_path);
	free(pathIndex);
	free(index);
	free(pathLow);
	free(pathHigh);
	free(treeCopy);
	free(wCopy);
	free(parentEdgeIsHeavy);
	for (int i = 0; i < _pathCount; i++) {
		free(invIndex[i]);
	}
	free(invIndex);
}

void TreeRMQ2::initialize(int n, int *tree, int r, int *w1, int *w2)
{
	treeCopy = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		treeCopy[i] = tree[i];
	}
	w1Copy = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		w1Copy[i] = w1[i];
	}
	w2Copy = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 0; i <= n; i++) {
		w2Copy[i] = w2[i];
	}
	int *child;
	int *firstChild;
	construct_tree(n, tree, r, &child, &firstChild);
	int *dfs = (int *)malloc(sizeof(int) * (n + 1));
	int *idfs = (int *)malloc(sizeof(int) * n);
	int *p = (int *)malloc(sizeof(int) * (n + 1));
	int *temp_vertex = (int *)malloc(sizeof(int) * n);
	int *temp_out = (int *)malloc(sizeof(int) * n);
	int num = 0;
	dfs[r] = num;
	idfs[num++] = r;
	p[r] = 0;
	temp_vertex[0] = r;
	temp_out[0] = firstChild[r];
	int SP = 0;
	while (SP != -1) {
		int v = temp_vertex[SP];
		if (temp_out[SP] < firstChild[v + 1]) {
			int c = child[temp_out[SP]++];
			dfs[c] = num;
			idfs[num++] = c;
			p[c] = v;
			temp_vertex[SP + 1] = c;
			temp_out[SP + 1] = firstChild[c];
			SP++;
			continue;
		}
		SP--;
	}
	int *ND = (int *)malloc(sizeof(int) * (n + 1));
	for (int i = 1; i <= n; i++) {
		ND[i] = 1;
	}
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		ND[p[v]] += ND[v];
	}
	is_in_path = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		is_in_path[i] = 0;
	}
	parentEdgeIsHeavy = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		parentEdgeIsHeavy[i] = 0;
	}
	for (int d = 1; d < n; d++) {
		int v = idfs[d];
		if (ND[v] > ND[p[v]] / 2) {
			is_in_path[v] = 1;
			is_in_path[p[v]] = 1;
			parentEdgeIsHeavy[v] = 1;
		}
	}
	int pathCount = 0;
	char *processed = (char *)malloc(sizeof(char) * (n + 1));
	for (int i = 1; i <= n; i++) {
		processed[i] = 0;
	}
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		if (processed[v] || !parentEdgeIsHeavy[v]) {
			continue;
		}
		processed[v] = 1;
		int u = v;
		while (parentEdgeIsHeavy[u]) {
			u = treeCopy[u];
			processed[u] = 1;
			if (u == r) {
				break;
			}
		}
		pathCount++;
	}

	_pathCount = pathCount;
	invIndex = (int **)malloc(sizeof(int *) * pathCount);
	pathRMQ2 = new RMQ2 *[pathCount];
	for (int i = 0; i < pathCount; i++) {
		pathRMQ2[i] = new RMQ2();
	}
	pathLow = (int *)malloc(sizeof(int) * pathCount);
	pathHigh = (int *)malloc(sizeof(int) * pathCount);
	for (int i = 1; i <= n; i++) {
		processed[i] = 0;
	}
	pathIndex = (int *)malloc(sizeof(int) * (n + 1));
	index = (int *)malloc(sizeof(int) * (n + 1));
	pathCount = 0;
	for (int d = n - 1; d > 0; d--) {
		int v = idfs[d];
		if (processed[v] || !parentEdgeIsHeavy[v]) {
			continue;
		}
		processed[v] = 1;
		pathIndex[v] = pathCount;
		int currentIndex = 0;
		index[v] = currentIndex++;
		pathLow[pathCount] = v;
		int u = v;
		while (parentEdgeIsHeavy[u]) {
			u = treeCopy[u];
			processed[u] = 1;
			pathIndex[u] = pathCount;
			index[u] = currentIndex++;
			if (u == r) {
				break;
			}
		}
		pathHigh[pathCount] = u;
		int *array1 = (int *)malloc(sizeof(int) * currentIndex);
		int *array2 = (int *)malloc(sizeof(int) * currentIndex);
		for (int i = 0; i < currentIndex; i++) {
			array1[i] = w1Copy[v];
			array2[i] = w2Copy[v];
			v = p[v];
		}
		pathRMQ2[pathCount]->initialize(currentIndex, array1, array2);
		free(array1);
		free(array2);
		invIndex[pathCount] = (int *)malloc(sizeof(int) * currentIndex);
		v = idfs[d];
		for (int i = 0; i < currentIndex; i++) {
			invIndex[pathCount][i] = v;
			v = p[v];
		}
		pathCount++;
	}
	free(child);
	free(firstChild);
	free(dfs);
	free(idfs);
	free(p);
	free(ND);
	free(temp_vertex);
	free(temp_out);
	free(processed);
}
void TreeRMQ2::query(int a, int d, int *min1, int *min2)
{
	// we assume that a is an ancestor of d
	*min1 = w1Copy[d];
	*min2 = w2Copy[d];
	while (1) {
		if (a == d) {
			if (w1Copy[d] < *min1 || (w1Copy[d] == *min1 && w2Copy[d] < *min2)) {
				*min1 = w1Copy[d];
				*min2 = w2Copy[d];
			}
			break;
		}
		if (!parentEdgeIsHeavy[d]) {
			if (w1Copy[d] < *min1 || (w1Copy[d] == *min1 && w2Copy[d] < *min2)) {
				*min1 = w1Copy[d];
				*min2 = w2Copy[d];
			}
			d = treeCopy[d];
			continue;
		}
		if (!is_in_path[a] || pathIndex[a] != pathIndex[d]) {
			int i = pathIndex[d];
			int res1;
			int res2;
			pathRMQ2[i]->query(index[d], index[pathHigh[i]], &res1, &res2);
			if (res1 < *min1 || (res1 == *min1 && res2 < *min2)) {
				*min1 = res1;
				*min2 = res2;
			}
			d = treeCopy[pathHigh[i]];
			continue;
		}
		// if(pathIndex[a]==pathIndex[d])
		{
			int res1;
			int res2;
			pathRMQ2[pathIndex[a]]->query(index[d], index[a], &res1, &res2);
			if (res1 < *min1 || (res1 == *min1 && res2 < *min2)) {
				*min1 = res1;
				*min2 = res2;
			}
			break;
		}
	}
}
int TreeRMQ2::queryVertex(int a, int d)
{
	// we assume that a is an ancestor of d
	int min1 = w1Copy[d];
	int min2 = w2Copy[d];
	int minVertex = d;
	while (1) {
		// printf("here");
		if (a == d) {
			if (w1Copy[d] < min1 || (w1Copy[d] == min1 && w2Copy[d] < min2)) {
				min1 = w1Copy[d];
				min2 = w2Copy[d];
				minVertex = d;
			}
			break;
		}
		if (!parentEdgeIsHeavy[d]) {
			if (w1Copy[d] < min1 || (w1Copy[d] == min1 && w2Copy[d] < min2)) {
				min1 = w1Copy[d];
				min2 = w2Copy[d];
				minVertex = d;
			}
			d = treeCopy[d];
			continue;
		}
		if (!is_in_path[a] || pathIndex[a] != pathIndex[d]) {
			int i = pathIndex[d];
			int res1;
			int res2;
			pathRMQ2[i]->query(index[d], index[pathHigh[i]], &res1, &res2);
			if (res1 < min1 || (res1 == min1 && res2 < min2)) {
				min1 = res1;
				min2 = res2;
				minVertex = invIndex[i][pathRMQ2[i]->queryIndex(index[d], index[pathHigh[i]])];
			}
			d = treeCopy[pathHigh[i]];
			continue;
		}
		// if(pathIndex[a]==pathIndex[d])
		{
			int res1;
			int res2;
			pathRMQ2[pathIndex[a]]->query(index[d], index[a], &res1, &res2);
			if (res1 < min1 || (res1 == min1 && res2 < min2)) {
				min1 = res1;
				min2 = res2;
				int i = pathIndex[a];
				minVertex = invIndex[i][pathRMQ2[i]->queryIndex(index[d], index[a])];
			}
			break;
		}
	}
	return minVertex;
}
TreeRMQ2::~TreeRMQ2()
{
	for (int i = 0; i < _pathCount; i++) {
		delete pathRMQ2[i];
	}
	// free(pathRMQ2);
	delete[] pathRMQ2;
	free(is_in_path);
	free(pathIndex);
	free(index);
	free(pathLow);
	free(pathHigh);
	free(treeCopy);
	free(w1Copy);
	free(w2Copy);
	free(parentEdgeIsHeavy);
	for (int i = 0; i < _pathCount; i++) {
		free(invIndex[i]);
	}
	free(invIndex);
}

} // namespace rmq