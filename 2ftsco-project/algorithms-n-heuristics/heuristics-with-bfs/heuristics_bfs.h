#include "../../choudhary-srcs/choudhary.h"

class simpleBFS {
	int n;
	int *gOut;
	int *firstOut;
	char *found;
	int *Q;

	public:
	void initialize(int _n, int *_gOut, int *_firstOut)
	{
		n = _n;
		gOut = _gOut;
		firstOut = _firstOut;
		found = (char *)malloc(sizeof(char) * (n + 1));
		Q = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found[i] = 0;
		}
	}
	~simpleBFS()
	{
		free(found);
		free(Q);
	}

	public:
	char canReach2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
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
				(*n_edges)++;
				if (u == f1 || u == f2) {
					continue;
				}
				if (!found[u]) {
					Q[last++] = u;
					found[u] = 1;
					if (u == y) {
						foundY = 1;
						break;
					}
				}
			}
		}
		for (int i = 0; i < last; i++) {
			found[Q[i]] = 0;
		}
		*n_vertices = last;
		return foundY;
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		char r1 = canReach2(x, y, f1, f2, n_vertices, n_edges);
		if (!r1) {
			return 0;
		}
		int nv = 0;
		int ne = 0;
		char r2 = canReach2(y, x, f1, f2, &nv, &ne);
		*n_vertices += nv;
		(*n_edges) += ne;
		return r2;
	}
};

class biBFS {
	int n;
	int *gOut;
	int *firstOut;
	int *gOutR;
	int *firstOutR;
	char *found1;
	int *Q1;
	char *found2;
	int *Q2;

	public:
	void initialize(int _n, int *_gOut, int *_firstOut)
	{
		n = _n;
		gOut = _gOut;
		firstOut = _firstOut;
		gOutR = (int *)malloc(sizeof(int) * firstOut[n + 1]);
		firstOutR = (int *)malloc(sizeof(int) * (n + 2));
		for (int i = 1; i <= n + 1; i++) {
			firstOutR[i] = 0;
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				firstOutR[gOut[i] + 1]++;
			}
		}
		for (int i = 2; i <= n + 1; i++) {
			firstOutR[i] += firstOutR[i - 1];
		}
		int *tempOut = (int *)malloc(sizeof(int) * (n + 1));
		for (int i = 1; i <= n; i++) {
			tempOut[i] = firstOutR[i];
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				int y = gOut[i];
				gOutR[tempOut[y]++] = x;
			}
		}
		free(tempOut);

		found1 = (char *)malloc(sizeof(char) * (n + 1));
		Q1 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found1[i] = 0;
		}
		found2 = (char *)malloc(sizeof(char) * (n + 1));
		Q2 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found2[i] = 0;
		}
	}
	~biBFS()
	{
		free(found1);
		free(Q1);
		free(found2);
		free(Q2);
		free(gOutR);
		free(firstOutR);
	}

	public:
	char canReach2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		int first1 = 0;
		int last1 = 0;
		int first2 = 0;
		int last2 = 0;
		Q1[last1++] = x;
		found1[x] = 1;
		int v1 = x;
		Q2[last2++] = y;
		found2[y] = 1;
		int v2 = y;
		int out1 = firstOut[x];
		int out2 = firstOutR[y];
		char dir = 1;
		while (1) {
			if (dir) {
				if (out1 < firstOut[v1 + 1]) {
					(*n_edges)++;
					int u = gOut[out1++];
					if (found2[u]) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2 + 1;
						return 1;
					}
					if (u != f1 && u != f2 && !found1[u]) {
						Q1[last1++] = u;
						found1[u] = 1;
					}
				}
				else {
					first1++;
					if (first1 == last1) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2;
						return 0;
					}
					v1 = Q1[first1];
					out1 = firstOut[v1];
				}
				dir = 0;
			}
			else {
				if (out2 < firstOutR[v2 + 1]) {
					(*n_edges)++;
					int u = gOutR[out2++];
					if (found1[u]) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2 + 1;
						return 1;
					}
					if (u != f1 && u != f2 && !found2[u]) {
						Q2[last2++] = u;
						found2[u] = 1;
					}
				}
				else {
					first2++;
					if (first2 == last2) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2;
						return 0;
					}
					v2 = Q2[first2];
					out2 = firstOutR[v2];
				}
				dir = 1;
			}
		}
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		char r1 = canReach2(x, y, f1, f2, n_vertices, n_edges);
		if (!r1) {
			return 0;
		}
		int nv = 0;
		int ne = 0;
		char r2 = canReach2(y, x, f1, f2, &nv, &ne);
		*n_vertices += nv;
		*n_edges += ne;

		return r2;
	}
};

class qBFS {
	int n;
	int *gOut;
	int *firstOut;
	int *gOutR;
	int *firstOutR;
	char *found1;
	int *Q1;
	char *found2;
	int *Q2;
	char *found3;
	int *Q3;
	char *found4;
	int *Q4;

