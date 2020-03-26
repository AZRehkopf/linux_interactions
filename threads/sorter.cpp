#include "sorter.h"

mutex mtx;
condition_variable adv_cond;
int complete_count = 0;
mutex complete_count_mtx;

// Sort matrix row, reverse every other row for snake-like pattern
void row_sort(int row_num, vector<vector<int>> &matrix) {
	sort(matrix[row_num].begin(), matrix[row_num].end());
	
	if (row_num % 2 == 1) 
		reverse(matrix[row_num].begin(), matrix[row_num].end());
}

// Sort matrix column
void col_sort(int row_num, vector<vector<int>> &matrix) {
	vector<int> temp_vec;
	int matrix_dim = matrix.size();

	// Create temporary vector to hold column
	for (int i = 0; i < matrix_dim; i++) {
		temp_vec.push_back(matrix[i][row_num]);
	}

	sort(temp_vec.begin(), temp_vec.end());

	// Put sorted column back into matrix
	for (int i = 0; i < matrix_dim; i++) {
		matrix[i][row_num] = temp_vec[i];
	}
} 

// Ensure complete count is not incremented simultaneously
void thread_safe_increment() {
	// Aquire lock before incrementing
	const lock_guard<mutex> lock(complete_count_mtx);
	complete_count++;

	// lock_guard releases complete_count_mtx when it goes out of scope accoring to C++11 standard
}

// Calculates phases based on matrix dimension and alternates between row sort 
// and column sort until the matrix is sorted
void sort_worker(int row_num, vector<vector<int>> &matrix) {
	unique_lock<mutex> lck(mtx);
	int phases = 2*log2(matrix.size()) + 1;
	col_sort(row_num, matrix);

	for (int i = 0; i < phases; i++) {
		if (i % 2 == 0) {
			row_sort(row_num, matrix);
		} else { 
			col_sort(row_num, matrix);
		}
		
		// Signal phase is complete and notify other threads
		thread_safe_increment();
		adv_cond.notify_all();

		// Wait for all threads to complete
		while(complete_count < (matrix.size() * (i+1))) adv_cond.wait(lck);
	}
}