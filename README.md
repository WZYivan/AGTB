# Ano Geomatic ToolBox (AGTB)

AGTB is a toolkit designed for Geomatics Engineering students, primarily containing algorithms commonly used in Adjustment, Photogrammetry, and Geodesy during undergraduate studies. `AGTB` features the following characteristics:

- **Easy to Use**: A header-only library design reduces dependencies. Heavy use of Concepts and template metaprogramming provides clearer interfaces and compile-time error messages.
- **Easy to Extend**: Extensive use of class and template-based dependency injection allows custom algorithm logic to be plugged into generic interfaces by specializing only a few implementation classes.
- **Compile-Time Optimization**: Many constants in Geodesy are represented as compile-time constants, reducing runtime calculations. All algorithm interfaces are template-based, capturing incorrect parameter inputs at compile-time and providing clear error messages.
- **User-Friendly Exceptions**: All thrown exceptions provide clear details including the error cause, expected behavior, and the location where the exception was thrown, helping users locate errors quickly.

## Importing AGTB

`AGTB` has the following dependencies:

- `gcem` : 1.18.0
- `Eigen` : 5.0.0
```cpp
using Matrix = Eigen::MatrixXd;
```
- `Boost` : 1.83.0
- `C++23` : g++14

All third-party libraries require only their header files. `AGTB` uses Conan as a package manager during development and testing, making it easy to import `AGTB` using `conan`. A `conanfile.py` is provided for development.

## Using AGTB

I recommend importing `AGTB` using the following methods:

```cpp
#include <AGTB/Geodesy.hpp>
#include <AGTB/Photogrammetry.hpp>
#include <AGTB/Adjustment.hpp>
#include <AGTB/IO.hpp>

namespace ag = AGTB::Geodesy;
namespace ap = AGTB::Photogrammetry;
namespace aa = AGTB::Adjustment;
namespace aio = AGTB::IO;
```

`AGTB` contains many other header files and nested namespaces. However, I recommend including only these four header files and their corresponding namespaces. The symbols within these namespaces have been filtered to avoid leaking internal implementations and causing confusion. Of course, please avoid accessing the `detail` namespace or including any header files under the `details` folder, as they belong to the unstable internal implementation. In particular, any modifications to the header files under `details` will propagate to all implementations in the entire library, which may lead to severe compilation errors.

Below, each module will be introduced in detail.

## Geodesy

The Geodesy module currently provides functions for Projection, Geodetic Solution, Ellipsoid Mathematics, and Ellipsoid Metadata.

`AGTB` provides the following data structures for describing the most basic geodetic reference:

```cpp
enum class Ellipsoids // Represents the type of ellipsoid
{
    Krasovski,
    IE1975,
    WGS84,
    CGCS2000
};

template <Ellipsoids __ellipsoid>
struct EllipsoidGeometry // Provides specialized geometric parameters for each ellipsoid
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

enum class Units // Describes the angular system
{
    Radian, // I recommend using radians in all cases
    Degree,
    Minute,
    Second
};

// Representation of longitude and latitude, defaulting to radians. Attempting to modify this is not recommended.
// An exception is thrown if constructed with values exceeding the defined range.
template <Units __unit = Units::Radian>
class Longitude{ /* ... */};
template <Units __unit = Units::Radian>
class Latitude{ /* ... */};

template <Concept::EllipsoidGeometry __ellipsoid_geometry>
struct LatitudeConstants {/* ... */}; // Constants related to latitude for an ellipsoid
```

With the definition of the basic geodetic model, `AGTB` provides the following fundamental algorithms for the ellipsoid surface:

```cpp
// namespace Geodesy
using EllipsoidMath::CurvatureRadiusCollection; // Represents the meridian radius of curvature, prime vertical radius of curvature, and mean radius of curvature at a point
using EllipsoidMath::MeridianArcBottom; // Meridian arc length at the foot of the latitude
using EllipsoidMath::MeridianArcLength; // Meridian arc length corresponding to a given latitude
using EllipsoidMath::ParallelCircleArcLength; // Parallel circle arc length corresponding to a given latitude
using EllipsoidMath::PrincipleCurvatureRadii; // Principal curvature radii at a given latitude, returning `CurvatureRadiusCollection`
```

