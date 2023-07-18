cmake_minimum_required(VERSION 3.25)
project(ods-conv C)

set(CMAKE_C_STANDARD 11)

find_package(libzip REQUIRED)
find_package(libxml2 REQUIRED)

add_executable(ods-conv ods-conv.c)

target_link_libraries(ods-conv
    libzip::zip
    LibXml2::LibXml2)
