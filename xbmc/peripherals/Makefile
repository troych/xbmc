SRCS=EventScanner.cpp \
     EventScanRate.cpp \
     Peripherals.cpp \
     PortMapper.cpp \

LIB=peripherals.a

include ../../Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