### Geodetic Solution (Direct and Inverse)

`AGTB` provides the following structures as the interface for Geodetic Solution:

```cpp
enum class Solutions
{
    Gauss,
    Bessel,
    OrdinaryPlane
};

template <Solutions __solution>
struct Solver
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};
```

You should use the `Solutions` enumeration values to instantiate the `Solver` template to obtain the corresponding labeled solver. You can use it as follows:

```cpp
// Obtain the solver
using solver = ag::Solver<ag::Solutions::Bessel>;
// Algorithm configuration
using config = solver::Config<ag::Ellipsoids::Krasovski, ag::Units::Radian>;

config::Lat B1(47, 46, 52.647'0);
config::Lon L1(35, 49, 36.330'0);
AGTB::Angle A1(44, 12, 13.664);
double S = 44'797.282'6;
// Forward solution
config::ForwardResult rf = solver::Forward<config>(L1, B1, S, A1);

config::Lat Bf1(30, 30, 0), Bf2(-37, 43, 44.1);
config::Lon Lf1(114, 20, 0), Lf2(51, 16, 32.5);
// Inverse solution
config::InverseResult ri = solver::Inverse<config>(L1, B1, rf.L, rf.B);
```

`AGTB`'s geodetic solvers provide `Forward` and `Inverse` as the interfaces for forward and inverse solutions. If they are template functions, they should be given a `Config` to configure the ellipsoid. The `Config` structure also provides `ForwardResult` and `InverseResult`, as well as `Lat` and `Lon` corresponding to the given units to represent latitude and longitude. The separation of `Solver` and `Config` is because the solver is independent of the ellipsoid.

### Projection

`AGTB`'s projections first rely on Spatial References. Spatial References are a series of structures used to describe geographic and projected coordinate systems and their corresponding coordinates.

```cpp
// namespace Geodesy
using GeoCS = SpatialRef::GeographicCoordinateSystem; // Geographic Coordinate System
using ProjCS = SpatialRef::ProjectCoordinateSystem; // Projected Coordinate System

template <ProjCS __proj, auto... __args>
using ProjUtils = SpatialRef::ProjUtils<__proj, __args...>::Using; // Extracts potentially used utility classes for the corresponding projected coordinates
```

With these as a basis, `AGTB` defines projection solvers that establish a one-to-one correspondence between the geographic coordinate system and the projected coordinate system:

```cpp
template <GeoCS __geo, ProjCS __proj>
struct Projector
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};
```

You can use it as follows:

```cpp
using projector = ag::Projector<ag::GeoCS::Geodetic, ag::ProjCS::GaussKruger>;
using config = projector::Config<ag::Ellipsoids::CGCS2000, ag::GaussZoneInterval::D6, ag::Units::Radian>;
using GeoCoord = config::GeoCoord;
using ProjCoord = config::ProjCoord;

ag::Longitude<config::unit> L_from(115, 0, 0);
ag::Latitude<config::unit> B_from(i, 0, 0);
GeoCoord geo_coord_from{ L_from, B_from};
ProjCoord gauss_proj_coord = projector::Project<config>(geo_coord_from);
GeoCoord geo_coord_inv = projector::Project<config>(gauss_proj_coord);
```

First, instantiate the `Projector` with the desired coordinate systems. Then, use the corresponding `Config` to configure the `Project` method. This method automatically selects the projection direction based on the parameter types. The reason for the two-step instantiation separation design is the same as for `Solver`: the projection solver type is independent of the ellipsoid and projection coordinate system parameters.

## Photogrammetry

The design of the `AGTB` Photogrammetry module is much simpler compared to the Geodesy module. It mainly consists of several different, non-template solvers. Since most of the theory in photogrammetry is based on the principle of least squares adjustment, which often involves iterative algorithms, the algorithms themselves do not throw exceptions. Instead, the status of the iteration is included in the returned structure.

