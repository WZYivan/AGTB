# Ano Geomatic ToolBox (AGTB)

**AGTB** 是一个面向测绘工程专业学生的工具库。主要包含了本科阶段常用的平差、摄影测量、大地测量三部分的算法。`AGTB`具有以下特点:

- **易于使用**: 使用纯头文件库减少依赖，大量使用概念和模板元编程以提供更清晰的接口和编译时错误信息。
- **易于拓展**: 大量使用基于类和模板的依赖注入，仅需特化少量的实现类，就可以将自定义算法逻辑用于通用接口。
- **编译时优化**: 将大地测量中大量的常量用编译时常量表示，减少了运行时的计算。同时，所有算法接口均使用模板实现，错误的参数输入将在编译时被捕获，并提供清晰的错误信息。
- **更友好的异常**: 所有抛出的异常提供更清晰的错误原因、期待的情况、抛出的位置等，帮助用户更快地定位错误。

# 导入AGTB

`AGTB`有以下依赖:
- `gcem` : 1.18.0
- `Eigen` : 5.0.0
```cpp
using Matrix = Eigen::MatrixXd;
```
- `Boost` : 1.83.0
- `C++23` : g++14

所有的三方库只需要它们的头文件。`AGTB`在开发、测试时均使用conan作为包管理器，因此使用`conan`将很容易导入`AGTB`，也提供了开发时的`conanfile.py`。

# 使用AGTB

我推荐使用以下方法导入`AGTB`:

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

在`AGTB`中，还有很多其他头文件和嵌套的命名空间，但我只推荐包含这四个头文件和对应的命名空间。这些命名空间的的符号经过了筛选，不会泄露内部的实现而导致混淆。当然，请不要试图访问`detail`命名空间或包含`details`文件夹下的任何头文件，它们都属于不稳定的内部实现，尤其是`details`下的头文件，对其的任何修改都会传播到整个库的所有实现中，这可能导致严重的编译错误。

下面将一一介绍每个模块。

## Geodesy

大地测量模块目前提供了投影、大地主题结算、椭球面数学和椭球面元数据几个功能。

`AGTB`提供了以下的数据结构用于最基本的地理基准的描述:
```cpp
enum class Ellipsoids // 表示椭球体的类型
{
    Krasovski,
    IE1975,
    WGS84,
    CGCS2000
};

template <Ellipsoids __ellipsoid>
struct EllipsoidGeometry // 为每个椭球体提供特化的几何参数的实现
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

enum class Units // 对角度系统的描述
{
    Radian,// 我推荐在任何情况下都使用弧度
    Degree,
    Minute,
    Second
};

// 经纬度的描述，默认为弧度，不应该试图修改。被以超出定义的数值构造时，会抛出异常
template <Units __unit = Units::Radian>
class Longitude{ /* ... */};
template <Units __unit = Units::Radian>
class Latitude{ /* ... */};

template <Concept::EllipsoidGeometry __ellipsoid_geometry>
struct LatitudeConstants {/* ... */}; // 椭球体与纬度相关的常数
```

有了对大地测量基本模型的定义，`AGTB`提供以下这些椭球面的基本算法:
```cpp
// namespace Geodesy
using EllipsoidMath::CurvatureRadiusCollection; // 表示一点子午圈曲率、卯酉圈曲率、平均曲率的对象
using EllipsoidMath::MeridianArcBottom; // 子午线底点纬度
using EllipsoidMath::MeridianArcLength; // 某点纬度对应的子午线长
using EllipsoidMath::ParallelCircleArcLength; // 某点纬度对应的平行圈长
using EllipsoidMath::PrincipleCurvatureRadii; // 某点纬度对应的主曲率半径，返回`CurvatureRadiusCollection`
```

### 大地主题解算

`AGTB`提供以下结构作为大地主题解算的接口:

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

你应该使用`Solutions`枚举值来实例化`Solver`模板，以获取对应标签的求解器，可以如下使用:

```cpp
// 获取求解器
using solver = ag::Solver<ag::Solutions::Bessel>;
// 算法配置
using config = solver::Config<ag::Ellipsoids::Krasovski, ag::Units::Radian>;

config::Lat B1(47, 46, 52.647'0);
config::Lon L1(35, 49, 36.330'0);
AGTB::Angle A1(44, 12, 13.664);
double S = 44'797.282'6;
// 正解
config::ForwardResult rf = solver::Forward<config>(L1, B1, S, A1);

config::Lat Bf1(30, 30, 0), Bf2(-37, 43, 44.1);
config::Lon Lf1(114, 20, 0), Lf2(51, 16, 32.5);
// 反解
config::InverseResult ri = solver::Inverse<config>(L1, B1, rf.L, rf.B);
```

