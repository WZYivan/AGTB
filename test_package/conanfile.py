import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run


class agtbTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            targets = {"SpaceResection" : False, 
                       "EigenIO" : False, 
                       "StringUtils" : False,
                       "Geodesy_Base": False,
                       "Bessel_Coeff": False,
                       "Geodesy_PC" : False,
                       "MeridianArc_Part" : False,
                       "AngleUtils" : False,
                       "ConceptUtils" : False,
                       "Adj_Traverse_ClosedLoop" : True}
            for (tar, b) in targets.items():
                cmd = cmd = os.path.join(self.cpp.build.bindir, tar)
                if b:
                    self.run(cmd, env="conanrun")
