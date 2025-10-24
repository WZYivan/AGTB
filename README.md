# Ano Geomatics ToolBox

**AGTB** is a header-only library containing a series of algorithms for **Geomatics Science**. Currently focused on photogrammetry and spatial computation algorithms.

## Installation

This is a conan package. Use the following command to install:

```bash
conan install .
```

## Space Resection Module

AGTB provides an Ordinary Least Squares (OLS) iterative optimization method for space resection with a pure functional interface.

### Quick Example

```cpp
#include <AGTB/SpaceResection.hpp>
#include <sstream>

int main()
{
    namespace srs = AGTB::SpaceResection::Solve;

    // Read input data from any stream
    std::istringstream iss{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};
    
    // Parse data into photo and object coordinates
    auto [photo, obj] = srs::ReadMatrix<srs::MatrixLayout::PhotoLeft>(iss);
    
    // Display input matrices
    srs::Print::Matrix(photo, "photo"); // in mm
    srs::Print::Matrix(obj, "obj");
    
    // Convert photo coordinates from mm to meters
    photo /= 1000;

    // Define internal orientation parameters
    srs::InternalElements internal{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000,  // 153.24mm -> meters
        .m = 50000};          // scale denominator

    // Solve space resection using quick method with SVD
    auto result = srs::QuickSolve<srs::NormalizationTag::SVD>(internal, photo, obj);
    
    // Check solution status and display results
    if (result.info == srs::Info::Success)
    {
        srs::Print::Result(result); // photo coordinates in mm
    }
    else
    {
        throw std::runtime_error("QuickSolve failed");
    }
}
```

### Core Functions Reference

#### Data Input Functions

##### `ReadMatrix<MatrixLayout>`
```cpp
template<MatrixLayout layout>
auto ReadMatrix(std::istream& is, std::string sep = ",");
```
**Purpose**: Parses coordinate data from input stream into matrices.

**Layout Options**:
- `MatrixLayout::PhotoLeft`: Photo coordinates in left 2 columns, object coordinates in right 3 columns
- `MatrixLayout::PhotoRight`: Photo coordinates in right 2 columns, object coordinates in left 3 columns  
- `MatrixLayout::PhotoOnly`: Only photo coordinates (2 columns)
- `MatrixLayout::ObjectOnly`: Only object coordinates (3 columns)

**Returns**: Tuple containing parsed matrices based on layout.

#### Solving Functions

##### `QuickSolve`
```cpp
template<NormalizationTag nTag = NormalizationTag::Cholesky>
SpaceResectionSolveResult QuickSolve(
    const InternalElements& internal,
    const Matrix& photo, 
    const Matrix& object,
    size_t max_loop = 50,
    double threshold = 3e-5);
```
**Purpose**: Fast space resection solution using simplified coefficient calculation (NoAngles method).

**Parameters**:
- `internal`: Internal orientation elements (x0, y0, f, m)
- `photo`: Image coordinates matrix (n×2) in meters
- `object`: Object coordinates matrix (n×3) 
- `max_loop`: Maximum iterations (default: 50)
- `threshold`: Convergence threshold in radians (default: 3e-5 ≈ 6.2 arc-seconds)

**Normalization Methods**:
- `NormalizationTag::Cholesky`: Cholesky decomposition (default, faster)
- `NormalizationTag::SVD`: Singular Value Decomposition (more stable)

##### Alternative Solvers
- `SimplifiedSolve`: Uses KappaOnly coefficient method (balanced accuracy/speed)
- `PreciseSolve`: Uses FullAngles coefficient method (highest accuracy)
- `GeneralSolve`: Template-based solver with full customization

#### Data Structures

##### `InternalElements`
```cpp
struct InternalElements {
    double x0, y0;  // Principal point coordinates (meters)
    double f;       // Focal length (meters) 
    double m;       // Photo scale denominator
};
```

##### `SpaceResectionSolveResult`
```cpp
struct SpaceResectionSolveResult {
    ExternalElements external;  // Solved exterior orientation
    Matrix rotate;             // 3×3 rotation matrix
    Matrix sigma;              // Accuracy assessment matrix
    Matrix photo;              // Adjusted photo coordinates (mm)
    double m0;                 // Unit weight mean square error
    Info info;                 // Solution status
};
```

#### Output Functions

##### `Print::Matrix`
```cpp
void Matrix(const Matrix& m, const std::string msg, 
           const Eigen::IOFormat& fmt = fmt::bordered, 
           std::ostream& os = std::cout);
```
**Purpose**: Formatted matrix output with descriptive message.

##### `Print::Result`
```cpp
void Result(const SpaceResectionSolveResult& result, std::ostream& os = std::cout);
```
**Purpose**: Comprehensive result display including:
- Exterior orientation parameters
- Adjusted photo coordinates  
- Rotation matrix
- Error analysis
- Unit weight error

#### Status Enumeration

##### `Info`
```cpp
enum class Info : size_t {
    Success,        // Solution converged successfully
    NotConverged,   // Maximum iterations reached
    Failed,         // Numerical or input failure
    Unknown         // Unknown status
};
```

### Input Data Format

The library expects coordinate data in CSV format (without head). For the example above:

```
# photo_x, photo_y, object_X, object_Y, object_Z
-86.15,  -68.99,  36589.41, 25273.32, 2195.17
-53.40,   82.21,  37631.08, 31324.51,  728.69
-14.78,  -76.63,  39100.97, 24934.98, 2386.50
 10.46,   64.43,  40426.54, 30319.81,  757.31
```

**Note**: Photo coordinates should be provided in **millimeters** and will be converted to meters internally. Object coordinates are typically in the project coordinate system units.

### Algorithm Characteristics

- **Method**: Iterative least squares with collinearity equations
- **Convergence**: Based on angular corrections (< 3e-5 radians)
- **Minimum Control Points**: 4 points required
- **Output**: Photo coordinates are returned in millimeters for convenience

### Dependencies

- Eigen3 (for matrix operations)
- C++17 or later
- Conan for package management