`AGTB`的大地主题求解器提供`Forward`和`Inverse`作为正、反解的接口，若他们为模板函数，则应该给予它们`Config`来配置地球椭球体。`Config`结构体中还提供了`ForwardResult`和`InverseResult`以及与给定单位对应的`Lat`和`Lon`表示经纬度。`Solver`和`Config`的分离设计是因为求解器是与地球椭球体无关的。

### 投影

`AGTB`的投影首先依赖于空间参考。空间参考是用于描述地理和投影坐标系及其对应坐标的一系列结构体。

```cpp
// namespace Geodesy
using GeoCS = SpatialRef::GeographicCoordinateSystem; // 地理坐标系统
using ProjCS = SpatialRef::ProjectCoordinateSystem; // 投影坐标系统 

template <ProjCS __proj, auto... __args>
using ProjUtils = SpatialRef::ProjUtils<__proj, __args...>::Using; // 提取可能使用的、对应投影坐标的工具类
```

有这些作为基础，`AGTB`定义了地理坐标系和投影坐标系之间一一对应的投影求解器:

```cpp
template <GeoCS __geo, ProjCS __proj>
struct Projector
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};
```

你可以按照如下方式使用它:

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

首先，使用期望的坐标系实例化`Projector`，接着用对应的`Config`来配置`Project`方法，此方法根据参数的类型自动选择不同方向的投影。进行双重实例化的分离设计的原因与`Solver`相同，即投影求解器的类型与地球椭球体和投影坐标系参数是分离的。

## Photogrammetry

`AGTB`摄影测量模块的设计相较于大地测量模块要简单的多，主要有几个不同的、非模板的求解器组成。由于摄影测量理论大部分基于最小二乘法的平差原理，它们大多是需要迭代的算法，因此算法本身是不抛异常的，而是在返回的结构体中包含一个表示迭代状况的枚举值。

下面是摄影测量模块的基础定义:
```cpp
// 迭代状况
enum class IterativeSolutionInfo : size_t
{
    Success,
    NotConverged,
    Failed,
    Unknown
};

// 内外方位元素
struct InteriorOrientationElements{ /* ... */ };
struct ExteriorOrientationElements{ /* ... */ };
```

### 空间后方交会

此求解器定义如下:

```cpp
struct SpaceResection{
    template < ... >
    struct Config { /* ... */ };

    struct Param;
    struct Result;

    static Result Solve< ... > (Param);
};
```

空前、空后的`Config`都是用于配置矩阵求逆算法和共线条件方程化简算法的，它们属于内部实现，在使用时，`Solve`具有默认的模板参数，因此可以忽略`Config`。你可以如下使用:

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

### 空间前方交会

空前求解器与空后求解器的设计类似，不同的是它有三套`Solve` `Param` `Result`分别对应点投影系数法（一对点）、点投影系数法（多对点）和共线条件方程法（多像），其定义如下:

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

你可以如下使用:

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

此命名空间下有用于常用摄影测量坐标系相互转换和矩阵构造相关的自由函数，也是API的一部分。

## Adjustment

此模块主要包含观测数据的平差求解器（API的一部分）和误差平差的相关算法实现（内部实现），主要聚焦于观测数据的平差，即内业计算。定义了如下枚举来表示不同的路线类型:

```cpp
enum class RouteType
{
    ClosedLoop, // 闭合，且已知边是环线的一部分
    ClosedConnecting, // 闭合，但已知边不是环线的一部分，当作附和
    Connecting, // 附和
    Net // 网
};
```

### 水准测量

`AGTB`提供了水准网和单一水准路线的平差求解器。

#### 单一路线

```cpp
template < ... >
struct ElevationParam { /* ... */ }; // 对应路线的参数
struct ElevationVariable { /* ... */ }; // 平差过程的中间变量，即内业计算表除去输入的部分
struct ElevationInfo { /* ... */ }; // 平差质量信息，如K之类
struct ElevationAdjustResult { /* ... */ }; // 平差结果，由Variable和Info两部分组成

template < ... >
ElevationAdjustResult Adjust(const ElevationParam< ... > &param, int place){ /* ... */ } // 解算接口，place表示计算时取位的精度

template < ... >
std::string AdjustmentTable( ... ){ /* ... */ } // 将平差结果和平差参数组合为一个字符串（内业计算表）
```

