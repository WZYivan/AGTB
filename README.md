# Ano Geomatics ToolBox

**AGTB** is a header-only library containing a series of algorithms for **Geomatics Science**. Currently focused on photogrammetry, geodesy and adjustment.

## Installation

This is a conan package. Use the following command to install:

```bash
conan create .
```

## Dependence

- Eigen3/[*] (for matrix operations)
- gcem /[*] (for constexpr math functions)
- C++23 or later
- Conan for package management

## Interface

### Adjustment

#### Traverse

AGTB provides adjustment solver for closed or connecting traversing.

```cpp
#include <AGTB/Adjustment/Traverse.hpp>
#include <AGTB/Utils/Angles.hpp> // for class `Angle`
#include <print>

namespace aa = AGTB::Adjustment;
namespace aat = aa::Traverse;
using AGTB::Utils::Angles::Angle;

int main()
{
    aat::TraverseParam p1{
        .route_type = aa::RouteType::Closed,
        .distances = {105.22, 80.18, 129.34, 78.16},
        .angles = {{107, 48, 30}, {73, 0, 24}, {89, 33, 48}, {89, 36, 30}},
        .azi_beg = {125, 30, 0},
        .x_beg = 506.32,
        .y_beg = 215.65};
    aat::ClosedAdjustor a1(p1);
    a1.Adjust(2);

    std::println(">>> Closed:\n{}", aat::SolveResultOf(a1));

    aat::TraverseParam p2{
        .route_type = aa::RouteType::Connecting,
        .distances = {225.85, 139.03, 172.57, 100.07, 102.48},
        .angles = {Angle(99, 1, 0), Angle(167, 45, 36), Angle(123, 11, 24), Angle(189, 20, 36), Angle(179, 59, 18), Angle(129, 27, 24)},
        .azi_beg = Angle(237, 59, 30),
        .azi_end = Angle(46, 45, 24),
        .x_beg = 2507.65,
        .y_beg = 1215.64,
        .x_end = 2166.70,
        .y_end = 1757.28};
    aat::ConnectingAdjustor a2(p2);
    a2.Adjust(2);

    std::println(">>> Connecting:\n{}", aat::SolveResultOf(a2));
}
```

### Geodesy

AGTB provides a series of implements for geodesy. Including solution, transform and some basic ellipsoid parameters.

#### Base (Ellipsoid)

AGTB provides a series of constants to describe ellipsoid. You can also use AGTB macros define your ellipsoid and work with AGTB.

```cpp
#include <AGTB/Geodesy/Base.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>
#include <iostream>

namespace ag = AGTB::Geodesy;
namespace au = AGTB::Utils;

template <ag::EllipsoidConcept E>
void PE()
{
    std::println(
        "\na={}\nb={}\nc={}\nalpha={}\ne1_2={}\ne2_2={}",
        E::a, E::b, E::c, E::alpha, E::e1_2, E::e2_2);
}

AGTB_DEFINE_QUICK_ELLIPSOID(my_ellipsoid, 6378245.0, 6356863.018773047);

template <ag::GeodeticLatitudeConstantsConcept L>
void PL(L l)
{
    std::println(
        "\nB={}\nt={}\ntau_2={}\nW={}\nV={}",
        l.B, l.t, l.nu_2, l.W, l.V);
}

int main()
{
    using ag::Ellipsoid::Krasovski;

    std::cout << "Krasovski" << std::endl;
    PE<Krasovski>();

    std::cout << "my ellipsoid" << std::endl;
    PE<my_ellipsoid>();

    std::cout << "Constants at B(45.0)" << std::endl;
    PL(
        ag::GeodeticLatitudeConstants<Krasovski>(
            au::Angles::FromDMS(45)));
}

```

To define an ellipsoid, you can do as below:
```cpp
AGTB_DEFINE_QUICK_ELLIPSOID(my_ellipsoid, a, b); // You only know basic parameters (a, b)
AGTB_DEFINE_PRECISE_ELLIPSOID(my_ellipsoid, ...); // You know all parameters (a, b, c, alpha, e, e')
```

