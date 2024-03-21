cmake_minimum_required(VERSION 3.5)

project(AquaOS C CXX ASM_NASM)

if (WIN32)
    message("You are trying to compile AquaOS on a Windows system, It's recommended that you build on Linux, you may run into trouble")
endif()

set(CXX_STANDARD 20)

add_subdirectory("src/boot/efi")