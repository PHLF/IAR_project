#ifndef UTILS_H
#define UTILS_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <numbers>

namespace sim
{

template <class... Ts> struct overloaded : Ts...
{
    using Ts::operator()...;
};


using ffloat = double;

struct Coords
{
    float x;
    float y;
};

struct Line
{
    float a;
    float b;
};

struct Angle
{
    constexpr Angle(int degrees) : degrees((degrees + 360) % 360) {}
    operator int() { return degrees; }

    int16_t degrees{};
};

constexpr auto operator+(Angle lhs, Angle rhs) -> Angle
{
    return {lhs.degrees + rhs.degrees};
}

constexpr auto operator""_o(unsigned long long degrees) -> Angle
{
    return {static_cast<int>(degrees)};
}

constexpr auto pwl_cos(int angle)
{
    static constexpr std::array COS_LERP_TABLE{
        Line{ -0.016424814709723144,  1.478233323875082},
        Line{ -0.012270755498886779, 1.2471718433532035},
        Line{ -0.008125280351372666,  1.104663088738287},
        Line{-0.0051534137365287684, 1.0415224600334059},
        Line{ -0.003216433591184367, 1.0160883165176824},
        Line{-0.0019953215550787447,  1.006178632193012},
        Line{-0.0012349396401846444, 1.0023649153320713},
        Line{-0.0004720063435395209,                1.0}
    };
    static constexpr auto PHI_INV = 1.0 / std::numbers::phi;

    auto   theta = 90.0;
    size_t i     = 0;
    for (; i + 1 < COS_LERP_TABLE.size() && angle < (theta *= PHI_INV); ++i) {}
    auto const [a, b] = COS_LERP_TABLE[i];

    return a * angle + b;
}

constexpr auto dist(Coords const& a, Coords const& b) -> ffloat
{
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

constexpr auto cos(Angle angle) -> sim::ffloat
{
    if (angle.degrees < 90)  return  pwl_cos(angle);
    if (angle.degrees < 180) return -pwl_cos(180 - angle);
    if (angle.degrees < 270) return -pwl_cos(angle - 180);
    return                           pwl_cos(360 - angle);
}

constexpr auto sin(Angle angle) -> sim::ffloat
{
    return -cos(angle + 90_o);
}

} // namespace sim

#endif // UTILS_H
