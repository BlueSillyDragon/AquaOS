/**
Snow Operating System
Copyright (c) UtsumiFuyuki 2025, 2026
 
File: hal/timers/timer.cpp

Description:
Timer abstractions

Author:
UtsumiFuyuki
October 20th, 2026
**/

#include <hal/serial.hpp>
#include <hal/amd64/timers/hpet.hpp>
#include <hal/amd64/apic/apic.hpp>
#include <hal/timer.hpp>

void hal::initializeTimers() {
    #if defined (__x86_64)
        // TODO: Add fallbacks for HPET
        hal::x64::enableHpet();
        hal::x64::enableLapic();
    #endif
}