Here are the basic definitions for the Photogrammetry module:

```cpp
// Iteration status
enum class IterativeSolutionInfo : size_t
{
    Success,
    NotConverged,
    Failed,
    Unknown
};

// Interior and Exterior Orientation Elements
struct InteriorOrientationElements{ /* ... */ };
struct ExteriorOrientationElements{ /* ... */ };
```

### Space Resection (Backward Intersection)

This solver is defined as follows:

```cpp
struct SpaceResection{
    template < ... >
    struct Config { /* ... */ };

    struct Param;
    struct Result;

    static Result Solve< ... > (Param);
};
```

The `Config` before and after `SpaceResection` are used to configure matrix inversion algorithms and collinearity condition equation simplification algorithms, which are internal implementations. When using `Solve`, it has default template parameters, so `Config` can be ignored. You can use it as follows:

```cpp
std::istringstream iss{
    "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
    "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
    "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
    "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};

ap::Matrix photo(4, 2), obj(4, 3), all(4, 5);
aio::ReadEigen(iss, all);
photo = all.leftCols(2),
obj = all.rightCols(3);
photo /= 1000; // mm -> m

ap::InteriorOrientationElements interior{
    .x0 = 0,
    .y0 = 0,
    .f = 153.24 / 1000,
    .m = 50000};

ap::SpaceResection::Param p = {
    .interior = interior,
    .image = photo,
    .object = obj};

ap::SpaceResection::Result result = ap::SpaceResection::Solve(p);
```

### Space Intersection (Forward Intersection)

The forward intersection solver has a design similar to the backward intersection solver. However, it has three sets of `SolveParamResult` corresponding to the Point Projection Coefficient Method (for a pair of points), Point Projection Coefficient Method (for multiple pairs of points), and Collinearity Condition Equation Method (for multiple images). Its definition is as follows:

```cpp
struct SpaceIntersection
{
    struct IsolatedParam { /* ... */};
    using Param = IsolatedParam;
    using PairedParam = std::pair<Param, Param>;
    struct Result { /* ... */ };

    struct BatchParam { /* ... */ };
    using BatchResult = Matrix;

    template < ... >
    struct Config { /* ... */ };

    using OlsParam = std::span<const Param>;
    struct OlsResult { /* ... */ };

    static Result Solve(Param, Param){ /* ... */ };
    static Result Solve(PairedParam){ /* ... */ };
    static BatchResult Solve(BatchParam, BatchParam){ /* ... */ };
    static OlsResult Solve(OlsParam){ /* ... */ };
}
```

You can use it as follows:

```cpp
double f = 150;
using solver = ap::SpaceIntersection;

ap::ExteriorOrientationElements
    ex_left{4999.770168, 4999.728897, 2000.002353, 0.00021500, 0.02906441, 0.09524706},
    ex_right{5896.828551, 5070.244316, 2030.443250, 0.01443355, 0.04601826, 0.11046904};
ap::InteriorOrientationElements
    in_left{.f = f},
    in_right{.f = f};

ap::Matrix img(5, 4);
img << 51.758, 80.555, -39.953, 78.463,
    14.618, -0.231, -76.006, 0.036,
    49.88, -0.782, -42.201, -1.022,
    86.14, -1.346, -7.706, -2.112,
    48.035, -79.962, -44.438, -79.736;
solver::BatchParam
    sibp_left{ex_left, in_left, img.leftCols(2)},
    sibp_right{ex_right, in_right, img.rightCols(2)};
solver::BatchResult result = solver::Solve(sibp_left, sibp_right);

solver::Param
    ols_1{ex_left, in_left, img(0, 0), img(0, 1)},
    ols_2{ex_right, in_right, img(0, 2), img(0, 3)};
solver::OlsResult ols_result = solver::Solve({{ols_1, ols_2}});
```

### Transform

This namespace contains free functions for common photogrammetric coordinate system transformations and matrix construction, which are part of the API.

