cmake_minimum_required(VERSION 3.28)

project(SnowOS C CXX ASM_NASM)

if (WIN32)
    message("You are trying to build SnowOS on a Windows System, you may encounter unexpected behavior!")
endif()

add_subdirectory(snowboot/efi)
add_subdirectory(yuki)