你可以如下使用:

```cpp
aa::ElevationParam<aa::RouteType::ClosedLoop> p1{
    .distances = {0.8, 0.5, 1.2, 0.5, 1.0},
    .h = {0.230, 0.260, -0.550, -0.450, 0.490},
    .H_beg = 12.000};
aa::ElevationAdjustResult r1 = aa::Adjust(p1, 3);
std::println(">>> ClosedLoop \n{}", aa::AdjustmentTable(p1, r1));
```

#### 水准网

`AGTB`使用了`BGL`中的图来表示水准网的结构，不同于单一路线，水准网的平差结果将直接应用于输入上，但总体接口设计依旧是类似的，接口定义如下：

```cpp
using ElevationNet = NamedGraph<VertexProperty, EdgeProperty>;
struct ElevationNetVariable { /* ... */ };

ElevationNetVariable Adjust(ElevationNet &net, double unit_p = 1.0) { /* ... */ } // 单位权与水准网分离
void PrintElevationNet(const ElevationNet &net) { /* ... */ } // 将水准网打印  
```

你可以如下使用:

```cpp
aa::ElevationNet net{};

net.AddVertex()( // 添加测站点
    "A", {237.483, true}); // 名称，{高程， 是（控制点）}

net.AddEdge(true /* 自动添加不存在的顶点 */)( // 添加测线
    "A", "B", "1", {5.835, 3.5})( // 起点，终点，边名，{高差，测线长}
    "B", "C", "2", {3.782, 2.7})(
    "A", "C", "3", {9.640, 4.0})(
    "D", "C", "4", {7.384, 3.0})(
    "A", "D", "5", {2.270, 2.5});

auto var = aa::Adjust(net, 10.0);
aa::PrintElevationNet(net);

double rmse = aa::MeanRootSquareError(var.V, var.A.rows(), var.A.cols(), var.P);
std::println("rmse = {}", rmse);
```

### 导线测量

`AGTB`提供了单一导线的平差求解器。

#### 单一导线

与水准测量单一导线接口设计完全一致，仅名称不同，不再赘述，你可以如下使用:

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

`AGTB`可以从无头以`,`分隔的CSV文件中读取Eigen矩阵，且支持任何流。同时，可以以`Eigen::IOFormat`的格式将Eigen矩阵打印到任何流中。

```cpp
aio::PrintEigen(matrix, "describe this matrix", stream);
// describe this matrix
// matrix

aio::ReadEigen(stream, matrix);
```

### JSON

`AGTB`基于`using PropTree = boost::property_tree::ptree`提供了json的读写功能，通过特化`JsonParser`可以通过`ParseJson`函数实现任何类型的解析功能。`AGTB`还提供了一系列函数（位于`struct AGTB::Container::PTree`）来简化`PropTree`的只读访问。提供的`ReadJson`和`PrintJson`均有流和文件的重载版本。

```cpp
AGTB::PropTree ptree {};
aio::ReadJson("data.json", ptree);
aio::ReadJson(stream, ptree);
aio::PrintJson(json, "out.json");
aio::PrintJson(json, stream);
```

#### JsonParser

`AGTB`通过`JsonParser`和`ParseJson`的组合来实现从`PropTree`中解析特点的类型，其定义如下：

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

`JsonParser`至少需要实现两个静态函数`Parse`和`Expect`，否则不满足约束条件，一个例子如下:

```cpp
template <>
struct JsonParser<Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>>
{
    using Target = Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>;

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        return DoParse(json, "distances", "angles", "azi_beg", "x_beg", "y_beg");
    }

    template <typename __ptree>
    Target ParseConfig(const __ptree &json) const
    {
        return DoParse(json, dis, ang, ab, x, y);
    }
    JsonParser(std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
        : dis(distances), ang(angles), ab(azi_beg), x(x_beg), y(y_beg)
    {
    }
    ~JsonParser() = default;

private:
    std::string dis, ang, ab, x, y;

    template <typename __ptree>
    static Target DoParse(const __ptree &json, std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
    {
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, distances);
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, angles);
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, azi_beg);
        AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(json, x_beg, double);
        AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(json, y_beg, double);

        return {
            .distances = PTree::ArrayTo<std::vector<double>>(json, distances),
            .angles = PTree::ArrayView(json, angles) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, azi_beg)),
            .x_beg = PTree::Value<double>(json, x_beg),
            .y_beg = PTree::Value<double>(json, y_beg)};
    }

public:
    static std::string Expect()
    {
        return R"(
{
    "distances" : [105.22, 80.18, 129.34, 78.16],
    "angles" : [
        [107, 48, 32], 
        [73, 0, 24], 
        [89, 33, 48], 
        [89, 36, 30]
    ],
    "azi_beg" : [125, 30, 0],
    "x_beg" : 506.32,
    "y_beg" : 215.65
}
        )";
    }
};
```