AGTB also provide basic lat lon.
```cpp
// in AGTB/Geodesy/Base.hpp

class GeodeticLatitude;
class GeodeticLongitude;
```

#### Meridian Arc Solution

AGTB provide forward and inverse solution ofr meridian are length.
```cpp
#include <AGTB/Geodesy/MeridianArc.hpp>

namespace ag = AGTB::Geodesy;
namespace age = ag::Ellipsoid;
using Solver = ag::MeridianArcSolver<age::Krasovski, ag::EllipsoidBasedOption::Specified>;

double len = Solver::Forward(B);
GeodeticLatitude  B = Solver::Inverse(len /* , threshold *for an iterative solution* */);
```

#### Principle Curvature Radii

```cpp
#include <AGTB/Geodesy/PrincipleCurvatureRadii.hpp>

namespace ag = AGTB::Geodesy;
namespace age = ag::Ellipsoid;
auto [M, N] = ag::PrincipleCurvatureRadii<age::Krasovski, ag::EllipsoidBasedOption::Specified>(B);
```

#### Solution

```TODO```

#### Projection

AGTB provides a series of projector for inverse and forward solution in specific projection. Now Gauss is available.

```cpp
#include "AGTB/Geodesy/Projection/GaussKruger.hpp"
#include "AGTB/Utils/Angles.hpp"

int main()
{
    namespace ag = AGTB::Geodesy;
    namespace agpg = ag::Projection::GaussKruger;
    namespace au = AGTB::Utils;
    namespace aua = au::Angles;

    using projector = agpg::Projector<ag::Ellipsoid::CGCS2000, ag::EllipsoidBasedOption::General>;
    using aua::Angle;

    auto rf = projector::Forward(
        aua::FromDMS(114, 0),
        aua::FromDMS(30, 0));

    std::println("x = {}\ny = {}\nZoneY = {}\n", rf.x, rf.y, rf.ZoneY());

    auto ri = projector::Inverse(rf.x, rf.y, rf.zone);

    std::println("\nB = {}\nL= {}", Angle::FromRad(ri.B).ToString(), Angle::FromRad(ri.L).ToString());
}
```

### Photogrammetry

#### Space Resection

AGTB provides an Ordinary Least Squares (OLS) iterative optimization method for space resection with a pure functional interface.

```cpp
#include <AGTB/Photogrammetry/SpaceResection.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <sstream>

namespace aei = AGTB::EigenIO;
namespace ap = AGTB::Photogrammetry;
namespace apsr = ap::SpaceResection;
namespace al = AGTB::Linalg;

int main()
{

    std::istringstream iss{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};

    ap::Matrix photo(4, 2), obj(4, 3), all(4, 5);
    aei::ReadEigen(iss, all);
    photo = all.leftCols(2),
    obj = all.rightCols(3);

    aei::PrintEigen(photo, "photo"); // mm
    aei::PrintEigen(obj, "obj");
    photo /= 1000; // mm -> m

    ap::InteriorOrientationElements internal{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000,
        .m = 50000};
    auto result = apsr::QuickSolve<al::LinalgOption::SVD>(internal, photo, obj);
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

### IO

AGTB provides some IO interface to adapt 3rd party lib like Eigen3.

#### Eigen

AGTB provides a Eigen reader and quick printer. You can read from ```std::istream``` and print to ```std::ostream```. The ```AGTB::EigenIO::ReadEigen``` read like the given ```MatrixXd``` and ```AGTB::EigenIO::ReadEigenCustom<>``` read like the given template parameter.

```cpp
#include <AGTB/IO/Eigen.hpp>
#include <iostream>
#include <Eigen/Dense>
#include <cassert>

