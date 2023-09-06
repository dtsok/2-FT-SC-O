#include "tree_node.h"
#include "../sccs-tarjan/sccs.h"

TreeNode::TreeNode()
{
	/* public constructor */
}

TreeNode::TreeNode(cndp::common::DimacsGraph *graph)
{
	this->graph = graph;
	this->n_nodes = graph->vertex_size;
	this->n_edges = graph->edge_size;
	this->n_chil = 0;
	this->node_depth = 0;
	this->vertex2children = new int[n_nodes + 1]; // map vertex of graph to the children it belongs
	this->vertexMap = new int[n_nodes + 1];		  // map graph vertex label to children vertex label
	for (int i = 0; i <= n_nodes; i++) {
		vertex2children[i] = vertexMap[i] = 0;
	}
	this->children = new TreeNode *[n_nodes + 1]; // initialize with size n and after the
												  // decomposition shrink the array to n_chil
}

TreeNode::~TreeNode()
{
	if (graph != nullptr) {
		delete graph;
		graph = nullptr;
	}

	if (children != nullptr) {
		delete[] children;
		children = nullptr;
	}

	if (vertex2children != nullptr) {
		delete[] vertex2children;
		vertex2children = nullptr;
	}
	if (vertexMap != nullptr) {
		delete[] vertexMap;
		vertexMap = nullptr;
	}
	if (vertexMapReverse != nullptr) {
		delete[] vertexMapReverse;
		vertexMapReverse = nullptr;
	}

	if (chObj != nullptr) {
		delete chObj->objReverseReach;
		delete chObj;
		chObj = nullptr;
	}

	if (oneFaultOracle != nullptr) {
		delete oneFaultOracle;
		oneFaultOracle = nullptr;
	}

	if (store_ch_for_sep != nullptr) {
		for (int i = 0; i < size_of_separator; i++) {
			delete store_ch_for_sep[i];
		}
		delete[] store_ch_for_sep;
	}

	if (g_out != nullptr) {
		delete[] g_out;
	}

	if (g_first_out != nullptr) {
		delete[] g_first_out;
	}
}

void TreeNode::shrinkChildrenArray()
{
	TreeNode **temp = new TreeNode *[this->n_chil];
	for (int i = 0; i < n_chil; i++) {
		temp[i] = this->children[i];
	}
	delete[] children;
	this->children = temp;
}

void TreeNode::addChild(cndp::common::ConnectedComponent *scc, int c_id)
{
	int *indecies = createIndexOfVertexInCC(this->graph, scc);
	/* first update vertexMap and vertex2children arrays of the parent node */
	for (int i = 1; i <= scc->GetTotalVertices(); i++) {
		int v = scc->GetVertex(i);
		this->vertex2children[v] = c_id; // v belongs to c_id child
		this->vertexMap[v] = i;			 // new label of v (in child(c_id)) = i
										 // printf("%d  ",this->vertexMapReverse[v]);
	}
	cndp::common::DimacsGraph *subgraph = createSubgraph(scc, indecies);
	delete[] indecies;
	TreeNode *child = new TreeNode(subgraph);
	child->node_depth = this->node_depth + 1;
	constructReverseMap(child, scc);
	this->children[c_id] = child;
}

void TreeNode::constructReverseMap(TreeNode *node, cndp::common::ConnectedComponent *parent_scc)
{
	node->vertexMapReverse = new int[node->n_nodes + 1];
	node->vertexMapReverse[0] = 0;
	// printf("\nrevesrse map\n");
	for (int i = 1; i <= parent_scc->GetTotalVertices(); i++) {
		int v = parent_scc->GetVertex(i);
		// node -> vertexMap[i] = v;
		node->vertexMapReverse[i] = this->vertexMapReverse[v];
		// printf("%d ---> %d\n", i, node -> vertexMapReverse[i]);
	}
}

void TreeNode::decomposeNode(bool *separator)
{

	if (this->n_nodes >= 2) {
		
		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(this->graph, separator);

		this->n_chil = sccObj.t_cc;
		// cout<<"\n#ch = "<<n_chil<<"\n\n";
		if (n_chil > 0) {
			// construct child nodes from sccs
			for (int i = 0; i < n_chil; i++) {
				// cout<<"~> ";
				// sccObj.cc_array[i] -> PrintVertices();
				addChild(sccObj.cc_array[i], i);
				// cout<<endl;
			}
			// shrink array from n to n_chil
			shrinkChildrenArray();
			// cout<<"\ndepth = "<<this->node_depth<<"\n";
		}
		else {
			// if there aren't any children free the arrays - no need to keep
			// them
			delete[] vertexMap;
			vertexMap = nullptr;
			// delete[] vertexMapReverse;
			// vertexMapReverse = nullptr;
			delete[] vertex2children;
			vertex2children = nullptr;
			delete[] children;
			children = nullptr;
		}
	}
}

