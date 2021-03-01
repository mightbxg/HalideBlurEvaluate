#include <Halide.h>

using namespace Halide;

namespace {

class HalideBlur : public Halide::Generator<HalideBlur> {
public:
    Input<Buffer<uint8_t>> input { "input", 2 };
    Output<Buffer<uint8_t>> output { "output", 2 };

    GeneratorParam<int> tile_x { "tile_x", 32 };
    GeneratorParam<int> tile_y { "tile_y", 8 };

    void generate()
    {
        Var x("x"), y("y");
        Func padded("padded"), padded16("padded16");
        Func blur_x("blur_x"), blur_y("blur_y");

        padded = BoundaryConditions::repeat_edge(input);
        padded16(x, y) = cast<uint16_t>(padded(x, y));
        blur_x(x, y) = (padded16(x - 1, y) + padded16(x, y) + padded16(x + 1, y)) / 3;
        blur_y(x, y) = (blur_x(x, y - 1) + blur_x(x, y) + blur_x(x, y + 1)) / 3;
        output(x, y) = cast<uint8_t>(blur_y(x, y));

        // schedule
        Var xo("xo"), yo("yo"), xi("xi"), yi("yi");
        if (get_target().has_gpu_feature()) {
            output.gpu_tile(x, y, xi, yi, tile_x, tile_y);
        } else {
            output.split(y, y, yi, 8).parallel(y).vectorize(x, 8);
            blur_x.store_at(blur_y, y).compute_at(blur_y, yi).vectorize(x, 8);
        }
    }
};

} // namespace

HALIDE_REGISTER_GENERATOR(HalideBlur, halide_blur)
