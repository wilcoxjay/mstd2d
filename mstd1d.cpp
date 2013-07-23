#include <G3D/Random.h>

#include <iostream>

#include <cassert>
#include <cstring>

#include <ctime>


#include <omp.h>

int max(int a, int b) {
    return a < b ? b : a;
}

int read_int_or_error(int argc, char** argv) {
    int result;
    if (argc == 0) {
        std::cout << "requires an argument." << std::endl;
        exit(1);
    }
    char* has_error = 0;
    result = (int) strtol(argv[0], &has_error, 10);
    if (*has_error != 0) {
        std::cout << "could not convert argument " << argv[0] << " to int. unconvertible part: " << has_error << std::endl;
        exit(1);
    }

    return result;
}

double read_double_or_error(int argc, char** argv) {
    double result;
    if (argc == 0) {
        std::cout << "requires an argument." << std::endl;
        exit(1);
    }
    char* has_error = 0;
    result =  strtod(argv[0], &has_error);
    if (*has_error != 0) {
        std::cout << "could not convert argument " << argv[0] << " to double. unconvertible part: " << has_error << std::endl;
        exit(1);
    }

    return result;
}


long long NUM_TRIALS = 1000000LL;
int REPORT_FREQ = 10000;
int N = 100;
double p = 0.5;
bool print_mstds = true;
bool print_bals = false;
bool print_diffdoms = false;
bool report_symmetry = false;
bool try_all = false;
bool check = false;
int fringe = 0;
int removeLow = 0;
int removeHi = -1;  // -1 so that we can check if enabled using removeLow <= removeHi

const char* conflicts[][2] = {{"-p", "-check"}, {"-trials", "-check"}, {"-report", "-check"}, {"-try-all", "-check"}, {"-no-print-sets", "-check"}};

void process_args(int argc, char **argv) {
    --argc; ++argv;

    int i, j, k;
    for (i = 0; i < (int) (sizeof(conflicts)/(2*sizeof(char*))); ++i) {
        for (j = 0; j < argc; ++j) {
            for (k = 0; k < argc; ++k) {
                if (j == k) continue;
                if (strcmp(conflicts[i][0], argv[j]) == 0 && strcmp(conflicts[i][1], argv[k]) == 0) {
                    std::cerr << "Options " << conflicts[i][0] << " and " << conflicts[i][1] << " conflict." << std::endl;
                    exit(1);
                }
            }
        }
    }

    while (argc > 0) {
        if (strcmp("-N", argv[0]) == 0) {
            --argc; ++argv;
            N = read_int_or_error(argc, argv);
            --argc; ++argv;
        } else if (strcmp("-p", argv[0]) == 0) {
            --argc; ++argv;
            p = read_double_or_error(argc, argv);
            --argc; ++argv;
        } else if (strcmp("-trials", argv[0]) == 0) {
            --argc; ++argv;
            NUM_TRIALS = read_int_or_error(argc, argv);
            --argc; ++argv;
        } else if (strcmp("-report", argv[0]) == 0) {
            --argc; ++argv;
            REPORT_FREQ = read_int_or_error(argc, argv);
            --argc; ++argv;
        } else if (strcmp("-no-print-mstds", argv[0]) == 0) {
            --argc; ++argv;
            print_mstds = false;
        } else if (strcmp("-print-diffdoms", argv[0]) == 0) {
            --argc; ++argv;
            print_diffdoms = true;
        } else if (strcmp("-print-bals", argv[0]) == 0) {
            --argc; ++argv;
            print_bals = true;
        } else if (strcmp("-report-symmetry", argv[0]) == 0) {
            --argc; ++argv;
            report_symmetry = true;
        } else if (strcmp("-try-all", argv[0]) == 0) {
            --argc; ++argv;
            try_all = true;
            NUM_TRIALS = 1LL << N;
        } else if (strcmp("-check", argv[0]) == 0) {
            --argc; ++argv;
            check = true;
        } else if (strcmp("-fringe", argv[0]) == 0) {
            --argc; ++argv;
            fringe = read_int_or_error(argc, argv);
            assert(fringe > 0);
            --argc; ++argv;
        } else if (strcmp("-remove", argv[0]) == 0) {
            --argc; ++argv;
            removeLow = read_int_or_error(argc, argv);
            --argc; ++argv;
            assert(removeLow >= 0);
            removeHi = read_int_or_error(argc, argv);
            --argc; ++argv;
            assert(removeHi >= removeLow);
            assert(removeHi < N);

       } else if (strcmp("-help", argv[0]) == 0) {
            std::cout << "Options:" << std::endl
                      << "    -N NUM\t\tSample subsets of {0, ..., NUM-1}. (Default=" << N << ".)" << std::endl
                      << "    -p PROB\t\tSample each element with probability PROB. (Default=" << p << ".)" << std::endl
                      << "    -trials NUM\t\tTake this many samples (Default=" << NUM_TRIALS << ".)" << std::endl
                      << "    -report NUM\t\tPrint a 'still alive' message after every NUM trials (Default=" << REPORT_FREQ << ".) If 0, don't report." << std::endl
                      << "    -no-print-mstds\tDon't print when an MSTD set is found." << std::endl
                      << "    -print-bals\tPrint when a balanced set is found." << std::endl
                      << "    -print-diffdoms\tPrint when a difference dominated set is found." << std::endl
                      << "    -report-symmetry\tIf a set is going to be printed and is symmetric, append a note to that effect." << std::endl
                      << "    -try-all\t\tInstead of sampling randomly, just try every subset. Requires N < 64." << std::endl
                      << "    -check\t\tInstead of searching for MSTD sets, read sets one per line on stdin and compute sumsets and diffsets. (Default=" << check << ")." << std::endl
                      << "    -fringe NUM\t\tInstead of computing the entire sumset/diffset, compute only the contribution from the NUM-fringe. 0 to disable. (Default=" << fringe << ")." << std::endl
                      << "    -remove LOW HI\t\tRemove all elements in range [LOW,HI], inclusive, before processing. Requires -check. (Default=[empty range])." << std::endl
                      << "    -help\t\tPrint this message" << std::endl;

            exit(0);
        } else {
            std::cout << "unknown option " << argv[0] << std::endl;
            exit(1);
        }
    }

    if (removeLow <= removeHi && !check) {
        std::cout << "-remove requires -check" << std::endl;
        exit(1);
    }

    if (try_all && N >= 64) {
        std::cout << "Cannot try all subsets when N >= 64." << std::endl;
        exit(1);
    }
}