void TreeNode::decomposeNode(bool *separator, int sep_size, int global_id, std::string graph_name)
{

	// if graph has at atleast 2 vertices construct choudhary d.s. and further
	// decompose the node
	if (this->n_nodes >= 2) {
		
		global_ID = global_id;
		removed_separator = true;
		size_of_separator = sep_size;

		/* construct and use whole ds for every node that participates in the separator set */

		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(this->graph, separator);

		this->n_chil = sccObj.t_cc;
		// cout<<"\n#ch = "<<n_chil<<"\n\n";
		if (n_chil > 0) {
			// construct child nodes from sccs
			for (int i = 0; i < n_chil; i++) {
				// cout<<"~> ";
				// sccObj.cc_array[i] -> PrintVertices();
				addChild(sccObj.cc_array[i], i);
				// cout<<endl;
			}
			// shrink array from n to n_chil
			shrinkChildrenArray();
			// cout<<"\ndepth = "<<this->node_depth<<"\n";
		}
		else {
			// if there aren't any children release the arrays - no need to keep
			// them
			delete[] vertexMap;
			vertexMap = nullptr;
			// delete[] vertexMapReverse;
			// vertexMapReverse = nullptr;
			delete[] vertex2children;
			vertex2children = nullptr;
			delete[] children;
			children = nullptr;
		}
	}
}

void TreeNode::decomposeNode(int removeVertex)
{
	// if graph has at atleast 2 vertices construct choudhary d.s. and further
	// decompose the node
	if (this->n_nodes >= 2) {
		
		/* initialize whole Choudhary data structure */
		// this->chObj = new Choudhary(this->graph, removeVertex);

		/* initialize 1-Fault Tolerant Strong Connectivity Oracle */
		// this->oneFaultOracle = new oneFTSC(this->n_nodes, this->graph->out_edges, this->graph->out_edges_start_index);

		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(this->graph, removeVertex);

		this->n_chil = sccObj.t_cc;
		// cout<<"\n#ch = "<<n_chil<<"\n\n";
		if (n_chil > 0) {
			// construct child nodes from sccs
			for (int i = 0; i < n_chil; i++) {
				// cout<<"~> ";
				// sccObj.cc_array[i] -> PrintVertices();
				addChild(sccObj.cc_array[i], i);
				// cout<<endl;
			}
			// shrink array from n to n_chil
			shrinkChildrenArray();
			// cout<<"\ndepth = "<<this->node_depth<<"\n";
		}
		else {
			// if there aren't any children release the arrays - no need to keep
			// them
			delete[] vertexMap;
			vertexMap = nullptr;
			// delete[] vertexMapReverse;
			// vertexMapReverse = nullptr;
			delete[] vertex2children;
			vertex2children = nullptr;
			delete[] children;
			children = nullptr;
		}
	}
}

void TreeNode::decomposeNode(int removeVertex, bool init_aux_ds)
{
	// if graph has at atleast 2 vertices init choudhary d.s. and 1 fault and further
	// decompose the node
	if (this->n_nodes >= 2) {

		if (init_aux_ds) {
			/* initialize whole Choudhary data structure */
			this->chObj = new Choudhary(this->graph, removeVertex);

			/* initialize 1-Fault Tolerant Strong Connectivity Oracle */
			this->oneFaultOracle = new oneFTSC(this->n_nodes, this->graph->out_edges, this->graph->out_edges_start_index);
		}

		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(this->graph, removeVertex);

		this->n_chil = sccObj.t_cc;
		// cout<<"\n#ch = "<<n_chil<<"\n\n";
		if (n_chil > 0) {
			// construct child nodes from sccs
			for (int i = 0; i < n_chil; i++) {
				// cout<<"~> ";
				// sccObj.cc_array[i] -> PrintVertices();
				addChild(sccObj.cc_array[i], i);
				// cout<<endl;
			}
			// shrink array from n to n_chil
			shrinkChildrenArray();
			// cout<<"\ndepth = "<<this->node_depth<<"\n";
		}
		else {
			// if there aren't any children release the arrays - no need to keep
			// them
			delete[] vertexMap;
			vertexMap = nullptr;
			// delete[] vertexMapReverse;
			// vertexMapReverse = nullptr;
			delete[] vertex2children;
			vertex2children = nullptr;
			delete[] children;
			children = nullptr;
		}
	}
}

