#ifndef CNDP_COMMON_DIMACS_GRAPH_H_
#define CNDP_COMMON_DIMACS_GRAPH_H_

#include "utilities.h"

namespace cndp {
namespace common {

class DimacsGraph {

	/*--------------------------------------------------------------------------*
	 | It read the input graph files and store the information. For example,	|
	 | number of edges, vertices, incoming edges, outgoing edges, etc..			|
	 *--------------------------------------------------------------------------*/
	public:
	//	public Typedefs and Enums. /* not defined */
	enum class EdgeType { BOTH = 0, FORWARD = 1, REVERSE = 2 } edge_type = EdgeType::BOTH;

	//	public static const data members /* not defined */
	string input_graph_file_name = "result_output";

	int vertex_size = 0, edge_size = 0; /* number of nodes, arcs */
	bool *deleted_vertices = 0;
	int *index_of_vertex_in_cc = 0;
	int *vertex_degree = 0;

	// outgoing edges
	int *out_edges_start_index = 0, *out_edges_end_index = 0, *out_edges = 0;

	// incoming edges ( reverse adjacency list)
	int *in_edges_start_index = 0, *in_edges_end_index = 0, *in_edges = 0;

	//	public Constructors and assignment operators
	DimacsGraph();
	DimacsGraph(const char *graph_file, int starting_vertex);

	//	public Destructor
	virtual ~DimacsGraph();

	//	public Methods, including static methods
	void SetReverseGraph();			// It reverse the graph itself
	DimacsGraph *GetReverseGraph(); // It creates the new copy of reverse graph

	inline int DecodeVertex(int index)
	{ // We assume that index is started from 1.
		return (index + this->starting_vertex - 1);
	}

	int GetStartVertex();

	void CreateAdjencyList();
	void CalculateInitialVerticesDegree();
	void ReCalculateVerticesDegree(int vertex);

	void PrintIncomingEdges(int vertex);
	void PrintOutgoingEdges(int vertex);

	bool RemoveEdgesBetweenVertices(int vertex_u, int vertex_v, EdgeType removing_edge_type);
	double GetRemovedEdgesNumber();
	double GetRemovedEdgePercentage();

	std::string GetInputGraphFileName();
	//	int GetVertexSize();
	//	int GetEdgeSize();
	//	bool IsVertexDeleted(int vertex);
	//	void DeleteVertex(int vertex, bool is_delete);

	// outgoing edges
	//	int GetOutEdgesStartIndex(int vertex);
	//	int GetOutEdgesEndIndex(int vertex);
	//	int GetOutEdge(int index);

	// incoming edges
	//	int GetInEdgesStartIndex(int vertex);
	//	int GetInEdgesEndIndex(int vertex);
	//	int GetInEdge(int index);

	protected:
	void InitializeGraph(int total_vertices, int total_edges);
	void Init(int total_vertices, int total_edges);

	public:
	//	private Typedefs and Enums /* not defined */
	//	private static const data members /* not defined */

	//	private Methods, including static methods
	inline int EncodeVertex(int vertex)
	{
		return (vertex - this->starting_vertex + 1);
	}
	void ReadGraph(const char *graph_file);
	bool RemoveEdge(int vertex_u, int vertex_v);

	//	private Data Members (except static const data members)
	static const int MAXLINE = 100;
	char line[MAXLINE];	 /* stores input line */
	int *input_file = 0; // stores input arcs

	int starting_vertex = 0;
	int removed_edge_counter = 0;
};
} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_DIMACS_GRAPH_H_ */