	public:
	void initialize(int _n, int *_gOut, int *_firstOut)
	{
		n = _n;
		gOut = _gOut;
		firstOut = _firstOut;
		gOutR = (int *)malloc(sizeof(int) * firstOut[n + 1]);
		firstOutR = (int *)malloc(sizeof(int) * (n + 2));
		for (int i = 1; i <= n + 1; i++) {
			firstOutR[i] = 0;
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				firstOutR[gOut[i] + 1]++;
			}
		}
		for (int i = 2; i <= n + 1; i++) {
			firstOutR[i] += firstOutR[i - 1];
		}
		int *tempOut = (int *)malloc(sizeof(int) * (n + 1));
		for (int i = 1; i <= n; i++) {
			tempOut[i] = firstOutR[i];
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				int y = gOut[i];
				gOutR[tempOut[y]++] = x;
			}
		}
		free(tempOut);

		found1 = (char *)malloc(sizeof(char) * (n + 1));
		Q1 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found1[i] = 0;
		}
		found2 = (char *)malloc(sizeof(char) * (n + 1));
		Q2 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found2[i] = 0;
		}
		found3 = (char *)malloc(sizeof(char) * (n + 1));
		Q3 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found3[i] = 0;
		}
		found4 = (char *)malloc(sizeof(char) * (n + 1));
		Q4 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found4[i] = 0;
		}
	}
	~qBFS()
	{
		free(found1);
		free(Q1);
		free(found2);
		free(Q2);
		free(found3);
		free(Q3);
		free(found4);
		free(Q4);
		free(gOutR);
		free(firstOutR);
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		int first1 = 0;
		int last1 = 0;
		int first2 = 0;
		int last2 = 0;
		int first3 = 0;
		int last3 = 0;
		int first4 = 0;
		int last4 = 0;
		Q1[last1++] = x;
		found1[x] = 1;
		int v1 = x;
		Q2[last2++] = y;
		found2[y] = 1;
		int v2 = y;
		Q3[last3++] = y;
		found3[y] = 1;
		int v3 = y;
		Q4[last4++] = x;
		found4[x] = 1;
		int v4 = x;
		int out1 = firstOut[x];
		int out2 = firstOutR[y];
		int out3 = firstOut[y];
		int out4 = firstOutR[x];
		char dir = 1;
		char search = 1;
		while (search) {
			if (dir == 1) {
				if (out1 < firstOut[v1 + 1]) {
					(*n_edges)++;
					int u = gOut[out1++];
					if (u == y || found2[u]) {
						search = 0;
						break;
					}
					if (u != f1 && u != f2 && !found1[u]) {
						Q1[last1++] = u;
						found1[u] = 1;
					}
				}
				else {
					first1++;
					if (first1 == last1) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						for (int i = 0; i < last3; i++) {
							found3[Q3[i]] = 0;
						}
						for (int i = 0; i < last4; i++) {
							found4[Q4[i]] = 0;
						}
						*n_vertices = last1 + last2 + last3 + last4;
						return 0;
					}
					v1 = Q1[first1];
					out1 = firstOut[v1];
				}
				dir = 2;
			}
			else if (dir == 2) {
				if (out2 < firstOutR[v2 + 1]) {
					(*n_edges)++;
					int u = gOutR[out2++];
					if (u == x || found1[u]) {
						search = 0;
						break;
					}
					if (u != f1 && u != f2 && !found2[u]) {
						Q2[last2++] = u;
						found2[u] = 1;
					}
				}
				else {
					first2++;
					if (first2 == last2) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						for (int i = 0; i < last3; i++) {
							found3[Q3[i]] = 0;
						}
						for (int i = 0; i < last4; i++) {
							found4[Q4[i]] = 0;
						}
						*n_vertices = last1 + last2 + last3 + last4;
						return 0;
					}
					v2 = Q2[first2];
					out2 = firstOutR[v2];
				}
				dir = 3;
			}
			else if (dir == 3) {
				if (out3 < firstOut[v3 + 1]) {
					(*n_edges)++;
					int u = gOut[out3++];
					if (u == x || found4[u]) {
						search = 0;
						break;
					}
					if (u != f1 && u != f2 && !found3[u]) {
						Q3[last3++] = u;
						found3[u] = 1;
					}
				}
				else {
					first3++;
					if (first3 == last3) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						for (int i = 0; i < last3; i++) {
							found3[Q3[i]] = 0;
						}
						for (int i = 0; i < last4; i++) {
							found4[Q4[i]] = 0;
						}
						*n_vertices = last1 + last2 + last3 + last4;
						return 0;
					}
					v3 = Q3[first3];
					out3 = firstOut[v3];
				}
				dir = 4;
			}
			else {
				if (out4 < firstOutR[v4 + 1]) {
					(*n_edges)++;
					int u = gOutR[out4++];
					if (u == y || found3[u]) {
						search = 0;
						break;
					}
					if (u != f1 && u != f2 && !found4[u]) {
						Q4[last4++] = u;
						found4[u] = 1;
					}
				}
				else {
					first4++;
					if (first4 == last4) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						for (int i = 0; i < last3; i++) {
							found3[Q3[i]] = 0;
						}
						for (int i = 0; i < last4; i++) {
							found4[Q4[i]] = 0;
						}
						*n_vertices = last1 + last2 + last3 + last4;
						return 0;
					}
					v4 = Q4[first4];
					out4 = firstOutR[v4];
				}
				dir = 1;
			}
		}
		if (dir == 1 || dir == 2) {
			dir = 3;
			while (1) {
				if (dir == 3) {
					if (out3 < firstOut[v3 + 1]) {
						(*n_edges)++;
						int u = gOut[out3++];
						if (u == x || found4[u]) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 2;
							return 1;
						}
						if (u != f1 && u != f2 && !found3[u]) {
							Q3[last3++] = u;
							found3[u] = 1;
						}
					}
					else {
						first3++;
						if (first3 == last3) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 1;
							return 0;
						}
						v3 = Q3[first3];
						out3 = firstOut[v3];
					}
					dir = 4;
				}
				else {
					if (out4 < firstOutR[v4 + 1]) {
						(*n_edges)++;
						int u = gOutR[out4++];
						if (u == y || found3[u]) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 2;
							return 1;
						}
						if (u != f1 && u != f2 && !found4[u]) {
							Q4[last4++] = u;
							found4[u] = 1;
						}
					}
					else {
						first4++;
						if (first4 == last4) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 1;
							return 0;
						}
						v4 = Q4[first4];
						out4 = firstOutR[v4];
					}
					dir = 3;
				}
			}
		}
		else {
			dir = 1;
			while (1) {
				if (dir == 1) {
					if (out1 < firstOut[v1 + 1]) {
						(*n_edges)++;
						int u = gOut[out1++];
						if (u == y || found2[u]) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 2;
							return 1;
						}
						if (u != f1 && u != f2 && !found1[u]) {
							Q1[last1++] = u;
							found1[u] = 1;
						}
					}
					else {
						first1++;
						if (first1 == last1) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 1;
							return 0;
						}
						v1 = Q1[first1];
						out1 = firstOut[v1];
					}
					dir = 2;
				}
				else {
					if (out2 < firstOutR[v2 + 1]) {
						(*n_edges)++;
						int u = gOutR[out2++];
						if (u == x || found1[u]) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 2;
							return 1;
						}
						if (u != f1 && u != f2 && !found2[u]) {
							Q2[last2++] = u;
							found2[u] = 1;
						}
					}
					else {
						first2++;
						if (first2 == last2) {
							for (int i = 0; i < last1; i++) {
								found1[Q1[i]] = 0;
							}
							for (int i = 0; i < last2; i++) {
								found2[Q2[i]] = 0;
							}
							for (int i = 0; i < last3; i++) {
								found3[Q3[i]] = 0;
							}
							for (int i = 0; i < last4; i++) {
								found4[Q4[i]] = 0;
							}
							*n_vertices = last1 + last2 + last3 + last4 + 1;
							return 0;
						}
						v2 = Q2[first2];
						out2 = firstOutR[v2];
					}
					dir = 1;
				}
			}
		}
	}
};

