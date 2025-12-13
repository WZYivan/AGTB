# Ano Geomatics ToolBox (AGTB)

**AGTB** is a header-only C++ library providing comprehensive algorithms for **Geomatics Science**.

## Features

- **Easy to Use**: Extensive use of concepts and compile-time constraints with clear function interfaces, providing intuitive error messages
- **Easy to Extend**: Template specialization replaces complex if-else statements - simply add to enums and specialize templates
- **Compile-Time Optimized**: Most algorithms implemented as templates with strong concept constraints
- **Clear Error Messages**: Custom macros with styled exception information including function names and source locations (`std::source_location`)

## Installation

AGTB is available as a Conan package:

```bash
conan create .
```

## Version

**Current Version**: `0.0.5-1113`

## Dependencies

- **Eigen3** - Matrix operations
- **gcem** - Constexpr math functions
- **C++23** or later
- **Conan** - Package management

## Quick Start

AGTB provides unified interfaces for common geomatics tasks through simple function names like `Project`, `Solve`, and `Adjust`. Specify your task using template parameters like `*TParam<...>` and configure with `*Param` structures.

## Modules

### Adjustment

Perform adjustment calculations for traversing and elevation networks using the `Adjust` function template.

#### Traverse Adjustment

```cpp
#include <AGTB/Adjustment/Traverse.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>

namespace aa = AGTB::Adjustment;
using AGTB::Utils::Angles::Angle;

int main() {
    // Closed traverse example
    aa::TraverseParam<aa::RouteType::Closed> closed_params{
        .distances = {105.22, 80.18, 129.34, 78.16},
        .angles = {{107, 48, 32}, {73, 0, 24}, {89, 33, 48}, {89, 36, 30}},
        .azi_beg = {125, 30, 0},
        .x_beg = 506.32,
        .y_beg = 215.65
    };
    
    for (Angle &a : closed_params.angles) {
        a = a.TakePlace(0);
    }
    
    auto closed_result = aa::Adjust(closed_params, 2, 0);
    std::println(">>> Closed Traverse:\n{}", 
                 aa::AdjustmentTable(closed_params, closed_result));

    // Connecting traverse example  
    aa::TraverseParam<aa::RouteType::Connecting> connecting_params{
        .distances = {225.85, 139.03, 172.57, 100.07, 102.48},
        .angles = {{99, 1, 0.0}, Angle(167, 45, 36), {123, 11, 24}, 
                   Angle(189, 20, 36), Angle(179, 59, 18), {129, 27, 24}},
        .azi_beg = Angle(237, 59, 30),
        .azi_end = Angle(46, 45, 24),
        .x_beg = 2507.65,
        .y_beg = 1215.64,
        .x_end = 2166.70,
        .y_end = 1757.28
    };
    
    for (Angle &a : connecting_params.angles) {
        a = a.TakePlace(0);
    }
    
    auto connecting_result = aa::Adjust(connecting_params, 2, 0);
    std::println(">>> Connecting Traverse:\n{}", 
                 aa::AdjustmentTable(connecting_params, connecting_result));
}
```

#### Elevation Adjustment

```cpp
#include <AGTB/Adjustment/Elevation.hpp>
#include <print>

namespace aa = AGTB::Adjustment;

int main() {
    // Closed elevation network
    aa::ElevationParam<aa::RouteType::ClosedLoop> closed_params{
        .distances = {0.8, 0.5, 1.2, 0.5, 1.0},
        .h = {0.230, 0.260, -0.550, -0.450, 0.490},
        .H_beg = 12.000
    };
    
    auto closed_result = aa::Adjust(closed_params, 3);
    std::println(">>> ClosedLoop Elevation:\n{}", 
                 aa::AdjustmentTable(closed_params, closed_result));

    // Connecting elevation network
    aa::ElevationParam<aa::RouteType::Connecting> connecting_params{
        .distances = {1.6, 2.1, 1.7, 2.0},
        .h = {2.331, 2.813, -2.224, 1.430},
        .H_beg = 45.286,
        .H_end = 49.579
    };
    
    auto connecting_result = aa::Adjust(connecting_params, 3);
    std::println(">>> Connecting Elevation:\n{}", 
                 aa::AdjustmentTable(connecting_params, connecting_result));
}
```

#### Elevation Net Adjustment

To adjust an elevation net, **AGTB** uses a boost::graph lib to represent its structure and wrap is to a new class with simpler api for user.

```cpp
#include <AGTB/Adjustment/ElevationNet.hpp>

namespace aa = AGTB::Adjustment;

int main()
{
    aa::ElevationNet net{};

    net.AddVertex()(
        "A", {237.483, true});

    net.AddEdge(true)(
        "A", "B", "1", {5.835, 3.5})(
        "B", "C", "2", {3.782, 2.7})(
        "A", "C", "3", {9.640, 4.0})(
        "D", "C", "4", {7.384, 3.0})(
        "A", "D", "5", {2.270, 2.5});

    aa::Adjust(net, 10.0);
    aa::PrintElevationNet(net);
}
```

### Geodesy

*Currently supports Gauss-Kruger projection and geodetic solutions (Gauss and Bessel methods).*

#### Gauss-Kruger Projection

**AGTB** provides unified API for coordinate projection and zone transformation:

```cpp
#include <AGTB/Geodesy/Project.hpp>
#include <print>

int main()
{
    namespace ag = AGTB::Geodesy;
    using projector = ag::Projector<ag::GeoCS::Geodetic, ag::ProjCS::GaussKruger>;
    using config = projector::Config<ag::Ellipsoids::CGCS2000, ag::GaussZoneInterval::D6, ag::Units::Radian>;

    // Coordinate transformation
    ag::Longitude<config::unit> L_from(115, 0, 0);
    ag::Latitude<config::unit> B_from(39, 54, 0);
    
    config::geo_coord geo_coord{L_from, B_from};
    config::proj_coord proj_coord = projector::Project<config>(geo_coord);
    config::geo_coord result = projector::Project<config>(proj_coord);

    // Zone transformation
    using K_config = projector::Config<ag::Ellipsoids::Krasovski, ag::GaussZoneInterval::D3, ag::Units::Radian>;
    K_config::proj_coord src{.x = 1'944'359.609, .y = 240'455.456'3, .zone = 20};
    auto target = projector::ReProject<K_config>(src, 21);  // Transform to adjacent zone
}
```

#### Geodetic Solutions

Supports both Gauss and Bessel methods for direct and inverse geodetic problems:

**Gauss Method:**
```cpp
#include <AGTB/Geodesy/Solve.hpp>

int main()
{
    using solver = ag::Solver<ag::Solutions::Gauss>;
    using config = solver::Config<ag::Ellipsoids::CGCS2000, ag::Units::Radian>;

    ag::Latitude<> B1(47, 46, 52.647'0), B2(48, 4, 9.638'4);
    ag::Longitude<> L1(35, 49, 36.330'0), L2(36, 14, 45.050'5);

    // Inverse: distance and azimuth from coordinates
    config::InverseResult inv_result = solver::Inverse<config>(L1, B1, L2, B2);
    
    // Forward: coordinates from distance and azimuth  
    config::ForwardResult fwd_result = solver::Forward<config>(L1, B1, inv_result.s, inv_result.a_forwards);
}
```

**Bessel Method:**
```cpp
#include <AGTB/Geodesy/Solve.hpp>

int main()
{
    using solver = ag::Solver<ag::Solutions::Bessel>;
    using config = solver::Config<ag::Ellipsoids::Krasovski, ag::Units::Radian>;

    config::Lat B1(47, 46, 52.647'0);
    config::Lon L1(35, 49, 36.330'0);
    AGTB::Angle azimuth(44, 12, 13.664);
    double distance = 44'797.282'6;

    // Forward solution
    config::ForwardResult result = solver::Forward<config>(L1, B1, distance, azimuth);
    
    // Inverse solution
    config::InverseResult inv_result = solver::Inverse<config>(L1, B1, result.L, result.B);
}
```

### Photogrammetry

*Currently provides evaluated space resection functionality with more algorithms in development.*

#### Space Resection

```cpp
#include <AGTB/Photogrammetry/SpaceResection.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <sstream>

namespace ai = AGTB::IO;
namespace ap = AGTB::Photogrammetry;
namespace al = AGTB::Linalg;

int main() {
    std::istringstream data_stream{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};

    ap::Matrix photo_coords(4, 2), object_coords(4, 3), combined(4, 5);
    ai::ReadEigen(data_stream, combined);
    
    photo_coords = combined.leftCols(2);
    object_coords = combined.rightCols(3);

    ai::PrintEigen(photo_coords, "Photo Coordinates (mm)");
    ai::PrintEigen(object_coords, "Object Coordinates");
    
    photo_coords /= 1000; // Convert mm to meters

    ap::InteriorOrientationElements interior{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000, // mm to meters
        .m = 50000};

    ap::SpaceResectionParam params{
        .interior = interior,
        .photo = std::move(photo_coords),
        .object = std::move(object_coords)};

    using SolutionParams = ap::SpaceResectionTParam<
        al::LinalgOption::Cholesky, 
        ap::CollinearityEquationCoeffOption::FullAngles>;
        
    auto result = ap::Solve<SolutionParams>(params);

    if (result.info == ap::IterativeSolutionInfo::Success) {
        std::println("Space Resection Result:\n{}", result.ToString());
    } else {
        throw std::runtime_error("Space resection solution failed");
    }
}
```

### Utils

Provide universal utilities for development.

#### Angles

Here provides an `Angle` class representing `dd.mmss` and a serires of functions to convert between radiuse, dms and string-foramt of dms.

#### DataFrame

A simple implment to simulate `pandas.DataFrame` using `boost::multi_array` and `Eigen::Matrix`. It provides access interface similar to `iloc` and `loc`  and data managment interface using `Eigen::Matrix`. A internal object `inner_frame` is provided to adapt `boost::indices`.

```cpp
#include <AGTB/Utils/DataFrame.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <AGTB/IO/CSV.hpp>
#include <print>

int main()
{
    using AGTB::IO::PrintEigen;
    using AGTB::IO::ReadCSV;
    using AGTB::Utils::DataFrame;

    DataFrame<double> df(3, 4);
    auto nf = df.NumericFrame<double>(); // it's a ref, not a copy
    nf.col(0).fill(1.0);
    nf.col(1).fill(2.0);
    nf.col(2) = nf.col(0) + nf.col(1);
    PrintEigen(nf, "numeric frame:");

    std::println("{}", df.ToString());
    auto str_df = // another DataFrame
        df.Cast<std::string>([](const double &v)
                             { return std::format("val: {}", v); });
    std::println("{}", str_df.ToString());

    auto csv = ReadCSV<double>("../dat/csv/Draft.csv", ",", true);
    csv.ColKeys(1) = "123";
    std::println("{}", csv.ToString());
}
```

## Documentation

For detailed API documentation and advanced usage examples, please refer to the source code headers and test cases.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
