#include "shearsort.h"

// Read in matrix values from file
void populate_matrix(vector<vector<int>> &matrix) {
	FILE *in_file = fopen("input.txt", "r");
	vector<int> row;
	char line[MAXCHAR];

	if (in_file == NULL) {
		printf("Error: could not open input.txt\n");
	}
	
	while (fgets(line, MAXCHAR, in_file) != NULL) {
		stringstream input_stream(line);

		for (int i; input_stream >> i;) {
			row.push_back(i);
			if (input_stream.peek() == ',')
				input_stream.ignore();
		}
		matrix.push_back(row);
		row.clear();
	}

	fclose(in_file);
}

// Spawn threads to sort matrix based on matrix dimensions
void spawn_threads(vector<vector<int>> &matrix) {
	int thread_num = matrix.size();
	thread worker_threads[thread_num];

	// Spawn one sorting thread for every matrix row
	for (int i = 0; i < thread_num; i++) {
		worker_threads[i] =  thread (sort_worker, i, ref(matrix));
	}

	// Wait for all threads to complete for returning
	for (int i = 0; i < thread_num; i++) {
		worker_threads[i].join();
	}
}

// Print matrix in uman readable format
void print_matrix(vector<vector<int>> &matrix) {
	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix[i].size(); j++) {
			printf("%d\t", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(void) {
	vector<vector<int>> matrix;

	populate_matrix(matrix);
	spawn_threads(matrix);
	print_matrix(matrix);

	return 0;
}