void do_remove(char* set) {
    assert(removeLow <= removeHi);
    assert (removeHi < N);

    int dist = removeHi - removeLow + 1;
    for (int i = removeLow; i < N; i++) {
        set[i] = i + dist < N ? set[i+dist] : 0;
    }
}

bool is_mstd(const char* set, char* sumset, char* diffset, int* sum_count, int* diff_count) {
    memset(sumset, 0, 2*N);
    memset(diffset, 0, 2*N);

    for (int i = 0; i < N; ++i) {
        if (set[i] == 0) continue;
        for (int j = i; j < N; ++j) {
            if (set[j] == 0) continue;
            sumset[i + j] = 1;
            diffset[i - j + N] = 1;
            diffset[j - i + N] = 1;
        }
    }

    int sums = 0;
    int diffs = 0;
    for (int i = 0; i < 2*N; ++i) {
        sums += sumset[i];
        diffs += diffset[i];
    }

    *sum_count = sums;
    *diff_count = diffs;

    return sums > diffs;
}

bool is_fringe_mstd(int fringe, const char* set, char* sumset, char* diffset, int* sum_count, int* diff_count) {
    memset(sumset, 0, 2*N);
    memset(diffset, 0, 2*N);

    for (int i = 0; i < fringe; ++i) {
        if (set[i] == 0) continue;
        for (int j = i; j < fringe; ++j) {
            if (set[j] == 0) continue;
            sumset[i + j] = 1;
            diffset[i - j + N] = 1;
            diffset[j - i + N] = 1;
        }
    }
    for (int i = 1; i <= fringe; ++i) {
        if (set[N-i] == 0) continue;
        for (int j = 0; j < fringe; ++j) {
            if (set[j] == 0) continue;
            sumset[N - i + j] = 1;
            diffset[N - i - j + N] = 1;
            diffset[j + i] = 1;  // j - (N - i) + N = j + i
        }
    }
    for (int i = 1; i <= fringe; ++i) {
        if (set[N-i] == 0) continue;
        for (int j = i; j <= fringe; ++j) {
            if (set[N-j] == 0) continue;
            sumset[(N - i) + (N - j)] = 1;
            diffset[(N - i) + j] = 1; // -(N - j) + N = j
            diffset[(N - j) + i] = 1; // -(N - i) + N = i
        }
    }

    int sums = 0;
    int diffs = 0;
    for (int i = 0; i < fringe; ++i) {
        sums += sumset[i];
        diffs += diffset[i];
    }
    for (int i = max(fringe, 2*N - fringe - 1); i < 2*N ; ++i) {
        sums += sumset[i];
        diffs += diffset[i];
    }

    *sum_count = sums;
    *diff_count = diffs;

    return sums > diffs;
}

void seedFast(G3D::Random& r, char* set) {
    G3D::uint32 data = r.bits();
    for (int i = 0; i < N; ++i) {
        if (i % 32 == 31) {
            data = r.bits();
        }
        set[i] = data & 1;
        data >>= 1;
    }
}

