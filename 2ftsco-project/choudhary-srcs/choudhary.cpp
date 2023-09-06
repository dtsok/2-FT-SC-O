#include "choudhary.h"
#include <fstream>
#include <sstream>
#include <stack>

Choudhary::Choudhary()
{
	/* public constructor */
}

Choudhary::Choudhary(cndp::common::DimacsGraph *graph, int root)
{
	initializeDataStructure(graph, root);
	initializeReverseDataStructure(graph, root);
	deleteAuxArrays();
	objReverseReach->deleteAuxArrays();
}

Choudhary::Choudhary(cndp::common::DimacsGraph *graph, int root, string dir, int ID)
{
	initializeDataStructureForWriting(graph, root);

	graph->SetReverseGraph();
	this->objReverseReach = new Choudhary();
	this->objReverseReach->initializeDataStructureForWriting(graph, root);
	graph->SetReverseGraph();

	writeToFile("choudhary_data_" + dir, ID, graph->vertex_size);
	objReverseReach->writeToFile("choudhary_data_rev_" + dir, ID, graph->vertex_size);

	deleteAuxArrays();
	objReverseReach->deleteAuxArrays();
}

Choudhary::~Choudhary()
{
	delete[] idom;
	delete[] blue;
	delete[] red;

	delete[] arrayDepthsBlue;
	delete[] arrayDepthsRed;

	delete[] arrayIdomDepthsBlue;
	delete[] arrayIdomDepthsRed;

	delete[] depthOfDetoursRED;
	delete[] depthOfDetoursBLUE;

	delete[] depthOfAD_bb;
	delete[] depthOfAD_br;
	delete[] depthOfAD_rb;
	delete[] depthOfAD_rr;

	delete nextVertexBlue;
	delete nextVertexRed;

	if (write_to_file || !init_from_file) {
		if (firstVerticesRED != nullptr) {
			delete[] firstVerticesRED;
		}
		if (firstVerticesBLUE != nullptr) {
			delete[] firstVerticesBLUE;
		}

		if (firstVerticesAD_bb != nullptr) {
			delete[] firstVerticesAD_bb;
		}
		if (firstVerticesAD_br != nullptr) {
			delete[] firstVerticesAD_br;
		}
		if (firstVerticesAD_rb != nullptr) {
			delete[] firstVerticesAD_rb;
		}
		if (firstVerticesAD_rr != nullptr) {
			delete[] firstVerticesAD_rr;
		}
		if (pseudoParentsBlue != nullptr) {
			delete[] pseudoParentsBlue;
		}
		if (pseudoParentsRed != nullptr) {
			delete[] pseudoParentsRed;
		}
	}

	if (!write_to_file) {
		delete ncaObj;
		delete nextDominatorObj;
		delete obj_x1;
		delete obj_x2;
		delete obj_w1;
		delete obj_w2;
		delete obj_wbb;
		delete obj_wbr;
		delete obj_wrb;
		delete obj_wrr;
	}
}

void Choudhary::deleteAuxArrays()
{
	/* we only need the depth of these vertices */
	delete[] firstVerticesRED;
	firstVerticesRED = nullptr;
	delete[] firstVerticesBLUE;
	firstVerticesBLUE = nullptr;
	delete[] firstVerticesAD_bb;
	firstVerticesAD_bb = nullptr;
	delete[] firstVerticesAD_br;
	firstVerticesAD_br = nullptr;
	delete[] firstVerticesAD_rb;
	firstVerticesAD_rb = nullptr;
	delete[] firstVerticesAD_rr;
	firstVerticesAD_rr = nullptr;
	delete[] pseudoParentsBlue;
	pseudoParentsBlue = nullptr;
	delete[] pseudoParentsRed;
	pseudoParentsRed = nullptr;
}

void Choudhary::initializeFromFile(string dir, int ID)
{
	init_from_file = true;
	std::ifstream input(dir + "/outputFile_" + std::to_string(ID) + ".txt");
	string line, token;
	while (getline(input, line)) {
		std::istringstream iss(line);
		iss >> token; // first word of line
		if (token == "ID") {
			iss >> token;
			if (std::stoi(token) != ID) {
				printf("\nERROR @ ID\n");
			}
		}
		else if (token == "r") {
			iss >> token;
			r = std::stoi(token);
			iss >> token;
			nr = std::stoi(token);
		}
		else if (token == "blue") {
			blue = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				blue[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "red") {
			red = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				red[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "idom") {
			idom = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				idom[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "arrayDepthsBlue") {
			arrayDepthsBlue = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				arrayDepthsBlue[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "arrayDepthsRed") {
			arrayDepthsRed = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				arrayDepthsRed[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "arrayIdomDepthsBlue") {
			arrayIdomDepthsBlue = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				arrayIdomDepthsBlue[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "arrayIdomDepthsRed") {
			arrayIdomDepthsRed = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				arrayIdomDepthsRed[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "depthOfDetoursBLUE") {
			depthOfDetoursBLUE = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				depthOfDetoursBLUE[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "depthOfDetoursRED") {
			depthOfDetoursRED = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				depthOfDetoursRED[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "depthOfAD_bb") {
			depthOfAD_bb = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				depthOfAD_bb[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "depthOfAD_br") {
			depthOfAD_br = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				depthOfAD_br[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "depthOfAD_rb") {
			depthOfAD_rb = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				depthOfAD_rb[i] = std::stoi(token);
				iss >> token;
			}
		}
		else if (token == "depthOfAD_rr") {
			depthOfAD_rr = new int[nr + 1];
			iss >> token;
			for (int i = 0; i <= nr; i++) {
				depthOfAD_rr[i] = std::stoi(token);
				iss >> token;
			}
		}
	}
	input.close();

	nextVertexBlue = new ChildQueries;
	nextVertexBlue->initialize(nr, this->blue, this->r);

	nextVertexRed = new ChildQueries;
	nextVertexRed->initialize(nr, this->red, this->r);

	ncaObj = new NCA;
	ncaObj->initialize(nr, this->idom, this->r);

	nextDominatorObj = new ChildQueries;
	nextDominatorObj->initialize(nr, this->idom, this->r);

	obj_x1 = new TreeRMQ2;
	obj_x1->initialize(nr, this->blue, this->r, this->arrayIdomDepthsBlue, this->arrayDepthsBlue);

	obj_x2 = new TreeRMQ2;
	obj_x2->initialize(nr, this->red, this->r, this->arrayIdomDepthsRed, this->arrayDepthsRed);

	obj_w1 = new TreeRMQ;
	obj_w1->initialize(nr, this->blue, this->r, this->arrayIdomDepthsBlue);

	obj_w2 = new TreeRMQ;
	obj_w2->initialize(nr, this->red, this->r, this->arrayIdomDepthsRed);

	obj_wbb = new TreeRMQ2;
	obj_wbb->initialize(nr, this->blue, this->r, this->arrayIdomDepthsBlue, this->depthOfAD_bb);

	obj_wbr = new TreeRMQ2;
	obj_wbr->initialize(nr, this->red, this->r, this->arrayIdomDepthsBlue, this->depthOfAD_br);

	obj_wrb = new TreeRMQ2;
	obj_wrb->initialize(nr, this->blue, this->r, this->arrayIdomDepthsRed, this->depthOfAD_rb);

	obj_wrr = new TreeRMQ2;
	obj_wrr->initialize(nr, this->red, this->r, this->arrayIdomDepthsRed, this->depthOfAD_rr);
}

void Choudhary::writeToFile(string dir, int ID, int graph_size)
{
	system(("mkdir -p " + dir + "/").c_str());
	std::ofstream output(dir + "/outputFile_" + std::to_string(ID) + ".txt"); //, std::ios_base::app);
	output << "ID " << std::to_string(ID) << "\n";
	output << "r " << std::to_string(r) << " " << std::to_string(nr) << " " << std::to_string(graph_size) << "\n";
	output << "blue ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(blue[i]) << " ";
	}
	output << "\nred ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(red[i]) << " ";
	}
	output << "\nidom ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(idom[i]) << " ";
	}
	output << "\narrayDepthsBlue ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(arrayDepthsBlue[i]) << " ";
	}
	output << "\narrayDepthsRed ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(arrayDepthsRed[i]) << " ";
	}
	output << "\narrayIdomDepthsBlue ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(arrayIdomDepthsBlue[i]) << " ";
	}
	output << "\narrayIdomDepthsRed ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(arrayIdomDepthsRed[i]) << " ";
	}
	output << "\ndepthOfDetoursBLUE ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(depthOfDetoursBLUE[i]) << " ";
	}
	output << "\ndepthOfDetoursRED ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(depthOfDetoursRED[i]) << " ";
	}
	output << "\ndepthOfAD_bb ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(depthOfAD_bb[i]) << " ";
	}
	output << "\ndepthOfAD_br ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(depthOfAD_br[i]) << " ";
	}
	output << "\ndepthOfAD_rb ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(depthOfAD_rb[i]) << " ";
	}
	output << "\ndepthOfAD_rr ";
	for (int i = 0; i <= nr; i++) {
		output << std::to_string(depthOfAD_rr[i]) << " ";
	}
	output << "\n";
	output.close();
}

