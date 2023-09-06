#include "dimacs_graph.h"

namespace cndp {
namespace common {

DimacsGraph::DimacsGraph()
{

	/*--------------------------------------------------------------------------*
	 | It read the input graph files and store the information. For example,	|
	 | number of edges, vertices, incoming edges, outgoing edges, etc..			|
	 *--------------------------------------------------------------------------*/
}
DimacsGraph::DimacsGraph(const char *graph_file, int starting_vertex)
{

	this->starting_vertex = starting_vertex;
	this->input_graph_file_name = Utilities::GetStringValue(graph_file);

	// cout << "Start to Read the Graph File..." << endl;
	this->ReadGraph(graph_file);
}
//	public Destructor
DimacsGraph::~DimacsGraph()
{

	if (this->out_edges != 0) {
		delete[] this->out_edges;
		this->out_edges = 0;
	}

	if (this->out_edges_start_index != 0) {
		delete[] this->out_edges_start_index;
		this->out_edges_start_index = 0;
	}
	if (this->in_edges != 0) {
		delete[] this->in_edges;
		this->in_edges = 0;
	}

	if (this->in_edges_start_index != 0) {
		delete[] this->in_edges_start_index;
		this->in_edges_start_index = 0;
	}
	if (deleted_vertices != 0) {
		delete[] deleted_vertices;
		deleted_vertices = 0;
	}

	if (out_edges_end_index != 0) {
		delete[] out_edges_end_index;
		out_edges_end_index = 0;
	}
	if (in_edges_end_index != 0) {
		delete[] in_edges_end_index;
		in_edges_end_index = 0;
	}
}
//	public Methods, including static methods

// It reverse the graph itself
void DimacsGraph::SetReverseGraph()
{

	// Exchange the index
	int *temp = out_edges_start_index;
	out_edges_start_index = in_edges_start_index;
	in_edges_start_index = temp;

	// Exchange the end index
	temp = out_edges_end_index;
	out_edges_end_index = in_edges_end_index;
	in_edges_end_index = temp;

	// Exchange the edges
	temp = out_edges;
	out_edges = in_edges;
	in_edges = temp;
}

// It creates the new copy of reverse graph
DimacsGraph *DimacsGraph::GetReverseGraph()
{

	DimacsGraph *g = new DimacsGraph();

	g->Init(vertex_size, edge_size);
	g->starting_vertex = starting_vertex;
	g->input_graph_file_name = input_graph_file_name;

	int size = vertex_size + 2;
	memcpy(g->out_edges_start_index, in_edges_start_index, size * sizeof(int));
	memcpy(g->in_edges_start_index, out_edges_start_index, size * sizeof(int));

	memcpy(g->out_edges_end_index, in_edges_end_index, size * sizeof(int));
	memcpy(g->in_edges_end_index, out_edges_end_index, size * sizeof(int));

	size = edge_size;
	memcpy(g->out_edges, in_edges, size * sizeof(int));
	memcpy(g->in_edges, out_edges, size * sizeof(int));

	size = vertex_size + 1;
	memcpy(g->deleted_vertices, deleted_vertices, size * sizeof(bool));

	return g;
}
int DimacsGraph::GetStartVertex()
{
	return this->starting_vertex;
}
void DimacsGraph::CreateAdjencyList()
{

	this->InitializeGraph(this->vertex_size, this->edge_size);

	int *temp_out_index = new int[this->vertex_size + 2];
	int *temp_in_index = new int[this->vertex_size + 2];

	int x, y;

	for (int i = 0; i <= this->vertex_size + 1; i++) {
		temp_out_index[i] = temp_in_index[i] = 0;
	}

	for (int k = 0; k < this->edge_size; k++) {

		x = this->EncodeVertex(this->input_file[2 * k]);
		y = this->EncodeVertex(this->input_file[2 * k + 1]);

		// Be careful, here we started the index from 1. (x+1)
		// Because we have to do it for later to calculate the
		// cumulative degree of GfirstOut & GfirstIn
		this->out_edges_start_index[x + 1]++;
		this->in_edges_start_index[y + 1]++;
	}

	for (int k = 1; k <= this->vertex_size + 1; k++) {

		this->out_edges_start_index[k] += this->out_edges_start_index[k - 1];
		this->in_edges_start_index[k] += this->in_edges_start_index[k - 1];

		temp_out_index[k] = this->out_edges_start_index[k];
		temp_in_index[k] = this->in_edges_start_index[k];
	}

	// Create the edges from input file
	for (int k = 0; k < this->edge_size; k++) {

		x = this->EncodeVertex(this->input_file[2 * k]);
		y = this->EncodeVertex(this->input_file[2 * k + 1]);

		this->out_edges[temp_out_index[x]++] = y;
		this->in_edges[temp_in_index[y]++] = x;
	}

	// set the end position of the vertex in Adjency
	for (int i = 1; i <= vertex_size; i++) {

		out_edges_end_index[i] = out_edges_start_index[i + 1] - 1;
		in_edges_end_index[i] = in_edges_start_index[i + 1] - 1;
	}

	delete[] temp_in_index;
	delete[] temp_out_index;
	delete[] this->input_file;
}
void DimacsGraph::CalculateInitialVerticesDegree()
{

	this->vertex_degree = new int[this->vertex_size + 1];

	int vertex = 0;
	for (int i = 1; i <= this->vertex_size; i++) {

		vertex = this->EncodeVertex(i);
		// initialize degree 0 for all vertex

		this->vertex_degree[vertex] = 0;

		// count total indegree
		this->vertex_degree[vertex] = this->in_edges_end_index[vertex] - this->in_edges_start_index[vertex] + 1;

		// count total out degree
		this->vertex_degree[vertex] +=
			(this->out_edges_end_index[vertex] - this->out_edges_start_index[vertex] + 1);
	}
}
void DimacsGraph::ReCalculateVerticesDegree(int vertex_to_be_remove)
{

	int vertex = 0;

	// reset indegree
	for (int i = this->in_edges_start_index[vertex_to_be_remove];
		 i <= this->in_edges_end_index[vertex_to_be_remove]; i++) {

		vertex = this->EncodeVertex(this->in_edges[i]);
		this->vertex_degree[vertex]--;
	}

	// reset outdegree
	for (int i = this->out_edges_start_index[vertex_to_be_remove];
		 i <= this->out_edges_end_index[vertex_to_be_remove]; i++) {

		vertex = this->EncodeVertex(this->out_edges[i]);
		this->vertex_degree[vertex]--;
	}
}

void DimacsGraph::PrintIncomingEdges(int vertex)
{
	cout << " Incoming edges of [" << vertex << "]:";
	for (int i = this->in_edges_start_index[vertex]; i < this->in_edges_start_index[vertex + 1]; i++) {
		cout << this->in_edges[i] << " ";
	}
	cout << endl;
}
void DimacsGraph::PrintOutgoingEdges(int vertex)
{

	cout << " Outgoing edges of [" << vertex << "]:";
	for (int i = this->out_edges_start_index[vertex]; i < this->out_edges_start_index[vertex + 1]; i++) {
		cout << this->out_edges[i] << " ";
	}
	cout << endl;
}
bool DimacsGraph::RemoveEdgesBetweenVertices(int vertex_u, int vertex_v, EdgeType removing_edge_type)
{

	//    cout << "Removing edge between " << from_vertex << " and " << to_vertex
	//            << endl;

	// Remove an outgoing edge u-->v
	bool is_removed_forward = this->RemoveEdge(vertex_u, vertex_v);

	// Remove an incoming edge v-->u (u<--v)
	bool is_removed_reverse = this->RemoveEdge(vertex_v, vertex_u);

	// Just to decrease the edge_index in callee method
	if (removing_edge_type == EdgeType::FORWARD) {
		return is_removed_forward;
	}
	else if (removing_edge_type == EdgeType::REVERSE) {
		return is_removed_reverse;
	}
	else if (removing_edge_type == EdgeType::BOTH) {
		return (is_removed_forward || is_removed_reverse);
	}
	else {
		return false;
	}
}

bool DimacsGraph::RemoveEdge(int from_vertex, int to_vertex)
{

	// Both u & v should not be zero
	if (from_vertex == 0 || to_vertex == 0) {
		return false;
	}

	bool is_from_deleted = false, is_to_deleted = false;
	for (int edge_index = out_edges_start_index[from_vertex]; edge_index <= out_edges_end_index[from_vertex];
		 edge_index++) {

		if (out_edges[edge_index] == to_vertex) {

			out_edges[edge_index] = out_edges[out_edges_end_index[from_vertex]];
			out_edges[out_edges_end_index[from_vertex]] = 0;
			out_edges_end_index[from_vertex]--;
			is_from_deleted = true;

			//            cout << "Removed Edge:" << from_vertex << "-->" << to_vertex
			//                    << endl;

			break;
		}
	}
	// If it's an outgoing edges for from_vertex then
	// It's an incoming edges for to_vertex.
	for (int edge_index = in_edges_start_index[to_vertex]; edge_index <= in_edges_end_index[to_vertex];
		 edge_index++) {

		if (in_edges[edge_index] == from_vertex) {

			in_edges[edge_index] = in_edges[in_edges_end_index[to_vertex]];
			in_edges[in_edges_end_index[to_vertex]] = 0;
			in_edges_end_index[to_vertex]--;

			// cout << "Removed(IN):" << to_vertex << "<--" << from_vertex << endl;

			is_to_deleted = true;
			break;
		}
	}
	// Check the edge removal correctness
	if ((is_from_deleted && !is_to_deleted) || (!is_from_deleted && is_to_deleted)) {

		cerr << "Error in Edge removing process: DimacsGraph:RemoveEdge()" << endl;
		exit(-1);
	}
	this->removed_edge_counter++;
	return (is_from_deleted && is_to_deleted);
}

double DimacsGraph::GetRemovedEdgesNumber()
{
	return this->removed_edge_counter;
}
double DimacsGraph::GetRemovedEdgePercentage()
{
	double removed_percentage = edge_size * removed_edge_counter / (double)100;
	return removed_percentage;
}

string DimacsGraph::GetInputGraphFileName()
{
	return this->input_graph_file_name;
}
/*int DimacsGraph::GetVertexSize() {
 return this->vertex_size;
 }
 int DimacsGraph::GetEdgeSize() {
 return this->edge_size;
 }*/
/*bool DimacsGraph::IsVertexDeleted(int vertex) {
 return this->deleted_vertices[vertex];
 }
 void DimacsGraph::DeleteVertex(int vertex, bool is_delete) {
 this->deleted_vertices[vertex] = is_delete;
 }*/

// outgoing edges
/*int DimacsGraph::GetOutEdgesStartIndex(int vertex) {
 return this->out_edges_start_index[vertex];
 }
 int DimacsGraph::GetOutEdgesEndIndex(int vertex) {
 return this->out_edges_end_index[vertex];
 }
 int DimacsGraph::GetOutEdge(int index) {
 return this->out_edges[index];
 }*/

// incoming edges
/*int DimacsGraph::GetInEdgesStartIndex(int vertex) {
 return this->in_edges_start_index[vertex];
 }
 int DimacsGraph::GetInEdgesEndIndex(int vertex) {
 return this->in_edges_end_index[vertex];
 }
 int DimacsGraph::GetInEdge(int index) {
 return this->in_edges[index];
 }*/

void DimacsGraph::InitializeGraph(int total_vertices, int total_edges)
{

	this->Init(total_vertices, total_edges);
	for (int i = 0; i <= total_vertices + 1; i++) {
		this->out_edges_start_index[i] = this->in_edges_start_index[i] = this->out_edges_end_index[i] =
			this->in_edges_end_index[i] = 0;
	}
	for (int i = 0; i < total_edges; i++) {
		this->out_edges[i] = this->in_edges[i] = 0;
	}
	for (int i = 0; i <= total_vertices; i++) {
		deleted_vertices[i] = false;
	}
}
void DimacsGraph::Init(int total_vertices, int total_edges)
{

	this->vertex_size = total_vertices;
	this->edge_size = total_edges;

	if (total_vertices > 0) {
		this->out_edges_start_index = new int[total_vertices + 2];
		this->in_edges_start_index = new int[total_vertices + 2];

		this->out_edges_end_index = new int[total_vertices + 2];
		this->in_edges_end_index = new int[total_vertices + 2];

		this->deleted_vertices = new bool[total_vertices + 1];
	}
	if (total_edges > 0) {
		this->edge_size = total_edges;
		this->out_edges = new int[total_edges];
		this->in_edges = new int[total_edges];
	}
}
void DimacsGraph::ReadGraph(const char *graph_file)
{

	int x, y;
	int p = 0;

	FILE *input = fopen(graph_file, "r");
	if (!input) {
		fprintf(stderr, "Error opening file \"%s\".\n", graph_file);
		exit(-1);
	}

	while (fgets(this->line, MAXLINE, input) != nullptr) {
		switch (line[0]) {
			case '\n':
				break;
			case '\0':
				break;
			case 'p':
				if (sscanf(line, "p %d %d", &vertex_size, &edge_size) != 2) {
					fprintf(stderr, "Error reading graph file (%s).\n", graph_file);
					exit(-1);
				}
				this->input_file = new int[2 * this->edge_size];
				break;
			case 'a':
				if (sscanf(line, "a %d %d", &x, &y) != 2) {
					fprintf(stderr, "Error reading graph arc (%s).\n", graph_file);
					exit(-1);
				}
				assert(x <= vertex_size);
				assert(y <= vertex_size);

				// no self loop
				if (x == y) {
					break;
				}

				input_file[p++] = x;
				input_file[p++] = y;

				if (p > 2 * edge_size) {
					fprintf(stderr, "Error! Graph has >%d arcs.\n", edge_size);
					exit(-1);
				}
				break;
			default:
				fprintf(stderr, "Error reading graph (%s).\n", graph_file);
				exit(-1);
		}
	}
	// fprintf(stderr, "END reading graph (%s).\n", graph_file);
	fclose(input);
}
} // namespace common
} // namespace cndp
