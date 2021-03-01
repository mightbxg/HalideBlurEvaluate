#include "halide_blur.h"
#include <HalideBuffer.h>
#include <chrono>
#include <halide_image_io.h>
#include <iostream>

using namespace std;
using namespace Halide;

auto now()
{
    return chrono::high_resolution_clock::now();
}

template <typename T1, typename T2>
double elapsed(const T1& t1, const T2& t2)
{
    return chrono::duration<double>(t2 - t1).count() * 1e3;
}

int main(int argc, char* argv[])
{
    auto md = halide_blur_metadata();
    cout << "target: " << md->target << endl;
    string fn_input = argc > 1 ? argv[1] : "../../gray.png";
    Runtime::Buffer<uint8_t> input = Tools::load_image(fn_input);
    Runtime::Buffer<uint8_t> output(input.width(), input.height());

    constexpr int samples = 10;
    constexpr int iterations = 100;
    double best_time = numeric_limits<double>::infinity();
    for (int s = 0; s < samples; ++s) {
        auto t1 = now();
        for (int i = 0; i < iterations; ++i) {
            halide_blur(input, output);
        }
        best_time = min(best_time, elapsed(t1, now()));
    }
    best_time /= iterations;
    cout << "best_time: " << best_time << " ms" << endl;

    Tools::save_image(output, "blur.png");

    return 0;
}