class simple_sBFS {
	int n;
	int *gOut;
	int *firstOut;
	char *found;
	int *Q;

	public:
	int N;
	char *is_seed;
	int *map;
	int **sNum;
	int **sND;

	public:
	void initialize(int _n, int *_gOut, int *_firstOut, int _N, char maxDeg)
	{
		n = _n;
		gOut = _gOut;
		firstOut = _firstOut;
		found = (char *)malloc(sizeof(char) * (n + 1));
		Q = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found[i] = 0;
		}

		N = _N;
		is_seed = (char *)malloc(sizeof(char) * (n + 1));
		for (int i = 1; i <= n; i++) {
			is_seed[i] = 0;
		}
		map = (int *)malloc(sizeof(int) * (n + 1));
		if (!maxDeg) {
			int *vertices = (int *)malloc(sizeof(int) * n);
			for (int i = 0; i < n; i++) {
				vertices[i] = i + 1;
			}
			int index = 0;
			for (int t = 0; t < N; t++) {
				int i = t + rand() % (n - t);
				int v = vertices[i];
				is_seed[v] = 1;
				map[v] = index++;
				int temp = vertices[i];
				vertices[i] = vertices[t];
				vertices[t] = temp;
			}
			free(vertices);
		}

		sNum = (int **)malloc(sizeof(int *) * N);
		sND = (int **)malloc(sizeof(int *) * N);
		char *tempFound = (char *)malloc(sizeof(char) * (n + 1));
		int *tempQ = (int *)malloc(sizeof(int) * n);
		int *idfs = (int *)malloc(sizeof(int) * (n + 1));
		int *temp_vertex = (int *)malloc(sizeof(int) * n);
		int *temp_out = (int *)malloc(sizeof(int) * n);
		int *p = (int *)malloc(sizeof(int) * (n + 1));
		for (int r = 1; r <= n; r++) {
			if (!is_seed[r]) {
				continue;
			}
			for (int i = 1; i <= n; i++) {
				tempFound[i] = 0;
			}
			int first = 0;
			int last = 0;
			for (int i = 1; i <= n; i++) {
				p[i] = 0;
			}
			tempQ[last++] = r;
			tempFound[r] = 1;
			while (first != last) {
				int v = tempQ[first++];
				for (int i = firstOut[v]; i < firstOut[v + 1]; i++) {
					int u = gOut[i];
					if (!tempFound[u]) {
						tempQ[last++] = u;
						tempFound[u] = 1;
						p[u] = v;
					}
				}
			}
			int index = map[r];
			sNum[index] = (int *)malloc(sizeof(int) * (n + 1));
			for (int i = 1; i <= n; i++) {
				sNum[index][i] = 0;
			}
			int num = 1;
			sNum[index][r] = num;
			idfs[num++] = r;
			temp_vertex[0] = r;
			temp_out[0] = firstOut[r];
			int SP = 0;
			while (SP != -1) {
				int v = temp_vertex[SP];
				char descend = 0;
				for (int i = temp_out[SP]; i < firstOut[v + 1]; i++) {
					int u = gOut[i];
					if (sNum[index][u] == 0 && p[u] == v) {
						sNum[index][u] = num;
						idfs[num++] = u;
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
			sND[index] = (int *)malloc(sizeof(int) * (n + 1));
			for (int i = 1; i <= n; i++) {
				sND[index][i] = 1;
			}
			for (int i = num - 1; i >= 1; i--) {
				int v = idfs[i];
				sND[index][p[v]] += sND[index][v];
			}
		}
		free(tempFound);
		free(tempQ);
		free(idfs);
		free(temp_vertex);
		free(temp_out);
		free(p);
	}
	~simple_sBFS()
	{
		free(found);
		free(Q);
		free(is_seed);
		free(map);
		for (int i = 0; i < N; i++) {
			free(sNum[i]);
			free(sND[i]);
		}
		free(sNum);
		free(sND);
	}

	public:
	char canReach2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
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
				(*n_edges)++;
				if (u == f1 || u == f2) {
					continue;
				}
				if (!found[u]) {
					if (is_seed[u]) {
						int index = map[u];
						if (sNum[index][y] != 0 &&
							(sNum[index][f1] == 0 || sNum[index][f1] > sNum[index][y] || sNum[index][y] >= sNum[index][f1] + sND[index][f1]) &&
							(sNum[index][f2] == 0 || sNum[index][f2] > sNum[index][y] || sNum[index][y] >= sNum[index][f2] + sND[index][f2])) {

							for (int i = 0; i < last; i++) {
								found[Q[i]] = 0;
							}
							*n_vertices = last + 1;
							return 1;
						}
					}
					Q[last++] = u;
					found[u] = 1;
					if (u == y) {
						foundY = 1;
						break;
					}
				}
			}
		}
		for (int i = 0; i < last; i++) {
			found[Q[i]] = 0;
		}
		*n_vertices = last;
		return foundY;
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		char r1 = canReach2(x, y, f1, f2, n_vertices, n_edges);
		if (!r1) {
			return 0;
		}
		int nv = 0;
		int ne = 0;
		char r2 = canReach2(y, x, f1, f2, &nv, &ne);
		*n_vertices += nv;
		(*n_edges) += ne;
		return r2;
	}
};

