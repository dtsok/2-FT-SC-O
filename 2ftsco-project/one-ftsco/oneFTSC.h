#ifndef ONE_FTSCO_
#define ONE_FTSCO_

#include "../rmq-srcs/RMQ.h"

namespace rmq {

void get_reverse(int, int *, int *, int **, int **);
void get_dominators(int, int *, int *, int, int **);
void COMPRESS(int, int *, int *, int *, int *);
void DFS(int, int *, int *, int, int **, int **, int **);

void get_h(int, int *, int *, int *, int *, int *, int **);
int find(int *, int);
void unite(int *, int *, int, int);

class oneFTSC_naive {
	public:
	int *gOut;
	int *firstOut;
	int *Q;
	char *found;
	oneFTSC_naive(int n, int *_gOut, int *_firstOut);
	~oneFTSC_naive();
	char query(int x, int y, int f);
};

class oneFTSC {
	private:
	NCA *ncaH;
	NCA *ncaHR;
	ChildQueries *ancD;
	ChildQueries *ancDR;

	public:
	oneFTSC(int n, int *gOut, int *firstOut);
	char query(int x, int y, int f);
	~oneFTSC();
};

} // namespace rmq

#endif