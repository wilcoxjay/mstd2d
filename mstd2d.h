#ifndef MSTD_H
#define MSTD_H
class SparseSet2d;
class DenseSet2d;

void compute_sizes(const SparseSet2d& set,
                   DenseSet2d& sum_set, DenseSet2d& diff_set,
                   int& sum_size, int& diff_size);
bool is_mstd(const SparseSet2d& set);
#endif  // MSTD_H