void seedProb(G3D::Random& r, char* set) {
    for (int i = 0; i < N; ++i) {
        if (r.uniform() < p) {
            set[i] = 1;
        } else {
            set[i] = 0;
        }
    }
}

void seed(long long s, char* set) {
    for (int i = 0; i < N; ++i) {
        set[i] = s & 1;
        s >>= 1;
    }
}

int count_set(const char* set) {
    int result = 0;
    for (int i = 0; i < N; ++i) {
        result += set[i];
    }
    return result;
}
bool symmetric(const char* set) {
    int maxI = 0;
    for (int i = N-1; i >= 0; --i) {
        if (set[i] != 0) {
            maxI = i;
            break;
        }
    }
    for (int i = 0; i <= maxI; ++i) {
        if (set[i] != set[maxI - i]) {
            return false;
        }
    }
    return true;
}

void print_set(const char* set) {
    std::cout << "{";
    bool started = false;
    for (int i = 0; i < N; ++i) {
        if (set[i] != 0) {
            if (started) {
                std::cout << ", ";
            }
            started = true;

            std::cout << i;
        }
    }
    std::cout << "}" << std::endl;;
}

void report(char *set, int sum_count, int diff_count) {
#                       pragma omp critical
    {
        print_set(set);
        std::cout << " (of size " << count_set(set) << ") has " << sum_count << " sums and " << diff_count << " differences.";
        if (fringe > 0) {
            std::cout << " Note fringe size = " << fringe << ".";
        }
        if (report_symmetry && symmetric(set)) {
            std::cout << " Note: this set is symmetric.";
        }
        std::cout << std::endl;
    }
}

void count_and_report(char* set, char* sumset, char* diffset, long long* mstd, long long* bal, long long* diff) {
    int sum_count, diff_count;

    bool result;
    if (fringe > 0) {
        result = is_fringe_mstd(fringe, set, sumset, diffset, &sum_count, &diff_count);
    } else {
        result = is_mstd(set, sumset, diffset, &sum_count, &diff_count);
    }

    if (result) {
        if (print_mstds) {
            report(set, sum_count, diff_count);
        }
        (*mstd)++;
    } else if (sum_count == diff_count) {
        if (print_bals) {
            report(set, sum_count, diff_count);
        }
        (*bal)++;
    } else {
        if (print_diffdoms) {
            report(set, sum_count, diff_count);
        }
        (*diff)++;
    }
}


int main(int argc, char **argv) {
    process_args(argc, argv);

    long long num_mstds = 0;
    long long num_balanced = 0;
    long long num_diffdom = 0;

#   pragma omp parallel
    {
        time_t t = time(0);

        G3D::Random r((G3D::uint32)t + 13 * omp_get_thread_num());
        long long my_num_mstds = 0;
        long long my_num_balanced = 0;
        long long my_num_diffdom = 0;

        char *set     = (char*)malloc(N);
        char *sumset  = (char*)malloc(2*N);
        char *diffset = (char*)malloc(2*N);

        if (check) {
#pragma omp master
            {
                NUM_TRIALS = 0;
                while (1) {
                    memset(set, 0, N);
                    int err;
                    int n;
                    while (1 == (err = scanf("%d", &n))) {
                        assert(n < N);
                        set[n] = 1;
                    }
                    if (err == EOF) break;
                    NUM_TRIALS++;
                    if (removeLow <= removeHi) {
                        do_remove(set);
                    }
                    count_and_report(set, sumset, diffset, &my_num_mstds, &my_num_balanced, &my_num_diffdom);
                    if (getchar() == EOF) break;  // otherwise, throw away one character and try for the next set
                }
            }
        } else {
#           pragma omp for nowait schedule(guided)
            for (long long i = 0; i < NUM_TRIALS; ++i) {
                if (REPORT_FREQ > 0 && i % REPORT_FREQ == 0) {
#                   pragma omp critical
                    {
                        std::cerr << "i = " << i << std::endl;
                    }
                }

                if (try_all) {
                    seed(i, set);
                } else {
                    if (fabs(p - 0.5) < 0.001) {
                        seedFast(r, set);
                    } else {
                        seedProb(r, set);
                    }
                }
                count_and_report(set, sumset, diffset, &my_num_mstds, &my_num_balanced, &my_num_diffdom);
            }
        }
#       pragma omp atomic
        num_mstds += my_num_mstds;
#       pragma omp atomic
        num_balanced += my_num_balanced;
#       pragma omp atomic
        num_diffdom += my_num_diffdom;
    }

    std::cout << "done. found (" << num_mstds << "," << num_balanced << "," << num_diffdom << ") (MSTD,bal,diff) sets after "
              << NUM_TRIALS << " trials. ratio = " << ((double)num_mstds)/((double)NUM_TRIALS) << std::endl;
}