这个`AGTB`的内部实现作为例子，演示了如何正确实例化`JsonParser`并配置自定义行为:

- `Expect` : 此函数给出一个默认情况下期待的json格式，`Parse`应按照此格式进行解析
- `Parse` : 单参数版本`ParseJson`将默认调用此函数，若不存在，则尝试默认构造一个`JsonParser`，并调用其`ParseConfig`
- `ParseConfig` : 双参数版本`ParseJson`将默认调用传入的`JsonParser`的此函数，若不存在，则调用`Parse`。你可以在`JsonParser`的特化版本中定义任何其他的成员以自定义其行为，这里通过5个字符串成员来解析特定的json键。

你应该注意到了实现中使用的`AGTB_JSON_PARSER_VALIDATE_*_KEY`系列宏，这是`AGTB`提供的，**仅用于`JsonParser`内部的**json键检查，他会在指定类型的键不存在时抛出带详细信息的异常，包括缺失的键名、默认期待的json格式等。

这里再简要介绍`PTree`系列自由函数的使用:
- `ArrayView` : 将当前`PropTree`（或指定键的子树）转为数组视图，可见例子中的`"distances"`键
- `MapView` : 将当前`PropTree`（或指定键的子树）转为字典视图，在例子中即为整个原始树
- `Value` : 将当前`PropTree`（或指定键的子树）转为值（需给定类型），在例子中即为`"x_beg"`键
- `ArrayTo` : 将当前`PropTree`（或指定键的子树）的数组视图转为容器

这些函数提供了对于`PropTree`的快捷、直观的访问方法。

## Macros

`AGTB`中存在大量的宏，一些仅供内部使用，一些同时为用户设计，还有一些专门提供给用户来定义`AGTB`的部分行为。

### 仅内部使用但应知晓其存在

- `AGTB_THROW( ... )` : `AGTB`抛出的，带有风格化信息的异常
- `AGTB_TEMPLATE_NOT_SPECIALIZED()` : 表示若没有特化版本，则触发编译时错误，因为不存在默认实现
- `AGTB_FILE_NOT_IMPELEMENT()` : 表示该文件未被实现或因为种种原因不应该被包含

### 同时提供给用户

- `AGTB_JSON_PARSER_VALIDATE_*_KEY` : 用于`JsonPaser`内部的json键检查

### 专门提供给用户

这些必须在包含前定义以调整`AGTB`的行为

- `#define AGTB_DISABLE_NOTE` : 抑制`AGTB`的所有`#warning`
- `#define AGTB_ENABLE_DEBUG` : 开启`AGTB`的调试输出
- `#define AGTB_DEBUG_INFO_VERBOSE_ALL` : 开启`AGTB`的全部调试输出，可能导致输出过于混乱
- `#define AGTB_ENABLE_EXP` : 开启`AGTB`的实验性功能

## Auxiliary

`AGTB`还有一系列用于辅助上述四个模块工作的内部模块，一些符号可能来自于这里，我只展示一部分。

### Utils

- `Angles` : 和六十进制角度计算以及角度转换相关的工具
- `Angle` : `Angles`下表示六十进制角度的类
- `Math` : 辅助性数学函数，如测绘专业的舍入规则和约等于
- `Concept`: 元编程相关
- `Error` : 各类异常

### Container

- `NamedGraph` : `BGL`中类的具名包装，可以通过名称访问边和顶点，而不止通过索引
- `PropTree, PropPath` : `boost::property_tree::ptree, path`的别名
- `PTree` : 用于访问`PropTree`的一系列自由函数

### Linalg

- `LinalgOption` : 用于线性代数算法的一些选项，主要是求逆的方法
- `CorrectionOlsSolve` : 最小二乘原理下改正数的等效解
- `NormalEquationMatrixInverse` : $(A^T P A)^{-1}$，正规方程矩阵的逆
- `Rotate*` : 旋转矩阵相关
- `Cs*` : 坐标转换相关

## Version

- `AGTB` : `0.0.14`
- `Date` : `26.1.7`