## Adjustment

This module mainly includes adjustment solvers for observation data (part of the API) and related algorithm implementations for adjustment (internal implementation). It focuses primarily on the adjustment of observation data, i.e., indoor calculations. The following enumeration is defined to represent different route types:

```cpp
enum class RouteType
{
    ClosedLoop, // Closed loop, where the known edge is part of the loop
    ClosedConnecting, // Closed, but the known edge is not part of the loop, treated as connecting
    Connecting, // Connecting
    Net // Network
};
```

### Leveling

`AGTB` provides adjustment solvers for leveling networks and single leveling routes.

#### Single Route

```cpp
template < ... >
struct ElevationParam { /* ... */ }; // Parameters corresponding to the route
struct ElevationVariable { /* ... */ }; // Intermediate variables in the adjustment process, i.e., parts of the indoor calculation table excluding the input
struct ElevationInfo { /* ... */ }; // Adjustment quality information, such as K
struct ElevationAdjustResult { /* ... */ }; // Adjustment result, composed of Variable and Info parts

template < ... >
ElevationAdjustResult Adjust(const ElevationParam< ... > &param, int place){ /* ... */ } // Solution interface, 'place' indicates the precision for calculations

template < ... >
std::string AdjustmentTable( ... ){ /* ... */ } // Combines the adjustment result and adjustment parameters into a string (indoor calculation table)
```

You can use it as follows:

```cpp
aa::ElevationParam<aa::RouteType::ClosedLoop> p1{
    .distances = {0.8, 0.5, 1.2, 0.5, 1.0},
    .h = {0.230, 0.260, -0.550, -0.450, 0.490},
    .H_beg = 12.000};
aa::ElevationAdjustResult r1 = aa::Adjust(p1, 3);
std::println(">>> ClosedLoop \n{}", aa::AdjustmentTable(p1, r1));
```

#### Leveling Network

`AGTB` uses graphs from `BGL` to represent the structure of the leveling network. Unlike a single route, the adjustment result of a leveling network is directly applied to the input. However, the overall interface design remains similar. The interface definition is as follows:

```cpp
using ElevationNet = NamedGraph<VertexProperty, EdgeProperty>;
struct ElevationNetVariable { /* ... */ };

ElevationNetVariable Adjust(ElevationNet &net, double unit_p = 1.0) { /* ... */ } // Unit weight is separate from the leveling network
void PrintElevationNet(const ElevationNet &net) { /* ... */ } // Print the leveling network
```

You can use it as follows:

```cpp
aa::ElevationNet net{};

net.AddVertex()( // Add survey point
     "A ", {237.483, true}); // Name, {Elevation, Is Control Point}

net.AddEdge(true /* Automatically add non-existent vertices */)( // Add survey line
     "A ",  "B ",  "1 ", {5.835, 3.5})( // Start, End, Edge Name, {Height Difference, Line Length}
     "B ",  "C ",  "2 ", {3.782, 2.7})(
     "A ",  "C ",  "3 ", {9.640, 4.0})(
     "D ",  "C ",  "4 ", {7.384, 3.0})(
     "A ",  "D ",  "5 ", {2.270, 2.5});

auto var = aa::Adjust(net, 10.0);
aa::PrintElevationNet(net);

double rmse = aa::MeanRootSquareError(var.V, var.A.rows(), var.A.cols(), var.P);
std::println( "rmse = {} ", rmse);
```

### Traverse Surveying

`AGTB` provides an adjustment solver for single traverses.

#### Single Traverse

The interface design is identical to that of the single leveling route, only the names differ. This will not be elaborated further. You can use it as follows:

```cpp
aa::TraverseParam<aa::RouteType::ClosedLoop> p1{
    .distances = {55.150, 66.521, 53.729, 66.661},
    .angles = {{90, 57, 35}, {89, 9, 46}, {92, 3, 38}, {87, 50, 2}},
    .azi_beg = {125, 30, 0},
    .x_beg = 500.00,
    .y_beg = 600.00};
aa::TraverseAdjustResult r1 = aa::Adjust(p1, 3, 0);
std::println("{}", aa::AdjustmentTable(p1, r1));
```

