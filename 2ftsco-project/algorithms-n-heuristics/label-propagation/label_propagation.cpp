#include "label_propagation.h"

void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void randomize(int *arr, int num)
{
	// Start from the last element and swap one by one. We don't
	// need to run for the first element that's why i > 0

	for (int i = num; i > 0; i--) {
		int j = rand() % i + 1;
		// Swap arr[i] with the element at random index
		swap(&arr[i], &arr[j]);
	}
}

int Label_Propagation(cndp::common::DimacsGraph *graph)
{
	int n = graph->vertex_size;
	int *Final_Labels = new int[n + 1];
	int *Count_Labels = new int[n + 1]; // how many times apperas each label
	int *process_vertices_matrix = new int[n + 1];
	int *stack_1 = new int[n + 1]; // put all the labels
	int *stack_2 = new int[n + 1]; // put the majority labels

	for (int i = n; i >= 0; i--) {
		Count_Labels[i] = 0;
	}
	for (int i = n; i >= 0; i--) {
		process_vertices_matrix[i] = Final_Labels[i] = i;
	}
	// for (int i = cc -> GetTotalVertices(); i >=1; i--) process_vertices_matrix[cc -> GetVertex(i)] =
	// Final_Labels[cc -> GetVertex(i)] = cc -> GetVertex(i);
	for (int i = n; i >= 0; i--) {
		stack_1[i] = stack_2[i] = 0;
	}

	/*We are processing the vertices with a uniformly random order*/

	int stack_pointer_1;
	int stack_pointer_2;

	int iteration_flag = 0;
	int maxin = 0;
	int count = 1;
	int in_edges_num = 0;
	int most_out_labels = 0;
	int most_out_labels_ID = 0;
	while (count > 0) {

		count = 0;

		most_out_labels = 0;
		most_out_labels_ID = 0;

		randomize(process_vertices_matrix, n);

		for (int start = 1; start <= n; start++) {

			int i = process_vertices_matrix[start];
			if (i == 0) {
				continue;
			}
			// printf("processing vertex %d with label %d\n",i, Final_Labels[i]);
			stack_pointer_1 = 0;
			in_edges_num = graph->in_edges_end_index[i] - graph->in_edges_start_index[i] + 1;
			if (maxin < in_edges_num) {
				maxin = in_edges_num;
			}
			for (int j = graph->in_edges_start_index[i]; j < graph->in_edges_end_index[i] + 1; j++) {
				int v = graph->in_edges[j];
				/*label of v  = Final_Label[v]*/
				int l = Final_Labels[v];
				if (Count_Labels[l] == 0) {
					/* We haven't visited vertex v until that time --> put it in the Stack*/
					stack_1[stack_pointer_1] = l;
					stack_pointer_1++;
				}
				Count_Labels[l]++;
			}

			int l_mine = Final_Labels[i];
			if (Count_Labels[l_mine] == 0) {
				/* We haven't visited vertex v until that time --> put it in the Stack*/
				stack_1[stack_pointer_1] = l_mine;
				stack_pointer_1++;
			}
			Count_Labels[l_mine] += 1;

			/*Assign the label to vertex i*/
			int max_appearances = 0;

			stack_pointer_2 = 0;
			for (int k = 0; k < stack_pointer_1; k++) {
				int j = stack_1[k];
				if (Count_Labels[j] > max_appearances) {
					max_appearances = Count_Labels[j];
					stack_pointer_2 = 0; // empty the stack?
					stack_2[stack_pointer_2] = j;
					stack_pointer_2++;
				}
				else if (Count_Labels[j] == max_appearances) {
					stack_2[stack_pointer_2] = j;
					stack_pointer_2++;
				}

				Count_Labels[j] = 0;
			}

			if (Count_Labels[l_mine] > max_appearances) {
				max_appearances = Count_Labels[l_mine];
				stack_pointer_2 = 0; // empty the stack?
				stack_2[stack_pointer_2] = l_mine;
				stack_pointer_2++;
			}
			else if (Count_Labels[l_mine] == max_appearances) {
				stack_2[stack_pointer_2] = l_mine;
				stack_pointer_2++;
			}

			Count_Labels[l_mine] = 0;

			int counter = 0;
			for (int j = graph->out_edges_start_index[i]; j < graph->out_edges_end_index[i] + 1; j++) {
				// int v = Gout[j];
				int v = graph->out_edges[j];
				int l = Final_Labels[v];
				if (Count_Labels[l] == 0) {
					counter++;
				}
				Count_Labels[l]++;
			}
			if (counter > most_out_labels) {
				most_out_labels = counter;
				most_out_labels_ID = i;
			}
			// for (int j =GfirstOut[i];j< GlastOut[i]; j++) {
			for (int j = graph->out_edges_start_index[i]; j < graph->out_edges_end_index[i] + 1; j++) {
				// for (int j =graph -> out_edges_start_index[i]; j < graph -> out_edges_end_index[i+1]; j++) {
				// int v = Gout[j];
				int v = graph->out_edges[j];
				int l = Final_Labels[v];
				// int l = Final_Labels[v];
				Count_Labels[l] = 0;
			}

			/*Final Assignement*/

			if (in_edges_num == 0) {
				continue;
			}
			if (stack_pointer_2 == 1) {
				int check = Final_Labels[i];
				Final_Labels[i] = stack_2[0];
				if (Final_Labels[i] != check) {
					count++;
				}
			}
			else {
				/*Select uniformly Random*/

				int r = rand() % (stack_pointer_2);

				int check = Final_Labels[i];
				Final_Labels[i] = stack_2[r];
				if (Final_Labels[i] != check) {
					count++;
				}
			}
		}

		iteration_flag++;
	}

	delete[] process_vertices_matrix;
	delete[] Final_Labels;
	delete[] Count_Labels;
	delete[] stack_1;

	delete[] stack_2;

	return most_out_labels_ID;
}

