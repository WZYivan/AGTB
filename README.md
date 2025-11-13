# Ano Geomatics ToolBox

**AGTB** is a header-only library containing a series of algorithms for **Geomatics Science**.

## Features

- Easy to use: `AGTB` defines a tons of concept and compile time constraint and provides a series of function interface. I wish these design will make it easy to use and provide clear error message.
- Easy to extend: `AGTB` use template specification to replace if-else statements. To extend, you should add something to the `enum`, and specify template for it, others will automatically search for your specification by template parameters. 
- Compile time: Most algorithms in `AGTB` are implemented in template strongly constrained by concept. If you make something wrong, the compiler will give you clear information.
- Error message: `AGTB` defines some macros to throw exceptions with styled information. It tells you which function throw exception and where it happens (`std::source_location`).

## Installation

This is a conan package. Use the following command to install:

```bash
conan create .
```

## Version

```python
version = "0.0.5-1113"
```

## Dependence

- Eigen3/[*] (for matrix operations)
- gcem /[*] (for constexpr math functions)
- C++23 or later
- Conan for package management

## Interface

In `AGTB`, you can execute a family of tasks by a simply named function, such as `Project`, `Solve` or `Adjust`. Such function are a family of override template to handle different tasks. To specify your task, you should do like `using Tp = *TParam< ... >` and `*Param p = { ... }`.

## Adjustment

`AGTB` provide interface `Adjust` to execute adjustment task for traversing or elevation. `Adjust` is a function template, it accepts `*Param<RouteType>` and returns `*AdjustResult`. You can convert them to `std::string` using `T::ToString()` or `AdjustmentTable(*AdjustResult)`. `RouteType` is an enum to specify route type.

### Examples

- Traverse:
```cpp
#include <AGTB/Adjustment/Traverse.hpp>
#include <AGTB/Utils/Angles.hpp> // for class `Angle`
#include <print>

namespace aa = AGTB::Adjustment;
using AGTB::Utils::Angles::Angle;

int main()
{
    aa::TraverseParam<aa::RouteType::Closed> p1{
        .distances = {105.22, 80.18, 129.34, 78.16},
        .angles = {{107, 48, 32}, {73, 0, 24}, {89, 33, 48}, {89, 36, 30}},
        .azi_beg = {125, 30, 0},
        .x_beg = 506.32,
        .y_beg = 215.65};
    for (Angle &a : p1.angles)
    {
        a = a.TakePlace(0);
    }
    auto r1 = aa::Adjust(p1, 2, 0);
    std::println(">>> Closed:\n{}", aa::AdjustmentTable(p1, r1));

    aa::TraverseParam<aa::RouteType::Connecting> p2{
        .distances = {225.85, 139.03, 172.57, 100.07, 102.48},
        .angles = {{99, 1, 0.0}, Angle(167, 45, 36), {123, 11, 24}, Angle(189, 20, 36), Angle(179, 59, 18), {129, 27, 24}},
        .azi_beg = Angle(237, 59, 30),
        .azi_end = Angle(46, 45, 24),
        .x_beg = 2507.65,
        .y_beg = 1215.64,
        .x_end = 2166.70,
        .y_end = 1757.28};
    for (Angle &a : p2.angles)
    {
        a = a.TakePlace(0);
    }
    auto r2 = aa::Adjust(p2, 2, 0);

    std::println(">>> Connecting:\n{}", aa::AdjustmentTable(p2, r2));
}
```

- Elevation:
```cpp
#include <AGTB/Adjustment/Elevation.hpp>
#include <print>
namespace aa = AGTB::Adjustment;

int main()
{
    aa::ElevationParam<aa::RouteType::Closed> p1{
        .distances = {0.8, 0.5, 1.2, 0.5, 1.0},
        .h = {0.230, 0.260, -0.550, -0.450, 0.490},
        .H_beg = 12.000};
    aa::ElevationAdjustResult r1 = aa::Adjust(p1, 3);
    std::println(">>> Closed \n{}", aa::AdjustmentTable(p1, r1));

    aa::ElevationParam<aa::RouteType::Connecting> p2{
        .distances = {1.6, 2.1, 1.7, 2.0},
        .h = {2.331, 2.813, -2.224, 1.430},
        .H_beg = 45.286,
        .H_end = 49.579};
    aa::ElevationAdjustResult r2 = aa::Adjust(p2, 3);
    std::println(">>> Connecting \n{}", aa::AdjustmentTable(p2, r2));
}
```

## Geodesy (Incomplete)

Currently, `AGTB` only provides interface for `GaussKruger Project`. But there are stills a tons of pre-defined coefficients and functions for further developing.

### Examples

- Project:
```cpp
#include <AGTB/Geodesy/Project.hpp>
#include <print>

int main()
{
    namespace ag = AGTB::Geodesy;
    namespace au = AGTB::Utils;
    using au::Angles::Angle;

    using Tp = ag::GaussProjectTParam<ag::EllipsoidType::CGCS2000, ag::GaussZoneInterval::D6>;

    for (int i = 1; i != 6; ++i)
    {
        ag::Longitude
            L_from(115, 0, 0);
        ag::Latitude
            B_from(15 * i, 0, 0);

        Tp::GeoCoord geo_coord_from{
            L_from,
            B_from};

        Tp::GaussCoord gauss_proj_coord =
            ag::Project<Tp>(geo_coord_from);

        Tp::GeoCoord geo_coord_to =
            ag::Project<Tp>(gauss_proj_coord);

        std::println("B_from = {} L_from = {}",
                     B_from.ToAngle().ToString(),
                     L_from.ToAngle().ToString());
        std::println("x = {} y = {} ZoneY = {}", gauss_proj_coord.x, gauss_proj_coord.y, gauss_proj_coord.ZoneY());
        std::println("B_to = {} L_to = {}",
                     geo_coord_to.B.ToAngle().ToString(),
                     geo_coord_to.L.ToAngle().ToString());
        std::println("B_dif = {} L_dif = {}\n",
                     Angle::FromRad(geo_coord_to.B.Rad() - B_from.Rad()).ToString(),
                     Angle::FromRad(geo_coord_to.L.Rad() - L_from.Rad()).ToString());
    }
}
```

## Photogrammetry (Incomplete)

Currently, `AGTB` only provides evaluated interface for `Space Resection`. Other tasks are implemented but not evaluated.

### Examples
- Space Resection:
```cpp
#include <AGTB/Photogrammetry/SpaceResection.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <sstream>

namespace ai = AGTB::IO;
namespace ap = AGTB::Photogrammetry;
namespace al = AGTB::Linalg;

int main()
{

    std::istringstream iss{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};

    ap::Matrix photo(4, 2), obj(4, 3), all(4, 5);
    ai::ReadEigen(iss, all);
    photo = all.leftCols(2),
    obj = all.rightCols(3);

    ai::PrintEigen(photo, "photo"); // mm
    ai::PrintEigen(obj, "obj");
    photo /= 1000; // mm -> m

    ap::InteriorOrientationElements internal{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000,
        .m = 50000};

    ap::SpaceResectionParam p = {
        .interior = internal,
        .photo = std::move(photo),
        .object = std::move(obj)};

    using spTp = ap::SpaceResectionTParam<al::LinalgOption::Cholesky, ap::CollinearityEquationCoeffOption::FullAngles>;
    auto result = ap::Solve<spTp>(p);

    if (result.info == ap::IterativeSolutionInfo::Success)
    {
        std::println(std::cout, "{}", result.ToString());
    }
    else
    {
        throw std::runtime_error("QuickSolve failed");
    }
}
```