#include <omp.h>
#include <random>

#include "hit.h"
#include "parse.hpp"

static const int32_t kChunkSize = 350;

uint64_t GetIterAmount(const std::string& input_file) {
    std::ifstream in(input_file, std::ios::binary);
    std::string data;

    if (std::getline(in, data)) {
        return std::stoll(data);
    }

    in.close();

    return 0;
}

int main(int argc, char** argv) {
    Arguments args;
    ArgParser parser(args);
    parser.Parse(argc, argv);

    const float* axis_range = get_axis_range();
    uint64_t iter_amount = GetIterAmount(args.GetInputFilename());

    if (iter_amount == 0) {
        ErrorHandler::ShowError(ErrorHandler::kInvalidIterationsNumber);
    }

    omp_set_num_threads(args.GetThreadsCount());
    uint64_t hits_count = 0;

    double start = omp_get_wtime();

#pragma omp parallel if (!args.GetIfNoOmp())
    {
        uint64_t local_hits_count = 0;
        thread_local std::mt19937 generator(std::random_device{}());
        const float* local_axis_range = get_axis_range();
        std::uniform_real_distribution<float> x(local_axis_range[0], local_axis_range[1]);
        std::uniform_real_distribution<float> y(local_axis_range[2], local_axis_range[3]);
        std::uniform_real_distribution<float> z(local_axis_range[4], local_axis_range[5]);

#pragma omp for schedule(static, kChunkSize) nowait
        for (uint64_t k = 0; k < iter_amount; ++k) {
            if (hit_test(x(generator), y(generator), z(generator))) {
                ++local_hits_count;
            }
        }
#pragma omp atomic
         hits_count += local_hits_count;
    }

    double end = omp_get_wtime();
    end -= start;
    end *= 1000;

    double v = static_cast<double>(hits_count) / iter_amount;
    v *= ((axis_range[1] - axis_range[0]) * (axis_range[3] - axis_range[2]) * (axis_range[5] - axis_range[4]));

    std::ofstream out(args.GetOutputFilename(), std::ios::binary);
    out << v << '\n';
    out.close();

    int threads_in_ans = 0;
    if (!args.GetIfNoOmp()) {
        threads_in_ans = args.GetThreadsCount();
    }

    printf("Time %i thread(s)): %g ms\n", threads_in_ans, end);

    return 0;
}
