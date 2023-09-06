#ifndef RMQ_DS
#define RMQ_DS
namespace rmq {

void construct_tree(int n, int *p, int r, int **_child, int **_firstChild);

class ChildQueries {
	private:
	int *dfs;
	int *ND;
	int *child;
	int *firstChild;
	int query(int d, int i, int j);

	public:
	char is_ancestor(int a, int d);
	void initialize(int n, int *tree, int r);
	int query(int a, int d);
	~ChildQueries();
};

class RMQ {
	private:
	int n;
	int **minR;
	int **minL;
	int **minRIndex;
	int **minLIndex;
	int *_log;

	public:
	void initialize(int _n, int *A);
	int query(int a, int b);
	int queryIndex(int a, int b);
	~RMQ();
};

class RMQ2 {
	private:
	int n;
	int **minR;
	int **minL;
	int **minRIndex;
	int **minLIndex;
	int *_log;

	public:
	void initialize(int _n, int *A1, int *A2);
	void query(int a, int b, int *w1, int *w2);
	int queryIndex(int a, int b);
	~RMQ2();
};

class NCA {
	private:
	int *dfs;
	int *ET;
	int *iET;
	int *index;
	RMQ *rmq;

	public:
	void initialize(int n, int *tree, int r);
	int query(int x, int y);
	~NCA();
};

class TreeRMQ {
	private:
	RMQ **pathRMQ;
	char *is_in_path;
	int *pathIndex;
	int *index;
	int *pathLow;
	int *pathHigh;
	int *treeCopy;
	int *wCopy;
	char *parentEdgeIsHeavy;
	int _pathCount;
	int **invIndex;

	public:
	void initialize(int n, int *tree, int r, int *w);
	int query(int a, int d);
	int queryVertex(int a, int d);
	~TreeRMQ();
};

class TreeRMQ2 {
	private:
	RMQ2 **pathRMQ2;
	char *is_in_path;
	int *pathIndex;
	int *index;
	int *pathLow;
	int *pathHigh;
	int *treeCopy;
	int *w1Copy;
	int *w2Copy;
	char *parentEdgeIsHeavy;
	int _pathCount;
	int **invIndex;

	public:
	void initialize(int n, int *tree, int r, int *w1, int *w2);
	void query(int a, int d, int *min1, int *min2);
	int queryVertex(int a, int d);
	~TreeRMQ2();
};
} // namespace rmq
#endif