class simple_sBFS_CH {
	int n;
	int *gOut;
	int *firstOut;
	char *found;
	int *Q;

	public:
	int N;
	char *is_seed;
	int *map;
	Choudhary **CH;

	public:
	void initialize(cndp::common::DimacsGraph *G, int _N, char maxDeg)
	{
		n = G->vertex_size;
		int m = G->edge_size;
		gOut = (int *)malloc(sizeof(int) * m);
		firstOut = (int *)malloc(sizeof(int) * (n + 2));
		for (int i = 0; i < m; i++) {
			gOut[i] = G->out_edges[i];
		}
		for (int i = 1; i <= n + 1; i++) {
			firstOut[i] = G->out_edges_start_index[i];
		}

		found = (char *)malloc(sizeof(char) * (n + 1));
		Q = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found[i] = 0;
		}

		N = _N;
		is_seed = (char *)malloc(sizeof(char) * (n + 1));
		for (int i = 1; i <= n; i++) {
			is_seed[i] = 0;
		}
		map = (int *)malloc(sizeof(int) * (n + 1));
		if (!maxDeg) {
			int *vertices = (int *)malloc(sizeof(int) * n);
			for (int i = 0; i < n; i++) {
				vertices[i] = i + 1;
			}
			int index = 0;
			for (int t = 0; t < N; t++) {
				int i = t + rand() % (n - t);
				int v = vertices[i];
				is_seed[v] = 1;
				map[v] = index++;
				int temp = vertices[i];
				vertices[i] = vertices[t];
				vertices[t] = temp;
			}
			free(vertices);
		}

