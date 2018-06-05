#ifndef SIMPLELIGHTING_H_
#define SIMPLELIGHTING_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>

inline glm::vec3 Simplelight(float theta, float phi)
{
    // if (fabs(theta) <= M_PI / 8.0f)
    // {
    //     return glm::vec3(8.0f, 8.0f, 8.0f);
    // }
    //
    // return glm::vec3(0.0f, 0.0f, 0.0f);

    const float scalar = 4.0f;
    float monochromatic = std::max(0.0f, 5 * cos(theta) - 4) + std::max(
        0.0, -4 * sin(theta - M_PI) * cos(phi - 2.5) - 3);
    return scalar * glm::vec3(monochromatic, monochromatic, monochromatic);
}

#endif
