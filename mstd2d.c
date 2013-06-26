#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <limits.h>

#define N 5
#define DOUBLE (2*N + 1)

char sum_set[2*N+1][2*N+1];
char diff_set[2*N+1][2*N+1];

typedef char* Set;

int in_bounds_set(int i) {
    return 0 <= i && i < N;
}

int in_bounds_double_set(int i) {
    return 0 <= i && i < DOUBLE;
}

char get(Set s, int i, int j) {
    assert(in_bounds_set(i) && in_bounds_set(j));
    return s[i*N + j];
}

void set(Set s, int i, int j, char val) {
    assert(in_bounds_set(i) && in_bounds_set(j));
    s[i*N + j] = val;
}

void clear_set(Set s) {
    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    set(s, i, j, 0);
	}
    }
}

char contains(Set set, int i, int j) {
    return set[i * N + j];
}

void clear_sum_diff_set() {
    int i, j;
    for (i = 0; i < DOUBLE; ++i) {
	for (j = 0; j < DOUBLE; ++j) {
	    sum_set[i][j] = 0;
	    diff_set[i][j] = 0;
	}
    }
}

void print_set(Set set) {
    printf("{");
    int i, j, started = 0;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    if (get(set, i, j)) {
		if (started) {
		    printf(", ");
		}
		started = 1;
		printf("(%d,%d)", i, j);
	    }
	}
    }
    printf("}\n");
}

int is_mstd(Set set) {
    clear_sum_diff_set();
    int i1, j1, i2, j2;
    for (i1 = 0; i1 < N; ++i1) {
	for (j1 = 0; j1 < N; ++j1) {
	    if (!contains(set, i1, j1)) {
		continue;
	    }
	    //printf ("(%d,%d) is in the set.\n", i1, j1);
	    for (i2 = i1; i2 < N; ++i2) {
		int s1  = i1 + i2;
		int d11 = i1 - i2 + N;
		int d21 = i2 - i1 + N;
		
		for (j2 = 0; j2 < N; ++j2) {
		    if (!contains(set, i2, j2)) {
			continue;
		    }
		    // printf ("(%d,%d) is in the set.\n", i2, j2);
		    int s2  = j1 + j2;
		    int d12 = j1 - j2 + N;
		    int d22 = j2 - j1 + N;


		    // printf("(%d,%d) + (%d,%d) = (%d,%d)\n", i1, j1, i2, j2, s1, s2);
		    // printf("(%d,%d) - (%d,%d) = (%d,%d)\n", i1, j1, i2, j2, d11-N, d12-N);
		    // printf("(%d,%d) - (%d,%d) = (%d,%d)\n", i2, j2, i1, j1, d21-N, d22-N);

		    //printf("s(%d,%d)\n", s1, s2);
		    //printf("d(%d,%d)\n", d11-N, d12-N);
		    //printf("d(%d,%d)\n", d21-N, d22-N);

		    assert(in_bounds_double_set(s1) && in_bounds_double_set(d11)
			   && in_bounds_double_set(d21) && in_bounds_double_set(s2)
			   && in_bounds_double_set(d12) && in_bounds_double_set(d22));

		    sum_set[s1][s2]    = 1;
		    diff_set[d11][d12] = 1;
		    diff_set[d21][d22] = 1;

		}
	    }
	}
    }

    int sums = 0, diffs = 0;
    int i, j;
    for (i = 0; i < DOUBLE; ++i) {
	for (j = 0; j < DOUBLE; ++j) {
	    sums += sum_set[i][j];
	    diffs += diff_set[i][j];
	}
    }

    //print_set(set);
    //printf("   has %d sums and %d differences\n", sums, diffs);

    return sums > diffs;
}

void make_set(Set s, unsigned long long seed) {
    clear_set(s);

    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    int k = i * N + j;
	    if ((seed >> k) & 1) {
		set(s, i, j, 1);
	    }
	}
    }
}

char my_set[N*N];

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    unsigned long long i = 7130732;
    for (i = 0; i < (1LL << (1 << N)); ++i) {
	make_set(my_set, i);
	if (is_mstd(my_set)) {
	    print_set(my_set);
	    printf("    (with seed %lld)\n", i);

	}
    }
    return 0;
}