		CH = (Choudhary **)malloc(sizeof(Choudhary *) * N);
		for (int v = 1; v <= n; v++) {
			if (!is_seed[v]) {
				continue;
			}
			CH[map[v]] = new Choudhary(G, v);
		}
	}
	~simple_sBFS_CH()
	{
		free(found);
		free(Q);
		free(is_seed);
		free(map);
		for (int i = 0; i < N; i++) {
			delete CH[i];
		}
		free(CH);
	}

	public:
	char canReach2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
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
				(*n_edges)++;
				if (u == f1 || u == f2) {
					continue;
				}
				if (!found[u]) {
					Q[last++] = u;
					found[u] = 1;
					if (u == y) {
						foundY = 1;
						break;
					}
				}
			}
		}
		for (int i = 0; i < last; i++) {
			found[Q[i]] = 0;
		}
		*n_vertices = last;
		return foundY;
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		if (is_seed[x]) {
			return CH[map[x]]->stronglyConnected(y, f1, f2);
		}
		if (is_seed[y]) {
			return CH[map[y]]->stronglyConnected(x, f1, f2);
		}

		char XreachY = 0;
		char YreachX = 0;
		for (int i = 0; i < N; i++) {
			int res1 = CH[i]->reachabilityQuery(x, f1, f2);
			int res2 = CH[i]->reachabilityQuery(y, f1, f2);
			if (res1 != res2) {
				return 0;
			}
			int res3 = CH[i]->objReverseReach->reachabilityQuery(x, f1, f2);
			int res4 = CH[i]->objReverseReach->reachabilityQuery(y, f1, f2);
			if (res3 != res4) {
				return 0;
			}
			XreachY |= res3 && res2;
			YreachX |= res4 && res1;
			if (XreachY && YreachX) {
				return 1;
			}
		}

		char r1 = XreachY;
		if (!r1) {
			r1 = canReach2(x, y, f1, f2, n_vertices, n_edges);
		}
		if (!r1) {
			return 0;
		}
		int nv = 0;
		int ne = 0;
		char r2 = YreachX;
		if (!r2) {
			r2 = canReach2(y, x, f1, f2, &nv, &ne);
		}
		*n_vertices += nv;
		(*n_edges) += ne;
		return r2;
	}
};

class bi_sBFS_CH {
	int n;
	int *gOut;
	int *firstOut;
	int *gOutR;
	int *firstOutR;
	char *found1;
	int *Q1;
	char *found2;
	int *Q2;
	int N;
	char *is_seed;
	int *map;
	Choudhary **CH;

	public:
	void initialize(cndp::common::DimacsGraph *G, int _N, char maxDeg)
	{
		n = G->vertex_size;
		int m = G->edge_size;
		gOut = (int *)malloc(sizeof(int) * m);
		firstOut = (int *)malloc(sizeof(int) * (n + 2));
		for (int i = 0; i < m; i++) {
			gOut[i] = G->out_edges[i];
		}
		for (int i = 1; i <= n + 1; i++) {
			firstOut[i] = G->out_edges_start_index[i];
		}

		gOutR = (int *)malloc(sizeof(int) * firstOut[n + 1]);
		firstOutR = (int *)malloc(sizeof(int) * (n + 2));
		for (int i = 1; i <= n + 1; i++) {
			firstOutR[i] = 0;
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				firstOutR[gOut[i] + 1]++;
			}
		}
		for (int i = 2; i <= n + 1; i++) {
			firstOutR[i] += firstOutR[i - 1];
		}
		int *tempOut = (int *)malloc(sizeof(int) * (n + 1));
		for (int i = 1; i <= n; i++) {
			tempOut[i] = firstOutR[i];
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				int y = gOut[i];
				gOutR[tempOut[y]++] = x;
			}
		}
		free(tempOut);

		found1 = (char *)malloc(sizeof(char) * (n + 1));
		Q1 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found1[i] = 0;
		}
		found2 = (char *)malloc(sizeof(char) * (n + 1));
		Q2 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found2[i] = 0;
		}

		N = _N;
		is_seed = (char *)malloc(sizeof(char) * (n + 1));
		for (int i = 1; i <= n; i++) {
			is_seed[i] = 0;
		}
		map = (int *)malloc(sizeof(int) * (n + 1));
		if (!maxDeg) {
			int *vertices = (int *)malloc(sizeof(int) * n);
			for (int i = 0; i < n; i++) {
				vertices[i] = i + 1;
			}
			int index = 0;
			for (int t = 0; t < N; t++) {
				int i = t + rand() % (n - t);
				int v = vertices[i];
				is_seed[v] = 1;
				map[v] = index++;
				int temp = vertices[i];
				vertices[i] = vertices[t];
				vertices[t] = temp;
			}
			free(vertices);
		}

		CH = (Choudhary **)malloc(sizeof(Choudhary *) * N);
		for (int v = 1; v <= n; v++) {
			if (!is_seed[v]) {
				continue;
			}
			CH[map[v]] = new Choudhary(G, v);
		}
	}
	~bi_sBFS_CH()
	{
		free(found1);
		free(Q1);
		free(found2);
		free(Q2);
		free(gOutR);
		free(firstOutR);
		free(is_seed);
		free(map);
		for (int i = 0; i < N; i++) {
			delete CH[i];
		}
		free(CH);
	}

	public:
	char canReach2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		int first1 = 0;
		int last1 = 0;
		int first2 = 0;
		int last2 = 0;
		Q1[last1++] = x;
		found1[x] = 1;
		int v1 = x;
		Q2[last2++] = y;
		found2[y] = 1;
		int v2 = y;
		int out1 = firstOut[x];
		int out2 = firstOutR[y];
		char dir = 1;
		while (1) {
			if (dir) {
				if (out1 < firstOut[v1 + 1]) {
					(*n_edges)++;
					int u = gOut[out1++];
					if (u == y || found2[u]) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2 + 1;
						return 1;
					}
					if (u != f1 && u != f2 && !found1[u]) {
						Q1[last1++] = u;
						found1[u] = 1;
					}
				}
				else {
					first1++;
					if (first1 == last1) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2;
						return 0;
					}
					v1 = Q1[first1];
					out1 = firstOut[v1];
				}
				dir = 0;
			}
			else {
				if (out2 < firstOutR[v2 + 1]) {
					(*n_edges)++;
					int u = gOutR[out2++];
					if (u == x || found1[u]) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2 + 1;
						return 1;
					}
					if (u != f1 && u != f2 && !found2[u]) {
						Q2[last2++] = u;
						found2[u] = 1;
					}
				}
				else {
					first2++;
					if (first2 == last2) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2;
						return 0;
					}
					v2 = Q2[first2];
					out2 = firstOutR[v2];
				}
				dir = 1;
			}
		}
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		if (is_seed[x]) {
			return CH[map[x]]->stronglyConnected(y, f1, f2);
		}
		if (is_seed[y]) {
			return CH[map[y]]->stronglyConnected(x, f1, f2);
		}

		char XreachY = 0;
		char YreachX = 0;
		for (int i = 0; i < N; i++) {
			int res1 = CH[i]->reachabilityQuery(x, f1, f2);
			int res2 = CH[i]->reachabilityQuery(y, f1, f2);
			if (res1 != res2) {
				return 0;
			}
			int res3 = CH[i]->objReverseReach->reachabilityQuery(x, f1, f2);
			int res4 = CH[i]->objReverseReach->reachabilityQuery(y, f1, f2);
			if (res3 != res4) {
				return 0;
			}
			XreachY |= res3 && res2;
			YreachX |= res4 && res1;
			if (XreachY && YreachX) {
				return 1;
			}
		}

		char r1 = XreachY;
		if (!r1) {
			r1 = canReach2(x, y, f1, f2, n_vertices, n_edges);
		}
		if (!r1) {
			return 0;
		}
		int nv = 0;
		int ne = 0;
		char r2 = YreachX;
		if (!r2) {
			r2 = canReach2(y, x, f1, f2, &nv, &ne);
		}
		*n_vertices += nv;
		(*n_edges) += ne;
		return r2;
	}
};