## IO

### Eigen

`AGTB` can read Eigen matrices from CSV files without headers, using `,` as the delimiter. It also supports any stream. Additionally, Eigen matrices can be printed to any stream in the `Eigen::IOFormat` format.

```cpp
aio::PrintEigen(matrix, "describe this matrix", stream);
// describe this matrix
// matrix

aio::ReadEigen(stream, matrix);
```

### JSON

`AGTB` provides JSON read and write functionality based on `using PropTree = boost::property_tree::ptree`. By specializing `JsonParser`, the `ParseJson` function can be used to parse any type. `AGTB` also provides a series of functions (located in `struct AGTB::Container::PTree`) to simplify read-only access to `PropTree`. The provided `ReadJson` and `PrintJson` functions have both stream and file overloads.

```cpp
AGTB::PropTree ptree {};
aio::ReadJson("data.json", ptree);
aio::ReadJson(stream, ptree);
aio::PrintJson(json, "out.json");
aio::PrintJson(json, stream);
```

#### JsonParser

`AGTB` implements parsing specific types from `PropTree` through the combination of `JsonParser` and `ParseJson`. Their definitions are as follows:

```cpp
template <typename __target>
struct JsonParser
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

template <typename __target, typename __ptree>
__target ParseJson(const __ptree &json)
    requires /* ... */
{
    /* ... */
}

template <typename __target, typename __ptree>
__target ParseJson(const __ptree &json, const JsonParser<__target> &parser)
    requires /* ... */
{
    /* ... */
}
```

`JsonParser` needs to implement at least two static functions, `Parse` and `Expect`, otherwise it does not satisfy the constraint conditions. An example is as follows:

```cpp
template  < >
struct JsonParser <Adjustment::TraverseParam <Adjustment::RouteType::ClosedLoop > >
{
    using Target = Adjustment::TraverseParam <Adjustment::RouteType::ClosedLoop >;

    template  <typename __ptree >
    static Target Parse(const __ptree  &json)
    {
        return DoParse(json,  "distances ",  "angles ",  "azi_beg ",  "x_beg ",  "y_beg ");
    }

    template  <typename __ptree >
    Target ParseConfig(const __ptree  &json) const
    {
        return DoParse(json, dis, ang, ab, x, y);
    }
    JsonParser(std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::st ring y_beg)
        : dis(distances), ang(angles), ab(azi_beg), x(x_beg), y(y_beg)
    {
    }
    ~JsonParser() = default;

private:
    std::string dis, ang, ab, x, y;

    templ ate  <typename __ptree >
    static Target DoParse(const __ptree  &json, std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
    {
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, distances);
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, angles);
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, azi_beg);
        AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(json, x_beg, double) ;
        AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(json, y_beg, double);

        return {
            .distances = PTree::ArrayTo <std::vector <double > >(json, distances),
            .angles = PTree::ArrayView(json, angles) |
                      std::views::transform([](const auto  &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo <std::vector <double > >(sub)); }) |
                      std::ranges::to <std::vector <Angle > >(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo <std::vector <double > >(json, azi_beg)),
            .x_beg = PTree::Value <double >(json, x_beg),
            .y_beg = PTree::Value <double >(json, y_beg)};
    }

public:
    static std::string Expect()
    {
        return R "(
{
     "distances " : [105.22, 80.18, 129.34, 78.16],
     "angles " : [
        [107, 48, 32],
        [73, 0, 24],
        [89, 33, 48],
        [89, 36, 30]
    ],
     "azi_beg " : [125, 30, 0],
     "x_beg " : 506.32,
     "y_beg " : 215.65
}
        ) ";
    }
};
```

This internal implementation of `AGTB` is provided as an example, demonstrating how to correctly specialize `JsonParser` and configure custom behavior:

