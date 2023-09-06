#ifndef CNDP_COMMON_MAXCHILD_TREE_GRAPH_H_
#define CNDP_COMMON_MAXCHILD_TREE_GRAPH_H_

namespace cndp {
namespace common {

class MaxChildTree {

	/*----------------------------------------------------------------------------------*
	 | To execute the heuristics maximum children in a dominator tree or the maximum	|
	 | children in loop nesting tree, we will use it. Both heuristics will calculate	|
	 | the header (loop nesting tree) or immediate dominator (dominator tree) of each	|
	 | vertex. After that, both of the heuristics follow the identical process.			|
	 | So, when any heuristic has an information of the parent for each vertex, then	|
	 | it will process that information, create a tree, and find a vertex that has		|
	 | maximum children in a tree.														|
	 *----------------------------------------------------------------------------------*/

	public:
	//	public Typedefs and Enums. /* not defined */
	//	public static const data members /* not defined */
	//	public Constructors and assignment operators
	MaxChildTree()
	{

		/*----------------------------------------------------------------------------------*
		 | To execute the heuristics maximum children in a dominator tree or the maximum	|
		 | children in loop nesting tree, we will use it. Both heuristics will calculate	|
		 | the header (loop nesting tree) or immediate dominator (dominator tree) of each	|
		 | vertex. After that, both of the heuristics follow the identical process.			|
		 | So, when any heuristic has an information of the parent for each vertex, then	|
		 | it will process that information, create a tree, and find a vertex that has		|
		 | maximum children in a tree.														|
		 *----------------------------------------------------------------------------------*/
	}
	//	public Destructor
	virtual ~MaxChildTree()
	{
	}
	int GetMaxChildVertex(int *idom_header, int root, cndp::common::ConnectedComponent *cc, int &total_children)
	{

		total_children = 0;
		int total_vertices = cc->GetTotalVertices();

		// TREE GRAPH POINTER
		int *child_counter = new int[total_vertices + 1];

		// Initialize the pointers
		for (int i = 0; i <= total_vertices; i++) {
			child_counter[i] = 0;
		}

		idom_header[root] = 0;
		for (int vertex_i = 1; vertex_i <= total_vertices; vertex_i++) {
			child_counter[idom_header[vertex_i]]++;
		}

		int max_child = 0, temp_childs = 0;

		// We don't consider the source
		vector<int> L;
		for (int vertex_i = 1; vertex_i <= total_vertices; vertex_i++) {

			temp_childs = child_counter[vertex_i];

			if (max_child < temp_childs) {
				max_child = temp_childs;
				L.clear();
				L.push_back(vertex_i);
			}
			else if (max_child == temp_childs) { // we will choose one of them randomly.
				L.push_back(vertex_i);
			}
		}

		delete[] child_counter;

		// set the total children for input parameter
		total_children = max_child;

		int max_index = (int)L.size() - 1;

		// if multiple vertices have the same number of children, then let choose one of them randomly.
		// Moreover, if there is only one vertex in L, then random_index will be zero.
		int random_index = Utilities::GetRandomNumber(0, max_index);
		int max_vertex_index = L.at(random_index);
		
		/* comment above lines and uncomment bellow in order to remove the randomness */
		// int max_vertex_index = L.at(0); 
		
		L.clear();

		return cc->GetVertex(max_vertex_index);
	}
};
} // namespace common
} // namespace cndp
#endif /* CNDP_COMMON_MAXCHILD_TREE_GRAPH_H_ */
