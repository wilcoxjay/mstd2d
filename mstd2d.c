#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#define X 5
#define Y 5
#define DOUBLE(x) (2*x + 1)

// 4x6, 3x8, 2x12

char sum_set[DOUBLE(X)][DOUBLE(Y)];
char diff_set[DOUBLE(X)][DOUBLE(Y)];

typedef char* Set;

int in_bounds_set(int i, int j) {
    return 0 <= i && i < X && 0 <= j && j < Y;
}

int in_bounds_double_set(int i, int j) {
    return 0 <= i && i < DOUBLE(X) && 0 <= j && j < DOUBLE(Y);
}

#define LIN(i,j) (i*Y + j)
char get(Set s, int i, int j) {
    assert(in_bounds_set(i,j));
    return s[LIN(i,j)];
}

void set(Set s, int i, int j, char val) {
    assert(in_bounds_set(i,j));
    s[LIN(i,j)] = val;
}

void clear_set(Set s) {
    int i, j;
    for (i = 0; i < X; ++i) {
	for (j = 0; j < Y; ++j) {
	    set(s, i, j, 0);
	}
    }
}

char contains(Set set, int i, int j) {
    return set[LIN(i,j)];
}

void clear_sum_diff_set() {
    int i, j;
    for (i = 0; i < DOUBLE(X); ++i) {
	for (j = 0; j < DOUBLE(Y); ++j) {
	    sum_set[i][j] = 0;
	    diff_set[i][j] = 0;
	}
    }
}

void print_set(Set set) {
    printf("{");
    int i, j, started = 0;
    for (i = 0; i < X; ++i) {
	for (j = 0; j < Y; ++j) {
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
    for (i1 = 0; i1 < X; ++i1) {
	for (j1 = 0; j1 < Y; ++j1) {
	    if (!contains(set, i1, j1)) {
		continue;
	    }
	    //printf ("(%d,%d) is in the set.\n", i1, j1);
	    for (i2 = i1; i2 < X; ++i2) {
		int s1  = i1 + i2;
		int d11 = i1 - i2 + X;
		int d21 = i2 - i1 + X;
		
		for (j2 = 0; j2 < Y; ++j2) {
		    if (!contains(set, i2, j2)) {
			continue;
		    }
		    //printf ("(%d,%d) is in the set.\n", i2, j2);
		    int s2  = j1 + j2;
		    int d12 = j1 - j2 + Y;
		    int d22 = j2 - j1 + Y;


		    // printf("(%d,%d) + (%d,%d) = (%d,%d)\n", i1, j1, i2, j2, s1, s2);
		    // printf("(%d,%d) - (%d,%d) = (%d,%d)\n", i1, j1, i2, j2, d11-X, d12-Y);
		    // printf("(%d,%d) - (%d,%d) = (%d,%d)\n", i2, j2, i1, j1, d21-X, d22-Y);

		    // printf("s(%d,%d)\n", s1, s2);
		    // printf("d(%d,%d)\n", d11-N, d12-N);
		    // printf("d(%d,%d)\n", d21-N, d22-N);

		    assert(in_bounds_double_set(s1,s2) &&
			   in_bounds_double_set(d11,d12) &&
			   in_bounds_double_set(d21,d22));

		    sum_set[s1][s2]    = 1;
		    diff_set[d11][d12] = 1;
		    diff_set[d21][d22] = 1;

		}
	    }
	}
    }

    int sums = 0, diffs = 0;
    int i, j;
    for (i = 0; i < DOUBLE(X); ++i) {
	for (j = 0; j < DOUBLE(Y); ++j) {
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
    for (i = 0; i < X; ++i) {
	for (j = 0; j < Y; ++j) {
	    int k = LIN(i,j);
	    if ((seed >> k) & 1) {
		set(s, i, j, 1);
	    }
	}
    }
}

char my_set[X*Y];

#define UPPER_BOUND (1LL << (X*Y))

int main(int argc, char **argv) {
    int id;
    unsigned long long step;
    if (argc == 3) {
	id = atoi(argv[1]);
	int max = atoi(argv[2]);
	step = ((UPPER_BOUND + max - 1) / max);
    } else {
	// default to searching whole space
	id = 0;
	step = UPPER_BOUND;  
    }

    unsigned long long i;
    unsigned long long start = id * step;
    unsigned long long end = (id + 1) * step;
    printf("thread id %d searching from %lld to %lld.\n", id, start, end);
    fflush(stdout);
    for (i = start; i < end ; ++i) {
	make_set(my_set, i);
	if (is_mstd(my_set)) {
	    print_set(my_set);
	    printf("    (with seed %lld)\n", i);
	    fflush(stdout);
	}
    }
    return 0;
}