int main()
{
    using AGTB::EigenIO::ContainerOf;
    using AGTB::EigenIO::IsValidContainer;
    using AGTB::EigenIO::MMDOf;

    using ContainerXd = ContainerOf<Eigen::MatrixXd>;
    using MMDXd = MMDOf<Eigen::MatrixXd>;
    static_assert(std::is_same_v<ContainerXd, std::vector<std::vector<double>>>);

    ContainerXd cxd{
        {1, 2},
        {3, 4}};

    using Container3cd = ContainerOf<Eigen::Matrix3cd>;
    using MMD3cd = MMDOf<Eigen::Matrix3cd>;
    static_assert(std::is_same_v<Container3cd, std::array<std::array<std::complex<double>, 3>, 3>>);

    Container3cd c3cd{
        {{1, 2}, {3, 4}}};

    auto validation_xd = IsValidContainer<MMDXd, ContainerXd>(cxd);
    auto validation_3cd = IsValidContainer<MMD3cd, Container3cd>(c3cd);
    assert(validation_xd);
    assert(validation_3cd);

    std::string data{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};
    auto iss = std::istringstream(data);

    Eigen::MatrixXd mat(4, 5);
    AGTB::EigenIO::ReadEigen(iss, mat); // default shape
    AGTB::EigenIO::PrintEigen(mat, "Read mat");

    iss = std::istringstream(data);
    using MMD_4_2_d = AGTB::EigenIO::MatrixMetaData<double, 4, 2>; // custom shape
    Eigen::MatrixXd mat_cus(4, 2);
    AGTB::EigenIO::ReadEigenCustom<
        MMD_4_2_d,
        AGTB::EigenIO::ContainerOf<decltype(mat_cus)>>(iss, mat_cus);
    AGTB::EigenIO::PrintEigen(mat_cus, "Custom read(4, 2)");

    iss = std::istringstream(data);
    using MMD_2_4_d = AGTB::EigenIO::MatrixMetaData<double, 2, 4>;
    Eigen::MatrixXd mat_2_4(2, 4);
    AGTB::EigenIO::ReadEigenCustom<
        MMD_2_4_d,
        AGTB::EigenIO::ContainerOf<decltype(mat_2_4)>>(iss, mat_2_4);
    AGTB::EigenIO::PrintEigen(mat_2_4, "Custom read(2, 4)");

    return 0;
}
```

### Utils

AGTB also provides some utilities as helpers.

#### String

Here provide functions to manipulate ```std::string``` like below.

```cpp
#include <AGTB/Utils/String.hpp>
#include <print>
#include <iostream>

void P(auto &str)
{
    std::println(std::cout, "[{}]", str);
}

int main()
{
    using namespace AGTB::Utils;

    std::string ori = "  a test str   ";

    P(ori);

    auto ls = LStrip(ori),
         rs = RStrip(ori),
         lrs = LRStrip(ori),
         sws = SkipWhiteSpace(ori);

    P(ls);
    P(rs);
    P(lrs);
    P(sws);

    std::string arr = " 2,  5  , 3, 6";
    auto vec = SplitThenConv<int>(arr, ",");
    for (auto &i : vec)
    {
        std::print("{}, ", i);
    }
    std::cout << std::endl;
}
```

#### Angles

Here provides basic angle algorithms and a ```Angle``` class. It's a user-friendly interface, you can use as below:
```cpp
#include <AGTB/Utils/Angles.hpp>

namespace aua = AGTB::Utils::Angles;
using aua::Angle;

int main(){
    aua::FromDMS(d, m, s); // dms -> rad
    aua::ToDMS(rad); // rad -> dms (signed at the highest non-zero position)

    Angle a1 (d, m, s), a2 (d, m, s); // Angle(d, m, s) object
    a1 + a2; // angle operators
    a1 - a2;
    a1 / a2;
    a1 % a2;
    a1 * double(scale); // operator with double 
    a1.DMS(); // like aua::ToDMS()
    a1.Rad(); // -> rad
    a1.Sin(); // Internal trigonometric functions, execute by 'rad'
    a1.NormStd(); // {0, 360deg}
    a1.ToString(); // -> "{}d{}m{}s"
}
```

