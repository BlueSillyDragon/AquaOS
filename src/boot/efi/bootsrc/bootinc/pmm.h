#pragma once

class Pmm {

    public:
        EFI_PHYSICAL_ADDRESS allocatePages(UINTN pages);
};