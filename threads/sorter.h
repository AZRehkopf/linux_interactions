#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <vector>

using namespace std;

void sort_worker(int row_num, vector<vector<int>> &matrix);