#include <AGTB/IO/Eigen.hpp>
#include <iostream>
#include <Eigen/Dense>
#include <cassert>

int main()
{
    using AGTB::IO::detail::EigenIO::ContainerOf;
    using AGTB::IO::detail::EigenIO::IsValidContainer;
    using AGTB::IO::detail::EigenIO::MMDOf;

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
    AGTB::IO::ReadEigen(iss, mat);
    AGTB::IO::PrintEigen(mat, "Read mat");

    // iss = std::istringstream(data);
    // using MMD_4_2_d = AGTB::IO::EigenIO::MatrixMetaData<double, 4, 2>;
    // Eigen::MatrixXd mat_cus(4, 2);
    // AGTB::IO::EigenIO::ReadEigenCustom<
    //     MMD_4_2_d,
    //     AGTB::IO::EigenIO::ContainerOf<decltype(mat_cus)>>(iss, mat_cus);
    // AGTB::IO::PrintEigen(mat_cus, "Custom read(4, 2)");

    // iss = std::istringstream(data);
    // using MMD_2_4_d = AGTB::IO::EigenIO::MatrixMetaData<double, 2, 4>;
    // Eigen::MatrixXd mat_2_4(2, 4);
    // AGTB::IO::EigenIO::ReadEigenCustom<
    //     MMD_2_4_d,
    //     AGTB::IO::EigenIO::ContainerOf<decltype(mat_2_4)>>(iss, mat_2_4);
    // AGTB::IO::PrintEigen(mat_2_4, "Custom read(2, 4)");

    using Target = AGTB::Linalg::Matrix;
    using Parser = AGTB::IO::JsonParser<Target>;
    std::istringstream json_in(Parser::Expect());
    AGTB::PropTree json;
    AGTB::IO::ReadJson(json_in, json);
    Target tar = AGTB::IO::ParseJson<Target>(json);
    AGTB::IO::PrintEigen(tar, "From json");

    return 0;
}