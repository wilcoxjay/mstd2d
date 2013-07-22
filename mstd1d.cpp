#include <G3D/Random.h>

#include <iostream>

#include <cstring>

#include <ctime>


#include <omp.h>

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
bool print_sets = true;
bool try_all = false;
bool check = false;

void process_args(int argc, char **argv) {
    --argc; ++argv;
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
        } else if (strcmp("-no-print-sets", argv[0]) == 0) {
            --argc; ++argv;
            print_sets = false;
        } else if (strcmp("-try-all", argv[0]) == 0) {
            --argc; ++argv;
            try_all = true;
            NUM_TRIALS = 1LL << N;
       } else if (strcmp("-help", argv[0]) == 0) {
            std::cout << "Options:" << std::endl
                      << "    -N NUM\t\tSample subsets of {0, ..., NUM-1}. (Default=" << N << ".)" << std::endl
                      << "    -p PROB\t\tSample each element with probability PROB. (Default=" << p << ".)" << std::endl
                      << "    -trials NUM\t\tTake this many samples (Default=" << NUM_TRIALS << ".)" << std::endl
                      << "    -report NUM\t\tPrint a 'still alive' message after every NUM trials (Default=" << REPORT_FREQ << ".) If 0, don't report." << std::endl
                      << "    -no-print-sets\tDon't print when a set is found." << std::endl
                      << "    -try-all\t\tInstead of sampling randomly, just try every subset. Requires N < 64." << std::endl
                      << "    -help\t\tPrint this message" << std::endl;
            exit(0);
        } else {
            std::cout << "unknown option " << argv[0] << std::endl;
            exit(1);
        }
    }

    if (try_all && N >= 64) {
        std::cout << "Cannot try all subsets when N >= 64." << std::endl;
        exit(1);
    }
}

bool is_mstd(const char* set, char* sumset, char* diffset, int *sum_count, int *diff_count) {
    memset(sumset, 0, 2*N);
    memset(diffset, 0, 2*N);

    int sums, diffs;
    for (int i = 0; i < N; ++i) {
        if (set[i] == 0) continue;
        for (int j = i; j < N; ++j) {
            if (set[j] == 0) continue;
            sumset[i + j] = 1;
            diffset[i - j + N] = 1;
            diffset[j - i + N] = 1;
        }
    }

    sums = 0;
    diffs = 0;
    for (int i = 0; i < 2*N; ++i) {
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

int main(int argc, char **argv) {
    process_args(argc, argv);


    long long num_mstds = 0;
    long long num_balanced = 0;
    long long num_diffdom = 0;

#pragma omp parallel
    {
        time_t t = time(0);

        G3D::Random r((G3D::uint32)t + 13 * omp_get_thread_num());
        long long my_num_mstds = 0;
        long long my_num_balanced = 0;
        long long my_num_diffdom = 0;

        int sum_count, diff_count;

        char *set     = (char*)malloc(N);
        char *sumset  = (char*)malloc(2*N);
        char *diffset = (char*)malloc(2*N);

#pragma omp for nowait schedule(guided)
        for (long long i = 0; i < NUM_TRIALS; ++i) {
            if (REPORT_FREQ > 0 && i % REPORT_FREQ == 0) {
                std::cerr << "i = " << i << std::endl;
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

            if (is_mstd(set, sumset, diffset, &sum_count, &diff_count)) {
                my_num_mstds++;
                if (print_sets) {
                    #pragma omp critical
                    {
                        print_set(set);
                        std::cout << " (of size " << count_set(set) << ") has " << sum_count << " sums and " << diff_count << " differences." << std::endl;
                    }
                }
            } else if (sum_count == diff_count) {
                my_num_balanced++;
            } else {
                my_num_diffdom++;
            }
        }
        #pragma omp atomic
        num_mstds += my_num_mstds;
        #pragma omp atomic
        num_balanced += my_num_balanced;
        #pragma omp atomic
        num_diffdom += my_num_diffdom;
    }

    std::cout << "done. found (" << num_mstds << "," << num_balanced << "," << num_diffdom << ") (MSTD,bal,diff) sets after "
              << NUM_TRIALS << " trials. ratio = " << ((double)num_mstds)/((double)NUM_TRIALS) << std::endl;
}
