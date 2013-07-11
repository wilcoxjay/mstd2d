#include <G3D/Random.h>

#include <iostream>

#include <cstring>

#include <ctime>


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

int NUM_TRIALS = 1000000;
int REPORT_FREQ = 10000;
int N = 100;
bool print_sets = true;

char *set;
char *sumset;
char *diffset;

int sum_count;
int diff_count;

void process_args(int argc, char **argv) {
    --argc; ++argv;
    while (argc > 0) {
        if (strcmp("-N", argv[0]) == 0) {
            --argc; ++argv;
            N = read_int_or_error(argc, argv);
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
        } else if (strcmp("-help", argv[0]) == 0) {
            std::cout << "Options:" << std::endl
                      << "    -N NUM\t\tSample subsets of {0, ..., NUM-1}. (Default=" << N << ".)" << std::endl
                      << "    -trials NUM\t\tTake this many samples (Default=" << NUM_TRIALS << ".)" << std::endl
                      << "    -report NUM\t\tPrint a 'still alive' message after every NUM trials (Default=" << REPORT_FREQ << ".)" << std::endl
                      << "    -help\t\tPrint this message" << std::endl;
            exit(0);
        } else {
            std::cout << "unknown option " << argv[0] << std::endl;
            exit(1);
        }
    }
}

bool is_mstd() {
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
    
    sum_count = 0;
    diff_count = 0;
    for (int i = 0; i < 2*N; ++i) {
        sum_count += sumset[i];
        diff_count += diffset[i];
    }

    return sum_count > diff_count;
}

void seed(G3D::Random& r) {
    G3D::uint32 data = r.bits();
    for (int i = 0; i < N; ++i) {
        if (i % 32 == 31) {
            data = r.bits();
        }
        set[i] = data & 1;
        data >>= 1;
    }
}

void print_set() {
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
    time_t t = time(0);
    std::cerr << "(current time as uint32 = " << (G3D::uint32)t << ")" << std::endl;

    G3D::Random r((G3D::uint32)t);

    process_args(argc, argv);

    set     = (char*)malloc(N);
    sumset  = (char*)malloc(2*N);
    diffset = (char*)malloc(2*N);

    int num_mstds = 0;
    for (int i = 0; i < NUM_TRIALS; ++i) {
        if (i % REPORT_FREQ == 0) {
            std::cerr << "i = " << i << std::endl;
        }

        seed(r);

        if (is_mstd()) {
            num_mstds++;
            if (print_sets) {
                print_set();
                std::cout << "  has " << sum_count << " sums and " << diff_count << " differences." << std::endl;
            }
        }
    }

    std::cout << "done. found " << num_mstds << " after " << NUM_TRIALS << " trials. ratio = " << ((double)num_mstds)/((double)NUM_TRIALS) << std::endl;
}