int Label_Propagation(cndp::common::DimacsGraph *graph, cndp::common::ConnectedComponent *cc, int *index_in_cc)
{
	int n = graph->vertex_size;
	int *Final_Labels = new int[n + 1];
	int *Count_Labels = new int[n + 1]; // how many times apperas each label
	int *process_vertices_matrix = new int[n + 1];
	int *stack_1 = new int[n + 1]; // put all the labels
	int *stack_2 = new int[n + 1]; // put the majority labels

	for (int i = n; i >= 0; i--) {
		Count_Labels[i] = 0;
	}
	for (int i = n; i >= 0; i--) {
		process_vertices_matrix[i] = Final_Labels[i] = i;
	}
	// for (int i = cc -> GetTotalVertices(); i >=1; i--) process_vertices_matrix[cc -> GetVertex(i)] =
	// Final_Labels[cc -> GetVertex(i)] = cc -> GetVertex(i);
	for (int i = n; i >= 0; i--) {
		stack_1[i] = stack_2[i] = 0;
	}

	/*We are processing the vertices with a uniformly random order*/

	int stack_pointer_1;
	int stack_pointer_2;

	int iteration_flag = 0;
	int maxin = 0;
	int count = 1;
	int in_edges_num = 0;
	int most_out_labels = 0;
	int most_out_labels_ID = 0;
	while (count > 0) {

		count = 0;

		most_out_labels = 0;
		most_out_labels_ID = 0;

		randomize(process_vertices_matrix, n);

		for (int start = 1; start <= n; start++) {

			int i = process_vertices_matrix[start];
			if (i == 0 || index_in_cc[i] == 0) {
				continue;
			}
			// printf("processing vertex %d with label %d\n",i, Final_Labels[i]);
			stack_pointer_1 = 0;
			in_edges_num = graph->in_edges_end_index[i] - graph->in_edges_start_index[i] + 1;
			if (maxin < in_edges_num) {
				maxin = in_edges_num;
			}
			for (int j = graph->in_edges_start_index[i]; j < graph->in_edges_end_index[i] + 1; j++) {
				int v = graph->in_edges[j];
				if (index_in_cc[v] == 0) {
					continue;
				}
				/*label of v  = Final_Label[v]*/
				int l = Final_Labels[v];
				if (Count_Labels[l] == 0) {
					/* We haven't visited vertex v until that time --> put it in the Stack*/
					stack_1[stack_pointer_1] = l;
					stack_pointer_1++;
				}
				Count_Labels[l]++;
			}

			int l_mine = Final_Labels[i];
			if (Count_Labels[l_mine] == 0) {
				/* We haven't visited vertex v until that time --> put it in the Stack*/
				stack_1[stack_pointer_1] = l_mine;
				stack_pointer_1++;
			}
			Count_Labels[l_mine] += 1;

			/*Assign the label to vertex i*/
			int max_appearances = 0;

			stack_pointer_2 = 0;
			for (int k = 0; k < stack_pointer_1; k++) {
				int j = stack_1[k];
				if (Count_Labels[j] > max_appearances) {
					max_appearances = Count_Labels[j];
					stack_pointer_2 = 0; // empty the stack?
					stack_2[stack_pointer_2] = j;
					stack_pointer_2++;
				}
				else if (Count_Labels[j] == max_appearances) {
					stack_2[stack_pointer_2] = j;
					stack_pointer_2++;
				}

				Count_Labels[j] = 0;
			}

			if (Count_Labels[l_mine] > max_appearances) {
				max_appearances = Count_Labels[l_mine];
				stack_pointer_2 = 0; // empty the stack?
				stack_2[stack_pointer_2] = l_mine;
				stack_pointer_2++;
			}
			else if (Count_Labels[l_mine] == max_appearances) {
				stack_2[stack_pointer_2] = l_mine;
				stack_pointer_2++;
			}

			Count_Labels[l_mine] = 0;

			int counter = 0;
			for (int j = graph->out_edges_start_index[i]; j < graph->out_edges_end_index[i] + 1; j++) {
				// int v = Gout[j];
				int v = graph->out_edges[j];
				if (index_in_cc[v] == 0) {
					continue;
				}
				int l = Final_Labels[v];
				if (Count_Labels[l] == 0) {
					counter++;
				}
				Count_Labels[l]++;
			}
			if (counter > most_out_labels) {
				most_out_labels = counter;
				most_out_labels_ID = i;
			}
			// for (int j =GfirstOut[i];j< GlastOut[i]; j++) {
			for (int j = graph->out_edges_start_index[i]; j < graph->out_edges_end_index[i] + 1; j++) {
				// for (int j =graph -> out_edges_start_index[i]; j < graph -> out_edges_end_index[i+1]; j++) {
				// int v = Gout[j];
				int v = graph->out_edges[j];
				if (index_in_cc[v] == 0) {
					continue;
				}
				int l = Final_Labels[v];
				// int l = Final_Labels[v];
				Count_Labels[l] = 0;
			}

			/*Final Assignement*/

			if (in_edges_num == 0) {
				continue;
			}
			if (stack_pointer_2 == 1) {
				int check = Final_Labels[i];
				Final_Labels[i] = stack_2[0];
				if (Final_Labels[i] != check) {
					count++;
				}
			}
			else {
				/*Select uniformly Random*/

				int r = rand() % (stack_pointer_2);

				int check = Final_Labels[i];
				Final_Labels[i] = stack_2[r];
				if (Final_Labels[i] != check) {
					count++;
				}
			}
		}

		iteration_flag++;
	}

	delete[] process_vertices_matrix;
	delete[] Final_Labels;
	delete[] Count_Labels;
	delete[] stack_1;

	delete[] stack_2;

	return most_out_labels_ID;
}
