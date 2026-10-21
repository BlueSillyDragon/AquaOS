incdirs = include_directories(
    '../common/include/freestnd-c-hdrs/x86_64/include',
    '../common/include/freestnd-cxx-hdrs/x86_64/include',
    '../common/include/limine-protocol/include',
    'source/hal/flanterm/src',
    '../Limine',
    'include'
)

sources = files(
    'source/ke/kernel.cpp',
    'source/ke/interrupts.cpp',
    'source/ke/string.cpp',
    'source/ke/log.cpp',
    'source/ke/panic.cpp',
    'source/ke/kernel_api.cpp',
    'source/ke/sdbg.cpp',
    'source/ke/process.cpp',
    'source/ke/schedule.cpp',
    'source/ke/syscall.cpp',
    'source/ke/syscall.asm',
    'source/ke/stack.asm',
    'source/ke/context_switch.asm',
    'source/hal/hal.cpp',
    'source/hal/flanterm/src/flanterm.c',
    'source/hal/flanterm/src/flanterm_backends/fb.c',
    'source/hal/paging.cpp',
    'source/hal/serial.cpp',
    'source/hal/acpi_tables.cpp',
    'source/hal/timer.cpp',
    'source/hal/amd64/paging.cpp',
    'source/hal/amd64/gdt.asm',
    'source/hal/amd64/isr.asm',
    'source/hal/amd64/fred.cpp',
    'source/hal/amd64/fred.asm',
    'source/hal/amd64/syscall.cpp',
    'source/hal/amd64/timers/hpet.cpp',
    'source/hal/amd64/apic/apic.cpp',
    'source/mm/early_alloc.cpp',
    'source/mm/physical.cpp',
    'source/mm/vmem.cpp',
    'source/mm/vm.cpp',
    'source/mm/slab.cpp',
    'source/ldr/pe.cpp',
    'source/io/gz.cpp',
    'source/yk/syscall.cpp'
)

add_project_arguments(
    '--target=x86_64-unknown-windows-gnu',
    '-nostdinc',
    '-ffreestanding',
    '-DLIMINE_API_REVISION=6',
    '-DUACPI_BAREBONES_MODE',
    '-fno-stack-protector',
    '-fno-stack-check',
    '-fno-PIC',
    '-ffunction-sections',
    '-fdata-sections',
    '-m64',
    '-march=x86-64',
    '-mno-80387',
    '-mno-mmx',
    '-mno-sse',
    '-mno-sse2',
    '-mno-red-zone',
    '-mcmodel=kernel',
    '-fdeclspec',
    '-MMD',
    '-MP',
    '-g',

    language: ['cpp', 'c']
)

add_project_arguments(
    '-fwin64',

    language: 'nasm'
)

add_project_link_arguments(
    '-Wl,/subsystem:NATIVE',
    '-Wl,/entry:keMain',
    '-Wl,/largeaddressaware',
    '-Wl,/base:0xFFFFFFFF80000000',
    '-Wl,/nodefaultlib',
    '-Wl,/kernel',
    '-Wl,/debug',
    '-Wl,/verbose',
    '-static',

    language: ['cpp', 'c']
)

uacpi = subproject('uacpi')
aomi_test_program = subproject('aomi')
aina_test_program = subproject('aina')

uacpi_sources = uacpi.get_variable('sources')
sources += uacpi_sources

uacpi_includes = uacpi.get_variable('includes')

executable('yuki.exe', sources, include_directories: [incdirs, uacpi_includes])