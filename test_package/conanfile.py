import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run

import re

def parse_cmake_for_executables(cmake_content: str) -> dict:
    """
    Parses CMakeLists.txt content to find uncommented create_new_executable calls.

    Args:
        cmake_content: The content of the CMakeLists.txt file as a string.

    Returns:
        A dictionary where keys are target names and values are True if the
        call is uncommented, False if it's commented out.
    """
    # 正则表达式解释:
    # ^\s*              : 行首，匹配任意数量的空白字符（空格或制表符）
    # (?:\s*#\s*)?      : 可选的非捕获组，匹配注释前缀 (例如 " #" 或 "#")
    # create_new_executable : 匹配字面量 "create_new_executable"
    # \s*                : 匹配调用前的任意数量空白字符
    # \(\s*              : 匹配左括号 '(' 和其后的任意数量空白字符
    # ([^,\s)]+)         : 捕获组 1 (tar): 匹配目标名称，不能包含逗号、空格或右括号
    # \s*,\s*            : 匹配逗号 ',' 及其前后的任意数量空白字符
    # ([^\s)]+)\s*       : 捕获组 2 (file): 匹配源文件名，不能包含空格或右括号，后跟任意空白字符
    # \)                 : 匹配右括号 ')'
    # \s*$               : 匹配行尾前的任意数量空白字符，直到行尾
    pattern = re.compile(r'^\s*(?:\s*#\s*)?create_new_executable\(\s*([^,\s)]+)', re.IGNORECASE)

    results = []
    for line in cmake_content.splitlines():
        if line.startswith("#"):
            continue
        match = pattern.search(line)
        if match:
            results.append(match.group(1))
    return results

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
            with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "CMakeLists.txt"), 'r', encoding='utf-8') as f:
                contents = f.read()
            targets = parse_cmake_for_executables(contents)
            for tar in targets:
                cmd = os.path.join(self.cpp.build.bindir, tar)
                # self.run(cmd)
                
                self.run(cmd, env="conanrun")
