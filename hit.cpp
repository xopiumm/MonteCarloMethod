#include "hit.h"

bool hit_test(float x, float y, float z) {
    if ((2.0f * 2.0f * (y * y + z * z) - 2.0f * x * x * x + x * x * x * x) <= 0.0f) {
        return true;
    }

    return false;
}

const float* get_axis_range() {
    static const float range[6] = {0, 2, -0.65, 0.65, -0.65, 0.65};
    return range;
}