void TreeNode::decomposeNode(int removeVertex, int global_id, string graph_name)
{

	// if graph has at atleast 2 vertices construct choudhary d.s. and further
	// decompose the node
	if (this->n_nodes >= 2) {
		// store choudhary dst for current node - graph has vertices from 1 to
		// size_scc
		global_ID = global_id;

		/* construct and use whole ds */
		// this->chObj = new Choudhary(this->graph, removeVertex);

		/* write to file the ds */
		// this->chObj = new Choudhary(this->graph, removeVertex, graph_name, global_ID);

		/* read from file the ds */
		// chObj = new Choudhary();
		// chObj->objReverseReach = new Choudhary();
		// chObj->initializeFromFile("choudhary_data_"+graph_name,global_id);
		// chObj->objReverseReach->initializeFromFile("choudhary_data_rev_"+graph_name, global_id);

		/* initialize 1-Fault Tolerant Strong Connectivity Oracle */
		// this->oneFaultOracle = new oneFTSC(this->n_nodes, this->graph->out_edges, this->graph->out_edges_start_index);

		scc_tarjan sccObj = scc_tarjan();
		sccObj.decomposeSCC(this->graph, removeVertex);

		this->n_chil = sccObj.t_cc;
		// cout<<"\n#ch = "<<n_chil<<"\n\n";
		if (n_chil > 0) {
			// construct child nodes from sccs
			for (int i = 0; i < n_chil; i++) {
				// cout<<"~> ";
				// sccObj.cc_array[i] -> PrintVertices();
				addChild(sccObj.cc_array[i], i);
				// cout<<endl;
			}
			// shrink array from n to n_chil
			shrinkChildrenArray();
			// cout<<"\ndepth = "<<this->node_depth<<"\n";
		}
		else {
			// if there aren't any children release the arrays - no need to keep
			// them
			delete[] vertexMap;
			vertexMap = nullptr;
			// delete[] vertexMapReverse;
			// vertexMapReverse = nullptr;
			delete[] vertex2children;
			vertex2children = nullptr;
			delete[] children;
			children = nullptr;
		}
	}
}

cndp::common::DimacsGraph *TreeNode::createSubgraph(cndp::common::ConnectedComponent *scc, int *ind)
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

			sub1->input_file[p++] = this->vertexMap[x];
			sub1->input_file[p++] = this->vertexMap[y];

			// cout << x << "-->" << y << std::endl;
			// cout << this -> vertexMap[x] << "-->" << this -> vertexMap[y] <<
			// std::endl; cout<<endl;
		}
	}

	if (p > 2 * m) {
		fprintf(stderr, "Error! Graph has >%d arcs.\n", m);
		exit(-1);
	}
	// for(int i = 0; i < 2*m-1; i=i+2) cout<<sub1->input_file[i]<<" ->
	// "<<sub1->input_file[i+1]<<" /
	// "<<scc->GetVertex(sub1->input_file[i])<<"-"<<scc->GetVertex(sub1->input_file[i+1])<<endl;

	sub1->starting_vertex = 1;
	sub1->CreateAdjencyList();

	return sub1;
}

void TreeNode::printGraph(cndp::common::DimacsGraph *G)
{
	for (int i = 1; i <= G->vertex_size; i++) {
		for (int j = G->out_edges_start_index[i]; j <= G->out_edges_end_index[i]; j++) {
			cout << i << " ---> " << G->out_edges[j] << endl;
		}
	}
}

int *TreeNode::createIndexOfVertexInCC(cndp::common::ConnectedComponent *cc)
{
	int N = cc->GetTotalVertices();
	int *array = new int[N + 1];
	array[0] = 0;
	for (int i = 1; i <= N; i++) {
		int v = cc->GetVertex(i);
		array[v] = i;
		// cout<<v<<" - "<<i<<endl;
	}
	return array;
}

int *TreeNode::createIndexOfVertexInCC(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *cc)
{
	int N = cc->GetTotalVertices();
	int *array = new int[graph->vertex_size + 1];

	for (int i = 0; i <= graph->vertex_size; i++) {
		array[i] = 0;
	}

	for (int i = 1; i <= N; i++) {
		int v = cc->GetVertex(i);
		array[v] = i;
	}
	return array;
}

void TreeNode::copyArraysBFS(int vertex_size, int edge_size, int *out_edges, int *out_edges_start_index)
{
	n_bfs = vertex_size;
	e_bfs = edge_size;

	g_first_out = new int[n_bfs + 2]();
	for (int i = 0; i <= n_bfs + 1; i++) {
		g_first_out[i] = out_edges_start_index[i];
	}

	g_out = new int[e_bfs]();
	for (int i = 0; i < e_bfs; i++) {
		g_out[i] = out_edges[i];
	}
}
