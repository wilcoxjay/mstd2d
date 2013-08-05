class SparseSet2d;

#define XSIZE 5
#define YSIZE 5
#define DOUBLE(x) (2*x + 1)

void compute_sizes(const SparseSet2d& set, int& sum_size, int& diff_size);
bool is_mstd(const SparseSet2d& set);
