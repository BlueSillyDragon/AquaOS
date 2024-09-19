cmake_minimum_required(VERSION 3.28)

project(AquaOS)

if (WIN32)
    message("You are trying to build AquaOS on a Windows System, this has not been tested, and it's recommended to build on Linux")
endif()

add_subdirectory(src/boot/efi)