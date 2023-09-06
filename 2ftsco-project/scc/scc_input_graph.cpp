#include "scc_input_graph.h"

namespace cndp {
namespace common {

SCC::SCC()
{
}
SCC::~SCC()
{
	DestroySCC();
}
int SCC::ComputeSCCs()
{

	for (int v_i = 1; v_i <= dimacs_graph->vertex_size; v_i++) {
		if (this->stack_i[v_i] == -1) {
			this->FindSCCinDimacsGraph(v_i);
		}
	} // end for
	return this->total_component;
}
void SCC::FindSCCinDimacsGraph(int v_i)
{

	this->stack_s[++this->stack_s_top] = v_i;
	int iv = this->stack_s_top;
	this->stack_i[v_i] = iv;
	this->stack_b[++this->stack_b_top] = this->stack_i[v_i];

	// scan the outgoing edges from v, suppose v-->w
	for (int edge_index_v = this->dimacs_graph->out_edges_start_index[v_i];
		 edge_index_v <= this->dimacs_graph->out_edges_end_index[v_i]; edge_index_v++) {

		// suppose v-->w
		int w = this->dimacs_graph->out_edges[edge_index_v];

		int iw = this->stack_i[w];
		if (iw == -1) {
			this->FindSCCinDimacsGraph(w);
		}
		else {
			while ((this->stack_b[this->stack_b_top]) > iw) {
				stack_b[this->stack_b_top] = -1;
				this->stack_b_top--;
			}
		}
	}

	if (this->stack_b[this->stack_b_top] != iv) {
		return;
	}

	// This line means ==> this->stack_b[this->stack_b_top] = iv
	stack_b[this->stack_b_top] = -1;
	this->stack_b_top--;
	this->next_constant++;

	int scc_vsize = this->stack_s_top - iv + 1;
	if (scc_vsize <= 0) {
		return;
	}

	int scc_index = this->total_component + 1;

	int vertex = 0;
	while (this->stack_s_top >= iv) {

		vertex = this->stack_s[this->stack_s_top];
		// add into SCC.
		this->component_vertices[this->total_scc_vertices++] = vertex;
		this->vertex_scc_id[vertex] = scc_index;

		// Pop(stack_s)
		this->stack_i[vertex] = next_constant;
		this->stack_s[stack_s_top] = -1;
		this->stack_s_top--;
	}
	// cout << endl;

	this->components[++this->total_component] = scc_vsize;
	this->SetVerticesIndexOfNextSCC();
}

void SCC::PrintSCCVertices(int scc_index)
{

	cout << "SCC(" << scc_index << "){";
	for (int i = this->start_index; i < this->end_index; i++) {
		cout << this->component_vertices[i] << ", ";
	}
	cout << this->component_vertices[this->end_index] << "}" << endl;
}

void SCC::CopySCCs(cndp::common::ConnectedComponent **sccs)
{

	int scc_vsize = 0;
	for (int scc_index = 1; scc_index <= this->total_component; scc_index++) {

		this->SetIndexForSCC(scc_index);
		scc_vsize = end_index - start_index + 1;

		cndp::common::ConnectedComponent *scc = new cndp::common::ConnectedComponent(scc_vsize);

		for (int i = this->start_index; i <= this->end_index; i++) {
			scc->AddVertex(this->component_vertices[i]);
		}
		sccs[scc_index] = scc;
		// PrintSCCVertices(scc_index);

	} // end for (scc_index)
}
void SCC::InitSCC(DimacsGraph *dimacs_graph)
{

	this->dimacs_graph = dimacs_graph;
	int vertex_size = dimacs_graph->vertex_size;

	this->stack_s = new int[vertex_size];
	this->stack_b = new int[vertex_size];

	this->next_constant = vertex_size + 2;

	this->stack_i = new int[vertex_size + 1];
	this->vertex_scc_id = new int[vertex_size + 1];

	for (int i = 0; i <= vertex_size; i++) {
		this->stack_i[i] = this->vertex_scc_id[i] = -1;
	}
	this->stack_s_top = this->stack_b_top = -1;

	this->component_vertices = new int[vertex_size];
	this->total_scc_vertices = 0;

	this->components = new int[vertex_size + 1];
	this->total_component = 0;
	this->components[0] = false;

	this->start_index = 0;
	this->end_index = 0;
	this->looper = 0;
}

void SCC::DestroySCC()
{
	if (this->stack_s != 0) {
		delete[] this->stack_s;
		this->stack_s = 0;
	}

	if (this->stack_b != 0) {
		delete[] this->stack_b;
		this->stack_b = 0;
	}
	if (this->stack_i != 0) {
		delete[] this->stack_i;
		this->stack_i = 0;
	}
	this->stack_s_top = this->stack_b_top = -1;
	this->next_constant = 0;

	// remove scc components
	if (this->component_vertices != 0) {
		delete[] this->component_vertices;
		this->component_vertices = 0;
	}
	if (this->components != 0) {
		delete[] this->components;
		this->components = 0;
	}

	if (this->vertex_scc_id != 0) {
		delete[] this->vertex_scc_id;
		this->vertex_scc_id = 0;
	}

	this->total_scc_vertices = this->total_component = 0;

	// reset other variables.
	this->start_index = 0;
	this->end_index = 0;
	// this->scc_vertices = 0;
	this->looper = 0;
}
void SCC::SetVerticesIndexOfNextSCC()
{

	this->start_index = 0;
	this->end_index = this->total_scc_vertices - 1;

	if (this->total_component > 1) {

		this->start_index = this->components[this->total_component - 1];

		this->components[this->total_component] += this->components[this->total_component - 1];

		this->end_index = this->components[this->total_component] - 1;
	}
}

void SCC::SetIndexForSCC(int component_number)
{

	this->start_index = 0;
	this->end_index = this->components[1] - 1;

	if (component_number > 1) {

		this->start_index = this->components[component_number - 1];
		this->end_index = this->components[component_number] - 1;
	}
}

} // namespace common
} // namespace cndp
