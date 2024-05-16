cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(VendingMachineF)

target_sources(app PRIVATE src/main.c ../assig3/src-gen/Statechart.c)
target_include_directories(app PRIVATE ../assig3/src-gen ../assig3/src)
