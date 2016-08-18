set(SOURCES EventScanRate.cpp
            EventScanner.cpp
            Peripherals.cpp
            PeripheralUtils.cpp
)

set(HEADERS EventScanRate.h
            EventScanner.h
            Peripherals.h
            PeripheralTypes.h
            PeripheralUtils.h
)

core_add_library(peripherals)
