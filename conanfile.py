from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps

class agtbRecipe(ConanFile):
    name = "agtb"
    version = "0.0.12"
    package_type = "header-library"

    # Optional metadata
    license = "MIT"
    author = "lucas wang 2270060975@qq.com"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "A toolbox for geomatics"
    topics = ("Geomatics Science", "Photogrammetry", "Geodesy", "Adjustment", "Algorithm")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "include/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["agtb"]
        
    def requirements(self):
        self.requires("eigen/[*]")
        self.requires("gcem/[*]")
        self.requires("boost/[*]")

