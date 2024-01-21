cmake_minimum_required(VERSION 3.11)
project(TEST)
# 选择C++11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# 判断OS
set(INSTALL_PATH ${PROJECT_BINARY_DIR}/install)
# 配置其他库的目录

# 配置输出路径
set(EXECUTABLE_OUTPUT_PATH ${INSTALL_PATH}/bin)
set(LIBRARY_OUTPUT_PATH ${INSTALL_PATH}/bin)

# 源文件
aux_source_directory(${PROJECT_SOURCE_DIR} SRC)
# 可执行程序
add_executable(TEST_EXE ${SRC})