void Choudhary::initializeDataStructureForWriting(cndp::common::DimacsGraph *graph, int root)
{
	write_to_file = true;
	/* compute blue/red tree */
	compute_dst(graph, root);

	/* find the depth of the nodes in the trees */
	findDepthOfBlueRed(graph);

	/* find depth of immediate dominators - metric a() */
	findIdomDepths(graph);

	nextVertexBlue = new ChildQueries;
	nextVertexBlue->initialize(graph->vertex_size, this->blue, this->r);

	nextVertexRed = new ChildQueries;
	nextVertexRed->initialize(graph->vertex_size, this->red, this->r);

	/* find detours Di(v) - metric b() */
	findDetours(graph);

	/* find ancestor-detours ADij(v) - metric c() */
	findAncestorsDetours(graph);
}

void Choudhary::initializeDataStructure(cndp::common::DimacsGraph *graph, int root)
{
	/* compute blue/red tree */
	compute_dst(graph, root);

	/* find the depth of the nodes in the trees */
	findDepthOfBlueRed(graph);

	/* find depth of immediate dominators - metric alpha() */
	findIdomDepths(graph);

	/* init childQueries - used for the ancestor checking */
	nextVertexBlue = new ChildQueries;
	nextVertexBlue->initialize(graph->vertex_size, this->blue, this->r);

	nextVertexRed = new ChildQueries;
	nextVertexRed->initialize(graph->vertex_size, this->red, this->r);

	/* find detours Di(v) - metric beta() */
	findDetours(graph);

	/* find ancestor-detours ADij(v) - metric gamma() */
	findAncestorsDetours(graph);

	/* initialize auxilary d.s. for the queries */
	initializeRangeMinima(graph);
}

void Choudhary::initializeRangeMinima(cndp::common::DimacsGraph *graph)
{
	ncaObj = new NCA;
	ncaObj->initialize(graph->vertex_size, this->idom, this->r);

	nextDominatorObj = new ChildQueries;
	nextDominatorObj->initialize(graph->vertex_size, this->idom, this->r);

	obj_x1 = new TreeRMQ2;
	obj_x1->initialize(graph->vertex_size, this->blue, this->r, this->arrayIdomDepthsBlue, this->arrayDepthsBlue);

	obj_x2 = new TreeRMQ2;
	obj_x2->initialize(graph->vertex_size, this->red, this->r, this->arrayIdomDepthsRed, this->arrayDepthsRed);

	obj_w1 = new TreeRMQ;
	obj_w1->initialize(graph->vertex_size, this->blue, this->r, this->arrayIdomDepthsBlue);

	obj_w2 = new TreeRMQ;
	obj_w2->initialize(graph->vertex_size, this->red, this->r, this->arrayIdomDepthsRed);

	obj_wbb = new TreeRMQ2;
	obj_wbb->initialize(graph->vertex_size, this->blue, this->r, this->arrayIdomDepthsBlue, this->depthOfAD_bb);

	obj_wbr = new TreeRMQ2;
	obj_wbr->initialize(graph->vertex_size, this->red, this->r, this->arrayIdomDepthsBlue, this->depthOfAD_br);

	obj_wrb = new TreeRMQ2;
	obj_wrb->initialize(graph->vertex_size, this->blue, this->r, this->arrayIdomDepthsRed, this->depthOfAD_rb);

	obj_wrr = new TreeRMQ2;
	obj_wrr->initialize(graph->vertex_size, this->red, this->r, this->arrayIdomDepthsRed, this->depthOfAD_rr);
}

void Choudhary::initializeReverseDataStructure(cndp::common::DimacsGraph *graph, int root)
{
	graph->SetReverseGraph();
	this->objReverseReach = new Choudhary();
	this->objReverseReach->initializeDataStructure(graph, root);
	graph->SetReverseGraph();
}

void Choudhary::printTree(int *p, int n, char c)
{
	fprintf(stdout, "\n");
	for (int i = 0; i <= n; i++) {
		fprintf(stdout, "%c %d %d\n", c, p[i], i);
	}
}

inline void Choudhary::incc()
{
	count++;
}

void Choudhary::initComputation(int N)
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
inline void Choudhary::rcompress(int v, int c)
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

void Choudhary::dfsForConstruction(cndp::common::DimacsGraph *graph, int k)
{
	label2pre[k] = (++next);
	pre2label[next] = k;
	for (int j = graph->out_edges_start_index[k]; j <= graph->out_edges_end_index[k]; j++) {
		int v = graph->out_edges[j];
		incc();
		if (!label2pre[v]) {
			dfsForConstruction(graph, v);
			parent[label2pre[v]] = label2pre[k];
		}
	}
}