- `Expect`: This function provides a default expected JSON format. `Parse` should parse according to this format.
- `Parse`: The single-parameter version of `ParseJson` will call this function by default. If it does not exist, it will attempt to default-construct a `JsonParser` and call its `ParseConfig`.
- `ParseConfig`: The two-parameter version of `ParseJson` will call the `ParseConfig` function of the passed-in `JsonParser` by default. If it does not exist, it calls `Parse`. You can define any other members in the specialized version of `JsonParser` to customize its behavior. Here, five string members are used to parse specific JSON keys.

You might have noticed the use of the `AGTB_JSON_PARSER_VALIDATE_*_KEY` series of macros in the implementation. These are provided by `AGTB` and are only used for JSON key checking inside `JsonParser`. They throw exceptions with detailed information when a key of the specified type is missing, including the missing key name, the default expected JSON format, etc.

Here is a brief introduction to the usage of the `PTree` series of free functions:

- `ArrayView`: Converts the current `PropTree` (or the subtree specified by a key) to an array view, as shown in the example with the `"distances"` key.
- `MapView`: Converts the current `PropTree` (or the subtree specified by a key) to a dictionary view, which in the example is the entire original tree.
- `Value`: Converts the current `PropTree` (or the subtree specified by a key) to a value (type must be given), as in the example with the `"x_beg"` key.
- `ArrayTo`: Converts the array view of the current `PropTree` (or the subtree specified by a key) to a container.

These functions provide quick and intuitive access methods for `PropTree`.

## Macros

There are a large number of macros in `AGTB`. Some are for internal use only, some are designed for users, and others are specifically provided for users to define part of `AGTB`'s behavior.

### Internal Use Only (but good to know)

- `AGTB_THROW( ... )`: `AGTB` throws a stylized exception.
- `AGTB_TEMPLATE_NOT_SPECIALIZED()`: Indicates that a compile-time error should be triggered if no specialized version exists, as there is no default implementation.
- `AGTB_FILE_NOT_IMPELEMENT()`: Indicates that the file is not implemented or should not be included for various reasons.

### Available to Users and Internal Use

- `AGTB_JSON_PARSER_VALIDATE_*_KEY`: Used for JSON key checking inside `JsonPaser`.

### Specifically for Users

These must be defined before inclusion to adjust `AGTB`'s behavior.

- `#define AGTB_DISABLE_NOTE`: Suppresses all `#warning` messages from `AGTB`.
- `#define AGTB_ENABLE_DEBUG`: Enables `AGTB`'s debug output.
- `#define AGTB_DEBUG_INFO_VERBOSE_ALL`: Enables all of `AGTB`'s debug output, which may cause overly cluttered output.
- `#define AGTB_ENABLE_EXP`: Enables `AGTB`'s experimental features.

## Auxiliary

`AGTB` also has a series of internal modules to assist the work of the above four modules. Some symbols may come from here. I will only show a part of them.

### Utils

- `Angles`: Tools related to sexagesimal angle calculations and angle conversions.
- `Angle`: A class under `Angles` representing a sexagesimal angle.
- `Math`: Auxiliary mathematical functions, such as the rounding rules and approximate equality for geomatics professionals.
- `Concept`: Metaprogramming related.
- `Error`: Various exceptions.

### Container

- `NamedGraph`: A named wrapper for classes in `BGL`, allowing access to edges and vertices by name, not just by index.
- `PropTree, PropPath`: Aliases for `boost::property_tree::ptree, path`.
- `PTree`: A series of free functions for accessing `PropTree`.

### Linalg

- `LinalgOption`: Some options for linear algebra algorithms, mainly methods for inversion.
- `CorrectionOlsSolve`: Equivalent solution for corrections under the least squares principle.
- `NormalEquationMatrixInverse`: $(A^T P A)^{-1}$, the inverse of the normal equation matrix.
- `Rotate*`: Related to rotation matrices.
- `Cs*`: Related to coordinate system transformations.

## Version

- `AGTB`: `0.0.14`
- `Date`: `26.1.7`