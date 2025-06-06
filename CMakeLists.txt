cmake_minimum_required(VERSION 3.28)

project(SnowOS C CXX ASM_NASM)

if (WIN32)
    message("You are trying to build SnowOS on a Windows System, you may encounter unexpected behavior!")
endif()

add_custom_target(limine ALL
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
COMMAND rm -rf limine
COMMAND git clone https://github.com/limine-bootloader/limine.git --branch=v9.x-binary --depth=1
COMMAND make -C limine)

add_subdirectory(yuki)