void Choudhary::compute_dst(cndp::common::DimacsGraph *graph, int root)
{
	r = root;
	initComputation(graph->vertex_size + 1);

	int i, j;

	// pre-dfs
	next = 0;
	dfsForConstruction(graph, r);
	nr = next;

	// extra arrays used for B-R spanning trees construction
	bool *isred = new bool[nr + 1];
	int *eps = new int[nr + 1];

	red = new int[nr + 1];
	blue = new int[nr + 1];

	for (i = 0; i <= nr; i++) {
		label[i] = semi[i] = eps[i] = blue[i] = i;
		red[i] = parent[i];
		isred[i] = false;
	}
	blue[1] = red[1] = 1;

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
			eps[v] = u;
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
					blue[i] = v;
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

	// construct blue and red spanning trees
	for (i = 2; i <= nr; i++) {
		if ((!isred[eps[i]]) && (semi[eps[i]] < semi[i])) {
			isred[i] = true;
			int temp = red[i];
			red[i] = blue[i];
			blue[i] = temp;
		}
		else if ((blue[i] == dom[i]) || (red[i] == dom[i])) {
			blue[i] = red[i] = dom[i];
		}
	}

	// to store red/blue/dom trees
	// int *tempDom = new int[nr + 1]; // not needed for now
	// int *tempBlue = new int[nr + 1];
	tempBlue = new int[nr + 1];
	// int *tempRed = new int[nr + 1];
	tempRed = new int[nr + 1];
	for (int i = 0; i <= nr; i++) {
		// tempDom[i] = 0;
		tempBlue[i] = tempRed[i] = 0;
	}

	for (int i = 1; i <= nr; i++) {
		// fprintf(stdout, "dom[i] i p2l %d %d %d\n", dom[i], i, pre2label[i]);
		// dom[i] = pre2label[dom[i]];
		blue[i] = pre2label[blue[i]];
		red[i] = pre2label[red[i]];
	}

	for (int i = 1; i <= nr; i++) {
		// fprintf(stdout, "dom[i] i p2l %d %d %d\n", tempDom[i], i, pre2label[i]);
		// tempDom[pre2label[i]] = dom[i];
		tempBlue[pre2label[i]] = blue[i];
		tempRed[pre2label[i]] = red[i];
	}
	blue[0] = red[0] = 0;
	for (int i = 1; i <= nr; i++) {
		blue[i] = tempBlue[i];
		red[i] = tempRed[i];
	}

	// printTree(idom, graph -> vertex_size, 'D');
	// printTree(blue, graph -> vertex_size, 'b');
	// printTree(red, graph -> vertex_size, 'r');

	// delete [] idom;

	// delete [] tempDom;
	delete[] tempBlue;
	delete[] tempRed;

	delete[] dom;
	delete[] parent;
	delete[] semi;
	delete[] label;
	delete[] ubucket;
	delete[] eps;
	delete[] isred;
	delete[] pre2label;
	delete[] label2pre;
}

void Choudhary::recursionForDepth(int *array, int *arrayDepths, bool *traversed, int j)
{
	traversed[j] = true;
	if (j != array[j]) {
		if (!traversed[array[j]]) {
			recursionForDepth(array, arrayDepths, traversed, array[j]);
		}
		else {
			arrayDepths[j] = arrayDepths[array[j]] + 1;
			return;
		}
	}
	arrayDepths[j] = 1 + arrayDepths[array[j]];
}

void Choudhary::findDepthOfBlueRed(cndp::common::DimacsGraph *graph)
{
	this->arrayDepthsBlue = new int[graph->vertex_size + 1];
	this->arrayDepthsRed = new int[graph->vertex_size + 1];

	bool *traversedBlue = new bool[graph->vertex_size + 1];
	bool *traversedRed = new bool[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		traversedBlue[i] = traversedRed[i] = false;
		this->arrayDepthsBlue[i] = this->arrayDepthsRed[i] = -1;
	}

	// for the blue tree
	for (int i = 1; i <= graph->vertex_size; i++) {
		if (traversedBlue[i]) {
			continue;
		}
		recursionForDepth(blue, arrayDepthsBlue, traversedBlue, i);
	}
	// for the red tree
	for (int i = 1; i <= graph->vertex_size; i++) {
		if (traversedRed[i]) {
			continue;
		}
		recursionForDepth(red, arrayDepthsRed, traversedRed, i);
	}

	/*for (int i = graph->vertex_size; i >= 1; i--){
		cout<<"node "<<i<<" depthBlue "<<arrayDepthsBlue[i]<<" depthRed "<<arrayDepthsRed[i]<<endl;
	}*/

	delete[] traversedBlue;
	delete[] traversedRed;
}

void Choudhary::findIdomDepths(cndp::common::DimacsGraph *graph)
{
	this->arrayIdomDepthsBlue = new int[graph->vertex_size + 1];
	this->arrayIdomDepthsRed = new int[graph->vertex_size + 1];

	arrayIdomDepthsBlue[0] = arrayIdomDepthsRed[0] = -1;

	for (int i = 1; i <= graph->vertex_size; i++) {
		arrayIdomDepthsBlue[i] = arrayDepthsBlue[idom[i]];
		arrayIdomDepthsRed[i] = arrayDepthsRed[idom[i]];
	}
}

void Choudhary::dfsForDetour(cndp::common::DimacsGraph *graph, bool *visited, int fromVertex, int startingVertex)
{
	visited[fromVertex] = true;
	for (int j = graph->out_edges_start_index[fromVertex]; j <= graph->out_edges_end_index[fromVertex]; j++) {
		int v = graph->out_edges[j];

		if (nextVertexBlue->is_ancestor(v, startingVertex)) {
			visited[v] = true;
			if (arrayDepthsBlue[v] < depthOfDetoursBLUE[startingVertex]) {
				depthOfDetoursBLUE[startingVertex] = arrayDepthsBlue[v];
				firstVerticesBLUE[startingVertex] = v;
			}
		}
		if (nextVertexRed->is_ancestor(v, startingVertex)) {
			visited[v] = true;
			if (arrayDepthsRed[v] < depthOfDetoursRED[startingVertex]) {
				depthOfDetoursRED[startingVertex] = arrayDepthsRed[v];
				firstVerticesRED[startingVertex] = v;
			}
		}
		if (!visited[v]) {
			dfsForDetour(graph, visited, v, startingVertex);
		}
	}
}

void Choudhary::dfsForDetourPB(cndp::common::DimacsGraph *graph, bool *visited, int fromVertex, int startingVertex)
{
	visited[fromVertex] = true;
	for (int j = graph->out_edges_start_index[fromVertex]; j <= graph->out_edges_end_index[fromVertex]; j++) {
		int v = graph->out_edges[j];

		if (nextVertexBlue->is_ancestor(v, startingVertex) && v != pseudoParentsBlue[startingVertex] &&
			v != startingVertex) {
			// if(nextVertexBlue->is_ancestor(v, startingVertex) && v != pseudoParentsBlue[startingVertex]){
			// if(nextVertexBlue->is_ancestor(v, startingVertex) && v != startingVertex){
			visited[v] = true;
			if (arrayDepthsBlue[v] < depthOfAD_bb[startingVertex]) {
				depthOfAD_bb[startingVertex] = arrayDepthsBlue[v];
				firstVerticesAD_bb[startingVertex] = v;
			}
		}
		if (nextVertexRed->is_ancestor(v, startingVertex) && v != pseudoParentsBlue[startingVertex] &&
			v != startingVertex) {
			// if(nextVertexRed->is_ancestor(v, startingVertex) && v != pseudoParentsBlue[startingVertex]){
			// if(nextVertexRed->is_ancestor(v, startingVertex) && v != startingVertex){
			visited[v] = true;
			if (arrayDepthsRed[v] < depthOfAD_rb[startingVertex]) {
				depthOfAD_rb[startingVertex] = arrayDepthsRed[v];
				firstVerticesAD_rb[startingVertex] = v;
			}
		}
		if (!visited[v]) {
			dfsForDetourPB(graph, visited, v, startingVertex);
		}
	}
}
void Choudhary::dfsForDetourPR(cndp::common::DimacsGraph *graph, bool *visited, int fromVertex, int startingVertex)
{
	visited[fromVertex] = true;
	for (int j = graph->out_edges_start_index[fromVertex]; j <= graph->out_edges_end_index[fromVertex]; j++) {
		int v = graph->out_edges[j];

		if (nextVertexBlue->is_ancestor(v, startingVertex) && v != pseudoParentsRed[startingVertex] &&
			v != startingVertex) {
			// if(nextVertexBlue->is_ancestor(v, startingVertex) && v != pseudoParentsRed[startingVertex]){
			// if(nextVertexBlue->is_ancestor(v, startingVertex) && v != startingVertex){
			visited[v] = true;
			if (arrayDepthsBlue[v] < depthOfAD_br[startingVertex]) {
				depthOfAD_br[startingVertex] = arrayDepthsBlue[v];
				firstVerticesAD_br[startingVertex] = v;
			}
		}
		if (nextVertexRed->is_ancestor(v, startingVertex) && v != pseudoParentsRed[startingVertex] &&
			v != startingVertex) {
			// if(nextVertexRed->is_ancestor(v, startingVertex) && pseudoParentsRed[startingVertex]){
			// if(nextVertexRed->is_ancestor(v, startingVertex) && v != startingVertex){
			visited[v] = true;
			if (arrayDepthsRed[v] < depthOfAD_rr[startingVertex]) {
				depthOfAD_rr[startingVertex] = arrayDepthsRed[v];
				firstVerticesAD_rr[startingVertex] = v;
			}
		}
		if (!visited[v]) {
			dfsForDetourPR(graph, visited, v, startingVertex);
		}
	}
}