class bi_sBFS {
	int n;
	int *gOut;
	int *firstOut;
	int *gOutR;
	int *firstOutR;
	char *found1;
	int *Q1;
	char *found2;
	int *Q2;
	int N;
	char *is_seed;
	int *map;
	int **sNum;
	int **sND;
	int **sNumR;
	int **sNDR;
	/**/ int *seedArray;

	public:
	void initialize(int _n, int *_gOut, int *_firstOut, int _N, char maxDeg)
	{
		n = _n;
		gOut = _gOut;
		firstOut = _firstOut;
		gOutR = (int *)malloc(sizeof(int) * firstOut[n + 1]);
		firstOutR = (int *)malloc(sizeof(int) * (n + 2));
		for (int i = 1; i <= n + 1; i++) {
			firstOutR[i] = 0;
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				firstOutR[gOut[i] + 1]++;
			}
		}
		for (int i = 2; i <= n + 1; i++) {
			firstOutR[i] += firstOutR[i - 1];
		}
		int *tempOut = (int *)malloc(sizeof(int) * (n + 1));
		for (int i = 1; i <= n; i++) {
			tempOut[i] = firstOutR[i];
		}
		for (int x = 1; x <= n; x++) {
			for (int i = firstOut[x]; i < firstOut[x + 1]; i++) {
				int y = gOut[i];
				gOutR[tempOut[y]++] = x;
			}
		}
		free(tempOut);

