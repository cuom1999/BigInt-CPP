cmake_minimum_required(VERSION 3.16)
project(CS3353_Final)

set(CMAKE_CXX_STANDARD 14)
set (input "input.txt")

foreach(file IN LISTS input)
    configure_file(${file} ${file} COPYONLY)
endforeach()

add_executable(CS3353_Final main.cpp BigInt.cpp BigInt.h FastFourierTransform.cpp FastFourierTransform.h)