void Choudhary::findDetours(cndp::common::DimacsGraph *graph)
{
	// initialize arrays
	this->depthOfDetoursBLUE = new int[graph->vertex_size + 1];
	this->depthOfDetoursRED = new int[graph->vertex_size + 1];
	this->firstVerticesBLUE = new int[graph->vertex_size + 1];
	this->firstVerticesRED = new int[graph->vertex_size + 1];

	graph->SetReverseGraph();

	bool *visited = new bool[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		this->depthOfDetoursBLUE[i] = this->depthOfDetoursRED[i] = this->firstVerticesBLUE[i] =
			this->firstVerticesRED[i] = graph->vertex_size + 1;
		visited[i] = false;
	}

	for (int current = 1; current <= graph->vertex_size; current++) {

		// if current is root continue to the next node
		if (current == this->r) {
			continue;
		}

		dfsForDetour(graph, visited, current, current);

		/* reset arrays for next iteration */
		for (int v = 1; v <= graph->vertex_size; v++) {
			visited[v] = false;
		}
	}
	// reverse graph to initial state
	graph->SetReverseGraph();
	delete[] visited;
}

void Choudhary::findPseudoParents(int total_nodes)
{
	this->pseudoParentsBlue = new int[total_nodes + 1];
	this->pseudoParentsRed = new int[total_nodes + 1];

	// initialize to 0 because not every node has a pseudo-parent
	for (int i = 0; i <= total_nodes; i++) {
		pseudoParentsBlue[i] = pseudoParentsRed[i] = 0;
	}

	for (int current = 1; current <= total_nodes; current++) {
		int tempVertex = current;
		int idom_current = this->idom[current];

		/* while current != root find pseudo parent in the path r -> current
		  pseudo parent of v = nearest ancestor of v in tree T s.t. it has the same immediate dominator as v */

		// for the blue tree
		while (tempVertex != r) {
			tempVertex = this->blue[tempVertex];
			if (idom[tempVertex] == idom_current) {
				pseudoParentsBlue[current] = tempVertex;
				break;
			}
		}

		// for the red tree
		tempVertex = current;
		while (tempVertex != r) {
			tempVertex = this->red[tempVertex];
			if (idom[tempVertex] == idom_current) {
				pseudoParentsRed[current] = tempVertex;
				break;
			}
		}
	}
	/*cout<<"\n\npseudo parents\n";
	for(int i = 1; i <= cc -> GetTotalVertices(); i++){
		int current = cc -> GetVertex(i);
		cout<<"node "<<current<<" blue "<<pseudoParentsBlue[current]<<" red "<<pseudoParentsRed[current]<<endl;
	}*/
}

void Choudhary::findAncestorsDetours(cndp::common::DimacsGraph *graph)
{
	this->depthOfAD_bb = new int[graph->vertex_size + 1];
	this->depthOfAD_br = new int[graph->vertex_size + 1];
	this->depthOfAD_rb = new int[graph->vertex_size + 1];
	this->depthOfAD_rr = new int[graph->vertex_size + 1];

	this->firstVerticesAD_bb = new int[graph->vertex_size + 1];
	this->firstVerticesAD_br = new int[graph->vertex_size + 1];
	this->firstVerticesAD_rb = new int[graph->vertex_size + 1];
	this->firstVerticesAD_rr = new int[graph->vertex_size + 1];

	bool *visited = new bool[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		depthOfAD_bb[i] = depthOfAD_br[i] = depthOfAD_rb[i] = depthOfAD_rr[i] = graph->vertex_size + 1;
		firstVerticesAD_bb[i] = firstVerticesAD_br[i] = firstVerticesAD_rb[i] = firstVerticesAD_rr[i] =
			graph->vertex_size + 1;
		visited[i] = false;
	}

	// find pseudo parents
	this->findPseudoParents(graph->vertex_size);

	// reverse graph
	graph->SetReverseGraph();

	for (int current = 1; current <= graph->vertex_size; current++) {

		// if current is root continue to next node
		if (current == this->r) {
			continue;
		}

		/* search detours towards pseudo parent of current in blue/red tree
		dfs from pseudo_blue in the reverse => vertices reaching pseudo_blue in original graph
		from them select the highest ancestors and update the arrays. So here we have the cases
		1) blue -> blue ancestor detour
		2) red -> blue ancestor detour
		then reset the visited array and do dfs with pseudo_red as root and update the corresponding arrays, cases
		3) blue -> red a.d.
		4) red -> red a.d.
		then reset again visited and ancestor2avoid for next node */

		int pseudo_blue = this->pseudoParentsBlue[current];
		/* if pseudo parent of current exists (!=0) start dfs from this node, otherwise do not search for a detour
		 */
		if (pseudo_blue != 0) {
			dfsForDetourPB(graph, visited, pseudo_blue, current);
		}

		int pseudo_red = this->pseudoParentsRed[current];
		if (pseudo_red != 0) {
			// reset visited nodes
			for (int v = 1; v <= graph->vertex_size; v++) {
				visited[v] = false;
			}
			dfsForDetourPR(graph, visited, pseudo_red, current);
		}

		// reset visited nodes
		for (int v = 1; v <= graph->vertex_size; v++) {
			visited[v] = false;
		}
	}

	// reset graph to initial state
	graph->SetReverseGraph();
	delete[] visited;
}

bool Choudhary::isAncestor(char tree, int x, int y)
{
	if (tree == 'b') {
		// if((tree_label2pre_blue[x] <= tree_label2pre_blue[y]) && (tree_label2pre_blue[y] <
		// tree_label2pre_blue[x] + tree_descendant_size_blue[x])) return true; return false;
		return nextVertexBlue->is_ancestor(x, y);
	}
	else if (tree == 'r') {
		// if((tree_label2pre_red[x] <= tree_label2pre_red[y]) && (tree_label2pre_red[y] < tree_label2pre_red[x] +
		// tree_descendant_size_red[x])) return true; return false;
		return nextVertexRed->is_ancestor(x, y);
	}
	cout << "\n\n***Error during the ancestor/descedant checking***\n\n";
	exit(1);
}

bool Choudhary::stronglyConnected(int u, int failed_1, int failed_2)
{
	bool ans1 = reachabilityQuery(u, failed_1, failed_2);
	bool ans2 = objReverseReach->reachabilityQuery(u, failed_1, failed_2);
	return ans1 && ans2;
}