		found1 = (char *)malloc(sizeof(char) * (n + 1));
		Q1 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found1[i] = 0;
		}
		found2 = (char *)malloc(sizeof(char) * (n + 1));
		Q2 = (int *)malloc(sizeof(int) * n);
		for (int i = 1; i <= n; i++) {
			found2[i] = 0;
		}

		N = _N; /**/
		seedArray = (int *)malloc(sizeof(int) * N);
		is_seed = (char *)malloc(sizeof(char) * (n + 1));
		for (int i = 1; i <= n; i++) {
			is_seed[i] = 0;
		}
		map = (int *)malloc(sizeof(int) * (n + 1));
		if (!maxDeg) {
			int *vertices = (int *)malloc(sizeof(int) * n);
			for (int i = 0; i < n; i++) {
				vertices[i] = i + 1;
			}
			int index = 0;
			for (int t = 0; t < N; t++) {
				int i = t + rand() % (n - t);
				int v = vertices[i]; /**/
				seedArray[index] = v;
				is_seed[v] = 1;
				map[v] = index++;
				int temp = vertices[i];
				vertices[i] = vertices[t];
				vertices[t] = temp;
			}
			free(vertices);
		}

		sNum = (int **)malloc(sizeof(int *) * N);
		sND = (int **)malloc(sizeof(int *) * N);
		char *tempFound = (char *)malloc(sizeof(char) * (n + 1));
		int *tempQ = (int *)malloc(sizeof(int) * n);
		int *idfs = (int *)malloc(sizeof(int) * (n + 1));
		int *temp_vertex = (int *)malloc(sizeof(int) * n);
		int *temp_out = (int *)malloc(sizeof(int) * n);
		int *p = (int *)malloc(sizeof(int) * (n + 1));
		for (int r = 1; r <= n; r++) {
			if (!is_seed[r]) {
				continue;
			}
			for (int i = 1; i <= n; i++) {
				tempFound[i] = 0;
			}
			int first = 0;
			int last = 0;
			for (int i = 1; i <= n; i++) {
				p[i] = 0;
			}
			tempQ[last++] = r;
			tempFound[r] = 1;
			while (first != last) {
				int v = tempQ[first++];
				for (int i = firstOut[v]; i < firstOut[v + 1]; i++) {
					int u = gOut[i];
					if (!tempFound[u]) {
						tempQ[last++] = u;
						tempFound[u] = 1;
						p[u] = v;
					}
				}
			}
			int index = map[r];
			sNum[index] = (int *)malloc(sizeof(int) * (n + 1));
			for (int i = 1; i <= n; i++) {
				sNum[index][i] = 0;
			}
			int num = 1;
			sNum[index][r] = num;
			idfs[num++] = r;
			temp_vertex[0] = r;
			temp_out[0] = firstOut[r];
			int SP = 0;
			while (SP != -1) {
				int v = temp_vertex[SP];
				char descend = 0;
				for (int i = temp_out[SP]; i < firstOut[v + 1]; i++) {
					int u = gOut[i];
					if (sNum[index][u] == 0 && p[u] == v) {
						sNum[index][u] = num;
						idfs[num++] = u;
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
			sND[index] = (int *)malloc(sizeof(int) * (n + 1));
			for (int i = 1; i <= n; i++) {
				sND[index][i] = 1;
			}
			for (int i = num - 1; i >= 1; i--) {
				int v = idfs[i];
				sND[index][p[v]] += sND[index][v];
			}
		}

		sNumR = (int **)malloc(sizeof(int *) * N);
		sNDR = (int **)malloc(sizeof(int *) * N);
		for (int r = 1; r <= n; r++) {
			if (!is_seed[r]) {
				continue;
			}
			for (int i = 1; i <= n; i++) {
				tempFound[i] = 0;
			}
			int first = 0;
			int last = 0;
			for (int i = 1; i <= n; i++) {
				p[i] = 0;
			}
			tempQ[last++] = r;
			tempFound[r] = 1;
			while (first != last) {
				int v = tempQ[first++];
				for (int i = firstOutR[v]; i < firstOutR[v + 1]; i++) {
					int u = gOutR[i];
					if (!tempFound[u]) {
						tempQ[last++] = u;
						tempFound[u] = 1;
						p[u] = v;
					}
				}
			}
			int index = map[r];
			sNumR[index] = (int *)malloc(sizeof(int) * (n + 1));
			for (int i = 1; i <= n; i++) {
				sNumR[index][i] = 0;
			}
			int num = 1;
			sNumR[index][r] = num;
			idfs[num++] = r;
			temp_vertex[0] = r;
			temp_out[0] = firstOutR[r];
			int SP = 0;
			while (SP != -1) {
				int v = temp_vertex[SP];
				char descend = 0;
				for (int i = temp_out[SP]; i < firstOutR[v + 1]; i++) {
					int u = gOutR[i];
					if (sNumR[index][u] == 0 && p[u] == v) {
						sNumR[index][u] = num;
						idfs[num++] = u;
						temp_vertex[SP + 1] = u;
						temp_out[SP + 1] = firstOutR[u];
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
			sNDR[index] = (int *)malloc(sizeof(int) * (n + 1));
			for (int i = 1; i <= n; i++) {
				sNDR[index][i] = 1;
			}
			for (int i = num - 1; i >= 1; i--) {
				int v = idfs[i];
				sNDR[index][p[v]] += sNDR[index][v];
			}
		}
		free(tempFound);
		free(tempQ);
		free(idfs);
		free(temp_vertex);
		free(temp_out);
		free(p);
	}
	~bi_sBFS()
	{
		free(found1);
		free(Q1);
		free(found2);
		free(Q2);
		free(gOutR);
		free(firstOutR);
		free(is_seed);
		free(map);
		for (int i = 0; i < N; i++) {
			free(sNum[i]);
			free(sND[i]);
			free(sNumR[i]);
			free(sNDR[i]);
		}
		free(sNum);
		free(sND);
		free(sNumR);
		free(sNDR); /**/
		free(seedArray);
	}

	public:
	char canReach2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		/**/
		for (int i = 0; i < N; i++) {
			if (seedArray[i] == f1 || seedArray[i] == f2) {
				continue;
			}
			if (seedArray[i] == x) {
				if (!(sNum[i][f1] <= sNum[i][y] && sNum[i][y] < sNum[i][f1] + sND[i][f1]) &&
					!(sNum[i][f2] <= sNum[i][y] && sNum[i][y] < sNum[i][f2] + sND[i][f2])) {
					if (sNum[i][f1] == 0 || sNum[i][f2] == 0 || sNum[i][y] == 0) {
						printf("error\n");
					}
					return 1;
				}
			}
			else if (seedArray[i] == y) {
				if (!(sNumR[i][f1] <= sNumR[i][x] && sNumR[i][x] < sNumR[i][f1] + sNDR[i][f1]) &&
					!(sNumR[i][f2] <= sNumR[i][x] && sNumR[i][x] < sNumR[i][f2] + sNDR[i][f2])) {
					return 1;
				}
			}
			else {
				if (!(sNumR[i][f1] <= sNumR[i][x] && sNumR[i][x] < sNumR[i][f1] + sNDR[i][f1]) &&
					!(sNumR[i][f2] <= sNumR[i][x] && sNumR[i][x] < sNumR[i][f2] + sNDR[i][f2]) &&
					!(sNum[i][f1] <= sNum[i][y] && sNum[i][y] < sNum[i][f1] + sND[i][f1]) &&
					!(sNum[i][f2] <= sNum[i][y] && sNum[i][y] < sNum[i][f2] + sND[i][f2])) {
					return 1;
				}
			}
		}
		/**/
		int first1 = 0;
		int last1 = 0;
		int first2 = 0;
		int last2 = 0;
		Q1[last1++] = x;
		found1[x] = 1;
		int v1 = x;
		Q2[last2++] = y;
		found2[y] = 1;
		int v2 = y;
		int out1 = firstOut[x];
		int out2 = firstOutR[y];
		char dir = 1;
		while (1) {
			if (dir) {
				if (out1 < firstOut[v1 + 1]) {
					(*n_edges)++;
					int u = gOut[out1++];
					if (u == y || found2[u]) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2 + 1;
						return 1;
					}
					if (u != f1 && u != f2 && !found1[u]) {
						if (is_seed[u]) {
							int index = map[u];
							if (sNum[index][y] != 0 &&
								(sNum[index][f1] == 0 || sNum[index][f1] > sNum[index][y] ||
								 sNum[index][y] >= sNum[index][f1] + sND[index][f1]) &&
								(sNum[index][f2] == 0 || sNum[index][f2] > sNum[index][y] ||
								 sNum[index][y] >= sNum[index][f2] + sND[index][f2])) {

								for (int i = 0; i < last1; i++) {
									found1[Q1[i]] = 0;
								}
								for (int i = 0; i < last2; i++) {
									found2[Q2[i]] = 0;
								}
								*n_vertices = last1 + last2 + 1;
								return 1;
							}
						}
						Q1[last1++] = u;
						found1[u] = 1;
					}
				}
				else {
					first1++;
					if (first1 == last1) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2;
						return 0;
					}
					v1 = Q1[first1];
					out1 = firstOut[v1];
				}
				dir = 0;
			}
			else {
				if (out2 < firstOutR[v2 + 1]) {
					(*n_edges)++;
					int u = gOutR[out2++];
					if (u == x || found1[u]) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2 + 1;
						return 1;
					}
					if (u != f1 && u != f2 && !found2[u]) {
						if (is_seed[u]) {
							int index = map[u];
							if (sNumR[index][x] != 0 &&
								(sNumR[index][f1] == 0 || sNumR[index][f1] > sNumR[index][x] ||
								 sNumR[index][x] >= sNumR[index][f1] + sNDR[index][f1]) &&
								(sNumR[index][f2] == 0 || sNumR[index][f2] > sNumR[index][x] ||
								 sNumR[index][x] >= sNumR[index][f2] + sNDR[index][f2])) {

								for (int i = 0; i < last1; i++) {
									found1[Q1[i]] = 0;
								}
								for (int i = 0; i < last2; i++) {
									found2[Q2[i]] = 0;
								}
								*n_vertices = last1 + last2 + 1;
								return 1;
							}
						}
						Q2[last2++] = u;
						found2[u] = 1;
					}
				}
				else {
					first2++;
					if (first2 == last2) {
						for (int i = 0; i < last1; i++) {
							found1[Q1[i]] = 0;
						}
						for (int i = 0; i < last2; i++) {
							found2[Q2[i]] = 0;
						}
						*n_vertices = last1 + last2;
						return 0;
					}
					v2 = Q2[first2];
					out2 = firstOutR[v2];
				}
				dir = 1;
			}
		}
	}

	public:
	char areSC2(int x, int y, int f1, int f2, int *n_vertices, int *n_edges)
	{
		*n_vertices = 0;
		*n_edges = 0;
		if (x == y) {
			return 1;
		}
		char r1 = canReach2(x, y, f1, f2, n_vertices, n_edges);
		if (!r1) {
			return 0;
		}
		int nv = 0;
		int ne = 0;
		char r2 = canReach2(y, x, f1, f2, &nv, &ne);
		*n_vertices += nv;
		*n_edges += ne;
		return r2;
	}
};