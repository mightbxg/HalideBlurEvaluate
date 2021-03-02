#include "HalideBuffer.h"
#include "halide_blur.h"

#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

struct EmptyFunc {
    void operator()() { }
};

template <typename Func, typename Sync = EmptyFunc>
double benchmark(unsigned samples, unsigned iterations, Func func, Sync sync = Sync())
{
    assert(samples > 0 && iterations > 0);
    double best_time = numeric_limits<double>::infinity();
    for (unsigned s = 0; s < samples; ++s) {
        auto t1 = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < iterations; ++i) {
            func();
        }
        sync();
        best_time = min(best_time, chrono::duration<double>(chrono::high_resolution_clock::now() - t1).count());
    }
    best_time = best_time / iterations * 1e3; // milliseconds
    return best_time;
}

int main(int argc, char* argv[])
{
    auto md = halide_blur_metadata();
    cout << "target: " << md->target << endl;
    string fn_input = argc > 1 ? argv[1] : "../../gray.png";

    // opencv
    cv::Mat img_src = imread(fn_input, cv::IMREAD_GRAYSCALE);
    if (img_src.empty()) {
        cout << "cannot load source image: " << fn_input << endl;
        return 0;
    }
    const int rows = img_src.rows, cols = img_src.cols;
    cv::Mat img_ocv;
    double time_ocv = benchmark(10, 1000, [&] {
        cv::blur(img_src, img_ocv, cv::Size(3, 3), cv::Point(-1, -1), cv::BORDER_REFLECT);
    });
    cout << "time of ocv   : " << time_ocv << " ms" << endl;

    // halide
    cv::Mat img_halide(img_src.size(), CV_8UC1);
    Halide::Runtime::Buffer<uint8_t> input(img_src.data, cols, rows);
    input.set_host_dirty();
    Halide::Runtime::Buffer<uint8_t> output(img_halide.data, cols, rows);
    // call it once to initialize the halide runtime stuff
    halide_blur(input, output);
    output.copy_to_host();
    double time_halide = benchmark(
        10, 1000,
        [&] { halide_blur(input, output); },
        [&] { output.copy_to_host(); });
    cout << "time of halide: " << time_halide << " ms" << endl;

    // difference
    cv::imwrite("blur_halide.png", img_halide);
    cv::Mat diff;
    cv::absdiff(img_ocv, img_halide, diff);
    diff = diff - (diff == 1); // wipe out minor difference
    cout << "difference: " << cv::countNonZero(diff) << endl;
    return 0;
}