bool Choudhary::reachabilityQuery(int toVertex, int failed_1, int failed_2)
{
	if (toVertex == this->r) {
		return true;
	}
	/*	case 1 - blue tree
		some path is intact either in red or blue spanning tree. if neither of the failed vertices aren't ancestors
	   in blue tree then toVertex is reachable. (isAncestor = true if failed is anc of toV) */
	if (!isAncestor('b', failed_1, toVertex) && !isAncestor('b', failed_2, toVertex)) {
		return true;
	}
	/*	case 1 - red tree */
	if (!isAncestor('r', failed_1, toVertex) && !isAncestor('r', failed_2, toVertex)) {
		return true;
	}

	/* 	case 2 - failed_1
		both trees contain same failed vertex => failed vertex is dominator of toVertex => non reachable (some
	   failed vertex is ancestor of toVertex in both trees) */
	if (isAncestor('b', failed_1, toVertex) && isAncestor('r', failed_1, toVertex)) {
		return false;
	}
	/* 	case 2 - failed_2 */
	if (isAncestor('b', failed_2, toVertex) && isAncestor('r', failed_2, toVertex)) {
		return false;
	}

	/* 	case 3
	if none of the above conditions hold true then we use Choudarys DS
	first we should find the vertex v which is dominator of toVertex s.t. f_i lies in T_i(idom(v), v)
	if they lie between different dominators the answer is true, otherwise we should check for detours */

	if (isAncestor('r', failed_1, toVertex)) {
		int f = failed_1;
		failed_1 = failed_2;
		failed_2 = f;
	}

	// int lca_f1 = lcaNaive(this->idom, this->idomDepths, failed_1, toVertex);
	// int lca_f2 = lcaNaive(this->idom, this->idomDepths, failed_2, toVertex);
	int lca_f1 = ncaObj->query(failed_1, toVertex);
	int lca_f2 = ncaObj->query(failed_2, toVertex);

	/*if(lca_f1 != nca_f1 || lca_f2 != nca_f2){
		cout<<"\nERROR! @ lca\n";
		exit(1);
	}*/

	if (lca_f1 != lca_f2) {
		return true;
	}

	// int v = nextDominator(toVertex, lca_f1, failed_1, failed_2);
	int v = nextDominatorObj->query(lca_f1, toVertex);
	/*if(v != next_v){
		cout<<"\nERROR! @ nextDominator\n";
		exit(1);
	}*/

	toVertex = v;

	/*	step 1: check if detour exists from an ancestor above f. v. */
	if ((this->depthOfDetoursBLUE[toVertex] < this->nr + 1) &&
		(this->depthOfDetoursBLUE[toVertex] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	if ((this->depthOfDetoursRED[toVertex] < this->nr + 1) &&
		(this->depthOfDetoursRED[toVertex] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	/*	step 2: find vertex x1 and x2
		x1 = v. which <a_blue(), depth_blue> on PathBLUE(~f1,v) is min
		x1 = v. which <a_red(), depth_red> on PathBLUE(~f2,v) is min */
	int child_of_f1 = this->nextVertexBlue->query(failed_1, v);
	int child_of_f2 = this->nextVertexRed->query(failed_2, v);

	// int x1 = findXNaive(this->blue, this->arrayIdomDepthsBlue, toVertex, failed_1, this->nr);
	// int x2 = findXNaive(this->red, this->arrayIdomDepthsRed, toVertex, failed_2, this->nr);
	int x1 = findX('b', toVertex, child_of_f1);
	int x2 = findX('r', toVertex, child_of_f2);
	/*if(x1 != x1_new || x2 != x2_new){
		cout<<"\nERROR! @ findX\n";
		exit(1);
	}*/

	/*	step 3: (for loop) find vertex w for which a_blue()/a_red() on PathBLUE(~f1,~x1)/PathBLUE(~f2,~x2) is min
		unlike before (step 2) here w can be absent. this happens when v is child of the failed vertex.

		step 4: check depths a^i(w) < depth_i(fi) */
	// int w = findWNaive(this -> blue, this->arrayIdomDepthsBlue, x1, failed_1, this->nr);
	int w = findWcase2('b', x1, child_of_f1);
	/*if(this->arrayIdomDepthsBlue[w] != this->arrayIdomDepthsBlue[w_new]){
		cout<<"\nERROR! @ comp1 findWcase2\n";
		exit(1);
	}*/
	if ((w != -1) && (this->arrayIdomDepthsBlue[w] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	// w = findWNaive(this -> red, this->arrayIdomDepthsRed, x2, failed_2, this->nr);
	w = findWcase2('r', x2, child_of_f2);
	/*if(this->arrayIdomDepthsRed[w] != this->arrayIdomDepthsRed[w_new]){
		cout<<"\nERROR! @ comp2 findWcase2\n";
		exit(1);
	}*/
	if ((w != -1) && (this->arrayIdomDepthsRed[w] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	/*	step 5: (2nd for loop, 4 cases, bb, br, rb, rr)
		find vertex w for which <a_i(), c^i_j())> on Path_j(~xj,v) is min

		step 6: check depths c^i_j(w) < depth_i(fi) */

	int child_of_x1 = x1;
	int child_of_x2 = x2;
	if (x1 != v) {
		child_of_x1 = nextVertexBlue->query(x1, toVertex);
	}
	if (x2 != v) {
		child_of_x2 = nextVertexRed->query(x2, toVertex);
	}

	// blue to blue
	// w = findADNaive(this -> blue, this->arrayIdomDepthsBlue, this->depthOfAD_bb, toVertex, x1, this->nr);
	w = findWcase3('b', 'b', toVertex, x1, child_of_x1);
	/*if((w!=-1) && (depthOfAD_bb[w] != depthOfAD_bb[w_new])){
		cout<<"\nERROR! @ comp1 findWcase3\n";
		exit(1);
	}
	w = w_new;*/
	if ((w != -1) && (this->depthOfAD_bb[w] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	// blue to red
	// w = findADNaive(this -> red, this->arrayIdomDepthsBlue, this->depthOfAD_br, toVertex, x2, this->nr);
	w = findWcase3('b', 'r', toVertex, x2, child_of_x2);
	/*if((w!=-1) && (depthOfAD_br[w] != depthOfAD_br[w_new])){
		cout<<"\nERROR! @ comp2 findWcase3\n";
		exit(1);
	}
	w = w_new;*/
	if ((w != -1) && (this->depthOfAD_br[w] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	// red to red
	// w = findADNaive(this -> red, this->arrayIdomDepthsRed, this->depthOfAD_rr, toVertex, x2, this->nr);
	w = findWcase3('r', 'r', toVertex, x2, child_of_x2);
	/*if((w!=-1) && (depthOfAD_rr[w] != depthOfAD_rr[w_new])){
		cout<<"\nERROR! @ comp3 findWcase3\n";
		exit(1);
	}
	w = w_new;*/
	if ((w != -1) && (this->depthOfAD_rr[w] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	// red to blue
	// w = findADNaive(this -> blue, this->arrayIdomDepthsRed, this->depthOfAD_rb, toVertex, x1, this->nr);
	w = findWcase3('r', 'b', toVertex, x1, child_of_x1);
	/*if((w!=-1) && (depthOfAD_rb[w] != depthOfAD_rb[w_new])){
		cout<<"\nERROR! @ comp4 findWcase3\n";
		exit(1);
	}
	w = w_new;*/
	if ((w != -1) && (this->depthOfAD_rb[w] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	return false;
}

int Choudhary::findX(char tree, int v, int child_f)
{
	if (tree == 'b') {
		if (child_f == v) {
			return v;
		}
		return obj_x1->queryVertex(child_f, v);
	}
	else if (tree == 'r') {
		if (child_f == v) {
			return v;
		}
		return obj_x2->queryVertex(child_f, v);
	}
	cout << "\nERROR! @ findX\n";
	exit(1);
}

int Choudhary::findWcase2(char tree, int v, int child_f)
{
	if (tree == 'b') {
		if (child_f == v) {
			return -1; // we search in the path ~fi,~xi thus if child_f == xi => there is no vertex between fi,xi
		}
		v = this->blue[v]; // go to v's parent
		return obj_w1->queryVertex(child_f, v);
	}
	else if (tree == 'r') {
		if (child_f == v) {
			return -1;
		}
		v = this->red[v];
		return obj_w2->queryVertex(child_f, v);
	}
	cout << "\nERROR! @ findWcase2\n";
	exit(1);
}
int Choudhary::findWcase3(char i, char j, int v, int xj, int child_xj)
{
	if (xj == v) {
		return -1;
	}

	if (i == 'b') {
		if (j == 'b') {
			// if(blue[child_xj] == v)	return v;
			return obj_wbb->queryVertex(child_xj, v);
		}
		else if (j == 'r') {
			// if(red[child_xj] == v)	return v;
			return obj_wbr->queryVertex(child_xj, v);
		}
	}
	else if (i == 'r') {
		if (j == 'b') {
			// if(blue[child_xj] == v)	return v;
			return obj_wrb->queryVertex(child_xj, v);
		}
		else if (j == 'r') {
			// if(red[child_xj] == v)	return v;
			return obj_wrr->queryVertex(child_xj, v);
		}
	}
	cout << "\nERROR! @ findWcase3\n";
	exit(1);
}

/* FOR DEBUGGING/VALIDATING RESULTS ONLY - NON OPTIMAL METHODS */

void Choudhary::nonRecursiveDetours(cndp::common::DimacsGraph *graph)
{
	/*
	delete[] depthOfDetoursBLUE;
	delete[] depthOfDetoursRED;
	delete[] firstVerticesBLUE;
	delete[] firstVerticesRED;
	*/

	this->depthOfDetoursBLUE = new int[graph->vertex_size + 1];
	this->depthOfDetoursRED = new int[graph->vertex_size + 1];
	this->firstVerticesBLUE = new int[graph->vertex_size + 1];
	this->firstVerticesRED = new int[graph->vertex_size + 1];

	graph->SetReverseGraph();

	bool *visited = new bool[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		this->depthOfDetoursBLUE[i] = this->depthOfDetoursRED[i] = this->firstVerticesBLUE[i] =
			this->firstVerticesRED[i] = graph->vertex_size + 1;
		visited[i] = false;
	}

	for (int current = 1; current <= graph->vertex_size; current++) {

		// if current is root continue to the next node
		if (current == this->r) {
			continue;
		}

		int root = current;
		std::stack<int> s;
		s.push(root);
		while (!s.empty()) {
			int u = s.top();
			s.pop();
			if (!visited[u]) {

				visited[u] = true;

				for (int j = graph->out_edges_end_index[u]; j >= graph->out_edges_start_index[u]; j--) {
					int v = graph->out_edges[j];

					if (nextVertexBlue->is_ancestor(v, root)) {
						visited[v] = true;
						if (arrayDepthsBlue[v] < depthOfDetoursBLUE[root]) {
							depthOfDetoursBLUE[root] = arrayDepthsBlue[v];
							firstVerticesBLUE[root] = v;
						}
					}
					if (nextVertexRed->is_ancestor(v, root)) {
						visited[v] = true;
						if (arrayDepthsRed[v] < depthOfDetoursRED[root]) {
							depthOfDetoursRED[root] = arrayDepthsRed[v];
							firstVerticesRED[root] = v;
						}
					}
					if (!visited[v]) {
						s.push(v);
					}
				}
			}
		}

		/* reset arrays for next iteration */
		for (int v = 1; v <= graph->vertex_size; v++) {
			visited[v] = false;
		}
	}
	// reverse graph to initial state
	graph->SetReverseGraph();
	delete[] visited;
}

void Choudhary::dfsForDetourNO(cndp::common::DimacsGraph *graph, bool *ancestors2avoid, bool *visited,
							   int fromVertex)
{
	visited[fromVertex] = true;
	for (int j = graph->out_edges_start_index[fromVertex]; j <= graph->out_edges_end_index[fromVertex]; j++) {
		int v = graph->out_edges[j];
		if (ancestors2avoid[v]) {
			visited[v] = true;
			continue;
		}
		if (!visited[v]) {
			dfsForDetourNO(graph, ancestors2avoid, visited, v);
		}
	}
}

void Choudhary::findDetoursNO(cndp::common::DimacsGraph *graph)
{
	// initialize arrays
	this->depthOfDetoursBLUE = new int[graph->vertex_size + 1];
	this->depthOfDetoursRED = new int[graph->vertex_size + 1];
	this->firstVerticesBLUE = new int[graph->vertex_size + 1];
	this->firstVerticesRED = new int[graph->vertex_size + 1];

	graph->SetReverseGraph();

	bool *visited = new bool[graph->vertex_size + 1];
	bool *ancestors2avoid =
		new bool[graph->vertex_size + 1]; // mark them as explored but do not continue the traversal

	for (int i = 0; i <= graph->vertex_size; i++) {
		this->depthOfDetoursBLUE[i] = this->depthOfDetoursRED[i] = this->firstVerticesBLUE[i] =
			this->firstVerticesRED[i] = graph->vertex_size + 1;
		visited[i] = ancestors2avoid[i] = false;
	}

	for (int current = 1; current <= graph->vertex_size; current++) {

		// if current is root continue to the next node
		if (current == this->r) {
			continue;
		}

		// store path from root to current vertex in blue tree
		int temp_v = current;
		vector<int> tempStackBLUE;
		while (temp_v != r) {
			if (temp_v != current) {
				tempStackBLUE.push_back(temp_v);
				ancestors2avoid[temp_v] = true;
			}
			temp_v = this->blue[temp_v];
		}
		// add root to the stack
		tempStackBLUE.push_back(temp_v);
		ancestors2avoid[temp_v] = true;

		// store path from root to current vertex in red tree
		temp_v = current;
		vector<int> tempStackRED;
		while (temp_v != r) {
			if (temp_v != current) {
				tempStackRED.push_back(temp_v);
				ancestors2avoid[temp_v] = true;
			}
			temp_v = this->red[temp_v];
		}
		// add root to the stack
		tempStackRED.push_back(temp_v);
		ancestors2avoid[temp_v] = true;

		dfsForDetourNO(graph, ancestors2avoid, visited, current);

		// find the heighest in the corresponding tree
		// for the blue tree
		// for (auto it = tempStackBLUE.rbegin(); it != tempStackBLUE.rend(); ++it){
		for (int ancestorV : tempStackBLUE) {
			// int ancestorV = *it;
			// cout<<ancestorV<<endl;
			if (visited[ancestorV]) {
				// if it's not updated before
				if (depthOfDetoursBLUE[current] == graph->vertex_size + 1) {
					depthOfDetoursBLUE[current] = arrayDepthsBlue[ancestorV];
					firstVerticesBLUE[current] = ancestorV;
					// break;
				}
				else if (arrayDepthsBlue[ancestorV] < depthOfDetoursBLUE[current]) {
					depthOfDetoursBLUE[current] = arrayDepthsBlue[ancestorV];
					firstVerticesBLUE[current] = ancestorV;
				}
			}
		}

		// for the red tree
		// for (auto it = tempStackRED.rbegin(); it != tempStackRED.rend(); ++it){
		for (int ancestorV : tempStackRED) {
			// int ancestorV = *it;
			// cout<<ancestorV<<endl;
			if (visited[ancestorV]) {
				// if it's not updated before
				if (depthOfDetoursRED[current] == graph->vertex_size + 1) {
					depthOfDetoursRED[current] = arrayDepthsRed[ancestorV];
					firstVerticesRED[current] = ancestorV;
					// break;
				}
				else if (arrayDepthsRed[ancestorV] < depthOfDetoursRED[current]) {
					depthOfDetoursRED[current] = arrayDepthsRed[ancestorV];
					firstVerticesRED[current] = ancestorV;
				}
			}
		}

		/* reset arrays for next iteration */
		for (int v = 1; v <= graph->vertex_size; v++) {
			ancestors2avoid[v] = visited[v] = false;
		}
	}
	// reverse graph to initial state
	graph->SetReverseGraph();
	delete[] visited;
	delete[] ancestors2avoid;
}

void Choudhary::findAncestorsDetoursNO(cndp::common::DimacsGraph *graph)
{
	this->depthOfAD_bb = new int[graph->vertex_size + 1];
	this->depthOfAD_br = new int[graph->vertex_size + 1];
	this->depthOfAD_rb = new int[graph->vertex_size + 1];
	this->depthOfAD_rr = new int[graph->vertex_size + 1];

	this->firstVerticesAD_bb = new int[graph->vertex_size + 1];
	this->firstVerticesAD_br = new int[graph->vertex_size + 1];
	this->firstVerticesAD_rb = new int[graph->vertex_size + 1];
	this->firstVerticesAD_rr = new int[graph->vertex_size + 1];

	bool *visited = new bool[graph->vertex_size + 1];
	bool *ancestors2avoid = new bool[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		depthOfAD_bb[i] = depthOfAD_br[i] = depthOfAD_rb[i] = depthOfAD_rr[i] = graph->vertex_size + 1;
		firstVerticesAD_bb[i] = firstVerticesAD_br[i] = firstVerticesAD_rb[i] = firstVerticesAD_rr[i] =
			graph->vertex_size + 1;
		visited[i] = ancestors2avoid[i] = false;
	}

	// find pseudo parents
	this->findPseudoParents(graph->vertex_size);

	// reverse graph
	graph->SetReverseGraph();

	for (int current = 1; current <= graph->vertex_size; current++) {

		// if current is root continue to next node
		if (current == this->r) {
			continue;
		}

		// store path from root to current vertex in blue tree
		int temp_v = current;
		vector<int> tempStackBLUE;
		while (temp_v != r) {
			if (temp_v != current) {
				tempStackBLUE.push_back(temp_v);
				ancestors2avoid[temp_v] = true;
			}
			temp_v = this->blue[temp_v];
		}
		// add root to the stack
		tempStackBLUE.push_back(temp_v);
		ancestors2avoid[temp_v] = true;

		// store path from root to current vertex in red tree
		temp_v = current;
		vector<int> tempStackRED;
		while (temp_v != r) {
			if (temp_v != current) {
				tempStackRED.push_back(temp_v);
				ancestors2avoid[temp_v] = true;
			}
			temp_v = this->red[temp_v];
		}
		// add root to the stack
		tempStackRED.push_back(temp_v);
		ancestors2avoid[temp_v] = true;

		/* search detours towards pseudo parent of current in blue/red tree
		dfs from pseudo_blue in the reverse => vertices reaching pseudo_blue in original graph
		from them select the highest ancestors and update the arrays. So here we have the cases
		1) blue -> blue ancestor detour
		2) red -> blue ancestor detour
		then reset the visited array and do dfs with pseudo_red as root and update the corresponding arrays, cases
		3) blue -> red a.d.
		4) red -> red a.d.
		then reset again visited and ancestor2avoid for next node */

		int pseudo_blue = this->pseudoParentsBlue[current];
		/* if pseudo parent of current exists (!=0) start dfs from this node, otherwise do not search for a detour
		 */
		if (pseudo_blue != 0) {
			dfsForDetourNO(graph, ancestors2avoid, visited, pseudo_blue);

			// update arrays
			// blue -> blue
			for (int ancestorV : tempStackBLUE) {
				// avoid pseudo_blue because it's an ancestor but we search a detour for (towards it)
				// if(ancestorV == pseudo_blue) continue;

				if (visited[ancestorV]) {
					// if it's not updated before
					if (depthOfAD_bb[current] == graph->vertex_size + 1) {
						depthOfAD_bb[current] = arrayDepthsBlue[ancestorV];
						firstVerticesAD_bb[current] = ancestorV;
					}
					else if (arrayDepthsBlue[ancestorV] < depthOfAD_bb[current]) {
						depthOfAD_bb[current] = arrayDepthsBlue[ancestorV];
						firstVerticesAD_bb[current] = ancestorV;
					}
				}
			}
			// red -> blue
			for (int ancestorV : tempStackRED) {
				// if(ancestorV == pseudo_blue) continue;
				if (visited[ancestorV]) {
					// if it's not updated before
					if (depthOfAD_rb[current] == graph->vertex_size + 1) {
						depthOfAD_rb[current] = arrayDepthsRed[ancestorV];
						firstVerticesAD_rb[current] = ancestorV;
					}
					else if (arrayDepthsRed[ancestorV] < depthOfAD_rb[current]) {
						depthOfAD_rb[current] = arrayDepthsRed[ancestorV];
						firstVerticesAD_rb[current] = ancestorV;
					}
				}
			}
		}

		int pseudo_red = this->pseudoParentsRed[current];
		if (pseudo_red != 0) {
			// reset visited nodes
			for (int v = 1; v <= graph->vertex_size; v++) {
				visited[v] = false;
			}

			dfsForDetourNO(graph, ancestors2avoid, visited, pseudo_red);

			// update arrays
			// blue -> red
			for (int ancestorV : tempStackBLUE) {
				// if(ancestorV == pseudo_red) continue;
				if (visited[ancestorV]) {
					if (depthOfAD_br[current] == graph->vertex_size + 1) {
						depthOfAD_br[current] = arrayDepthsBlue[ancestorV];
						firstVerticesAD_br[current] = ancestorV;
					}
					else if (arrayDepthsBlue[ancestorV] < depthOfAD_br[current]) {
						depthOfAD_br[current] = arrayDepthsBlue[ancestorV];
						firstVerticesAD_br[current] = ancestorV;
					}
				}
			}
			// red -> red
			for (int ancestorV : tempStackRED) {
				// if(ancestorV == pseudo_red) continue;
				if (visited[ancestorV]) {
					if (depthOfAD_rr[current] == graph->vertex_size + 1) {
						depthOfAD_rr[current] = arrayDepthsRed[ancestorV];
						firstVerticesAD_rr[current] = ancestorV;
					}
					else if (arrayDepthsRed[ancestorV] < depthOfAD_rr[current]) {
						depthOfAD_rr[current] = arrayDepthsRed[ancestorV];
						firstVerticesAD_rr[current] = ancestorV;
					}
				}
			}
		}

		// reset visited nodes
		for (int v = 1; v <= graph->vertex_size; v++) {
			ancestors2avoid[v] = visited[v] = false;
		}
	}

	// reset graph to initial state
	graph->SetReverseGraph();
	delete[] visited;
	delete[] ancestors2avoid;
}

void Choudhary::findDepthInDominatorTree(cndp::common::DimacsGraph *graph, int *idomDepths)
{
	bool *visited = new bool[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		idomDepths[i] = -1;
		visited[i] = false;
	}

	for (int i = 1; i <= graph->vertex_size; i++) {
		if (visited[i]) {
			continue;
		}
		recursionForDepth(idom, idomDepths, visited, i);
	}

	delete[] visited;
}

bool Choudhary::reachabilityQueryNaive(int toVertex, int failed_1, int failed_2)
{
	int *idomDepths = new int[nr + 1]();
	/*	case 1 - blue tree
		some path is intact either in red or blue spanning tree. if neither of the failed vertices aren't ancestors
	   in blue tree then toVertex is reachable. (isAncestor = true if failed is anc of toV) */
	if (!isAncestor('b', failed_1, toVertex) && !isAncestor('b', failed_2, toVertex)) {
		return true;
	}
	/*	case 1 - red tree */
	if (!isAncestor('r', failed_1, toVertex) && !isAncestor('r', failed_2, toVertex)) {
		return true;
	}

	/* 	case 2 - failed_1
		both trees contain same failed vertex => failed vertex is dominator of toVertex => non reachable (some
	   failed vertex is ancestor of toVertex in both trees) */
	if (isAncestor('b', failed_1, toVertex) && isAncestor('r', failed_1, toVertex)) {
		return false;
	}
	/* 	case 2 - failed_2 */
	if (isAncestor('b', failed_2, toVertex) && isAncestor('r', failed_2, toVertex)) {
		return false;
	}

	/* 	case 3
	if none of the above conditions hold true then we use Choudarys DS
	first we should find the vertex v which is dominator of toVertex s.t. f_i lies in T_i(idom(v), v)
	if they lie between different dominators the answer is true, otherwise we should check for detours */

	if (isAncestor('r', failed_1, toVertex)) {
		int f = failed_1;
		failed_1 = failed_2;
		failed_2 = f;
	}

	int lca_f1 = lcaNaive(this->idom, idomDepths, failed_1, toVertex);
	int lca_f2 = lcaNaive(this->idom, idomDepths, failed_2, toVertex);

	if (lca_f1 != lca_f2) {
		return true;
	}

	int v = nextDominatorNaive(toVertex, lca_f1, failed_1, failed_2);

	toVertex = v;

	/*	step 1: check if detour exists from an ancestor above f. v. */
	if ((this->depthOfDetoursBLUE[toVertex] < this->nr + 1) &&
		(this->depthOfDetoursBLUE[toVertex] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	if ((this->depthOfDetoursRED[toVertex] < this->nr + 1) &&
		(this->depthOfDetoursRED[toVertex] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	/*	step 2: find vertex x1 and x2
		x1 = v. which <a_blue(), depth_blue> on PathBLUE(~f1,v) is min
		x1 = v. which <a_red(), depth_red> on PathBLUE(~f2,v) is min */

	int x1 = findXNaive(this->blue, this->arrayIdomDepthsBlue, toVertex, failed_1, this->nr);
	int x2 = findXNaive(this->red, this->arrayIdomDepthsRed, toVertex, failed_2, this->nr);

	/*	step 3: (for loop) find vertex w for which a_blue()/a_red() on PathBLUE(~f1,~x1)/PathBLUE(~f2,~x2) is min
		unlike before (step 2) here w can be absent. this happens when v is child of the failed vertex.

		step 4: check depths a^i(w) < depth_i(fi) */
	int w = findWNaive(this->blue, this->arrayIdomDepthsBlue, x1, failed_1, this->nr);
	if ((w != -1) && (this->arrayIdomDepthsBlue[w] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	w = findWNaive(this->red, this->arrayIdomDepthsRed, x2, failed_2, this->nr);
	if ((w != -1) && (this->arrayIdomDepthsRed[w] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	/*	step 5: (2nd for loop, 4 cases, bb, br, rb, rr)
		find vertex w for which <a_i(), c^i_j())> on Path_j(~xj,v) is min

		step 6: check depths c^i_j(w) < depth_i(fi) */

	// blue to blue
	w = findADNaive(this->blue, this->arrayIdomDepthsBlue, this->depthOfAD_bb, toVertex, x1, this->nr);
	if ((w != -1) && (this->depthOfAD_bb[w] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	// blue to red
	w = findADNaive(this->red, this->arrayIdomDepthsBlue, this->depthOfAD_br, toVertex, x2, this->nr);
	if ((w != -1) && (this->depthOfAD_br[w] < this->arrayDepthsBlue[failed_1])) {
		return true;
	}

	// red to red
	w = findADNaive(this->red, this->arrayIdomDepthsRed, this->depthOfAD_rr, toVertex, x2, this->nr);
	if ((w != -1) && (this->depthOfAD_rr[w] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	// red to blue
	w = findADNaive(this->blue, this->arrayIdomDepthsRed, this->depthOfAD_rb, toVertex, x1, this->nr);
	if ((w != -1) && (this->depthOfAD_rb[w] < this->arrayDepthsRed[failed_2])) {
		return true;
	}

	return false;
}

int Choudhary::findWNaive(int *tree, int *depths, int fromVertex, int upToVertex, int total_nodes)
{
	int min = total_nodes;
	int current = fromVertex;
	int minVertex = fromVertex;

	while (current != upToVertex) {
		// cout<<"\ncur "<<current<<endl;
		if (depths[current] <= min && current != fromVertex) {
			min = depths[current];
			minVertex = current;
		}
		current = tree[current];
	}
	if (minVertex == fromVertex) {
		// cout<<"\n-1\n";
		return -1;
	}
	// cout<<"min "<<min<<" minVertex "<<minVertex<<endl;
	return minVertex;
}

int Choudhary::findXNaive(int *tree, int *depths, int fromVertex, int upToVertex, int total_nodes)
{
	int min = total_nodes;
	int current = fromVertex;
	int minVertex = fromVertex;
	while (current != upToVertex) {
		// cout<<"\ncur "<<current<<endl;
		if (depths[current] <= min) {
			min = depths[current];
			minVertex = current;
		}
		current = tree[current];
	}
	// cout<<"min "<<min<<" minVertex "<<minVertex<<endl;
	return minVertex;
}

int Choudhary::findADNaive(int *tree, int *depths, int *ad_depth, int fromVertex, int upToVertex, int total_nodes)
{
	/* fromVertex = from this vertex up to the failed one (not the from which is the root of the tree) */
	int min = total_nodes;
	int min_ad = total_nodes;
	int current = fromVertex;
	int minVertex = -1;
	vector<int> minVertices;

	// first find min on path
	while (current != upToVertex) {
		// cout<<"vccc "<<current<<endl;
		if (depths[current] <= min) {
			min = depths[current];
			// cout<<"her\n"<<ad_depth[current]<<endl;
			if ((ad_depth[current] < nr + 1) && (ad_depth[current] <= min_ad)) {
				min_ad = ad_depth[current];
				minVertex = current;
			}
		}
		current = tree[current];
	}

	// cout<<"minVertex "<<minVertex<<endl;
	return minVertex;
}

int Choudhary::lcaNaive(int *parent, int *depth, int u, int v)
{
	if (u == v) {
		return u;
	}
	if (u == r || v == r) {
		return r;
	}
	if (parent[u] == parent[v]) {
		return parent[u];
	}
	if (depth[u] > depth[v]) {
		u = parent[u];
	}
	else if (depth[u] <= depth[v]) {
		v = parent[v];
	}
	return lcaNaive(parent, depth, u, v);
}

int Choudhary::nextDominatorNaive(int u, int idomV, int f1, int f2)
{
	vector<int> doms;
	int temp = u;
	while (temp != idomV) {
		doms.push_back(temp);
		temp = this->idom[temp];
	}
	int V = doms.back();
	return V;
}